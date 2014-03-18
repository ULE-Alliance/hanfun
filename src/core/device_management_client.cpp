// =============================================================================
/*!
 * \file       src/core/device_management_client.cpp
 *
 * This file contains the implementation of the Device Management : Client Role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <limits>

#include "hanfun/profiles.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"

using namespace HF;
using namespace HF::Core;

// =============================================================================
// DeviceManagementClient
// =============================================================================

// =============================================================================
// DeviceManagementClient::register_device
// =============================================================================
/*!
 * TODO Add support for optional interfaces.
 */
// =============================================================================
void DeviceManagement::Client::register_device ()
{
   Protocol::Address addr (0, 0);
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = REGISTER_CMD;

   RegisterMessage *payload = new RegisterMessage (DeviceInformation::EMC);

   for (IDevice::units_t::const_iterator dev_unit = device ()->units ().begin ();
        dev_unit != device ()->units ().end (); ++dev_unit)
   {
      Unit unit;
      unit.id      = (*dev_unit)->id ();
      unit.profile = (*dev_unit)->uid ();

      payload->units.push_back (unit);
   }

   message.payload = payload;

   sendMessage (addr, message);
}

// =============================================================================
// DeviceManagement::Client::deregister
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagement::Client::deregister (uint16_t address)
{
   Protocol::Address addr (0, 0);
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = DEREGISTER_CMD;

   DeregisterMessage *payload = new DeregisterMessage (address);

   message.payload = payload;

   sendMessage (addr, message);
}

// =============================================================================
// DeviceManagement::Client::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Client::payload_size (Protocol::Message::Interface &itf) const
{
   switch (itf.member)
   {
      case REGISTER_CMD:
         return payload_size_helper <RegisterResponse>();

      case DEREGISTER_CMD:
         return payload_size_helper <Protocol::Response>();

      default:
         return 0;
   }
}

// =============================================================================
// DeviceManagement::Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Result DeviceManagement::Client::handle_command (Protocol::Packet &packet, ByteArray &payload,
                                                 size_t offset)
{
   switch (packet.message.itf.member)
   {
      case REGISTER_CMD:
      {
         RegisterResponse registration;
         registration.unpack (payload, offset);
         registered (registration);

         break;
      }
      case DEREGISTER_CMD:
      {
         Protocol::Response response;
         response.unpack (payload, offset);
         deregistered (response);

         break;
      }
      default:
         break;
   }

   return Result::OK;
}

// =============================================================================
// DeviceManagement::Client::registered
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagement::Client::registered (RegisterResponse &response)
{
   if (response.code == Result::OK)
   {
      this->_address = response.address;
   }
}

void DeviceManagement::Client::deregistered (Protocol::Response &response)
{
   if (response.code == Result::OK)
   {
      this->_address = Protocol::BROADCAST_ADDR;
   }
}
