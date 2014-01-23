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
#include "hanfun/devices.h"

namespace HF
{
   /*!
    * This class represents the interface implemented by all Units.
    */
   struct IUnit:public IProfile
   {
      //! Id number of this unit on the device.
      virtual uint8_t id () const = 0;

      //! UID for the profile/interface contained in the unit.
      virtual uint16_t uid () const = 0;

      //! The device this unit is associated with.
      virtual IDevice *device () const = 0;
   };

   /*!
    * This is the parent class for all Units.
    */
   class AbstractUnit:public IUnit
   {
      protected:

      AbstractDevice *_device;
      uint8_t _id;

      public:

      //! Get the device associated with this unit.
      IDevice *device () const
      {
         return _device;
      }

      //! \see IUnit::id
      uint8_t id () const
      {
         return _id;
      }

      protected:

      AbstractUnit(uint8_t index, AbstractDevice *device):
         _device (device), _id (index)
      {}

      void sendMessage (Protocol::Address &addr, Protocol::Message &message)
      {
         _device->sendMessage (*this, addr, message);
      }
   };

   /*!
    * This the base template for all the Units.
    */
   template<class Profile>
   class Unit:public AbstractUnit, public Profile
   {
      public:

      Unit(uint8_t index, AbstractDevice *device):
         AbstractUnit (index, device)
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

      protected:

      void sendMessage (Protocol::Address &addr, Protocol::Message &message)
      {
         AbstractUnit::sendMessage (addr, message);
      }
   };

}  // namespace HF

#endif /* HF_UNITS_H */
