// =============================================================================
/*!
 * @file       src/interfaces/simple_keypad_client.cpp
 *
 * This file contains the implementation of the Simple Keypad interface : Client role.
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
// Simple Keypad Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::keypressed
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::keypressed(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::SIMPLE_KEYPAD;
   message.itf.member = KEYPRESSED_CMD;

   send(addr, message);
}
