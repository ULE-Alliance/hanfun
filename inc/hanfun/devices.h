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

#include "hanfun/core.h"
#include "hanfun/core/device_information.h"

using namespace HF::Core;

namespace HF
{

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
      virtual DeviceInformation *info () = 0;

   };

   /*!
    * This class provides the basic implementation for the Device API.
    */
   class AbstractDevice:public IDevice
   {
      protected:

      uint16_t _address;

      std::vector <IUnit *> _units;

      DeviceInformation _info;

      public:

      // =============================================================================
      // IDevice API
      // =============================================================================

      uint16_t address () const
      {
         return _address;
      }

      void address (uint16_t value)
      {
         _address = value & Protocol::BROADCAST_ADDR;
      }

      const std::vector <IUnit *> &units () const
      {
         return _units;
      }

      /*!
       * Add unit to devices unit lists.
       *
       * @param unit    pointer to the unit to add to the list.
       */
      void add (IUnit *unit);

      /*!
       * Remove unit from device's unit list.
       *
       * @param unit    pointer to the unit to remove from the list.
       */
      void remove (IUnit *unit);

      DeviceInformation *info ()
      {
         return &_info;
      }
      //!
      /*!
       * Send the \c message to the network the device with the the given address,
       * indicating the unit that is sending the message.
       *
       * \see Interface::sendMessage
       *
       * @param unit       unit that is sending the message.
       * @param addr       network address to send the message to.
       * @param message    the message to be sent.
       */
      virtual void sendMessage (IUnit &unit, Message::Address &addr, Message &message);

      protected:

      AbstractDevice(uint16_t address = Protocol::BROADCAST_ADDR):
         _address (address), _info (this) {}

   };

}  // namespace HF

#endif /* HF_DEVICE_H */
