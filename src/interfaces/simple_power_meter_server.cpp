// =============================================================================
/*!
 * \file       src/interfaces/simple_power_meter_server.cpp
 *
 * This file contains the implementation of the Simple Power Meter interface :
 * Server role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <cmath>
#include <cstring>

#include "hanfun/interfaces/simple_power_meter.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// SimplePowerMeterServer
// =============================================================================

SimplePowerMeterServer::SimplePowerMeterServer()
{
   memset (&_energy, 0, sizeof(Measurement));          // Energy measurement.

   memset (&_last_energy, 0, sizeof(Measurement));     // Energy measurement at last reset.
   memset (&_last_time, 0, sizeof(Measurement));       // Device time measurement at last reset.

   memset (&_power, 0, sizeof(Measurement));           // Instantaneous Power measurement.
   memset (&_avg_power, 0, sizeof(Measurement));       // Average Power measurement.

   _avg_power_interval = 0;                              // Average Power Interval.

   _power_factor       = 0;                              // Power Factor.

   memset (&_voltage, 0, sizeof(Measurement));         // Voltage measurement.
   memset (&_current, 0, sizeof(Measurement));         // Current measurement.
   memset (&_frequency, 0, sizeof(Measurement));       // Frequency measurement.

   _report_interval = 0;

   _last_periodic   = 0;
}

// =============================================================================
// SimplePowerMeterServer::report
// =============================================================================
/*!
 *
 */
// =============================================================================
SimplePowerMeter::Report *SimplePowerMeterServer::report ()
{
   SimplePowerMeter::Report *report = new (nothrow) SimplePowerMeter::Report ();

   if (report != nullptr)
   {
      report->energy      = this->energy ();        // Energy.

      report->last_energy = this->last_energy ();   // Energy at Last Reset.
      report->last_time   = this->last_time ();     // Time at Last Reset.

      report->power       = this->power ();         // Instantaneous Power.
      report->avg_power   = this->avg_power ();     // Average Power.

      // Average Power Interval.
      report->avg_power_interval = this->avg_power_interval ();

      report->voltage            = this->voltage ();   // Voltage.
      report->current            = this->current ();   // Current.
      report->frequency          = this->frequency (); // Frequency.

      report->power_factor       = this->power_factor (); // Power Factor.
   }

   return report;
}

// =============================================================================
// SimplePowerMeterServer::periodic
// =============================================================================
/*!
 *
 */
// =============================================================================
void SimplePowerMeterServer::periodic (uint32_t time)
{
   if (_report_interval > 0 && abs ((int64_t) _last_periodic - time) >= _report_interval)
   {
      Protocol::Address addr;
      Protocol::Message message;

      message.itf.role   = role ();
      message.itf.uid    = uid ();
      message.itf.member = REPORT_CMD;

      message.payload    = report ();

      sendMessage (addr, message);

      _last_periodic = time;
   }
}
