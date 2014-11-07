// =============================================================================
/*!
 * @file       src/interfaces/level_control_server.cpp
 *
 * This file contains the implementation of the Level Control interface : Server role.
 *
 * @version    1.1.1
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
// Level Control Interface : Server Role
// =============================================================================

// =============================================================================
// Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::level ()
{
   return this->_level;
}

// =============================================================================
// Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::level (uint8_t __level)
{
   uint8_t old = this->_level;

   this->_level = __level;

   Level old_attr (old);
   Level new_attr (this->_level);

   notify (old_attr, new_attr);
}

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_attribute (Protocol::Packet &packet, Common::ByteArray &payload,
                                         size_t offset)
{
   Common::Result result = AbstractInterface::handle_attribute (packet, payload, offset);

   if (result == Common::Result::OK && packet.message.type == Protocol::Message::SET_ATTR_REQ)
   {
      level_change (level ());
   }

   return result;
}

// =============================================================================
// Server::level_change
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::level_change (uint8_t new_level)
{
   level (new_level);
}
