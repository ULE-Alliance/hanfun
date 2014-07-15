// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Server role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <cmath>

#include "hanfun/interfaces/level_control.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// Level Control Interface : Server Role
// =============================================================================

// =============================================================================
// LevelControl::Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t LevelControl::Server::level ()
{
   return _level;
}

// =============================================================================
// LevelControl::Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControl::Server::level (uint8_t new_level)
{
   _level = new_level;
}

// =============================================================================
// LevelControl::Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result LevelControl::Server::handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   Message level_msg;

   if (packet.message.itf.member != LevelControl::SET_LEVEL_CMD)
   {
      return Common::Result::FAIL_SUPPORT;
   }

   level_msg.unpack (payload, offset);

   level_change (level_msg.level);

   return Common::Result::OK;
}

// =============================================================================
// LevelControl::Server::level_change
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControl::Server::level_change (uint8_t new_level)
{
   level (new_level);
}
