// =============================================================================
/*!
 * @file       src/interfaces/level_control_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Client role.
 *
 * @version    1.5.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/level_control.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::LevelControl;

// =============================================================================
// Level Control Interface : Client Role
// =============================================================================

#ifdef HF_ITF_LEVEL_CONTROL_SET_LEVEL_CMD
// =============================================================================
// Client::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::level(Protocol::Address &addr, uint8_t new_level)
{
   Message level_msg(new_level);

   Protocol::Message message(level_msg.size());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = LevelControl::Client::uid();
   message.itf.member = SET_LEVEL_CMD;


   level_msg.pack(message.payload);

   send(addr, message);
}

// =============================================================================
// Client::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::level(Protocol::Address &addr, float new_level)
{
   check_and_fix(new_level);
   uint8_t value = HF::Common::from_percent<uint8_t>(new_level);
   level(addr, value);
}

#endif

#ifdef HF_ITF_LEVEL_CONTROL_INCREASE_LEVEL_CMD
// =============================================================================
// Client::increase_level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::increase_level(Protocol::Address &addr, uint8_t increment)
{
   Message level_msg(increment);

   Protocol::Message message(level_msg.size());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = LevelControl::Client::uid();
   message.itf.member = INCREASE_LEVEL_CMD;

   level_msg.pack(message.payload);

   send(addr, message);
}

// =============================================================================
// Client::increase_level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::increase_level(Protocol::Address &addr, float increment)
{
   check_and_fix(increment);
   uint8_t value = HF::Common::from_percent<uint8_t>(increment);
   increase_level(addr, value);
}

#endif

#ifdef HF_ITF_LEVEL_CONTROL_DECREASE_LEVEL_CMD
// =============================================================================
// Client::decrease_level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::decrease_level(Protocol::Address &addr, uint8_t decrement)
{
   Message level_msg(decrement);

   Protocol::Message message(level_msg.size());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = LevelControl::Client::uid();
   message.itf.member = DECREASE_LEVEL_CMD;

   level_msg.pack(message.payload);

   send(addr, message);
}

// =============================================================================
// Client::decrease_level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::decrease_level(Protocol::Address &addr, float decrement)
{
   check_and_fix(decrement);
   uint8_t value = HF::Common::from_percent<uint8_t>(decrement);
   decrease_level(addr, value);
}
#endif
