// =============================================================================
/*!
 * @file       src/interfaces/simple_light_sensor.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Light Sensor interface.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/simple_light_sensor.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleLightSensor;

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
      return SimpleLightSensor::create_attribute(uid);
   }
}

// =============================================================================
// SimpleLightSensor::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleLightSensor::create_attribute(uint8_t uid)
{
   using namespace HF::Interfaces::SimpleLightSensor;

   SimpleLightSensor::Attributes attr = static_cast<SimpleLightSensor::Attributes>(uid);

   switch (attr)
   {
      case VALUE_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, attr,
                                                        Value::WRITABLE);
      }

      case MINIMUM_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, attr,
                                                        Minimum::WRITABLE);
      }

      case MAXIMUM_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, attr,
                                                        Maximum::WRITABLE);
      }

      case TOLERANCE_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, attr,
                                                        Tolerance::WRITABLE);
      }

      default:
         return nullptr;
   }
}
