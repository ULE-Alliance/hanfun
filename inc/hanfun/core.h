// =============================================================================
/*!
 * @file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * @version    1.5.3
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
    * @todo Add support for Identify interface.
    * @todo Add support for Tamper %Alert interface.
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
            static std::array<uint16_t, 3> const mandatory =
            {
               {
                  HF::Interface::DEVICE_INFORMATION,
                  HF::Interface::DEVICE_MANAGEMENT,
                  HF::Interface::ATTRIBUTE_REPORTING
               }
            };

            itfs.erase(std::remove_if(itfs.begin(), itfs.end(), [](const Common::Interface &itf)
            {
               return std::any_of(mandatory.begin(), mandatory.end(), [&itf](uint16_t uid)
               {
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
      template<Interface::UID _uid, typename Parent = AbstractService>
      struct Service: public Parent
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
            Parent(unit)
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

      typedef typename Units::InterfacesWrapper<Base, ITF...> InterfacesWrapper;
      typedef typename std::tuple<ITF...> interfaces_t;

      public:

      //! Core Services indexs.
      enum Inferface: uint8_t
      {
         DEV_INFO = 0,     //!< Device Information service index.
         DEV_MGT,          //!< Device Management service index.
         ATTR_RPT,         //!< Attribute Reporting service index.
#if HF_TIME_SUPPORT
         TIME,             //!< Time service index.
#endif
#if HF_BATCH_PROGRAM_SUPPORT
         BATCH_PROGRAM,    //!< Batch Programming service index.
#endif
#if HF_EVENT_SCHEDULING_SUPPORT
         EVENT_SCH,        //!< Event Scheduling service index.
#endif
#if HF_WEEKLY_SCHEDULING_SUPPORT
         WEEKLY_SCH,       //!< Weekly Scheduling service index.
#endif
#if HF_GROUP_SUPPORT
         GROUP_TABLE,      //!< Group Table service index.
         GROUP_MGT,        //!< Group Management service index.
#endif
         BIND_MGT,         //!< Bind Management service index.
      };

      typedef typename std::tuple_element<DEV_INFO, interfaces_t>::type DeviceInfo;

      typedef typename std::tuple_element<DEV_MGT, interfaces_t>::type DeviceMgt;

      typedef typename std::tuple_element<ATTR_RPT, interfaces_t>::type AttrReporting;

      static_assert(std::is_base_of<HF::Core::DeviceInformation::Server, DeviceInfo>::value,
                    "DeviceInfo must be of type HF::Core::DeviceInformation::Server");

      static_assert(std::is_base_of<HF::Core::AttributeReporting::IServer, AttrReporting>::value,
                    "AttrReporting must be of type HF::Core::AttributeReporting::Server");

#if HF_GROUP_SUPPORT
      typedef typename std::tuple_element<GROUP_TABLE, interfaces_t>::type GroupTable;

      static_assert(std::is_base_of<HF::Core::GroupTable::IServer, GroupTable>::value,
                    "GroupTable must be of type HF::Core::GroupTable::IServer");
#endif

#if HF_TIME_SUPPORT
      typedef typename std::tuple_element<TIME, interfaces_t>::type Time;

      static_assert(std::is_base_of<HF::Core::Time::Server, Time>::value,
                    "Time must be of type HF::Core::Time::Server");
#endif

#if HF_BATCH_PROGRAM_SUPPORT
      typedef typename std::tuple_element<BATCH_PROGRAM, interfaces_t>::type BatchProgram;

      static_assert(std::is_base_of<HF::Core::BatchProgramManagement::IServer, BatchProgram>::value,
                    "BatchProgram must be of type HF::Core::BatchProgramManagement::IServer");
#endif

#if HF_EVENT_SCHEDULING_SUPPORT
      typedef typename std::tuple_element<EVENT_SCH, interfaces_t>::type EventScheduling;

      static_assert(std::is_base_of<HF::Core::Scheduling::Event::IServer, EventScheduling>::value,
                    "EventSch must be of type HF::Core::Scheduling::Event::IServer");
#endif

#if HF_WEEKLY_SCHEDULING_SUPPORT
      typedef typename std::tuple_element<WEEKLY_SCH, interfaces_t>::type WeeklyScheduling;

      static_assert(std::is_base_of<HF::Core::Scheduling::Weekly::IServer, WeeklyScheduling>::value,
                    "WeeklyScheduling must be of type HF::Core::Scheduling::Weekly::IServer");
#endif

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
         return get<DeviceInfo, DEV_INFO>();
      }

      /*!
       * Device Information service.
       *
       * @return  pointer to the Device Information service
       */
      DeviceInfo *device_info()
      {
         return get<DeviceInfo, DEV_INFO>();
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management() const
      {
         return get<DeviceMgt, DEV_MGT>();
      }

      /*!
       * Device Management service.
       *
       * @return  pointer to the Device Management service
       */
      DeviceMgt *device_management()
      {
         return get<DeviceMgt, DEV_MGT>();
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting() const
      {
         return get<AttrReporting, ATTR_RPT>();
      }

      /*!
       * Attribute Reporting service.
       *
       * @return  pointer to the Attribute Reporting service
       */
      AttrReporting *attribute_reporting()
      {
         return get<AttrReporting, ATTR_RPT>();
      }

#if HF_GROUP_SUPPORT
      /*!
       * Get the pointer to the node's Group Table service.
       *
       * @return pointer to the node's Group Table service.
       */
      GroupTable *group_table()
      {
         return get<GroupTable, GROUP_TABLE>();
      }

      /*!
       * Get the pointer to the node's Group Table service.
       *
       * @return pointer to the node's Group Table service.
       */
      GroupTable *group_table() const
      {
         return get<GroupTable, GROUP_TABLE>();
      }
#endif

#if HF_TIME_SUPPORT
      /*!
       * Get the pointer to the node's time service.
       *
       * @return pointer to the node's time service.
       */
      Time *time()
      {
         return get<Time, TIME>();
      }

      /*!
       * Get the pointer to the node's time service.
       *
       * @return pointer to the node's time service.
       */
      Time *time() const
      {
         return get<Time, TIME>();
      }
#endif

#if HF_BATCH_PROGRAM_SUPPORT
      /*!
       * Get the pointer to the node's Batch Program management service.
       *
       * @return pointer to the node's Batch Program management service.
       */
      BatchProgram *batch_program()
      {
         return get<BatchProgram, BATCH_PROGRAM>();
      }

      /*!
       * Get the pointer to the node's Batch Program management service.
       *
       * @return pointer to the node's Batch Program management service.
       */
      BatchProgram *batch_program() const
      {
         return get<BatchProgram, BATCH_PROGRAM>();
      }
#endif

#if HF_EVENT_SCHEDULING_SUPPORT
      /*!
       * Get the pointer to the node's Event Scheduling service.
       *
       * @return pointer to the node's Event Scheduling service.
       */
      EventScheduling *event_scheduling()
      {
         return get<EventScheduling, EVENT_SCH>();
      }

      /*!
       * Get the pointer to the node's Event Scheduling service.
       *
       * @return pointer to the node's Event Scheduling service.
       */
      EventScheduling *event_scheduling() const
      {
         return get<EventScheduling, EVENT_SCH>();
      }
#endif

#if HF_WEEKLY_SCHEDULING_SUPPORT
      /*!
       * Get the pointer to the node's Weekly Scheduling service.
       *
       * @return pointer to the node's Weekly Scheduling service.
       */
      WeeklyScheduling *weekly_scheduling()
      {
         return get<WeeklyScheduling, WEEKLY_SCH>();
      }

      /*!
       * Get the pointer to the node's Weekly Scheduling service.
       *
       * @return pointer to the node's Weekly Scheduling service.
       */
      WeeklyScheduling *weekly_scheduling() const
      {
         return get<WeeklyScheduling, WEEKLY_SCH>();
      }
#endif

      // =============================================================================
      // IUnit API
      // =============================================================================

      //! @copydoc HF::Interface::handle
      Common::Result handle(HF::Protocol::Packet &packet, Common::ByteArray &payload,
                            uint16_t offset)
      {
#if HF_GROUP_SUPPORT
         Common::Result result = Common::Result::FAIL_UNKNOWN;
         InterfacesWrapper::for_each([&result, &packet, &payload, offset](HF::Interface &itf)
         {
            if (result != Common::Result::OK) // Message already handled, skip.
            {
               result = itf.handle(packet, payload, offset);
            }
         });

         return result;

#else
         return InterfacesWrapper::handle(packet, payload, offset);

#endif
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

      protected:

      template<typename T, uint8_t N>
      T *get() const
      {
         return const_cast<T *>(InterfacesWrapper::template get<N>());
      }
   };

   /*! @} */

} // namespace HF

#endif /* HF_CORE_H */
