// =============================================================================
/*!
 * @file       src/core/rssi.cpp
 *
 * This file contains the implementation of the common functionality for the
 * RSSI interface.
 *
 * @version    1.5.1
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/rssi.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::RSSI;

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
      return RSSI::create_attribute(uid);
   }
}

// =============================================================================
// RSSI::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *RSSI::create_attribute(uint8_t uid)
{
   using namespace HF::Core::RSSI;

   RSSI::Attributes attr = static_cast<RSSI::Attributes>(uid);

   switch (attr)
   {
      case RSSI_ATTR:
      {
         return new HF::Attributes::Attribute<int8_t>(HF::Interface::RSSI, attr, Value::WRITABLE);
      }

      default:
         return nullptr;
   }
}
