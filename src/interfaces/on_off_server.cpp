// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the On-Off interface : Server role.
 *
 * \version    0.4.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/on_off.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// On-Off Interface : Server Role
// =============================================================================

// =============================================================================
// OnOff::Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result OnOff::Server::handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   UNUSED (payload);
   UNUSED (offset);

   OnOff::CMD cmd = static_cast <OnOff::CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ON_CMD:
         on ();
         break;
      case OFF_CMD:
         off ();
         break;
      case TOGGLE_CMD:
         toggle ();
         break;
      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// OnOff::Server::on
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOff::Server::on ()
{
   state (true);
}

// =============================================================================
// OnOff::Server::off
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOff::Server::off ()
{
   state (false);
}

// =============================================================================
// OnOff::Server::toggle
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOff::Server::toggle ()
{
   state (!state ());
}

// =============================================================================
// OnOff::Server::state
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOff::Server::state (bool state)
{
   this->_state = state;
}

// =============================================================================
// OnOff::Server::state
// =============================================================================
/*!
 *
 */
// =============================================================================
bool OnOff::Server::state ()
{
   return _state;
}
