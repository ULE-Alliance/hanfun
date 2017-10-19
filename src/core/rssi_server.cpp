// =============================================================================
/*!
 * @file       src/core/rssi_server.cpp
 *
 * This file contains the implementation of the RSSI interface : Server role.
 *
 * @version    1.5.0
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
// RSSI Interface : Server Role
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
   HF::Attributes::UIDS result({RSSI_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {}

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
   using namespace HF::Core::RSSI;

   RSSI::Attributes attr = static_cast<RSSI::Attributes>(uid);

   switch (attr)
   {
      case RSSI_ATTR:
      {
         typedef HF::Attributes::Attribute<int8_t, Server> Attribute;

         auto getter = (int8_t (Server::*)(void) const) & Server::value;
         auto setter = (void (Server::*)(int8_t)) & Server::value;

         return new Attribute(*this, attr, getter, setter, Value::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::value
// =============================================================================
/*!
 *
 */
// =============================================================================
int8_t Server::value() const
{
   return _value;
}

// =============================================================================
// Server::value
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::value(int8_t __value)
{
   HF_SETTER_HELPER(Value, _value, __value);
}
