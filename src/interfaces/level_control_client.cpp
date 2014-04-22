// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Client role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
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
void LevelControl::Client::level (Protocol::Address &addr, uint8_t new_level)
{
   Message level_msg (new_level);

   Protocol::Message message (level_msg.size ());

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = LevelControl::Client::uid ();
   message.itf.member = SET_LEVEL_CMD;


   level_msg.pack (message.payload);

   sendMessage (addr, message);
}
