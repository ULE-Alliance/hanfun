// =============================================================================
/*!
 * @file       src/interfaces/simple_light_sensor_server.cpp
 *
 * This file contains the implementation of the Simple Light Sensor interface : Server role.
 *
 * @version    1.5.0
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
// Simple Light Sensor Interface : Server Role
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
   HF::Attributes::UIDS result({VALUE_ATTR, MINIMUM_ATTR, MAXIMUM_ATTR, TOLERANCE_ATTR});

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
   using namespace HF::Interfaces::SimpleLightSensor;

   SimpleLightSensor::Attributes attr = static_cast<SimpleLightSensor::Attributes>(uid);

   switch (attr)
   {
      case VALUE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint32_t, Server> Attribute;

         auto getter = (uint32_t (Server::*)(void) const) & Server::value;
         auto setter = (void (Server::*)(uint32_t)) & Server::value;

         return new Attribute(*this, attr, getter, setter, Value::WRITABLE);
      }

      case MINIMUM_ATTR:
      {
         typedef HF::Attributes::Attribute<uint32_t, Server> Attribute;

         auto getter = (uint32_t (Server::*)(void) const) & Server::minimum;
         auto setter = (void (Server::*)(uint32_t)) & Server::minimum;

         return new Attribute(*this, attr, getter, setter, Minimum::WRITABLE);
      }

      case MAXIMUM_ATTR:
      {
         typedef HF::Attributes::Attribute<uint32_t, Server> Attribute;

         auto getter = (uint32_t (Server::*)(void) const) & Server::maximum;
         auto setter = (void (Server::*)(uint32_t)) & Server::maximum;

         return new Attribute(*this, attr, getter, setter, Maximum::WRITABLE);
      }

      case TOLERANCE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint32_t, Server> Attribute;

         auto getter = (uint32_t (Server::*)(void) const) & Server::tolerance;
         auto setter = (void (Server::*)(uint32_t)) & Server::tolerance;

         return new Attribute(*this, attr, getter, setter, Tolerance::WRITABLE);
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
uint32_t Server::value() const
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
void Server::value(uint32_t __value)
{
   HF_SETTER_HELPER(Value, _value, __value);
}

// =============================================================================
// Server::minimum
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Server::minimum() const
{
   return _minimum;
}

// =============================================================================
// Server::minimum
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::minimum(uint32_t __value)
{
   HF_SETTER_HELPER(Minimum, _minimum, __value);
}

// =============================================================================
// Server::maximum
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Server::maximum() const
{
   return _maximum;
}

// =============================================================================
// Server::maximum
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::maximum(uint32_t __value)
{
   HF_SETTER_HELPER(Maximum, _maximum, __value);
}

// =============================================================================
// Server::tolerance
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Server::tolerance() const
{
   return _tolerance;
}

// =============================================================================
// Server::tolerance
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::tolerance(uint32_t __value)
{
   HF_SETTER_HELPER(Tolerance, _tolerance, __value);
}
