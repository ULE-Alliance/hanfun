// =============================================================================
/*!
 * @file       src/core/group_management_server.cpp
 *
 * This file contains the implementation of the Group Management service : Server role.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/group_management.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupManagement;

// =============================================================================
// Helper Functions
// =============================================================================

static bool operator==(Message const &lhs, Message const &rhs)
{
   return lhs.address == rhs.address &&
          lhs.device == rhs.device &&
          lhs.unit == rhs.unit;
}

// =============================================================================
// Group Management Interface : Server Role
// =============================================================================

// =============================================================================
// IServer::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t IServer::payload_size(Protocol::Message::Interface &itf) const
{
   switch (itf.member)
   {
      case CREATE_CMD:
         return payload_size_helper<CreateMessage>();

      case DELETE_CMD:
         return payload_size_helper<DeleteMessage>();

      case ADD_CMD:
         return payload_size_helper<AddMessage>();

      case REMOVE_CMD:
         return payload_size_helper<RemoveMessage>();

      case GET_INFO_CMD:
         return payload_size_helper<InfoMessage>();

      default:
         return 0;
   }
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *IServer::attribute(uint8_t uid)
{
   using namespace HF::Core::GroupManagement;

   GroupManagement::Attributes attr = static_cast<GroupManagement::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_GROUPS_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::number_of_groups;
         auto setter = nullptr;

         return new Attribute(*this, attr, getter, setter, NumberOfGroups::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// IServer::unit0
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Devices::Concentrator::IUnit0 &IServer::unit0() const
{
   return static_cast<Devices::Concentrator::IUnit0 &>(ServiceRole::unit());
}

// =============================================================================
// IServer::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle(Protocol::Packet &packet, Common::ByteArray &payload,
                               uint16_t offset)
{
   if (packet.message.itf.id == group_table().uid())
   {
      return group_table().handle(packet, payload, offset);
   }
   else
   {
      return Server::handle(packet, payload, offset);
   }
}

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                       uint16_t offset)
{
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case CREATE_CMD:
      {
         CreateMessage msg;
         msg.unpack(payload, offset);
         return create(packet, msg);
      }

      case DELETE_CMD:
      {
         DeleteMessage msg;
         msg.unpack(payload, offset);
         return remove(packet, msg);
      }

      case ADD_CMD:
      {

         AddMessage msg;
         msg.unpack(payload, offset);

         return add(packet, msg);
      }

      case REMOVE_CMD:
      {
         RemoveMessage msg;
         msg.unpack(payload, offset);
         return remove(packet, msg);
      }

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
      case GET_INFO_CMD:
      {
         InfoMessage msg;
         msg.unpack(payload, offset);
         return get_info(packet, msg);
      }
#endif

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::FAIL_SUPPORT;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::create
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::create(Protocol::Packet &packet, CreateMessage &msg)
{
   Common::Result result = Common::Result::OK;

   uint16_t address      = Group::NO_ADDR;

   if (entry(msg.name) != nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   if (entries().size() == Group::MAX_MEMBERS)  // Check if we have space for the new group
   {
      result = Common::Result::FAIL_RESOURCES;
      goto _end;
   }

   address = next_address();

   if (address == Group::NO_ADDR)
   {
      goto _end;
   }

   result = HF::Transport::Group::create(unit().device(), address);

   if (result != Common::Result::OK)
   {
      goto _end;
   }

   result = entries().save(address, msg.name);

   if (result != Common::Result::OK)
   {
      HF::Transport::Group::remove(unit().device(), address);
      goto _end;
   }

   this->created(entries().find(address));

   number_of_groups_update(1);

   _end:

   CreateResponse response(result, address);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::delete
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::remove(Protocol::Packet &packet, DeleteMessage &msg)
{
   Common::Result result = Common::Result::OK;

   auto entry            = entries().find(msg.address);

   Group group;

   if (entry == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   std::for_each(entry->members.begin(), entry->members.end(),
                 [this, entry](const Member &m)
   {
      group_table().remove(m.device, entry->address, m.unit);
   });

   group  = *entry;
   result = entries().destroy(*entry);

   if (result != Common::Result::OK)
   {
      goto _end;
   }

   this->deleted(group);
   number_of_groups_update(-1);

   HF::Transport::Group::remove(unit().device(), group.address);

   _end:

   DeleteResponse response(result);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::add
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::add(Protocol::Packet &packet, const AddMessage &msg)
{
   using namespace HF::Common;

   Member member;

   Result result = Common::Result::OK;

   auto device   = unit0().device_management()->entry(msg.device);

   auto group    = entry(msg.address);

   if (device == nullptr)
   {
      result = Result::FAIL_ARG;
      goto _end;
   }

   if (device->unit(0) == nullptr ||
       !device->unit(0)->has_interface(Interface::GROUP_TABLE, Role::SERVER_ROLE))
   {
      result = Result::FAIL_SUPPORT;
      goto _end;
   }

   if (group == nullptr || group->exists(msg.device, msg.unit))
   {
      result = Result::FAIL_ARG;
   }

   _end:

   if (result == Result::OK)
   {
      group_table().add(packet.source, msg, packet.message.reference);
   }
   else
   {
      added(packet.source, result, msg, packet.message.reference);
   }

   return result;
}

// =============================================================================
// Server::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::remove(Protocol::Packet &packet, const RemoveMessage &msg)
{
   Member member;

   Common::Result result = Common::Result::OK;

   auto group            = entry(msg.address);  // Try to find this group

   if (group == nullptr)                        // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
      goto _error;
   }

   member = Member(msg.device, msg.unit);

   if (!group->remove(member))
   {
      result = Common::Result::FAIL_ARG;
      goto _error;
   }

   this->removed(*group, member);

   HF::Transport::Group::remove(unit().device(), group->address, member.device);

   group_table().remove(member.device, group->address, member.unit);

   _error:

   RemoveResponse response(result);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
// =============================================================================
// Server::get_info
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::get_info(Protocol::Packet &packet, const InfoMessage &msg)
{
   Common::Result result = Common::Result::FAIL_ARG;
   InfoResponse response;

   auto group = entry(msg.address);                // try to find this group

   if (group == nullptr)       // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }
   else                             // found a group with the same address.
   {
      result           = Common::Result::OK;
      response.name    = group->name;
      response.members = group->members;

      this->got_info(*group);
   }

   response.code = result;

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}
#endif

// =============================================================================
// IServer::added
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::added(const Protocol::Address &addr, Common::Result result,
                    const AddMessage &request, uint8_t reference)
{
   GroupPtr group;

   if (result != Common::Result::OK)
   {
      goto _end;
   }

   group = entries().find(request.address);

   if (group == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   result = HF::Transport::Group::add(unit().device(), request.address, request.device);

   if (result != Common::Result::OK)
   {
      goto _end;
   }

   if (!group->add(request.device, request.unit))
   {
      result = Common::Result::FAIL_MODIFIED;

      HF::Transport::Group::remove(unit().device(), request.address, request.device);

      goto _end;
   }

   added(*group, Member(request.device, request.unit));

   _end:

   AddResponse response(result);

   Protocol::Message message(response.size());

   message.type       = Protocol::Message::COMMAND_RES;

   message.reference  = reference;

   message.itf.id     = IServer::uid();
   message.itf.member = ADD_CMD;
   message.itf.role   = SERVER_ROLE;

   response.pack(message.payload);

   send(addr, message);
}

// =============================================================================
// GroupManagement::IGroupTable
// =============================================================================

// =============================================================================
// IGroupTable::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void IGroupTable::add(const Protocol::Address &source, const Message &request, uint8_t reference)
{
   UNUSED(source);
   UNUSED(reference);

   GroupTable::Client::add(request.device, request.address, request.unit);
}

// =============================================================================
// GroupManagement::GroupTableClient
// =============================================================================

// =============================================================================
// IGroupTable::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void GroupTableClient::add(const Protocol::Address &source, const Message &request,
                           uint8_t reference)
{
   requests.emplace_front(source, request, reference);
   IGroupTable::add(source, request, reference);
}

// =============================================================================
// GroupTableClient::added
// =============================================================================
/*!
 *
 */
// =============================================================================
void GroupTableClient::added(const Protocol::Address &addr, const GroupTable::Response &response)
{
   Message message(response.group, addr.device, response.unit);

   auto it = std::find_if(requests.begin(), requests.end(), [&message](const Entry &entry)
   {
      return message == std::get<MESSAGE>(entry);
   });

   /* *INDENT-OFF* */
   HF_ASSERT(it != requests.end(), {return;});
   /* *INDENT-ON* */

   Protocol::Address source;
   uint8_t reference;

   std::tie(source, std::ignore, reference) = *it;

   IGroupTable::added(source, response.code, message, reference);
}
