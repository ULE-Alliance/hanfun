// =============================================================================
/*!
 * @file       inc/hanfun/units.h
 *
 * This file contains the definitions for the HAN-FUN unit implementation.
 *
 * @version    1.4.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
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
   // Forward declaration.
   namespace Core
   {
      struct IService;
   }

   /*!
    * This is the top-level namespace for the HAN-FUN units implementation.
    */
   namespace Units
   {
      /*!
       * @addtogroup units Units API
       * @ingroup devices
       *
       * This module contains the classes that define and implement the %Units API.
       * @{
       */

      /*!
       * This class represents the interface implemented by all HAN-FUN units.
       */
      struct IUnit: public Profiles::IProfile
      {
         virtual ~IUnit() {}

         /*!
          * Get the id number of this unit on the device.
          *
          * @return id number of this unit on the device.
          */
         virtual uint8_t id() const = 0;

         /*!
          * Reference to the device this unit belongs to.
          *
          * @return  reference to the device this unit belongs to
          */
         virtual IDevice &device() const = 0;

         //! @copydoc HF::Interface::handle
         virtual Common::Result handle(Protocol::Packet &packet, Common::ByteArray &payload,
                                       uint16_t offset) = 0;

         /*!
          * Create and send a new packet with the given message to the given address.
          *
          * @param [in] addr     address to send the message to.
          * @param [in] message  message to send.
          * @param [in] link     preferred link to send the message on.
          */
         virtual void send(const Protocol::Address &addr, Protocol::Message &message,
                           Transport::Link *link = nullptr) = 0;

         /*!
          * Return a vector containing a list of extra interfaces, other than the interfaces
          * specified by the profile the unit implements)
          *
          * @return  vector containing the list of extra interfaces.
          */
         virtual std::vector<Common::Interface> interfaces() const = 0;

         //! @copydoc HF::Interface::periodic
         virtual void periodic(uint32_t time) = 0;
      };

      /*!
       * This is the parent class for all HAN-FUN units.
       */
      class AbstractUnit: public IUnit
      {
         //! Reference to the device this unit belongs to.
         IDevice &_device;

         public:

         virtual ~AbstractUnit()
         {
            device().remove(this);
         }

         IDevice &device() const
         {
            return _device;
         }

         void send(const Protocol::Address &addr, Protocol::Message &message,
                   Transport::Link *link);

         std::vector<Common::Interface> interfaces() const
         {
            return std::vector<Common::Interface>();
         }

         void periodic(uint32_t time)
         {
            UNUSED(time);
         }

         protected:

         //! Unit ID used to identify a given unit in a given HAN-FUN device.
         uint8_t _id;

         /*!
          * Constructor
          *
          * @param [in] device   reference to the device that holds this unit.
          */
         AbstractUnit(IDevice &device): _device(device)
         {
            device.add(this);
         }

         /*!
          * Notify the attribute reporting service that the given attribute has changed value.
          *
          * @param [in] old_value   reference to the previous value of the attribute.
          * @param [in] new_value   reference to the current value of the attribute.
          */
         void notify(const HF::Attributes::IAttribute &old_value,
                     const HF::Attributes::IAttribute &new_value) const;
      };

      /*!
       * Proxy class for interface objects.
       */
      template<typename _Interface, typename _Proxy, typename = void>
      struct Proxy: public HF::Interfaces::Proxy<_Interface, _Proxy>
      {
         Proxy(_Proxy &_proxy): HF::Interfaces::Proxy<_Interface, _Proxy>(_proxy)
         {}

         _Proxy &unit() const
         {
            return HF::Interfaces::Proxy<_Interface, _Proxy>::proxy;
         }
      };

      /*!
       * Proxy class for service objects.
       */
      template<typename _Interface, typename _Proxy>
      struct Proxy<_Interface, _Proxy, EnableIf<Parent<HF::Core::IService, _Interface>>>:
         public _Interface
      {
         typedef _Interface base;

         Proxy(_Proxy &_proxy): _Interface(_proxy)
         {}
      };

      /*!
       * Helper class to handle optional interfaces in units.
       */
      template<typename Base, typename... ITF>
      class InterfacesWrapper: public Interfaces::Container<Base, Proxy<ITF, Base>...>
      {
         using Container = Interfaces::Container<Base, Proxy<ITF, Base>...>;

         public:

         InterfacesWrapper(Base &base): Container(base)
         {}
      };

      /*!
       * Helper template class to implement units.
       */
      template<class Profile, typename... ITF>
      class Unit: public HF::Units::AbstractUnit, public virtual Profile,
         public InterfacesWrapper<Unit<Profile, ITF...>, ITF...>
      {
         uint8_t _id;

         using Base    = Unit<Profile, ITF...>;
         using Wrapper = InterfacesWrapper<Unit<Profile, ITF...>, ITF...>;

         public:

         /*!
          * Constructor
          *
          * @param [in] id       unit identifier.
          * @param [in] device   device that contains this units.
          */
         Unit(uint8_t id, IDevice &device):
            Profile(), HF::Units::AbstractUnit(device),
            Wrapper(*this), _id(id)
         {}

         //! @copydoc HF::Units::IUnit::id
         uint8_t id() const
         {
            return _id;
         }

         //! @copydoc HF::Profiles::IProfile::uid
         uint16_t uid() const
         {
            return Profile::uid();
         }

         //! @copydoc HF::Interface::handle
         Common::Result handle(Protocol::Packet &packet, Common::ByteArray &payload,
                               uint16_t offset)
         {
            Common::Result result = Profile::handle(packet, payload, offset);

            // Message not handled by base profile, then try extra interfaces.
            if (result == Common::Result::FAIL_ARG)
            {
               return Wrapper::handle(packet, payload, offset);
            }

            return result;
         }

         //! @copydoc HF::Profiles::IProfile::attributes
         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            auto result = Profile::attributes(itf, pack_id, uids);

            Wrapper::attributes(result, itf, pack_id, uids);

            return result;
         }

         using HF::Units::AbstractUnit::send;

         //! @copydoc HF::Interfaces::AbstractInterface::send
         void send(const Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::send(addr, message, nullptr);
         }

         //! @copydoc HF::Units::AbstractUnit::notify
         void notify(const HF::Attributes::IAttribute &old_value,
                     const HF::Attributes::IAttribute &new_value) const
         {
            AbstractUnit::notify(old_value, new_value);
         }

         /*!
          * Return the list of optional interfaces implemented by this unit.
          *
          * @return  a vector containing the UIDs for the optional interfaces
          *          implemented by this unit.
          */
         std::vector<Common::Interface> interfaces() const
         {
            return Wrapper::interfaces();
         }

         void periodic(uint32_t time)
         {
            Profile::periodic(time);
            Wrapper::periodic(time);
         }
      };

      /*! @} */

   }  // namespace Units

}  // namespace HF

#endif /* HF_UNITS_H */
