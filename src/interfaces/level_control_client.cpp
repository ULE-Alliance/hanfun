// =============================================================================
/*!
 * \file       src/interfaces/level_control_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Client role.
 *
 * \version    1.0.0
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
   Level * level_attr = new Level (new_level, this);

   Protocol::Message * message = new Protocol::Message (level_attr->size ());

   message->itf.role   = SERVER_ROLE;
   message->itf.id     = LevelControl::Client::uid ();
   message->itf.member = Level::ID;

   message->type = Protocol::Message::SET_ATTR_REQ;

   level_attr->pack (message->payload);

   send (addr, *message);

   delete level_attr;
   delete message;
}
