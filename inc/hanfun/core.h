// =============================================================================
/*!
 * \file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * \version    0.3.2
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

         using Units::AbstractUnit::sendMessage;
      };

      /*!
       * This is the interface common to all Services.
       */
      struct IService:virtual public HF::Interface
      {
         //! The device this unit is associated with.
         virtual Unit0 &unit () = 0;
      };

      struct AbstractService:virtual public IService, virtual public HF::Interfaces::AbstractInterface
      {
         Unit0 &unit ()
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


         protected:

         Unit0 &_unit;

         AbstractService(Unit0 &unit):_unit (unit)
         {}

         //! \see AbstractInterface::attributes
         HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            return AbstractInterface::attributes (pack_id);
         }
         void sendMessage (Protocol::Address &addr, Protocol::Message &message)
         {
            unit ().sendMessage (addr, message, nullptr);
         }

         void sendMessage (Protocol::Address &addr, Protocol::Message &message, Transport::Link *link)
         {
            unit ().sendMessage (addr, message, link);
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
         static_assert (is_base_of <HF::Core::AbstractService, Parent>::value,
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
         struct Interface;
      }

   }  // namespace Core

   template<typename Base, typename... ITF>
   class Unit0:public Base
   {
      static_assert (is_base_of <HF::Core::Unit0, Base>::value,
                     "Base must be of type HF::Core::Unit0");

      public:

      typedef tuple <ITF...> interfaces_t;

      interfaces_t interfaces;

      typedef typename tuple_element <0, decltype (interfaces)>::type DeviceInfo;

      static_assert (is_base_of <HF::Core::DeviceInformation::Interface, DeviceInfo>::value,
                     "DeviceInfo must be of type HF::Core::DeviceInformation::Interface");

      typedef typename tuple_element <1, decltype (interfaces)>::type DeviceMgt;

      Unit0(HF::IDevice &device):
         Base (device), interfaces (ITF (*this) ...)
      {
         device.add (this);
      }

      // =============================================================================

      DeviceInfo *info () const
      {
         return const_cast <DeviceInfo *>(&get <0>(interfaces));
      }

      DeviceInfo *info ()
      {
         return &get <0>(interfaces);
      }

      DeviceMgt *device_management () const
      {
         return const_cast <DeviceMgt *>(&get <1>(interfaces));
      }

      DeviceMgt *device_management ()
      {
         return &get <1>(interfaces);
      }

      // =============================================================================
      // IUnit API
      // =============================================================================

      Common::Result handle (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
      {
         return handle_proxy <0, ITF...>(packet, payload, offset);
      }


      // =============================================================================

      private:

      template<uint8_t N, typename Head, typename... Tail>
      Common::Result handle_proxy (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
      {
         static_assert (is_base_of <HF::Core::IService, Head>::value,
                        "Head must be of type HF::Core::IService");

         Head &head = get <N>(interfaces);

         if (head.uid () == packet.message.itf.id)
         {
            return head.handle (packet, payload, offset);
         }
         else
         {
            return handle_proxy <N + 1, Tail...>(packet, payload, offset);
         }
      }

      template<uint8_t N>
      HF::Common::Result handle_proxy (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
      {
         UNUSED (packet);
         UNUSED (payload);
         UNUSED (offset);
         return Common::Result::FAIL_SUPPORT;
      }
   };

} // namespace HF

#endif /* HF_CORE_H */
