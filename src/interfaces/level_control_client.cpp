// =============================================================================
/*!
 * \file       src/interfaces/level_control_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Client role.
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
   message.itf.id     = LevelControl::Client::uid ();
   message.itf.member = SET_LEVEL_CMD;


   level_msg.pack (message.payload);

   send (addr, message);
}
