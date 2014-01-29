// =============================================================================
/*!
 * \file       src/core/device_management_client.cpp
 *
 * This file contains the implementation of the Device Management : Client Role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <limits>

#include "hanfun/profiles.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"

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
void DeviceManagementClient::register_device ()
{
   Protocol::Address addr;
   Protocol::Message message;

   message.itf.role   = role ();
   message.itf.uid    = uid ();
   message.itf.member = REGISTER_CMD;

   RegisterMessage *payload = new RegisterMessage (DeviceInformation::EMC);

   for (std::vector <IUnit *>::const_iterator dev_unit = _device->units ().begin ();
        dev_unit != _device->units ().end (); ++dev_unit)
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
// DeviceManagementClient::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
Result DeviceManagementClient::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   RegisterResponse registration;

   size_t payload_size = 0;

   switch (message.itf.member)
   {
      case REGISTER_CMD:
         payload_size = registration.size ();
         break;
      default:
         return Result::FAIL_SUPPORT;
   }

   Result result = AbstractInterface::handle (message, payload, offset, payload_size);

   if (result != Result::OK)
   {
      return result;
   }

   switch (message.itf.member)
   {
      case REGISTER_CMD:
         registration.unpack (payload, offset);
         registered (registration);
         break;
      default:
         break;
   }

   return Result::OK;
}

// =============================================================================
// DeviceManagementClient::registered
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagementClient::registered (RegisterResponse &response)
{
   if (response.code == Result::OK)
   {
      this->_address = response.address;
   }
}
