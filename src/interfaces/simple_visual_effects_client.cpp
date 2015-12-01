// =============================================================================
/*!
 * @file       src/interfaces/simple_visual_effects_client.cpp
 *
 * This file contains the implementation of the Simple Visual Effects interface : Client role.
 *
 * @version    x.x.x
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

// =============================================================================
// Simple Visual Effects Interface : Client Role
// =============================================================================


// =============================================================================
// Commands
// =============================================================================

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_ON_CMD
// =============================================================================
// Client::on
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::on(const Protocol::Address &addr, uint16_t duration)
{
   OnEffect effect(duration);

   Protocol::Message message(effect.size());

   effect.pack(message.payload);

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_VISUAL_EFFECTS;
   message.itf.member = ON_CMD;

   send(addr, message);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_OFF_CMD
// =============================================================================
// Client::off
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::off(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_VISUAL_EFFECTS;
   message.itf.member = OFF_CMD;

   send(addr, message);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD
// =============================================================================
// Client::blink
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::blink(const Protocol::Address &addr, const BlinkEffect &effect)
{
   Protocol::Message message(effect.size());

   effect.pack(message.payload);

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_VISUAL_EFFECTS;
   message.itf.member = BLINK_CMD;

   send(addr, message);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
// =============================================================================
// Client::fade
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::fade(const Protocol::Address &addr, const FadeEffect &effect)
{
   Protocol::Message message(effect.size());

   effect.pack(message.payload);

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_VISUAL_EFFECTS;
   message.itf.member = FADE_CMD;

   send(addr, message);
}
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
// =============================================================================
// Client::breathe
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::breathe(const Protocol::Address &addr, const BreatheEffect &effect)
{
   Protocol::Message message(effect.size());

   effect.pack(message.payload);

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SIMPLE_VISUAL_EFFECTS;
   message.itf.member = BREATHE_CMD;

   send(addr, message);
}
#endif
