// =============================================================================
/*!
 * @file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * @version    1.1.1
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
      struct Unit0:public Units::AbstractUnit
      {
         virtual ~Unit0() {}

         /*!
          * Constructor
          *
          * @param [in] device   reference to the device that holds this unit.
          */
         Unit0(IDevice &device):Units::AbstractUnit (device)
         {}

         uint8_t id () const
         {
            return 0;
         }

         uint16_t uid () const
         {
            return 0;
         }

         using Units::AbstractUnit::send;

         HF::Attributes::List attributes (Common::Interface itf, uint8_t pack_id,
                                          const HF::Attributes::UIDS &uids) const
         {
            UNUSED (itf);
            UNUSED (pack_id);
            UNUSED (uids);
            return HF::Attributes::List ();
         }

      };

      /*!
       * This is the interface common to all Services.
       */
      struct IService:virtual public HF::Interface
      {
         virtual ~IService() {}

         //! The device this unit is associated with.
         virtual Unit0 &unit () const = 0;
      };

      /*!
       * This is the parent class for all services implementations.
       */
      struct AbstractService:virtual public IService,
         virtual public HF::Interfaces::AbstractInterface
      {
         virtual ~AbstractService() {}

         Unit0 &unit () const
         {
            return _unit;
         }

         HF::Attributes::IAttribute *attribute (uint8_t uid)
         {
            return AbstractInterface::attribute (uid);
         }

         HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            return AbstractInterface::attributes (pack_id);
         }

         protected:

         //! Reference to the unit the service belongs to.
         Unit0 &_unit;

         /*!
          * Constructor.
          *
          * @param [in] unit  reference to the unit that holds this service.
          */
         AbstractService(Unit0 &unit):_unit (unit)
         {}

         //! @copydoc HF::Interfaces::AbstractInterface::send
         void send (const Protocol::Address &addr, Protocol::Message &message)
         {
            unit ().send (addr, message, nullptr);
         }

         //! @copydoc HF::Units::IUnit::send
         void send (const Protocol::Address &addr, Protocol::Message &message,
                    Transport::Link *link)
         {
            unit ().send (addr, message, link);
         }
      };

      /*!
       * Class template for all core services implementations.
       */
      template<Interface::UID _uid>
      struct Service:public AbstractService
      {
         //! @copydoc HF::Interface::uid
         uint16_t uid () const
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
            AbstractService (unit)
         {}

         /*!
          * Check if the given @c uid value matches the service's @c %UID value.
          *
          * @param [in] uid   %UID value to check against.
          *
          * @retval  true     if the values match.
          * @retval  false    otherwise.
          */
         bool check_uid (uint16_t uid) const
         {
            return Service::uid () == uid;
         }
      };

      /*!
       * Class template for all interfaces role implementations.
       */
      template<class Parent, Interface::Role _role>
      struct ServiceRole:public Parent
      {
         static_assert (std::is_base_of <HF::Core::AbstractService, Parent>::value,
                        "Parent must be of type HF::Core::AbstractService");

         //! @copydoc HF::Interface::role
         Interface::Role role () const
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
            Parent (unit)
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
   class Unit0:public Base
   {
      static_assert (std::is_base_of <HF::Core::Unit0, Base>::value,
                     "Base must be of type HF::Core::Unit0");

      public:

      typedef std::tuple <ITF...> interfaces_t;

      interfaces_t interfaces;

      typedef typename std::tuple_element <0, decltype (interfaces)>::type DeviceInfo;

      typedef typename std::tuple_element <1, decltype (interfaces)>::type DeviceMgt;

      typedef typename std::tuple_element <2, decltype (interfaces)>::type AttrReporting;

      static_assert (std::is_base_of <HF::Core::DeviceInformation::Server, DeviceInfo>::value,
                     "DeviceInfo must be of type HF::Core::DeviceInformation::Server");

      /*!
       * Constructor.
       *
       * @param [in] device   reference to the device that this unit belongs to.
       */
      Unit0(HF::IDevice &device):
         Base (device), interfaces (ITF (*this) ...)
      {
         // Add this unit to the device units list.
         device.add (this);
      }

      // =============================================================================

      /*!
       * Device Information service.
       *
       * @return  pointer to the Device Information service
       */
      DeviceInfo *device_info () const
      {
         return const_cast <DeviceInfo *>(&std::get <0>(interfaces));
      }

      /*!
       * Device Information service.
       *
       * @return  pointer to the Device Information service
       */
      DeviceInfo *device_info ()
      {
         return &std::get <0>(interfaces);
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management () const
      {
         return const_cast <DeviceMgt *>(&std::get <1>(interfaces));
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management ()
      {
         return &std::get <1>(interfaces);
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting () const
      {
         return const_cast <AttrReporting *>(&std::get <2>(interfaces));
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting ()
      {
         return &std::get <2>(interfaces);
      }

      // =============================================================================
      // IUnit API
      // =============================================================================

      //! @copydoc HF::Interface::handle
      Common::Result handle (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
      {
         Core::IService *service = find <0, ITF...>(packet.message.itf.id);

         if (service != nullptr)
         {
            return service->handle (packet, payload, offset);
         }
         else
         {
            return Common::Result::FAIL_SUPPORT;
         }
      }

      //! @copydoc HF::Profiles::IProfile::attributes
      HF::Attributes::List attributes (Common::Interface itf, uint8_t pack_id,
                                       const HF::Attributes::UIDS &uids) const
      {
         UNUSED (itf);
         UNUSED (pack_id);
         UNUSED (uids);
         return HF::Attributes::List ();
      }

      // =============================================================================

      private:

      /*!
       * Find the service for the given @c itf_uid.
       *
       * @param itf_uid    service %UID to search for.
       *
       * @return  pointer to the service object if found, @c nullptr otherwise.
       */
      template<uint8_t N, typename Head, typename... Tail>
      Core::IService *find (uint16_t itf_uid) const
      {
         static_assert (std::is_base_of <HF::Core::IService, Head>::value,
                        "Head must be of type HF::Core::IService");

         const Head &head = std::get <N>(interfaces);

         if (head.uid () == itf_uid)
         {
            return const_cast <Head *>(&head);
         }
         else
         {
            return find <N + 1, Tail...>(itf_uid);
         }
      }

      template<uint8_t N>
      Core::IService *find (uint16_t itf_uid) const
      {
         UNUSED (itf_uid);
         return nullptr;
      }
   };

   /*! @} */

} // namespace HF

#endif /* HF_CORE_H */
