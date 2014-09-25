// =============================================================================
/*!
 * \file       src/core/device_management_server.cpp
 *
 * This file contains the implementation of the Device Management : Server Role.
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
#include <algorithm>

#include "hanfun/core/device_management.h"
#include "hanfun/devices.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::DeviceManagement;

// =============================================================================
// DeviceManagement::Server API
// =============================================================================

Server::Server(HF::Devices::Concentrator::IUnit0 &unit):
   ServiceRole (unit)
{}

// =============================================================================
// Server::unit0
// =============================================================================
/*!
 *
 */
// =============================================================================
Devices::Concentrator::IUnit0 &Server::unit0 ()
{
   return static_cast <Devices::Concentrator::IUnit0 &>(ServiceRole::unit ());
}

// =============================================================================
// Server::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Server::payload_size (Protocol::Message::Interface &itf) const
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
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
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
// Server::register_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::register_device (Protocol::Packet &packet, Common::ByteArray &payload,
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

   auto device = entry (packet.link->uid ());

   if (device == nullptr)
   {
      device          = DevicePtr (new Device(), false);
      device->uid     = packet.link->uid ();
      device->address = next_address ();
   }

   device->emc     = reg_msg.emc;
   device->units   = reg_msg.units;

   RegisterResponse *reg_res = new RegisterResponse ();

   reg_res->emc     = DeviceInformation::EMC;

   reg_res->code    = save (&(*device));

   address          = (reg_res->code == Common::Result::OK ? device->address : Protocol::BROADCAST_ADDR);

   reg_res->address = address;

   packet.link->address (reg_res->address);

   Protocol::Message response (packet.message, reg_res->size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::Server::uid ();
   response.itf.member = REGISTER_CMD;

   reg_res->pack (response.payload);

   Protocol::Address res_addr (address, 0);

   send (res_addr, response, packet.link);

   delete reg_res;

   return Common::Result::OK;
}

// =============================================================================
// Server::deregister_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::deregister_device (Protocol::Packet &packet, Common::ByteArray &payload,
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

   result = deregister (*destination);

   DeregisterResponse res (result, incomming.address);

   Protocol::Message  response (packet.message, res.size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::Server::uid ();
   response.itf.member = DEREGISTER_CMD;

   res.pack (response.payload);

   Protocol::Address res_addr (packet.source.device, 0);

   send (res_addr, response);

   return Common::Result::OK;
}

// =============================================================================
// Server::deregister
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::deregister (Device &device)
{
   // TODO Remove group information.

   unit0 ().bind_management ()->entries.destroy (device.address);

   return destroy (&device);
}

// =============================================================================
// DefaultDeviceManagementServer API
// =============================================================================

DeviceManagement::DefaultServer::~DefaultServer()
{
   /* *INDENT-OFF* */
   std::for_each (_entries.begin (), _entries.end (), [](Device *dev) { delete dev; });
   /* *INDENT-ON* */
}

// =============================================================================
// DeviceManagement::DefaultServer::entries
// =============================================================================
/*!
 *
 */
// =============================================================================
std::vector <const DeviceManagement::Device *> DeviceManagement::DefaultServer::entries
   (uint16_t offset, uint16_t count) const
{
   std::vector <Device *>::const_iterator start = _entries.begin ();

   start += offset;

   count  = std::min (static_cast <int>(count), static_cast <int>(_entries.size () - offset));

   std::vector <Device *>::const_iterator end = start + count;

   return std::vector <const Device *>(start, end);
}

// =============================================================================
// DeviceManagement::DefaultServer::entry
// =============================================================================
/*!
 *
 */
// =============================================================================
DeviceManagement::DevicePtr DefaultServer::entry (uint16_t address) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(_entries.begin(), _entries.end(), [address](const Device *device)
   {
      return device->address == address;
   });
   /* *INDENT-ON* */

   if (it == _entries.end ())
   {
      return std::move(DeviceManagement::DevicePtr());
   }
   else
   {
      return std::move(DeviceManagement::DevicePtr(*(it.base())));
   }
}

// =============================================================================
// DefaultDeviceManagementServer::entry
// =============================================================================
/*!
 *
 */
// =============================================================================
DeviceManagement::DevicePtr DefaultServer::entry (const HF::UID::UID &uid) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(_entries.begin(), _entries.end(), [&uid](const Device *device)
   {
      return device->uid == uid;
   });
   /* *INDENT-ON* */

   if (it == _entries.end ())
   {
      return std::move(DeviceManagement::DevicePtr());
   }
   else
   {
      return std::move(DeviceManagement::DevicePtr(*(it.base())));
   }
}

// =============================================================================
// DeviceManagement::DefaultServer::save
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::DefaultServer::save (DeviceManagement::Device *device)
{
   if (device == nullptr)
   {
      return Common::Result::FAIL_UNKNOWN;
   }
   else if (device->address == HF::Protocol::BROADCAST_ADDR)
   {
      return Common::Result::FAIL_UNKNOWN;
   }

   // Add new entry into the database.

   auto other = entry (device->address);

   if (other == nullptr)
   {
      _entries.push_back (device);
      return Common::Result::OK;
   }
   else
   {
      return Common::Result::FAIL_ARG;
   }
}

// =============================================================================
// DeviceManagement::DefaultServer::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::DefaultServer::destroy (DeviceManagement::Device *device)
{
   if (device == nullptr)
   {
      return Common::Result::FAIL_UNKNOWN;
   }

   auto other = entry (device->address);

   if (other == nullptr)
   {
      return Common::Result::FAIL_ARG;
   }

   auto it = std::find (_entries.begin (), _entries.end (), device);

   if (it == _entries.end ())
   {
      return Common::Result::FAIL_ARG;
   }

   DeviceManagement::Device *entry = *it;

   _entries.erase (it);

   delete entry;

   return Common::Result::OK;
}

// =============================================================================
// DefaultServer::authorized
// =============================================================================
/*!
 *
 */
// =============================================================================
bool DefaultServer::authorized (uint8_t member, DeviceManagement::DevicePtr &source,
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
// DefaultServer::next_address
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t DefaultServer::next_address ()
{
   uint16_t result = START_ADDR;

   for (auto device = entry (result); device != nullptr; device = entry (++result))
   {}

   return result;
}
