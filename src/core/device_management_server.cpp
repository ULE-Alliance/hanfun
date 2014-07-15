// =============================================================================
/*!
 * \file       src/core/device_management_server.cpp
 *
 * This file contains the implementation of the Device Management : Server Role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#include <algorithm>

#include "hanfun/core/device_management.h"
#include "hanfun/devices.h"

using namespace HF;
using namespace HF::Core;

// =============================================================================
// DeviceManagement::Server API
// =============================================================================

DeviceManagement::Server::Server(HF::Devices::Concentrator::IUnit0 &unit):
   ServiceRole (unit)
{}

// =============================================================================
// BindManagement::Server::unit
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Devices::Concentrator::IUnit0 &DeviceManagement::Server::unit0 ()
{
   return static_cast <HF::Devices::Concentrator::IUnit0 &>(ServiceRole::unit ());
}

// =============================================================================
// DeviceManagemen::tServer::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Server::payload_size (Protocol::Message::Interface &itf) const
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
// DeviceManagement::Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::Server::handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                                         size_t offset)
{
   Common::Result result = AbstractInterface::check_message (packet.message, payload, offset);

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
// DeviceManagementServer::register_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::Server::register_device (Protocol::Packet &packet, Common::ByteArray &payload,
                                                          size_t offset)
{
   Common::Result result = AbstractInterface::check_payload_size (packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   RegisterMessage reg_msg;

   offset += reg_msg.unpack (payload, offset);

   uint16_t address        = Protocol::BROADCAST_ADDR;

   HF::UID::UID const *uid = packet.link->uid ();
   Device *device          = entry (uid);

   if (device == nullptr)
   {
      device          = new Device ();
      device->uid     = uid->clone ();
      device->emc     = reg_msg.emc;
      device->units   = reg_msg.units;
      device->address = next_address ();
   }

   RegisterResponse *reg_res = new RegisterResponse ();

   reg_res->emc     = DeviceInformation::EMC;

   reg_res->code    = save (device);

   address          = (reg_res->code == Common::Result::OK ? device->address : Protocol::BROADCAST_ADDR);

   reg_res->address = address;

   packet.link->address (reg_res->address);

   Protocol::Message response (packet.message, reg_res->size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::Server::uid ();
   response.itf.member = REGISTER_CMD;

   reg_res->pack (response.payload);

   Protocol::Address res_addr (address, 0);

   sendMessage (res_addr, response, packet.link);

   delete reg_res;

   return Common::Result::OK;
}

// =============================================================================
// DeviceManagementServer::deregister_device
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::Server::deregister_device (Protocol::Packet &packet, Common::ByteArray &payload,
                                                            size_t offset)
{
   Common::Result result = AbstractInterface::check_payload_size (packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   Device *source = entry (packet.source.device);

   if (source == nullptr)
   {
      return Common::Result::FAIL_AUTH;
   }

   DeregisterMessage incomming;

   offset += incomming.unpack (payload, offset);

   Device *destination = entry (incomming.address);

   if (destination == nullptr)
   {
      return Common::Result::FAIL_ARG;
   }

   if (!authorized (DEREGISTER_CMD, source, destination))
   {
      return Common::Result::FAIL_AUTH;
   }

   result = deregister (*destination);

   Protocol::Response res (result);

   Protocol::Message  response (packet.message, res.size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = DeviceManagement::Server::uid ();
   response.itf.member = DEREGISTER_CMD;

   res.pack (response.payload);

   Protocol::Address res_addr (packet.source.device, 0);

   sendMessage (res_addr, response);

   return Common::Result::OK;
}

// =============================================================================
// DeviceManagement::Server::deregister
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result DeviceManagement::Server::deregister (Device &device)
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
   _addr2device.clear ();
   _uid2device.clear ();

   /* *INDENT-OFF* */
   for_each (_entries.begin (), _entries.end (), [](Device *dev) { delete dev; });
   /* *INDENT-ON* */
}

// =============================================================================
// DeviceManagement::DefaultServer::entries
// =============================================================================
/*!
 *
 */
// =============================================================================
vector <DeviceManagement::Device *> DeviceManagement::DefaultServer::entries (uint16_t offset, uint16_t count)
{
   vector <Device *>::iterator start = _entries.begin ();

   start += offset;

   count  = min (static_cast <int>(count), static_cast <int>(_entries.size () - offset));

   vector <Device *>::iterator end = start + count;

   return vector <Device *>(start, end);
}

// =============================================================================
// DeviceManagement::DefaultServer::entry
// =============================================================================
/*!
 *
 */
// =============================================================================
DeviceManagement::Device *DeviceManagement::DefaultServer::entry (uint16_t address)
{
   return (_addr2device.count (address) != 0 ? _addr2device.at (address) : nullptr);
}

// =============================================================================
// DefaultDeviceManagementServer::entry
// =============================================================================
/*!
 *
 */
// =============================================================================
DeviceManagement::Device *DeviceManagement::DefaultServer::entry (HF::UID::UID const *uid)
{
   return (_uid2device.count (uid) != 0 ? _uid2device.at (uid) : nullptr);
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
   if (_uid2device.count (device->uid) == 0)
   {
      _entries.push_back (device);
      _addr2device[device->address] = device;
      _uid2device[device->uid]      = device;
   }

   return Common::Result::OK;
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

   if (_addr2device.count (device->address) == 0 ||
       _uid2device.count (device->uid) == 0)
   {
      return Common::Result::FAIL_ARG;
   }

   vector <DeviceManagement::Device *>::iterator it = find (_entries.begin (), _entries.end (), device);

   if (it == _entries.end ())
   {
      return Common::Result::FAIL_ARG;
   }

   DeviceManagement::Device *entry = *it;

   _entries.erase (it);
   _addr2device.erase (device->address);
   _uid2device.erase (device->uid);

   delete entry;

   return Common::Result::OK;
}

// =============================================================================
// DefaultDeviceManagementServer::authorized
// =============================================================================
/*!
 *
 */
// =============================================================================
bool DeviceManagement::DefaultServer::authorized (uint8_t member, DeviceManagement::Device *source, DeviceManagement::Device *destination)
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
