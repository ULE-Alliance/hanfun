// =============================================================================
/*!
 * @file       src/interfaces/simple_air_pressure.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Air Pressure interface.
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

#include "hanfun/interfaces/simple_air_pressure.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleAirPressure;

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
      return SimpleAirPressure::create_attribute(uid);
   }
}

// =============================================================================
// SimpleAirPressure::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleAirPressure::create_attribute(uint8_t uid)
{
   using namespace HF::Interfaces::SimpleAirPressure;

   SimpleAirPressure::Attributes attr = static_cast<SimpleAirPressure::Attributes>(uid);

   switch (attr)
   {
      case VALUE_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, attr,
                                                        Value::WRITABLE);
      }

      case MAXIMUM_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, attr,
                                                        Maximum::WRITABLE);
      }

      case MINIMUM_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, attr,
                                                        Minimum::WRITABLE);
      }

      case TOLERANCE_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, attr,
                                                        Tolerance::WRITABLE);
      }

      default:
         return nullptr;
   }
}
