// =============================================================================
/*!
 * @file       src/interfaces/simple_keypad_server.cpp
 *
 * This file contains the implementation of the Simple Keypad interface : Server role.
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

#include "hanfun/interfaces/simple_keypad.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleKeypad;

// =============================================================================
// Simple Keypad Interface : Server Role
// =============================================================================

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case KEYPRESSED_CMD:
      {
         KeyPressed msg;
         msg.unpack(payload, offset);
         key_pressed(packet.source, msg);
         break;
      }

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// Events
// =============================================================================

// =============================================================================
// Server::keypressed
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::key_pressed(const Protocol::Address &addr, KeyPressed &msg)
{
   UNUSED(addr);

   key_received(msg.key_id);
}
