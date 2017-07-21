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
   Common::Result result = Common::Result::FAIL_ARG;

   CreateResponse create_response;

   auto temp_entry = entry(msg.name);                // try to find this group

   Group group;

   if (temp_entry == nullptr)                    // Group not found. Create new.
   {
      if (entries().size() != Group::MAX_MEMBERS)  // Check if we have space for the new group
      {
         group.address = next_address();
         group.name    = msg.name;

         result        = entries().save(group);

         if (result == Common::Result::OK)
         {
            create_response.address = group.address;
         }
      }
      else
      {
         result = Common::Result::FAIL_ARG;
      }
   }
   else                             // found a group with the same name. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }

   create_response.code = result;


   Protocol::Message response(packet.message, create_response.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = GroupManagement::IServer::uid();
   response.itf.member = CREATE_CMD;

   create_response.pack(response.payload);

   Protocol::Address res_addr(packet.source);

   send(res_addr, response, packet.link);

   if (result == Common::Result::OK)
   {
      GroupPtr temp(&group);
      this->created(temp);

      number_of_groups_update(1);
   }

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

   DeleteResponse receive_response;

   Common::Result result = Common::Result::FAIL_ARG;

   auto temp_entry       = entry(msg.address);          // try to find this group

   Group group;

   if (temp_entry == nullptr)       // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }
   else                             // found a group with the same address.
   {
      group  = Group(*temp_entry);

      result = entries().destroy(group);
   }

   receive_response.code = result;

   Protocol::Message response(packet.message, receive_response.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = GroupManagement::IServer::uid();
   response.itf.member = DELETE_CMD;

   receive_response.pack(response.payload);

   Protocol::Address res_addr(packet.source);

   send(res_addr, response, packet.link);

   if (result == Common::Result::OK)
   {
      this->deleted(group);
      number_of_groups_update(-1);
   }


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
   Common::Result result = Common::Result::FAIL_ARG;

   auto group            = entry(msg.address);     // try to find this group

   if (group == nullptr)       // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }
   else                             // found a group with the same address.
   {
      Member received(msg.device, msg.unit);

      if (group->add_member(received))
      {
         result = Common::Result::OK;
      }
   }

   AddResponse receive_response;

   receive_response.code = result;

   Protocol::Message response(packet.message, receive_response.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = GroupManagement::IServer::uid();
   response.itf.member = ADD_CMD;

   receive_response.pack(response.payload);

   send(packet.source, response, packet.link);

   if (result == Common::Result::OK)
   {
      this->added(group);
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
   Common::Result result = Common::Result::FAIL_ARG;

   auto group            = entry(msg.address);     // try to find this group

   if (group == nullptr)       // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }
   else                             // found a group with the same address.
   {
      Member received(msg.device, msg.unit);

      if (group->remove_member(received))
      {
         result = Common::Result::OK;
      }
   }

   RemoveResponse receive_response;

   receive_response.code = result;

   Protocol::Message response(packet.message, receive_response.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = GroupManagement::IServer::uid();
   response.itf.member = REMOVE_CMD;

   receive_response.pack(response.payload);

   send(packet.source, response, packet.link);

   if (result == Common::Result::OK)
   {
      this->removed(*group);
   }

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
   InfoResponse receive_response;

   auto group = entry(msg.address);                // try to find this group

   if (group == nullptr)       // Group not found. Give an error
   {
      result = Common::Result::FAIL_ARG;
   }
   else                             // found a group with the same address.
   {
      result                   = Common::Result::OK;
      receive_response.name    = group->name;
      receive_response.members = group->members;
   }

   receive_response.code = result;

   Protocol::Message response(packet.message, receive_response.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = GroupManagement::IServer::uid();
   response.itf.member = GET_INFO_CMD;

   receive_response.pack(response.payload);

   send(packet.source, response, packet.link);

   if (result == Common::Result::OK)
   {
      this->got_info(*group);
   }

   return result;
}
#endif
