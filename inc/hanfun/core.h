// =============================================================================
/*!
 * @file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * @version    1.4.2
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_CORE_H
#define HF_CORE_H

#include "hanfun/common.h"
#include "hanfun/interface.h"
#include "hanfun/protocol.h"

#include "hanfun/units.h"

// =============================================================================
// API
// =============================================================================

namespace HF
{
   /*!
    * @addtogroup core Core Services & Interfaces
    *
    * This module contains the classes defining and implementing the
    * %Core Services and %Interfaces API.
    */

   /*!
    * This is the top-level namespace for the Core Services and %Interfaces
    * implementation.
    *
    * @todo Add support for Group Management service.
    * @todo Add support for Identify interface.
    * @todo Add support for Batch Program Management service.
    * @todo Add support for Event Scheduling service
    * @todo Add support for Weekly Scheduling service.
    * @todo Add support for Tamper %Alert interface.
    * @todo Add support for %Time service.
    * @todo Add support for Power service.
    * @todo Add support for Keep Alive service.
    */
   namespace Core
   {
      // Forward declaration.
      namespace DeviceInformation
      {
         struct Server;
      }

      /*!
       * @addtogroup core_common Common
       * @ingroup core
       *
       * This module contains the common classes for the %Core Services and
       * %Interfaces API implementation.
       * @{
       */
      /*!
       * This represent the special unit with ID/UID = 0.
       */
      struct Unit0: public Units::AbstractUnit
      {
         virtual ~Unit0() {}

         /*!
          * Constructor
          *
          * @param [in] device   reference to the device that holds this unit.
          */
         Unit0(IDevice &device): Units::AbstractUnit(device)
         {}

         uint8_t id() const
         {
            return 0;
         }

         uint16_t uid() const
         {
            return 0;
         }

         using Units::AbstractUnit::send;

         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            UNUSED(itf);
            UNUSED(pack_id);
            UNUSED(uids);
            return HF::Attributes::List();
         }

         protected:

         static void remove_mandatory(std::vector<Common::Interface> &itfs)
         {
            static std::array<uint16_t, 3> const mandatory({
               {HF::Interface::DEVICE_INFORMATION,
                HF::Interface::DEVICE_MANAGEMENT,
                HF::Interface::ATTRIBUTE_REPORTING}
            });

            itfs.erase(std::remove_if(itfs.begin(), itfs.end(), [](const Common::Interface &itf)
            {
               return std::any_of(mandatory.begin(), mandatory.end(), [&itf](uint16_t uid) {
                  return itf.id == uid;
               });
            }), itfs.end());
         }
      };

      /*!
       * This is the interface common to all Services.
       */
      struct IService: virtual public HF::Interface
      {
         virtual ~IService() {}

         //! The device this unit is associated with.
         virtual Unit0 &unit() const = 0;
      };

      /*!
       * This is the parent class for all services implementations.
       */
      struct AbstractService: virtual public IService,
         virtual public HF::Interfaces::AbstractInterface
      {
         virtual ~AbstractService() {}

         Unit0 &unit() const
         {
            return _unit;
         }

         HF::Attributes::IAttribute *attribute(uint8_t uid)
         {
            return AbstractInterface::attribute(uid);
         }

         HF::Attributes::UIDS attributes(uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            return AbstractInterface::attributes(pack_id);
         }

         protected:

         //! Reference to the unit the service belongs to.
         Unit0 &_unit;

         /*!
          * Constructor.
          *
          * @param [in] unit  reference to the unit that holds this service.
          */
         AbstractService(Unit0 &unit): _unit(unit)
         {}

         //! @copydoc HF::Interfaces::AbstractInterface::send
         void send(const Protocol::Address &addr, Protocol::Message &message)
         {
            unit().send(addr, message, nullptr);
         }

         //! @copydoc HF::Units::IUnit::send
         void send(const Protocol::Address &addr, Protocol::Message &message,
                   Transport::Link *link)
         {
            unit().send(addr, message, link);
         }
      };

      /*!
       * Class template for all core services implementations.
       */
      template<Interface::UID _uid>
      struct Service: public AbstractService
      {
         //! @copydoc HF::Interface::uid
         uint16_t uid() const
         {
            return _uid;
         }

         protected:

         /*!
          * Constructor.
          *
          * @param [in] unit  reference to the unit that holds this service.
          */
         Service(Unit0 &unit):
            AbstractService(unit)
         {}

         /*!
          * Check if the given @c uid value matches the service's @c %UID value.
          *
          * @param [in] uid   %UID value to check against.
          *
          * @retval  true     if the values match.
          * @retval  false    otherwise.
          */
         bool check_uid(uint16_t uid) const
         {
            return Service::uid() == uid;
         }
      };

      /*!
       * Class template for all interfaces role implementations.
       */
      template<class Parent, Interface::Role _role>
      struct ServiceRole: public Parent
      {
         static_assert(std::is_base_of<HF::Core::AbstractService, Parent>::value,
                       "Parent must be of type HF::Core::AbstractService");

         //! @copydoc HF::Interface::role
         Interface::Role role() const
         {
            return _role;
         }

         protected:

         /*!
          * Constructor.
          *
          * @param [in] unit  reference to the unit that holds this service.
          */
         ServiceRole(Unit0 &unit):
            Parent(unit)
         {}
      };

      /*! @} */

   }  // namespace Core

   /*!
    * @addtogroup core_common
    * @{
    */

   /*!
    * Helper template class used to define custom Unit 0 for devices.
    *
    * @tparam  Base  parent class.
    * @tparam  ITF   custom classes implementing the required services.
    */
   template<typename Base, typename... ITF>
   class Unit0: public Base, public Units::InterfacesWrapper<Base, ITF...>
   {
      static_assert(std::is_base_of<HF::Core::Unit0, Base>::value,
                    "Base must be of type HF::Core::Unit0");

      using InterfacesWrapper = Units::InterfacesWrapper<Base, ITF...>;
      using interfaces_t      = std::tuple<ITF...>;

      public:

      static constexpr uint8_t DEV_INFO = 0;    //!< Device Information service index.
      static constexpr uint8_t DEV_MGT  = 1;    //!< Device Management service index.
      static constexpr uint8_t ATTR_RPT = 2;    //!< Attribute Reporting service index.

      typedef typename std::tuple_element<DEV_INFO, interfaces_t>::type DeviceInfo;

      typedef typename std::tuple_element<DEV_MGT, interfaces_t>::type DeviceMgt;

      typedef typename std::tuple_element<ATTR_RPT, interfaces_t>::type AttrReporting;

      static_assert(std::is_base_of<HF::Core::DeviceInformation::Server, DeviceInfo>::value,
                    "DeviceInfo must be of type HF::Core::DeviceInformation::Server");

      static_assert(std::is_base_of<HF::Core::AttributeReporting::IServer, AttrReporting>::value,
                    "AttrReporting must be of type HF::Core::AttributeReporting::Server");

      /*!
       * Constructor.
       *
       * @param [in] device   reference to the device that this unit belongs to.
       */
      Unit0(HF::IDevice &device):
         Base(device), InterfacesWrapper(*(static_cast<Base *>(this)))
      {}

      // =============================================================================

      /*!
       * Device Information service.
       *
       * @return  pointer to the Device Information service
       */
      DeviceInfo *device_info() const
      {
         return const_cast<DeviceInfo *>(InterfacesWrapper::template get<DEV_INFO>());
      }

      /*!
       * Device Information service.
       *
       * @return  pointer to the Device Information service
       */
      DeviceInfo *device_info()
      {
         return const_cast<DeviceInfo *>(InterfacesWrapper::template get<DEV_INFO>());
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management() const
      {
         return const_cast<DeviceMgt *>(InterfacesWrapper::template get<DEV_MGT>());
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management()
      {
         return const_cast<DeviceMgt *>(InterfacesWrapper::template get<DEV_MGT>());
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting() const
      {
         return const_cast<AttrReporting *>(InterfacesWrapper::template get<ATTR_RPT>());
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting()
      {
         return const_cast<AttrReporting *>(InterfacesWrapper::template get<ATTR_RPT>());
      }

      // =============================================================================
      // IUnit API
      // =============================================================================

      //! @copydoc HF::Interface::handle
      Common::Result handle(HF::Protocol::Packet &packet, Common::ByteArray &payload,
                            uint16_t offset)
      {
         return InterfacesWrapper::handle(packet, payload, offset);
      }

      //! @copydoc HF::Profiles::IProfile::attributes
      HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                      const HF::Attributes::UIDS &uids) const
      {
         HF::Attributes::List result;
         InterfacesWrapper::attributes(result, itf, pack_id, uids);
         return result;
      }

      void periodic(uint32_t time)
      {
         Base::periodic(time);
         InterfacesWrapper::periodic(time);
      }

      std::vector<Common::Interface> interfaces() const
      {
         auto result = InterfacesWrapper::interfaces();
         Base::remove_mandatory(result);
         return result;
      }

      // =============================================================================
   };

   /*! @} */

} // namespace HF

#endif /* HF_CORE_H */
