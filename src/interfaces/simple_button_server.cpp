// =============================================================================
/*!
 * @file       src/interfaces/simple_button_server.cpp
 *
 * This file contains the implementation of the Simple Button interface : Server role.
 *
 * @version    1.5.3
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include <cstdlib>

#include "hanfun/interfaces/simple_button.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleButton;

// =============================================================================
// Simple Button Interface : Server Role
// =============================================================================

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS Server::attributes(uint8_t pack_id) const
{
   HF::Attributes::UIDS result({SHORT_PRESS_MAX_DURATION_ATTR, EXTRA_LONG_PRESS_MIN_DURATION_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {
#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
      result.push_back(DOUBLE_CLICK_GAP_DURATION_ATTR);
#endif
   }

   return result;
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Server::attribute(uint8_t uid)
{
   using namespace HF::Interfaces::SimpleButton;

   SimpleButton::Attributes attr = static_cast<SimpleButton::Attributes>(uid);

   switch (attr)
   {
      case SHORT_PRESS_MAX_DURATION_ATTR:
      {
         typedef HF::Attributes::Attribute<uint16_t, Server> Attribute;

         auto getter = (uint16_t (Server::*)(void) const) & Server::short_press_max_duration;
         auto setter = (void (Server::*)(uint16_t)) & Server::short_press_max_duration;

         return new Attribute(*this, attr, getter, setter, ShortPressMaxDuration::WRITABLE);
      }

      case EXTRA_LONG_PRESS_MIN_DURATION_ATTR:
      {
         typedef HF::Attributes::Attribute<uint16_t, Server> Attribute;

         auto getter = (uint16_t (Server::*)(void) const) & Server::extra_long_press_min_duration;
         auto setter = (void (Server::*)(uint16_t)) & Server::extra_long_press_min_duration;

         return new Attribute(*this, attr, getter, setter, ExtraLongPressMinDuration::WRITABLE);
      }

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
      case DOUBLE_CLICK_GAP_DURATION_ATTR:
      {
         typedef HF::Attributes::Attribute<uint16_t, Server> Attribute;

         auto getter = (uint16_t (Server::*)(void) const) & Server::double_click_gap_duration;
         auto setter = (void (Server::*)(uint16_t)) & Server::double_click_gap_duration;

         return new Attribute(*this, attr, getter, setter, DoubleClickGapDuration::WRITABLE);
      }
#endif

      default:
         return nullptr;
   }
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::pressed
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::pressed(const Protocol::Address &addr, uint16_t timestamp)
{
#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD

   if (_short_click_cmd)
   {
      uint16_t diff = std::abs(((int32_t) timestamp - (int32_t) _timestamp));

      if (diff < _double_click_gap_duration) // Double-Click detected.
      {
         double_click_press(addr);
      }
      else
      {
         short_press(addr);
      }
   }
   else
#endif
   {
      _timestamp = timestamp;
   }
}

// =============================================================================
// Server::released
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::released(const Protocol::Address &addr, uint16_t timestamp)
{
   uint16_t diff = std::abs(((int32_t) timestamp - (int32_t) _timestamp));

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD

   if (_short_click_cmd)
   {
      _short_click_cmd = false;
      return;
   }

#endif

   if (diff <= _short_press_max_duration)
   {
      short_press(addr);
      _short_click_cmd = true;
      _timestamp       = timestamp;
   }
   else
   {
      _short_click_cmd = false;
#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD

      if (_extra_long_press_min_duration != 0 && diff >= _extra_long_press_min_duration)
      {
         extra_long_press(addr);
      }
      else
#endif
      {
         long_press(addr);
      }
   }
}

// =============================================================================
// Server::short_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::short_press(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_BUTTON;
   message.itf.member = SHORT_PRESS_CMD;

   send(addr, message);
}

// =============================================================================
// Server::long_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::long_press(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_BUTTON;
   message.itf.member = LONG_PRESS_CMD;

   send(addr, message);
}

#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD
// =============================================================================
// Server::extra_long_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::extra_long_press(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_BUTTON;
   message.itf.member = EXTRA_LONG_PRESS_CMD;

   send(addr, message);
}
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
// =============================================================================
// Server::double_click_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::double_click_press(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_BUTTON;
   message.itf.member = DOUBLE_CLICK_PRESS_CMD;

   send(addr, message);
}
#endif

// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::short_press_max_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Server::short_press_max_duration() const
{
   return _short_press_max_duration;
}

// =============================================================================
// Server::short_press_max_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::short_press_max_duration(uint16_t __value)
{
   HF_SETTER_HELPER(ShortPressMaxDuration, _short_press_max_duration, __value);

   // Ensure extra long press minimum duration attribute is equal or greater than
   // short press maximum duration.
   if (_extra_long_press_min_duration != 0 &&
       _extra_long_press_min_duration < _short_press_max_duration)
   {
      extra_long_press_min_duration(_short_press_max_duration);
   }
}

// =============================================================================
// Server::extra_long_press_min_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Server::extra_long_press_min_duration() const
{
   return _extra_long_press_min_duration;
}

// =============================================================================
// Server::extra_long_press_min_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::extra_long_press_min_duration(uint16_t __value)
{
   HF_ASSERT((__value == 0 || __value >= _short_press_max_duration),
   {
      return;
   });

   HF_SETTER_HELPER(ExtraLongPressMinDuration, _extra_long_press_min_duration, __value);
}

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_GAP_DURATION_ATTR
// =============================================================================
// Server::double_click_gap_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Server::double_click_gap_duration() const
{
   return _double_click_gap_duration;
}

// =============================================================================
// Server::double_click_gap_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::double_click_gap_duration(uint16_t __value)
{
   HF_ASSERT(__value >= DOUBLE_CLICK_GAP_DURATION_MIN_VALUE,
   {
      return;
   });

   HF_SETTER_HELPER(DoubleClickGapDuration, _double_click_gap_duration, __value);
}
#endif
