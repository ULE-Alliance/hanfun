// =============================================================================
/*!
 * \file       /HAN-FUN/src/interfaces/simple_power_meter_client.cpp
 *
 * This file contains the implementation of the Simple Power Meter interface :
 * Client role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/simple_power_meter.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// SimplePowerMeterClient
// =============================================================================

// =============================================================================
// SimplePowerMeterClient::handle
// =============================================================================
/*!
 * TODO This needs more validation when reading a report received.
 */
// =============================================================================
bool SimplePowerMeterClient::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   if (!AbstractInterface::handle (message, payload, offset) ||
       message.itf.member != SimplePowerMeter::REPORT_CMD)
   {
      return false;
   }

   SimplePowerMeter::Report report;

   offset += report.unpack (payload, offset);

   this->report (report);

   return true;
}
