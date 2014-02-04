// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Client role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/level_control.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// Level Control Interface : Client Role
// =============================================================================

// =============================================================================
// LevelControl::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControlClient::level (Protocol::Address &addr, uint8_t new_level)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = SET_LEVEL_CMD;

   message.payload    = new Message (new_level);

   sendMessage (addr, message);
}
