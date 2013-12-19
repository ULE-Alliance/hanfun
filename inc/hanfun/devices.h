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

namespace HF
{
   // Forward declaration for IUnit interface.
   class IUnit;

   /*!
    * This class represents the interface that all devices MUST implement.
    */
   struct IDevice
   {
      /*!
       * Return the device address on the HAN-FUN network, when the device is registered,
       * or \c HF_BROADCAST_ADDR otherwise.
       *
       * @return  the device address on the HAN-FUN network,
       *          \c HF_BROADCAST_ADDR otherwise.
       */
      virtual uint16_t address () const = 0;

      /*!
       * Return the list of units registered in this device.
       *
       * @return     vector containing the device's registered units.
       */
      virtual const std::vector <IUnit *> &units () const = 0;

      /*!
       * Return a pointer to the HF::Core::DeviceInformation object associated
       * with the device.
       *
       * @return  pointer to a HF::Core::DeviceInformation.
       */
      virtual DeviceInformation *info () const = 0;
   };








      {

      {

      /*!
       *
       */

      /*!
       *
       */
   };

}  // namespace HF

#endif /* HF_DEVICE_H */
