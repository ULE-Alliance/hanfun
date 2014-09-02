// =============================================================================
/*!
 * \file       inc/hanfun/units.h
 *
 * This file contains the definitions for the units implementation.
 *
 * \version    0.4.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
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
         virtual IDevice &device () const = 0;

         //! \see Interface::handle
         virtual Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset) = 0;

         /*!
          * Create and send a new packet with the given message to the given address.
          *
          * @param [in] addr     address to send the message to.
          * @param [in] message  message to send.
          * @param [in] link     preferred link to send the message on.
          */
         virtual void send (const Protocol::Address &addr, Protocol::Message &message,
                            Transport::Link *link = nullptr) = 0;
      };

      /*!
       * This is the parent class for all Units.
       */
      class AbstractUnit:public IUnit
      {
         IDevice &_device;

         public:

         //! Get the device associated with this unit.
         IDevice &device () const
         {
            return _device;
         }

         void send (const Protocol::Address &addr, Protocol::Message &message,
                    Transport::Link *link = nullptr);

         protected:

         AbstractUnit(IDevice &device):
            _device (device)
         {
            device.add (this);
         }

         void notify (const HF::Attributes::IAttribute &old_value,
                      const HF::Attributes::IAttribute &new_value) const;
      };

      /*!
       * This the base template for all the Units.
       */
      template<class Profile>
      class Unit:public AbstractUnit, public Profile
      {
         uint8_t _id;

         public:

         Unit(uint8_t index, IDevice &device):
            AbstractUnit (device), _id (index)
         {}

         uint16_t uid () const
         {
            return Profile::uid ();
         }

         //! \see IUnit::id
         uint8_t id () const
         {
            return _id;
         }

         Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            return Profile::handle (packet, payload, offset);
         }

         HF::Attributes::List attributes (Common::Interface itf, uint8_t pack_id,
                                          const HF::Attributes::UIDS &uids) const
         {
            return Profile::attributes (itf, pack_id, uids);
         }

         protected:

         void send (const Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::send (addr, message, nullptr);
         }

         void notify (const HF::Attributes::IAttribute &old_value,
                      const HF::Attributes::IAttribute &new_value) const
         {
            AbstractUnit::notify (old_value, new_value);
         }
      };

   }  // namespace Units

}  // namespace HF

#endif /* HF_UNITS_H */
