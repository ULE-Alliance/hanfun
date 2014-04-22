// =============================================================================
/*!
 * \file       /HAN-FUN/src/devices.cpp
 *
 * This file contains the implementation of the common functionality for HAN-FUN
 * devices.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
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
// AbstractDevice::unit
// =============================================================================
/*!
 *
 */
// =============================================================================
Units::IUnit *AbstractDevice::unit (uint8_t id)
{
   if (_units.empty())
   {
      return nullptr;
   }

   /* *INDENT-OFF* */
   auto it = find_if(_units.begin(), _units.end(), [id](Units::IUnit *unit)
   {
      return unit->id () == id;
   });
   /* *INDENT-ON* */

   return *it;
}

// =============================================================================
// AbstractDevice::send
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractDevice::send (Protocol::Packet &packet)
{
   Transport::Link *tsp_link = packet.link;

   if (tsp_link == nullptr)
   {
      tsp_link = link (packet.destination.device);
   }

   if (tsp_link != nullptr)
   {
      Common::ByteArray array (packet.size ());

      packet.pack (array);

      tsp_link->send (array);
   }
}

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
