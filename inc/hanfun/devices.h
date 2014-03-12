// =============================================================================
/*!
 * \file       inc/hanfun/devices.h
 *
 * This file contains the definitions for the devices in a HAN-FUN network.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_DEVICE_H
#define HF_DEVICE_H

#include <cstring>

#include <string>
#include <vector>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"

namespace HF
{
   /*!
    * This class provides the basic implementation for the Device API.
    */
   template<typename DeviceInfo>
   struct AbstractDevice:public IDevice
   {
      DeviceInfo info;

      static_assert (is_base_of <HF::Core::DeviceInformation, DeviceInfo>::value,
                     "DeviceInfo must be of type HF::Core::DeviceInformation");

      // =============================================================================
      // IDevice API
      // =============================================================================

      const units_t &units () const
      {
         return _units;
      }

      /*!
       * Add unit to devices unit lists.
       *
       * @param unit    pointer to the unit to add to the list.
       */
      void add (IUnit *unit)
      {
         _units.push_front (unit);
      }

      /*!
       * Remove unit from device's unit list.
       *
       * @param unit    pointer to the unit to remove from the list.
       */
      void remove (IUnit *unit)
      {
         _units.remove (unit);
      }

      protected:

      //! List containing pointers to the units present in the device.
      units_t _units;

      AbstractDevice():info (DeviceInfo (this)) {}
   };

   /*!
    * Template for HAN-FUN devices.
    */
   template<typename DeviceInfo = HF::Core::DefaultDeviceInformation,
            typename DeviceMgt  = HF::Core::DeviceManagementClient>
   struct Device:public AbstractDevice <DeviceInfo>, public Transport::Endpoint
   {
      DeviceMgt management;

      static_assert (is_base_of <HF::Core::DeviceManagementClient, DeviceMgt>::value,
                     "DeviceMgt must be of type HF::Core::DeviceInformationClient");

      uint16_t address () const
      {
         return management.address ();
      }

      protected:

      Device():management (DeviceMgt (this)) {}
   };

   /*!
    * HAN-FUN Concentrator implementation.
    */
   template<typename DeviceInfo = HF::Core::DefaultDeviceInformation,
            typename DeviceMgt = HF::Core::DefaultDeviceManagementServer>
   struct Concentrator:public AbstractDevice <DeviceInfo>, public Transport::Endpoint
   {
      DeviceMgt management;

      static_assert (std::is_base_of <HF::Core::DeviceManagementServer, DeviceMgt>::value,
                     "DeviceMgt must be of type HF::Core::DeviceInformationServer");

      uint16_t address () const
      {
         return 0;
      }

      protected:

      Concentrator():management (DeviceMgt (this)) {}
   };

}  // namespace HF

#endif /* HF_DEVICE_H */
