// =============================================================================
/*!
 * \file       src/core/device_management_server.cpp
 *
 * This file contains the implementation of the Device Management : Server Role.
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
#include <algorithm>

#include "hanfun/core/device_management.h"
#include "hanfun/devices.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::DeviceManagement;

// =============================================================================
// HF::Core::DeviceManagement::IServer
// =============================================================================

// =============================================================================
// IServer::unit0
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Devices::Concentrator::IUnit0 &IServer::unit0 () const
{
   return static_cast <Devices::Concentrator::IUnit0 &>(ServiceRole::unit ());
}

// =============================================================================
// AbstractServer::entry
// =============================================================================
/*!
 *
 */
// =============================================================================
DevicePtr IServer::entry (uint16_t address) const
{
   // Build Device Management entry for self.
   if (address == unit ().device ().address ())
   {
      Device *device = new Device ();

      device->address = address;
      device->emc     = HF::Core::DeviceInformation::EMC;

      // FIXME Retrive device UID.
      // auto attr = this->unit0().device_info()->attribute(HF::Core::DeviceInformation::UID_ATTR);
      // device->uid = static_cast<HF::Attributes::Attribute<HF::UID::UID> *>(attr)->get();
      // delete attr;

      auto &units = unit ().device ().units ();

      /* *INDENT-OFF* */
      std::for_each(units.begin(), units.end(), [&device](const HF::Units::IUnit *unit)
      {
         device->units.push_back(DeviceManagement::Unit(*unit));
      });
      /* *INDENT-ON* */

      return DevicePtr (device, true);
   }
   else
   {
      return entries ().find (address);
   }
}

// =============================================================================
// HF::Core::DeviceManagement::AbstractServer
// =============================================================================

// =============================================================================
// AbstractServer::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t AbstractServer::payload_size (Protocol::Message::Interface &itf) const
{
   switch (itf.member)
   {
      case REGISTER_CMD:
         return payload_size_helper <RegisterMessage>();

      case DEREGISTER_CMD:
         return payload_size_helper <DeregisterMessage>();

      default:
         return 0;
   }
}

// =============================================================================
// AbstractServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result AbstractServer::handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                               size_t offset)
{
   Common::Result result = AbstractInterface::check (packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   if (packet.link == nullptr)
   {
      return Common::Result::FAIL_ARG;
   }

   switch (packet.message.itf.member)
   {
      case REGISTER_CMD:
         return register_device (packet, payload, offset);

      case DEREGISTER_CMD:
         return deregister_device (packet, payload, offset);

      default:
         return Common::Result::FAIL_SUPPORT;
   }
}

// =============================================================================
// AbstractServer::register_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result AbstractServer::register_device (Protocol::Packet &packet, Common::ByteArray &payload,
                                                size_t offset)
{
   Common::Result result = AbstractInterface::check_payload_size (packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   RegisterMessage reg_msg;

   offset += reg_msg.unpack (payload, offset);

   uint16_t address = Protocol::BROADCAST_ADDR;

   auto _entry      = entry (packet.link->uid ());

   Device device;

   if (_entry == nullptr)
   {
      device.uid     = packet.link->uid ();
      device.address = next_address ();
   }
   else
   {
      device = Device (*_entry);
   }

   device.emc   = reg_msg.emc;
   device.units = reg_msg.units;

   result       = entries ().save (device);
   address      = (result == Common::Result::OK ? device.address : Protocol::BROADCAST_ADDR);

   RegisterResponse *reg_res = new RegisterResponse (address);

   reg_res->emc  = DeviceInformation::EMC;
   reg_res->code = result;

   packet.link->address (address);

   // Update incoming packet source address with new allocated address for device.
   packet.source.device = address;

   Protocol::Message response (packet.message, reg_res->size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::AbstractServer::uid ();
   response.itf.member = REGISTER_CMD;

   reg_res->pack (response.payload);

   Protocol::Address res_addr (address, 0);

   send (res_addr, response, packet.link);

   delete reg_res;

   return result;
}

// =============================================================================
// AbstractServer::deregister_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result AbstractServer::deregister_device (Protocol::Packet &packet, Common::ByteArray &payload,
                                                  size_t offset)
{
   Common::Result result = AbstractInterface::check_payload_size (packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   auto source = entry (packet.source.device);

   if (source == nullptr)
   {
      return Common::Result::FAIL_AUTH;
   }

   DeregisterMessage incomming;

   offset += incomming.unpack (payload, offset);

   auto destination = entry (incomming.address);

   if (destination == nullptr)
   {
      return Common::Result::FAIL_ARG;
   }

   if (!authorized (DEREGISTER_CMD, source, destination))
   {
      return Common::Result::FAIL_AUTH;
   }

   result = deregister (destination);

   DeregisterResponse res (result, incomming.address);

   Protocol::Message  response (packet.message, res.size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::AbstractServer::uid ();
   response.itf.member = DEREGISTER_CMD;

   res.pack (response.payload);

   Protocol::Address res_addr (packet.source.device, 0);

   send (res_addr, response);

   return Common::Result::OK;
}

// =============================================================================
// AbstractServer::deregister
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result AbstractServer::deregister (DevicePtr &device)
{
   // TODO Remove group information.

   unit0 ().bind_management ()->entries ().destroy (device->address);

   auto res = entries ().destroy (device);

   if (res.first == Common::Result::OK)
   {
      this->deregistered (res.second);
   }

   return res.first;
}

// =============================================================================
// DefaultServer::authorized
// =============================================================================
/*!
 *
 */
// =============================================================================
bool AbstractServer::authorized (uint8_t member, DeviceManagement::DevicePtr &source,
                                 DeviceManagement::DevicePtr &destination)
{
   if (source == nullptr || destination == nullptr)
   {
      return false;
   }
   else if (member == DEREGISTER_CMD && source->address != destination->address)
   {
      return false;
   }
   else
   {
      return true;
   }
}

// =============================================================================
// IEntries API - Default Implementation
// =============================================================================

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
DevicePtr Entries::find (uint16_t address) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [address](const Device &device)
   {
      return device.address == address;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return std::move (DeviceManagement::DevicePtr ());
   }
   else
   {
      return std::move (DeviceManagement::DevicePtr (*(it.base ())));
   }
}

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
DevicePtr Entries::find (const HF::UID::UID &uid) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&uid](const Device &device)
   {
      return device.uid == uid;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return std::move (DeviceManagement::DevicePtr ());
   }
   else
   {
      return std::move (DeviceManagement::DevicePtr (*(it.base ())));
   }
}

// =============================================================================
// Entries::save
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::save (Device &device)
{
   if (device.address == HF::Protocol::BROADCAST_ADDR)
   {
      return Common::Result::FAIL_UNKNOWN;
   }

   // Add new entry into the database.

   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&device](const Device &other)
   {
      return device.address == other.address;
   });
   /* *INDENT-ON* */

   if (it != db.end ()) // Update existing entry.
   {
      db.erase (it);
   }

   db.push_back (device);

   return Common::Result::OK;
}

// =============================================================================
// Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
std::pair <Common::Result, DevicePtr> Entries::destroy (DevicePtr &device)
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&device](const Device &other)
   {
      return device->address == other.address;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return std::make_pair (Common::Result::FAIL_ARG, DevicePtr ());
   }

   auto res = std::make_pair (Common::Result::OK, DevicePtr (new Device (*it), true));

   db.erase (it);

   return res;
}

// =============================================================================
// DefaultServer::next_address
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entries::next_address () const
{
   uint16_t address    = DeviceManagement::START_ADDR;

   auto address_equals = [&address](const Device &device)
                         {
                            return device.address == address;
                         };

   while (std::any_of (db.begin (), db.end (), address_equals))
   {
      if (++address == Protocol::BROADCAST_ADDR)
      {
         break;
      }
   }

   return address;
}
