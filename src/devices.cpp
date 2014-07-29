// =============================================================================
/*!
 * \file       src/devices.cpp
 *
 * This file contains the implementation of the common functionality for HAN-FUN
 * devices.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
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
   if (_units.empty ())
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

   // Update message reference.
   if (packet.source.device == address ())
   {
      packet.message.reference = this->next_reference++;
   }

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
void AbstractDevice::receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   Common::Result result = Common::Result::FAIL_UNKNOWN;

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

// =============================================================================
// HF::Devices::Node
// =============================================================================

// =============================================================================
// HF::Devices::Concentrator
// =============================================================================

// =============================================================================
// HF::Devices::Concentrator::Transport
// =============================================================================

void HF::Devices::Concentrator::Transport::destroy ()
{
   remove (nullptr);
}

// =============================================================================
// HF::Devices::Concentrator::Transport::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void HF::Devices::Concentrator::Transport::remove (HF::Transport::Link *link)
{
   if (link != nullptr)
   {
      /* *INDENT-OFF* */
      std::for_each(endpoints.begin(), endpoints.end(), [link](HF::Transport::Endpoint *ep)
      {
         ep->disconnected (link);
      });
      /* *INDENT-ON* */

      links.remove (link);

      delete link;
   }
   else
   {
      /* *INDENT-OFF* */
      std::for_each (links.begin (), links.end (), [this](HF::Transport::Link *link)
      {
         std::for_each(endpoints.begin(), endpoints.end(), [link](HF::Transport::Endpoint *ep)
         {
            ep->disconnected (link);
         });

         delete link;
      });
      /* *INDENT-ON* */

      links.clear ();
   }
}

// =============================================================================
// HF::Devices::Concentrator::Transport::find
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Transport::Link *HF::Devices::Concentrator::Transport::find (uint16_t address)
{
   /* *INDENT-OFF* */
   auto it = std::find_if(links.begin(), links.end(),
                          [address](const HF::Transport::Link *link)
                          {
                             return link->address () == address;
                          });
   /* *INDENT-ON* */

   // Link was found.
   if (it != links.end ())
   {
      return *it;
   }

   return nullptr;
}
