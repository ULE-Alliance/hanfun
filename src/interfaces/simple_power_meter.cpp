// =============================================================================
/*!
 * \file       src/interfaces/alert.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Power Meter interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
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

   result += (sizeof(uint8_t) + energy.size ()); // Energy measurement.

   result += (sizeof(uint8_t) + last_energy.size ()); // Energy measurement at last reset.
   result += (sizeof(uint8_t) + last_time.size ());   // Device time measurement at last reset.

   result += (sizeof(uint8_t) + power.size ());     // Instantaneous Power measurement.
   result += (sizeof(uint8_t) + avg_power.size ()); // Average Power measurement.

   result += (sizeof(uint8_t) + sizeof(avg_power_interval)); // Average Power Interval.

   result += (sizeof(uint8_t) + sizeof(power_factor)); // Power Factor.

   result += (sizeof(uint8_t) + voltage.size ()); // Voltage measurement.
   result += (sizeof(uint8_t) + current.size ()); // Current measurement.

   result += (sizeof(uint8_t) + frequency.size ()); // Frequency measurement.

   return result;
}

// =============================================================================
// SimplePowerMeter::Report::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t SimplePowerMeter::Report::pack (ByteArray &array, size_t offset) const
{
   size_t  start = offset;

   uint8_t id    = 0;

   // Number of attributes.
   id      = 10;
   offset += array.write (offset, id);

   // Energy measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::ENERGY_ATTR);
   offset += array.write (offset, id);
   offset += energy.pack (array, offset);

   // Energy measurement at last reset.
   id      = static_cast <uint8_t>(SimplePowerMeter::ENERGY_AT_RESET_ATTR);
   offset += array.write (offset, id);
   offset += last_energy.pack (array, offset);

   // Device time measurement at last reset.
   id      = static_cast <uint8_t>(SimplePowerMeter::TIME_AT_RESET_ATTR);
   offset += array.write (offset, id);
   offset += last_time.pack (array, offset);

   // Instantaneous Power measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::POWER_ATTR);
   offset += array.write (offset, id);
   offset += power.pack (array, offset);

   // Average Power measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::AVG_POWER_ATTR);
   offset += array.write (offset, id);
   offset += avg_power.pack (array, offset);

   // Average Power Interval.
   id      = static_cast <uint8_t>(SimplePowerMeter::AVG_POWER_INTERVAL_ATTR);
   offset += array.write (offset, id);
   offset += array.write (offset, avg_power_interval);

   // Power Factor.
   id      = static_cast <uint8_t>(SimplePowerMeter::POWER_FACTOR_ATTR);
   offset += array.write (offset, id);
   offset += array.write (offset, power_factor);

   // Voltage measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::VOLTAGE_ATTR);
   offset += array.write (offset, id);
   offset += voltage.pack (array, offset);

   // Current measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::CURRENT_ATTR);
   offset += array.write (offset, id);
   offset += current.pack (array, offset);

   // Frequency measurement.
   id      = static_cast <uint8_t>(SimplePowerMeter::FREQUENCY_ATTR);
   offset += array.write (offset, id);
   offset += frequency.pack (array, offset);

   return offset - start;
}

// =============================================================================
// SimplePowerMeter::Report::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t SimplePowerMeter::Report::unpack (const ByteArray &array, size_t offset)
{
   size_t  start      = offset;

   uint8_t attr_count = 0;

   offset += array.read (offset, attr_count);  // FIXME Should check attribute count.

   for (uint8_t i = 0; i < attr_count; i++)
   {
      uint8_t id = 0;
      offset += array.read (offset, id);

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