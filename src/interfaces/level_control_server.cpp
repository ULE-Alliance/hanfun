// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the Level Control interface : Server role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
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
// LevelControlServer::level
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t LevelControlServer::level ()
{
   return _level;
}

// =============================================================================
// LevelControlServer::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControlServer::level (uint8_t new_level)
{
   _level = new_level;
}

// =============================================================================
// LevelControlServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Result LevelControlServer::handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset)
{
   Message level_msg;

   if (packet.message.itf.member != LevelControl::SET_LEVEL_CMD)
   {
      return Result::FAIL_SUPPORT;
   }

   level_msg.unpack (payload, offset);

   level_change (level_msg.level);

   return Result::OK;
}

// =============================================================================
// LevelControlServer::level_change
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControlServer::level_change (uint8_t new_level)
{
   level (new_level);
}

// =============================================================================
// LevelControlServer::attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *LevelControlServer::attribute (uint8_t uid)
{
   Attributes attr = static_cast <Attributes>(uid);

   switch (attr)
   {
      case LEVEL_ATTR:
         return new Attribute<uint8_t &> (this->uid(), attr, _level, Level::WRITABBLE);

      default:
         return nullptr;
   }
}
