// =============================================================================
/*!
 * @file       src/interfaces/level_control_server.cpp
 *
 * This file contains the implementation of the Level Control interface : Server role.
 *
 * @version    1.5.2
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
uint8_t Server::level()
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
void Server::level(uint8_t __level)
{
   uint8_t old = this->_level;

   this->_level = __level;

   Level old_attr(old, this);
   Level new_attr(this->_level, this);

   notify(old_attr, new_attr);
}

// =============================================================================
// Server::level
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::level(float new_level)
{
   check_and_fix(new_level);
   uint8_t value = HF::Common::from_percent<uint8_t>(new_level);
   level(value);
}


#ifdef HF_ITF_LEVEL_CONTROL_INCREASE_LEVEL_CMD

void Server::increase(uint8_t increment)
{
   int16_t new_value;

   new_value = this->_level + increment;

   check_and_fix(new_value);

   if (new_value != level())
   {
      level(static_cast<uint8_t>(new_value));
   }
}

void Server::increase(float increment)
{
   check_and_fix(increment);
   uint8_t value = HF::Common::from_percent<uint8_t>(increment);
   increase(value);
}
#endif

#ifdef HF_ITF_LEVEL_CONTROL_DECREASE_LEVEL_CMD

void Server::decrease(uint8_t decrement)
{
   int16_t new_value;

   new_value = this->_level - decrement;

   check_and_fix(new_value);

   if (new_value != level())
   {
      level(static_cast<uint8_t>(new_value));
   }
}

void Server::decrease(float decrement)
{
   check_and_fix(decrement);
   uint8_t value = HF::Common::from_percent<uint8_t>(decrement);
   decrease(value);
}
#endif

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_attribute(Protocol::Packet &packet, Common::ByteArray &payload,
                                        uint16_t offset)
{
   uint8_t old_level     = level();

   Common::Result result = AbstractInterface::handle_attribute(packet, payload, offset);

   if (result == Common::Result::OK && packet.message.type == Protocol::Message::SET_ATTR_REQ)
   {
      level_change(packet.source, old_level, level());
   }

   return result;
}

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   CMD cmd               = static_cast<CMD>(packet.message.itf.member);

   Common::Result result = AbstractInterface::check_payload_size(packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   Message level_msg;
   level_msg.unpack(payload, offset);

   uint8_t old_value = level();

   switch (cmd)
   {
#ifdef HF_ITF_LEVEL_CONTROL_SET_LEVEL_CMD
      case SET_LEVEL_CMD:
      {
         level(level_msg.level);
         break;
      }
#endif

#ifdef HF_ITF_LEVEL_CONTROL_INCREASE_LEVEL_CMD
      case INCREASE_LEVEL_CMD:
      {
         increase(level_msg.level);
         break;
      }
#endif

#ifdef HF_ITF_LEVEL_CONTROL_DECREASE_LEVEL_CMD
      case DECREASE_LEVEL_CMD:
      {
         decrease(level_msg.level);
         break;
      }
#endif
      default:
         return Common::Result::FAIL_SUPPORT;
   }

   level_change(packet.source, old_value, this->_level);

   return Common::Result::OK;
}

// =============================================================================
// Server::level_change
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::level_change(Protocol::Address &source, uint8_t old_level, uint8_t new_level)
{
   UNUSED(source);
   UNUSED(old_level);
   UNUSED(new_level);
}
