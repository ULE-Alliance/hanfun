// =============================================================================
/*!
 * \file       inc/hanfun/device.h
 *
 * This file contains the declaration of the API for a HAN-FUN device.
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

#ifndef HF_DEVICE_H
#define HF_DEVICE_H

#include <forward_list>

#include "hanfun/common.h"
#include "hanfun/transport.h"

namespace HF
{
   // Forward declaration of the devices namespace.
   namespace Core
   {
      namespace DeviceInformation
      {
         struct Server;

      }  // namespace DeviceInformation

      namespace AttributeReporting
      {
         struct Server;

      }  // namespace DeviceInformation

   }  // namespace Devices


   // Forward declaration of the units namespace.
   namespace Units
   {
      // Forward declaration of the Unit's interface.
      struct IUnit;

   }  // namespace Units

   // Forward declaration of the protocol namespace.
   namespace Protocol
   {
      // Forward declaration of the protocol packet structure.
      struct Packet;

   }  // namespace Protocol

   /*!
    * This class represents the interface that all devices MUST implement.
    */
   struct IDevice:public Transport::Endpoint
   {
      /*!
       * Unit list type.
       */
      struct units_t:public std::forward_list <Units::IUnit *>
      {
         units_t::size_type size () const
         {
            return distance (begin (), end ());
         }
      };

      struct IUnit0
      {
         virtual Core::DeviceInformation::Server  *device_info () const         = 0;

         virtual Core::DeviceInformation::Server  *device_info ()               = 0;

         virtual Core::AttributeReporting::Server *attribute_reporting () const = 0;

         virtual Core::AttributeReporting::Server *attribute_reporting ()       = 0;
      };

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
       * @return     list containing the device's registered units.
       */
      virtual const units_t &units () const = 0;

      /*!
       * Return pointer to the unit with the given id.
       *
       * @param [in] id    unit identifier.
       *
       * @return  pointer to the unit with the given id,
       *          nullptr otherwise.
       */
      virtual Units::IUnit *unit (uint8_t id) const = 0;

      /*!
       * Return pointer to the unit with the given id.
       *
       * @param [in] id    unit identifier.
       *
       * @return  pointer to the unit with the given id,
       *          nullptr otherwise.
       */
      virtual IUnit0 *unit0 () const = 0;

      /*!
       * Add unit to devices unit lists.
       *
       * @param unit    pointer to the unit to add to the list.
       */
      virtual void add (Units::IUnit *unit) = 0;

      /*!
       * Remove unit from device's unit list.
       *
       * @param unit    pointer to the unit to remove from the list.
       */
      virtual void remove (Units::IUnit *unit) = 0;

      /*!
       * Send given \c packet into the HAN-FUN network.
       *
       * @param packet  reference to the packet to send to the network.
       */
      virtual void send (Protocol::Packet &packet) = 0;
   };

}  // namespace HF

#endif /* DEVICE_H_ */
