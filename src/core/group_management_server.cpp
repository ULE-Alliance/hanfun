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
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                       uint16_t offset)
{
   CMD cmd               = static_cast<CMD>(packet.message.itf.member);

   Common::Result result = AbstractInterface::check_payload_size(packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

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
   Member member;

   Common::Result result = Common::Result::OK;

   auto group            = entry(msg.address);

   if (group == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   member = Member(msg.device, msg.unit);

   result = HF::Transport::Group::add(unit().device(), group->address, member.device);
   if(result != Common::Result::OK)
   {
      goto _end;
   }

   if (!group->add_member(member))
   {
      result = Common::Result::FAIL_ARG;
      HF::Transport::Group::remove(unit().device(), group->address, member.device);
      goto _end;
   }

   this->added(group, member);

   _end:

   AddResponse response(result);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

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
      goto _end;
   }

   member = Member(msg.device, msg.unit);

   if (!group->remove_member(member))
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   this->removed(*group, member);

   HF::Transport::Group::remove(unit().device(), group->address, member.device);

   _end:

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
