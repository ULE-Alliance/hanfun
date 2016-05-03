// =============================================================================
/*!
 * @file       src/core/time.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Time service.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/time.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Time;

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return Time::create_attribute(uid);
   }
}

// =============================================================================
// Time::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Time::create_attribute(uint8_t uid)
{
   using namespace HF::Core::Time;

   Time::Attributes attr = static_cast<Time::Attributes>(uid);

   switch (attr)
   {
      case TIME_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::TIME, attr, Value::WRITABLE);
      }

      default:
         return nullptr;
   }
}
