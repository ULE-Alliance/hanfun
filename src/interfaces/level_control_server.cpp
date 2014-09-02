// =============================================================================
/*!
 * \file       src/interfaces/level_control_server.cpp
 *
 * This file contains the implementation of the Level Control interface : Server role.
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
   return this->_level;
}

// =============================================================================
// LevelControl::Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void LevelControl::Server::level (uint8_t __level)
{
   uint8_t old = this->_level;

   this->_level = __level;

   Level old_attr (old);
   Level new_attr (this->_level);

   notify (old_attr, new_attr);
}

// =============================================================================
// LevelControl::Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result LevelControl::Server::handle_attribute (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   Common::Result result = AbstractInterface::handle_attribute(packet, payload, offset);

   if (result == Common::Result::OK && packet.message.type == Protocol::Message::SET_ATTR_REQ)
   {
      level_change(level());
   }

   return result;
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
