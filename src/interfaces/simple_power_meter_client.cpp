// =============================================================================
/*!
 * \file       /HAN-FUN/src/interfaces/simple_power_meter_client.cpp
 *
 * This file contains the implementation of the Simple Power Meter interface :
 * Client role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
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
// SimplePowerMeterClient::handle_command
// =============================================================================
/*!
 * TODO This needs more validation when reading a report received.
 */
// =============================================================================
Common::Result SimplePowerMeter::Client::handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   if (packet.message.itf.member != SimplePowerMeter::REPORT_CMD)
   {
      return Common::Result::FAIL_SUPPORT;
   }

   SimplePowerMeter::Report report;

   offset += report.unpack (payload, offset);

   this->report (report);

   return Common::Result::OK;
}
