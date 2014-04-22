// =============================================================================
/*!
 * \file       inc/hanfun/device.h
 *
 * This file contains the declaration of the API for a HAN-FUN device.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#ifndef HF_DEVICE_H
#define HF_DEVICE_H

#include "hanfun/common.h"
#include "hanfun/transport.h"

namespace HF
{
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
   struct IDevice: public Transport::Endpoint
   {
      /*!
       * Unit list type.
       */
      struct units_t:public forward_list <Units::IUnit *>
      {
         units_t::size_type size () const
         {
            return distance (begin (), end ());
         }
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