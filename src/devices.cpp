// =============================================================================
/*!
 * \file       src/devices.cpp
 *
 * This file contains the implementation of the common functionality for HAN-FUN
 * devices.
 *
 * \version    1.0.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
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
Units::IUnit *AbstractDevice::unit (uint8_t id) const
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

   if (it == _units.end ())
   {
      return nullptr;
   }
   else
   {
      return *it;
   }
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

   response_filter (packet);
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

   if (is_local (packet) && !repeated_filter (packet, payload))
   {
      IUnit *unit = this->unit (packet.destination.unit);

      if (unit != nullptr)
      {
         result = unit->handle (packet, payload, offset);
      }
   }

   // Send missing response.
   if (response_filter (packet))
   {
      Protocol::Packet *resp_packet = new Protocol::Packet (packet.message);
      resp_packet->link = packet.link;

      Protocol::Response resp (result);
      resp_packet->message.payload = Common::ByteArray (resp.size ());
      resp.pack (resp_packet->message.payload);

      send (*resp_packet);

      delete resp_packet;
   }
}

// =============================================================================
// HF::Devices::Node
// =============================================================================

// =============================================================================
// HF::Devices::Concentrator
// =============================================================================

// =============================================================================
// HF::Devices::Concentrator::AbstractBase
// =============================================================================

// =============================================================================
// Concentrator::AbstractBase::connected
// =============================================================================
/*!
 *
 */
// =============================================================================
void Concentrator::AbstractBase::connected (HF::Transport::Link *link)
{
   _links.push_front (link);

   // Check if a registration exists for this link.
   auto entry = unit0()->device_management ()->entry (link->uid ());

   if (entry != nullptr)
   {
      link->address (entry->address);
   }
}

// =============================================================================
// Concentrator::AbstractBase::disconnected
// =============================================================================
/*!
 *
 */
// =============================================================================
void Concentrator::AbstractBase::disconnected (HF::Transport::Link *link)
{
   _links.remove (link);
}

// =============================================================================
// Concentrator::AbstractBase::receive
// =============================================================================
/*!
 *
 */
// =============================================================================
void Concentrator::AbstractBase::receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   if (packet.destination.device == Protocol::BROADCAST_ADDR)
   {
      route_packet (packet, payload, offset);
   }
   else
   {
      AbstractDevice::receive (packet, payload, offset);
   }
}

// =============================================================================
// Concentrator::AbstractBase::link
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Transport::Link *Concentrator::AbstractBase::link (uint16_t addr) const
{
   if (_links.empty ())
   {
      return nullptr;
   }

   /* *INDENT-OFF* */
   auto it = std::find_if(_links.begin(), _links.end(),
                          [addr](HF::Transport::Link *link)
                          {
                             return link->address () == addr;
                          });
   /* *INDENT-ON* */

   if (it == _links.end ())
   {
      return nullptr;
   }

   return *it;
}

// =============================================================================
// Concentrator::AbstractBase::route_packet
// =============================================================================
/*!
 *
 */
// =============================================================================
void Concentrator::AbstractBase::route_packet (Protocol::Packet &packet, Common::ByteArray &payload,
                           size_t offset)
{
   auto &entries = unit0()->bind_management ()->entries;

   auto range    = entries.find (packet.source, packet.message.itf);

   // No bindings found !
   if (range.first == entries.end () && range.second == entries.end ())
   {
      return;
   }

   Protocol::Packet other = packet;

   other.message.payload.reserve (payload.size () - offset);

   std::copy (payload.begin () + offset, payload.end (), other.message.payload.begin ());

   /* *INDENT-OFF* */
   std::for_each(range.first, range.second,
                 [this, &other](const Core::BindManagement::Entry &entry)
                 {
                    other.link = nullptr;
                    other.destination = entry.destination;
                    this->send (other);
                 });
   /* *INDENT-ON* */
}

// =============================================================================
// HF::Devices::Concentrator::Transport
// =============================================================================

void Concentrator::Transport::destroy ()
{
   remove ((HF::Transport::Endpoint *) nullptr);
   remove ((HF::Transport::Link *) nullptr);
}

// =============================================================================
// HF::Devices::Concentrator::Transport::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void Concentrator::Transport::remove (HF::Transport::Link *link)
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
HF::Transport::Link *Concentrator::Transport::find (uint16_t address)
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
