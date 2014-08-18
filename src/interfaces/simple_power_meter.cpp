// =============================================================================
/*!
 * \file       src/interfaces/simple_power_meter.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Power Meter interface.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include <cstring>

#include "hanfun/interfaces/simple_power_meter.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// SimplePowerMeter / Report
// =============================================================================

SimplePowerMeter::Report::Report()
{
   memset (&energy, 0, sizeof(Measurement));          // Energy measurement.

   memset (&last_energy, 0, sizeof(Measurement));     // Energy measurement at last reset.
   memset (&last_time, 0, sizeof(Measurement));       // Device time measurement at last reset.

   memset (&power, 0, sizeof(Measurement));           // Instantaneous Power measurement.
   memset (&avg_power, 0, sizeof(Measurement));       // Average Power measurement.

   avg_power_interval = 0;                              // Average Power Interval.

   power_factor       = 0;                              // Power Factor.

   memset (&voltage, 0, sizeof(Measurement));         // Voltage measurement.
   memset (&current, 0, sizeof(Measurement));         // Current measurement.
   memset (&frequency, 0, sizeof(Measurement));       // Frequency measurement.

   std::fill (enabled.begin (), enabled.end (), false);
}

// =============================================================================
// SimplePowerMeter::Report::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t SimplePowerMeter::Report::size () const
{
   size_t result = sizeof(uint8_t);           // Number of attributes.

   if (enabled[SimplePowerMeter::ENERGY_ATTR])
   {
      result += (sizeof(uint8_t) + energy.size ()); // Energy measurement.
   }

   if (enabled[SimplePowerMeter::ENERGY_AT_RESET_ATTR])
   {
      result += (sizeof(uint8_t) + last_energy.size ()); // Energy measurement at last reset.
   }

   if (enabled[SimplePowerMeter::TIME_AT_RESET_ATTR])
   {
      result += (sizeof(uint8_t) + last_time.size ());   // Device time measurement at last reset.
   }

   if (enabled[SimplePowerMeter::POWER_ATTR])
   {
      result += (sizeof(uint8_t) + power.size ());     // Instantaneous Power measurement.
   }

   if (enabled[SimplePowerMeter::AVG_POWER_ATTR])
   {
      result += (sizeof(uint8_t) + avg_power.size ()); // Average Power measurement.
   }

   if (enabled[SimplePowerMeter::AVG_POWER_INTERVAL_ATTR])
   {
      result += (sizeof(uint8_t) + sizeof(avg_power_interval)); // Average Power Interval.
   }

   if (enabled[SimplePowerMeter::POWER_FACTOR_ATTR])
   {
      result += (sizeof(uint8_t) + sizeof(power_factor)); // Power Factor.
   }

   if (enabled[SimplePowerMeter::VOLTAGE_ATTR])
   {
      result += (sizeof(uint8_t) + voltage.size ()); // Voltage measurement.
   }

   if (enabled[SimplePowerMeter::CURRENT_ATTR])
   {
      result += (sizeof(uint8_t) + current.size ()); // Current measurement.
   }

   if (enabled[SimplePowerMeter::FREQUENCY_ATTR])
   {
      result += (sizeof(uint8_t) + frequency.size ()); // Frequency measurement.
   }

   return result;
}

// =============================================================================
// SimplePowerMeter::Report::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t SimplePowerMeter::Report::pack (Common::ByteArray &array, size_t offset) const
{
   size_t  start = offset;

   uint8_t id    = 0;

   // Number of attributes.

   id      = std::count (enabled.begin (), enabled.end (), true);
   offset += array.write (offset, id);

   // Energy measurement.
   id = static_cast <uint8_t>(SimplePowerMeter::ENERGY_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += energy.pack (array, offset);
   }

   // Energy measurement at last reset.
   id = static_cast <uint8_t>(SimplePowerMeter::ENERGY_AT_RESET_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += last_energy.pack (array, offset);
   }

   // Device time measurement at last reset.
   id = static_cast <uint8_t>(SimplePowerMeter::TIME_AT_RESET_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += last_time.pack (array, offset);
   }

   // Instantaneous Power measurement.
   if (enabled[id])
   {
      id      = static_cast <uint8_t>(SimplePowerMeter::POWER_ATTR);
      offset += array.write (offset, id);
      offset += power.pack (array, offset);
   }

   // Average Power measurement.
   if (enabled[id])
   {
      id      = static_cast <uint8_t>(SimplePowerMeter::AVG_POWER_ATTR);
      offset += array.write (offset, id);
      offset += avg_power.pack (array, offset);
   }

   // Average Power Interval.
   if (enabled[id])
   {
      id      = static_cast <uint8_t>(SimplePowerMeter::AVG_POWER_INTERVAL_ATTR);
      offset += array.write (offset, id);
      offset += array.write (offset, avg_power_interval);
   }

   // Power Factor.
   id = static_cast <uint8_t>(SimplePowerMeter::POWER_FACTOR_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += array.write (offset, power_factor);
   }

   // Voltage measurement.
   id = static_cast <uint8_t>(SimplePowerMeter::VOLTAGE_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += voltage.pack (array, offset);
   }

   // Current measurement.
   id = static_cast <uint8_t>(SimplePowerMeter::CURRENT_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += current.pack (array, offset);
   }

   // Frequency measurement.
   id = static_cast <uint8_t>(SimplePowerMeter::FREQUENCY_ATTR);

   if (enabled[id])
   {
      offset += array.write (offset, id);
      offset += frequency.pack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// SimplePowerMeter::Report::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t SimplePowerMeter::Report::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t  start      = offset;

   uint8_t attr_count = 0;

   offset += array.read (offset, attr_count);  // TODO Should check attribute count.

   for (uint8_t i = 0; i < attr_count; i++)
   {
      uint8_t id = 0;
      offset += array.read (offset, id);

      if (id < ENERGY_ATTR || id > __LAST_ATTR__)
      {
         break;
      }

      enabled[id] = true;

      switch (id)
      {
         case ENERGY_ATTR:   // Energy Attribute ID.
            offset += energy.unpack (array, offset);
            break;
         case ENERGY_AT_RESET_ATTR:   // Energy at Last Reset Attribute ID.
            offset += last_energy.unpack (array, offset);
            break;
         case TIME_AT_RESET_ATTR:   // Time at Last Reset Attribute ID.
            offset += last_time.unpack (array, offset);
            break;
         case POWER_ATTR:   // Instantaneous Power Attribute ID.
            offset += power.unpack (array, offset);
            break;
         case AVG_POWER_ATTR:   // Average Power Attribute ID.
            offset += avg_power.unpack (array, offset);
            break;
         case AVG_POWER_INTERVAL_ATTR:   // Average Power Interval Attribute ID.
            offset += array.read (offset, avg_power_interval);
            break;
         case VOLTAGE_ATTR:   // Voltage Attribute ID.
            offset += voltage.unpack (array, offset);
            break;
         case CURRENT_ATTR:   // Current Attribute ID.
            offset += current.unpack (array, offset);
            break;
         case FREQUENCY_ATTR:   // Frequency Attribute ID.
            offset += frequency.unpack (array, offset);
            break;
         case POWER_FACTOR_ATTR:   // Power Factor Attribute ID.
            offset += array.read (offset, power_factor);
            break;
         default:
            break;
      }
   }

   return offset - start;
}

// =============================================================================
// create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimplePowerMeter::create_attribute (uint8_t uid)
{
   return Interfaces::create_attribute ((SimplePowerMeter::Server *) nullptr, uid);
}
