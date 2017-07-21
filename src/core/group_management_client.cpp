// =============================================================================
/*!
 * @file       src/core/group_management_client.cpp
 *
 * This file contains the implementation of the Group Management service : Client role.
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
// Group Management Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::create
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::create(std::string name)
{
   Protocol::Address addr(0, 0);

   CreateMessage *payload = new CreateMessage(name);

   Protocol::Message message(payload->size());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = CREATE_CMD;

   send(addr, message);

   delete payload;
}

// =============================================================================
// Client::delete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove(uint16_t group)
{
   Protocol::Address addr(0, 0);

   DeleteMessage *payload = new DeleteMessage(group);

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = DELETE_CMD;

   send(addr, message);

   delete payload;
}

// =============================================================================
// Client::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::add(uint16_t group, uint16_t device, uint8_t unit)
{
   Protocol::Address addr(0, 0);

   AddMessage *payload = new AddMessage(group, device, unit);

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = ADD_CMD;

   send(addr, message);

   delete payload;
}

// =============================================================================
// Client::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove(uint16_t group, uint16_t device, uint8_t unit)
{
   Protocol::Address addr(0, 0);

   RemoveMessage *payload = new RemoveMessage(group, device, unit);

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = REMOVE_CMD;

   send(addr, message);

   delete payload;
}

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
// =============================================================================
// Client::get_info
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::get_info(uint16_t group)
{
   Protocol::Address addr(0, 0);

   InfoMessage *payload = new InfoMessage(group);

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = GET_INFO_CMD;

   send(addr, message);

   delete payload;
}
#endif

// =============================================================================
// Client::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Client::payload_size(Protocol::Message::Interface &itf) const
{
   switch (itf.member)
   {
      case CREATE_CMD:
         return payload_size_helper<CreateResponse>();

      case DELETE_CMD:
         return payload_size_helper<DeleteResponse>();

      case ADD_CMD:
         return payload_size_helper<AddResponse>();

      case REMOVE_CMD:
         return payload_size_helper<RemoveResponse>();

      case GET_INFO_CMD:
         return payload_size_helper<InfoResponse>();

      default:
         return 0;
   }
}

// =============================================================================
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   assert(packet.link != nullptr);

   switch (packet.message.itf.member)
   {
      case CREATE_CMD:
      {
         CreateResponse response;
         response.unpack(payload, offset);
         created(response);

         break;
      }

      case DELETE_CMD:
      {
         DeleteResponse response;
         response.unpack(payload, offset);
         deleted(response);

         break;
      }

      case ADD_CMD:
      {
         AddResponse response;
         response.unpack(payload, offset);
         added(response);

         break;
      }

      case REMOVE_CMD:
      {
         RemoveResponse response;
         response.unpack(payload, offset);
         removed(response);

         break;
      }

      case GET_INFO_CMD:
      {
         InfoResponse response;
         response.unpack(payload, offset);
         got_info(response);

         break;
      }

      default:
         return Common::Result::FAIL_UNKNOWN;
   }

   return Common::Result::OK;
}
