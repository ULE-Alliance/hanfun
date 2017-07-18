// =============================================================================
/*!
 * @file       src/core/group_table_client.cpp
 *
 * This file contains the implementation of the Group Table service : Client role.
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

#include "hanfun/core/group_table.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupTable;

// =============================================================================
// Group Table Interface : Client Role
// =============================================================================

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
   CMD cmd = static_cast<GroupTable::CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ADD_CMD:
      {
         GroupTable::Response response;
         response.unpack(payload, offset);

         this->added(packet.source, response);

         break;
      }

      case REMOVE_CMD:
      {
         GroupTable::Response response;
         response.unpack(payload, offset);

         this->removed(packet.source, response);

         break;
      }

      default:
      {
         return Common::Result::FAIL_SUPPORT;
      }
   }

   return Common::Result::OK;
}

// =============================================================================
// Client::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Client::payload_size(Protocol::Message::Interface &itf) const
{
   UNUSED(itf);

   return payload_size_helper<GroupTable::Response>();
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::add(const Protocol::Address &addr, const Entry &entry)
{
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.payload    = Common::ByteArray(entry.size());

   entry.pack(message.payload, 0);

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = ADD_CMD;

   send(addr, message);
}

// =============================================================================
// Client::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove(const Protocol::Address &addr, const Entry &entry)
{
   /* *INDENT-OFF* */
   HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message(entry.size());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = REMOVE_CMD;

   entry.pack(message.payload);

   send(addr, message);
}

// =============================================================================
// Client::remove_all
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove_all(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = REMOVE_ALL_CMD;

   send(addr, message);
}

// =============================================================================
// Client::read_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::read_entries(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = READ_ENTRIES_CMD;

   send(addr, message);
}

// =============================================================================
// Events
// =============================================================================

// =============================================================================
// Client::added
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::added(const Protocol::Address &addr, const GroupTable::Response &response)
{
   UNUSED(addr);
   UNUSED(response);
}

// =============================================================================
// Client::removed
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::removed(const Protocol::Address &addr, const GroupTable::Response &response)
{
   UNUSED(addr);
   UNUSED(response);
}
