// =============================================================================
/*!
 * \file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
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
    * This is the top level namespace for the HAN-FUN Core Services and Interfaces
    * implementation.
    */
   namespace Core
   {
      /*!
       * This represent the special unit with ID/UID = 0.
       */
      struct Unit0:public Units::AbstractUnit
      {
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
         //! The device this unit is associated with.
         virtual Unit0 &unit () const = 0;
      };

      struct AbstractService:virtual public IService, virtual public HF::Interfaces::AbstractInterface
      {
         Unit0 &unit () const
         {
            return _unit;
         }

         virtual void periodic (uint32_t time)
         {
            UNUSED (time);
         }

         //! \see Interface::attribute
         HF::Attributes::IAttribute *attribute (uint8_t uid)
         {
            return AbstractInterface::attribute (uid);
         }

         //! \see AbstractInterface::attributes
         HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            return AbstractInterface::attributes (pack_id);
         }

         protected:

         Unit0 &_unit;

         AbstractService(Unit0 &unit):_unit (unit)
         {}

         void send (const Protocol::Address &addr, Protocol::Message &message)
         {
            unit ().send (addr, message, nullptr);
         }

         void send (const Protocol::Address &addr, Protocol::Message &message, Transport::Link *link)
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
         uint16_t uid () const
         {
            return _uid;
         }

         protected:

         Service(Unit0 &unit):
            AbstractService (unit)
         {}

         protected:

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

         ServiceRole(Unit0 &unit):
            Parent (unit)
         {}

         //! \see Interface::role
         Interface::Role role () const
         {
            return _role;
         }
      };

      // Forward declaration.
      namespace DeviceInformation
      {
         struct Server;
      }

   }  // namespace Core

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

      Unit0(HF::IDevice &device):
         Base (device), interfaces (ITF (*this) ...)
      {
         device.add (this);
      }

      // =============================================================================

      DeviceInfo *device_info () const
      {
         return const_cast <DeviceInfo *>(&std::get <0>(interfaces));
      }

      DeviceInfo *device_info ()
      {
         return &std::get <0>(interfaces);
      }

      DeviceMgt *device_management () const
      {
         return const_cast <DeviceMgt *>(&std::get <1>(interfaces));
      }

      DeviceMgt *device_management ()
      {
         return &std::get <1>(interfaces);
      }

      AttrReporting *attribute_reporting () const
      {
         return const_cast <AttrReporting *>(&std::get <2>(interfaces));
      }

      AttrReporting *attribute_reporting ()
      {
         return &std::get <2>(interfaces);
      }

      // =============================================================================
      // IUnit API
      // =============================================================================

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

} // namespace HF

#endif /* HF_CORE_H */
