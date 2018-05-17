// =============================================================================
/*!
 * @file       src/interfaces/simple_visual_effects_server.cpp
 *
 * This file contains the implementation of the Simple Visual Effects interface : Server role.
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

#include "hanfun/interfaces/simple_visual_effects.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleVisualEffects;

#define CALL_EFFECT(_param, _method)                  \
   {                                                  \
      _param effect;                                  \
      uint16_t size = effect.unpack(payload, offset); \
      if (size != 0)                                  \
      {                                               \
         _method(packet.source, effect);              \
      }                                               \
      else                                            \
      {                                               \
         return Common::Result::FAIL_ARG;             \
      }                                               \
   }

// =============================================================================
// Simple Visual Effects Interface : Server Role
// =============================================================================

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
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ON_CMD:
      {
         CALL_EFFECT(OnEffect, on);
         break;
      }

      case OFF_CMD:
      {
         off(packet.source);
         break;
      }

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD
      case BLINK_CMD:
      {
         CALL_EFFECT(BlinkEffect, blink);
         break;
      }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
      case FADE_CMD:
      {
         CALL_EFFECT(FadeEffect, fade);
         break;
      }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
      case BREATHE_CMD:
      {
         CALL_EFFECT(BreatheEffect, breathe);
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

// =============================================================================
// Server::on
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::on(const Protocol::Address &addr, const OnEffect &effect)
{
   UNUSED(addr);
   UNUSED(effect);
}

// =============================================================================
// Server::off
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::off(const Protocol::Address &addr)
{
   UNUSED(addr);
}

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD
// =============================================================================
// Server::blink
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::blink(const Protocol::Address &addr, const BlinkEffect &effect)
{
   UNUSED(addr);
   UNUSED(effect);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
// =============================================================================
// Server::fade
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::fade(const Protocol::Address &addr, const FadeEffect &effect)
{
   UNUSED(addr);
   UNUSED(effect);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
// =============================================================================
// Server::breathe
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::breathe(const Protocol::Address &addr, const BreatheEffect &effect)
{
   UNUSED(addr);
   UNUSED(effect);
}
#endif
