// =============================================================================
/*!
 * @file       src/interfaces/simple_visual_effects.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Visual Effects interface.
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

#include "hanfun/interfaces/simple_visual_effects.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleVisualEffects;

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
      return SimpleVisualEffects::create_attribute(uid);
   }
}

// =============================================================================
// SimpleVisualEffects::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleVisualEffects::create_attribute(uint8_t uid)
{
   UNUSED(uid);
   return nullptr;
}
