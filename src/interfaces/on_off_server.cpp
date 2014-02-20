// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the On-Off interface : Server role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/on_off.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// On-Off Interface : Server Role
// =============================================================================

// =============================================================================
// OnOffServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Result OnOffServer::handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset)
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
         return Result::FAIL_SUPPORT;
   }

   return Result::OK;
}

// =============================================================================
// OnOffServer::attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *OnOffServer::attribute (uint8_t uid)
{
   Attributes attr = static_cast <Attributes>(uid);

   switch (attr)
   {
      case STATE_ATTR:
         return new Attribute<bool &> (this->uid(), attr, _state, State::WRITABBLE);

      default:
         return nullptr;
   }
}

// =============================================================================
// OnOffServer::on
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffServer::on ()
{
   state (true);
}

// =============================================================================
// OnOffServer::off
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffServer::off ()
{
   state (false);
}

// =============================================================================
// OnOffServer::toggle
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffServer::toggle ()
{
   state (!state ());
}

// =============================================================================
// OnOffServer::state
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffServer::state (bool state)
{
   this->_state = state;
}

// =============================================================================
// OnOffServer::state
// =============================================================================
/*!
 *
 */
// =============================================================================
bool OnOffServer::state ()
{
   return _state;
}
