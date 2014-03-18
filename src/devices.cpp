// =============================================================================
/*!
 * \file       /HAN-FUN/src/devices.cpp
 *
 * This file contains the implementation of the common functionality for HAN-FUN
 * devices.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"

#include "hanfun/protocol.h"

#include "hanfun/devices.h"

using namespace HF;
using namespace HF::Units;
using namespace HF::Devices;

// =============================================================================
// HF::AbstractDevice
// =============================================================================

// =============================================================================
// HF::AbstractDevice::receive
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractDevice::receive (Protocol::Packet &packet, ByteArray &payload, size_t offset)
{
   Result result = Result::FAIL_UNKNOWN;

   if (!repeated (packet) && is_local (packet))
   {
      IUnit *unit = this->unit (packet.destination.unit);

      if (unit != nullptr)
      {
         result = unit->handle (packet, payload, offset);
      }
   }

   respond (result, packet);
}
