// =============================================================================
/*!
 * \file       src/interfaces/simple_power_meter_client.cpp
 *
 * This file contains the implementation of the Simple Power Meter interface :
 * Client role.
 *
 * \version    1.0.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
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
