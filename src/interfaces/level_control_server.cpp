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
// LevelControlServer::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
bool LevelControlServer::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   Message level_msg;

   // Check for correct interface and command.
   if (!AbstractInterface::handle (message, payload, offset, level_msg.size ()))
   {
      return false;
   }

   level_msg.unpack (payload, offset);

   level_change (level_msg.level);

   return true;
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
