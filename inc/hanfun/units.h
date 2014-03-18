// =============================================================================
/*!
 * \file       inc/hanfun/units.h
 *
 * This file contains the definitions for the units implementation.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_UNITS_H
#define HF_UNITS_H

#include "hanfun/common.h"
#include "hanfun/profiles.h"

#include "hanfun/device.h"

namespace HF
{
   /*!
    * This is the top level namespace for the HAN-FUN units.
    */
   namespace Units
   {
      /*!
       * This class represents the interface implemented by all Units.
       */
      struct IUnit:public Profiles::IProfile
      {
         //! Id number of this unit on the device.
         virtual uint8_t id () const = 0;

         //! The device this unit is associated with.
         virtual IDevice *device () const = 0;

         //! \see Interface::handle
         virtual Result handle (Protocol::Packet &packet, ByteArray &payload, size_t offset) = 0;
      };

      /*!
       * This is the parent class for all Units.
       */
      class AbstractUnit:public IUnit
      {
         IDevice *_device;

         public:

         //! Get the device associated with this unit.
         IDevice *device () const
         {
            return _device;
         }

         protected:

         AbstractUnit(IDevice *device):
            _device (device)
         {}

         /*!
          * Create and send a new packet with the given message to the given address.
          *
          * @param [in] addr     address to send the message to.
          * @param [in] message  message to send.
          * @param [in] link     preferred link to send the message on.
          */
         void sendMessage (Protocol::Address &addr, Protocol::Message &message,
                           Transport::Link *link)
         {
            Protocol::Packet packet (message);

            packet.destination   = addr;
            packet.source.device = device ()->address ();
            packet.source.unit   = id ();
            packet.link          = link;
            device ()->send (packet);
         }
      };

      /*!
       * This the base template for all the Units.
       */
      template<class Profile>
      class Unit:public AbstractUnit, public Profile
      {
         uint8_t _id;

         public:

         Unit(uint8_t index, IDevice *device):
            AbstractUnit (device), _id (index)
         {
            if (device != nullptr)
            {
               device->add (this);
            }
         }

         uint16_t uid () const
         {
            return Profile::uid ();
         }

         //! \see IUnit::id
         uint8_t id () const
         {
            return _id;
         }

         Result handle (Protocol::Packet &packet, ByteArray &payload, size_t offset)
         {
            return Profile::handle (packet, payload, offset);
         }

         protected:

         void sendMessage (Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::sendMessage (addr, message, nullptr);
         }
      };

   }  // namespace Units

}  // namespace HF

#endif /* HF_UNITS_H */
