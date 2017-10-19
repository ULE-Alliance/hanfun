// =============================================================================
/*!
 * @file       src/core/time_server.cpp
 *
 * This file contains the implementation of the Time service : Server role.
 *
 * @version    1.5.0
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
// Time Interface : Server Role
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
   UNUSED(pack_id);

   return HF::Attributes::UIDS({TIME_ATTR});
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
   using namespace HF::Core::Time;

   Time::Attributes attr = static_cast<Time::Attributes>(uid);

   switch (attr)
   {
      case TIME_ATTR:
      {
         typedef HF::Attributes::Attribute<uint32_t, Server> Attribute;

         auto getter = (uint32_t (Server::*)(void) const) & Server::time;
         auto setter = (void (Server::*)(uint32_t)) & Server::time;

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
// Server::time
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Server::time() const
{
   return _time;
}

// =============================================================================
// Server::time
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::time(uint32_t __value)
{
   HF_SETTER_HELPER(Value, _time, __value);
}
