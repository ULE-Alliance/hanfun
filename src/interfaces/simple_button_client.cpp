// =============================================================================
/*!
 * @file       src/interfaces/simple_button_client.cpp
 *
 * This file contains the implementation of the Simple Button interface : Client role.
 *
 * @version    1.4.0
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
// Simple Button Interface : Client Role
// =============================================================================

// =============================================================================
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
#ifdef HF_ITF_SIMPLE_BUTTON_SHORT_PRESS_CMD
      case SHORT_PRESS_CMD:
      {
         short_press(packet.source);
         break;
      }
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_LONG_PRESS_CMD
      case LONG_PRESS_CMD:
      {
         long_press(packet.source);
         break;
      }
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD
      case EXTRA_LONG_PRESS_CMD:
      {
         extra_long_press(packet.source);
         break;
      }
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
      case DOUBLE_CLICK_PRESS_CMD:
      {
         double_click_press(packet.source);
         break;
      }
#endif

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// Commands
// =============================================================================

#ifdef HF_ITF_SIMPLE_BUTTON_SHORT_PRESS_CMD
// =============================================================================
// Client::short_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::short_press(const Protocol::Address &addr)
{
   UNUSED(addr);
}
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_LONG_PRESS_CMD
// =============================================================================
// Client::long_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::long_press(const Protocol::Address &addr)
{
   UNUSED(addr);
}
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD
// =============================================================================
// Client::extra_long_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::extra_long_press(const Protocol::Address &addr)
{
   UNUSED(addr);
}
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
// =============================================================================
// Client::double_click_press
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::double_click_press(const Protocol::Address &addr)
{
   UNUSED(addr);
}
#endif
