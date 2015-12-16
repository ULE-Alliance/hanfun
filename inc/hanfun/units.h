// =============================================================================
/*!
 * @file       inc/hanfun/units.h
 *
 * This file contains the definitions for the HAN-FUN unit implementation.
 *
 * @version    1.4.0
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
       * Helper class to handle optional interfaces in units.
       */
      template<typename Base, typename... ITF>
      class InterfacesWrapper
      {
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

         using interfaces_t = std::tuple<Proxy<ITF, Base>...>;

         //! Tuple containing the wrapped interfaces/services.
         interfaces_t _interfaces;

         public:

         /*!
          * Constructor
          *
          * @param [in] wrapper  reference to the object containing the interfaces.
          */
         InterfacesWrapper(Base &wrapper): _interfaces(Proxy<ITF, Base>(wrapper) ...)
         {}

         //! @copydoc HF::Interface::handle
         Common::Result handle(Protocol::Packet &packet, Common::ByteArray &payload,
                               uint16_t offset)
         {
            HF::Interface *itf = find<0, Proxy<ITF, Base>...>(packet.message.itf.id);

            if (itf != nullptr)
            {
               return itf->handle(packet, payload, offset);
            }
            else
            {
               return Common::Result::FAIL_SUPPORT;
            }
         }

         /*!
          * @copydoc HF::Profiles::IProfile::attributes
          *
          * @param [inout] attr_list   list to place the attributes UIDs.
          */
         void attributes(HF::Attributes::List &attr_list, Common::Interface itf, uint8_t pack_id,
                         const HF::Attributes::UIDS &uids) const
         {
            attributes_itf<0, Proxy<ITF, Base>...>(attr_list, itf, pack_id, uids);
         }

         /*!
          * Return the list of interfaces present in the wrapper.
          *
          * @return  a vector containing the UIDs for the interfaces
          *          wrapped by this class.
          */
         std::vector<Common::Interface> interfaces() const
         {
            std::vector<Common::Interface> result;
            result.reserve(sizeof ... (ITF));

            Common::Interface temp;
            /* *INDENT-OFF* */
            for_each ([&result, &temp](HF::Interface &itf)
            {
                temp.id = itf.uid ();
                temp.role = itf.role ();
                result.push_back (temp);
            });
            /* *INDENT-ON* */

            return result;
         }

         void periodic(uint32_t time)
         {
            /* *INDENT-OFF* */
            for_each ([time](HF::Interface &itf) { itf.periodic (time);});
            /* *INDENT-ON* */
         }

         /*!
          * Retrieve a pointer to the N optional interface implemented by this unit.
          *
          * @tparam N   index of the interface to retrieve the pointer to.
          *
          * @return  a pointer to the optional implemented interface.
          */
         template<uint8_t N>
         const typename std::tuple_element<N, interfaces_t>::type::base * get() const
         {
            return &std::get<N>(_interfaces);
         }

         protected:

         /*!
          * Call the given function for all the interfaces.
          *
          * @param [in] func  function to call with each of the optional implemented interfaces.
          */
         void for_each(std::function<void(HF::Interface &)> func) const
         {
            for_each<0, Proxy<ITF, Base>...>(func);
         }

         /*!
          * Call the given function for all the interfaces.
          *
          * @param [in] func  function to call with each of the optional implemented interfaces.
          */
         void for_each(std::function<void(HF::Interface &)> func)
         {
            for_each<0, Proxy<ITF, Base>...>(func);
         }

         protected:

         /*!
          * Find the interface with the given UID.
          *
          * @param itf_uid    the interface UID to search for in the wrapped interfaces.
          *
          * @tparam  N        index in the interfaces tuple to check if UID matches.
          * @tparam  Head     class for the interface at the given index.
          * @tparam  Tail     the classes associated with the remaining optional interfaces.
          *
          * @return           a pointer to the interface or @c nullptr if
          *                   the interface is not present.
          */
         template<uint8_t N, typename Head, typename... Tail>
         HF::Interface *find(uint16_t itf_uid) const
         {
            static_assert(std::is_base_of<HF::Interface, Head>::value,
                          "Head must be of type HF::Interface");

            const Head &head = std::get<N>(_interfaces);

            if (head.uid() == itf_uid)
            {
               return const_cast<Head *>(&head);
            }
            else
            {
               return find<N + 1, Tail...>(itf_uid);
            }
         }

         /*!
          * Final template instantiation that finds the wrapped interface
          * with the given UID.
          *
          * @param itf_uid    the interface UID to search for in the wrapped interfaces.
          *
          * @tparam  N  index in the interfaces tuple to check if UID matches.
          *
          * @return  @c nullptr, i.e. the interface is not present.
          */
         template<uint8_t N>
         HF::Interface *find(uint16_t itf_uid) const
         {
            UNUSED(itf_uid);
            return nullptr;
         }

         /*!
          * Helper template function to implement the HF::Units::Unit::for_each functionality.
          *
          * @param [in] func  function to call with the reference for the wrapped interface at
          *                   index @c N.
          *
          * @tparam  N     index in the wrapped interfaces tuple to check if UID matches.
          * @tparam  Head  class for the wrapped interface at the given index.
          * @tparam  Tail  the classes associated with the remaining wrapped interfaces.
          */
         template<uint8_t N, typename Head, typename... Tail>
         void for_each(std::function<void(HF::Interface &)> func) const
         {
            const auto &head = std::get<N>(this->_interfaces);

            func(*(const_cast<HF::Interface *>(static_cast<const HF::Interface *>(&head))));

            for_each<N + 1, Tail...>(func);
         }

         /*!
          * Helper template function to implement the HF::Units::Unit::for_each functionality.
          *
          * @param [in] func  function to call with the reference for the wrapped interface at
          *                   index @c N.
          *
          * @tparam  N     index in the wrapped interfaces tuple to check if UID matches.
          */
         template<uint8_t N>
         void for_each(std::function<void(HF::Interface &)> func) const
         {
            UNUSED(func);
         }

         /*!
          * Helper function used to provide HF::Units::Unit::attributes functionality.
          *
          *
          * @param [out] attrs      attribute list to append the attributes for the interface to.
          * @param [in]  itf        interface to get the attributes for.
          * @param [in]  pack_id    attributes pack id to use when retrieving the attributes.
          * @param [in]  uids       list of attribute uids to get attributes for if @c pack_id is
          *                         HF::Attributes::Pack::DYNAMIC.
          *
          * @tparam  N     index in the wrapped interfaces tuple to check if UID matches.
          * @tparam  Head  class for the wrapped interface at the given index.
          * @tparam  Tail  the classes associated with the remaining wrapped interfaces.
          */
         template<uint8_t N, typename Head, typename... Tail>
         void attributes_itf(HF::Attributes::List &attrs, Common::Interface itf,
                             uint8_t pack_id, const HF::Attributes::UIDS &uids) const
         {
            const auto &head = std::get<N>(this->_interfaces);

            if (head.uid() == itf.id)
            {
               auto result = HF::Attributes::get(head, pack_id, uids);
               attrs.merge(result);
            }
            else
            {
               attributes_itf<N + 1, Tail...>(attrs, itf, pack_id, uids);
            }
         }

         /*!
          * Helper function used to provide HF::Units::Unit::attributes functionality.
          *
          * Template expansion end.
          *
          * @param [out] attrs      attribute list to append the attributes for the interface to.
          * @param [in]  itf        interface to get the attributes for.
          * @param [in]  pack_id    attributes pack id to use when retrieving the attributes.
          * @param [in]  uids       list of attribute uids to get attributes for if @c pack_id is
          *                         HF::Attributes::Pack::DYNAMIC.
          *
          * @tparam N    index in the wrapped interfaces tuple to check if UID matches.
          */
         template<uint8_t N>
         void attributes_itf(HF::Attributes::List &attrs, Common::Interface itf,
                             uint8_t pack_id, const HF::Attributes::UIDS &uids) const
         {
            UNUSED(attrs);
            UNUSED(itf);
            UNUSED(pack_id);
            UNUSED(uids);
         }
      };

      /*!
       * Helper template class to implement units.
       */
      template<class Profile, typename... ITF>
      class Unit: public HF::Units::AbstractUnit, public virtual Profile,
         public Units::InterfacesWrapper<Unit<Profile, ITF...>, ITF...>
      {
         uint8_t _id;

         using Base              = Unit<Profile, ITF...>;
         using InterfacesWrapper = Units::InterfacesWrapper<Base, ITF...>;

         public:

         /*!
          * Constructor
          *
          * @param [in] id       unit identifier.
          * @param [in] device   device that contains this units.
          */
         Unit(uint8_t id, IDevice &device):
            Profile(), HF::Units::AbstractUnit(device),
            InterfacesWrapper(*this), _id(id)
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
               return InterfacesWrapper::handle(packet, payload, offset);
            }

            return result;
         }

         //! @copydoc HF::Profiles::IProfile::attributes
         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            auto result = Profile::attributes(itf, pack_id, uids);

            InterfacesWrapper::attributes(result, itf, pack_id, uids);

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
            return InterfacesWrapper::interfaces();
         }

         void periodic(uint32_t time)
         {
            Profile::periodic(time);
            InterfacesWrapper::periodic(time);
         }
      };

      /*! @} */

   }  // namespace Units

}  // namespace HF

#endif /* HF_UNITS_H */
