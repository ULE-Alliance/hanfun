// =============================================================================
/*!
 * @file       src/interfaces/simple_button.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Button interface.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/simple_button.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleButton;

// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Interfaces::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return SimpleButton::create_attribute(uid);
   }
}

// =============================================================================
// SimpleButton::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleButton::create_attribute(uint8_t uid)
{
   using namespace HF::Interfaces::SimpleButton;

   SimpleButton::Attributes attr = static_cast<SimpleButton::Attributes>(uid);

   switch (attr)
   {
      case SHORT_PRESS_MAX_DURATION_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, attr,
                                                        ShortPressMaxDuration::WRITABLE);
      }

      case EXTRA_LONG_PRESS_MIN_DURATION_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, attr,
                                                        ExtraLongPressMinDuration::WRITABLE);
      }

      case DOUBLE_CLICK_GAP_DURATION_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, attr,
                                                        DoubleClickGapDuration::WRITABLE);
      }

      default:
         return nullptr;
   }
}
