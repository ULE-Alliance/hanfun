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
#include "hanfun/units.h"

#include "hanfun/core/device_management.h"

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
   Message::Address addr;
   Message message;

   message.itf.role = role();
   message.itf.uid = uid();
   message.itf.member = REGISTER_CMD;

   RegisterMessage * payload = new RegisterMessage( _device->info()->emc(),
                                                    _device->info()->device_uid());

   for( auto dev_unit : _device->units() )
   {
      Unit unit;
      unit.id = dev_unit->id();
      unit.profile = dev_unit->uid();

      payload->units.push_back(unit);
   }

   message.payload = payload;

   sendMessage( addr, message );
}

// =============================================================================
// DeviceManagementClient::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
bool DeviceManagementClient::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   RegisterResponse registration;

   size_t payload_size = 0;

   switch (message.itf.member)
   {
      case REGISTER_CMD:
         payload_size = registration.size();
         break;
      default:
         payload_size = numeric_limits<size_t>::max();
         break;
   }

   if( !AbstractInterface::handle( message, payload, offset, payload_size ) )
   {
      return false;
   }

   switch (message.itf.member)
   {
      case REGISTER_CMD:
         registration.unpack( payload, offset );
         registered(registration);
         break;
      default:
         payload_size = numeric_limits<size_t>::max();
         break;
   }

   return true;
}

// =============================================================================
// DeviceManagementClient::registered
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagementClient::registered( RegisterResponse &response)
{
   if( response.code == Response::OK )
   {
      _device->address(response.address);
   }
}
