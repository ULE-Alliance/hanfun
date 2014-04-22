// =============================================================================
/*!
 * \file       inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright	Copyright &copy; &nbsp; 2013 Bithium S.A.
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
       * This is the interface common to all Services.
       */
      struct IService:virtual public HF::Interface
      {
         //! The device this unit is associated with.
         virtual IDevice *device () = 0;
      };

      struct AbstractService:virtual public IService, virtual public HF::Interfaces::AbstractInterface,
         public HF::Units::AbstractUnit
      {
         //! Id number of this unit on the device.
         uint8_t id () const
         {
            return 0;
         }

         //! \see HF::Interface::handle
         virtual Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            return Interfaces::AbstractInterface::handle (packet, payload, offset);
         }

         IDevice *device ()
         {
            return HF::Units::AbstractUnit::device ();
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

         AbstractService(IDevice *_device):
            HF::Units::AbstractUnit (_device)
         {}

         //! \see AbstractInterface::attributes
         virtual HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            return AbstractInterface::attributes (pack_id);
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

         Service(IDevice *_device):
            AbstractService (_device)
         {}

         using AbstractUnit::sendMessage;

         //! \see AbstractInterface::sendMessage
         void sendMessage (Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::sendMessage (addr, message, nullptr);
         }

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

         ServiceRole(IDevice *device):
            Parent (device)
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

   template<typename... ITF>
   class Unit0:public Units::AbstractUnit
   {
      typedef tuple <ITF...> interfaces_t;

      interfaces_t interfaces;

      public:

      typedef typename tuple_element <0, decltype (interfaces)>::type DeviceInfo;

      static_assert (is_base_of <HF::Core::DeviceInformation::Interface, DeviceInfo>::value,
                     "DeviceInfo must be of type HF::Core::DeviceInformation::Interface");

      typedef typename tuple_element <1, decltype (interfaces)>::type DeviceMgt;

      Unit0(HF::IDevice *device):
         Units::AbstractUnit (device), interfaces (ITF (device) ...)
      {
         device->add (this);
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

      DeviceMgt *management () const
      {
         return const_cast <DeviceMgt *>(&get <1>(interfaces));
      }

      DeviceMgt *management ()
      {
         return &get <1>(interfaces);
      }

      // =============================================================================
      // IUnit API
      // =============================================================================

      HF::Result handle (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
      {
         return handle_proxy <0, ITF...>(packet, payload, offset);
      }

      uint16_t uid () const
      {
         return 0;
      }

      //! \see IUnit::id
      uint8_t id () const
      {
         return 0;
      }

      // =============================================================================

      private:

      template<uint8_t N, typename Head, typename... Tail>
      HF::Result handle_proxy (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
      {
         // FIXME Check Head is IService.
         Head *head = &get <N>(interfaces);

         if (head->uid () == packet.message.itf.uid)
         {
            return head->handle (packet, payload, offset);
         }
         else
         {
            return handle_proxy <N + 1, Tail...>(packet, payload, offset);
         }
      }

      template<uint8_t N>
      HF::Result handle_proxy (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
      {
         UNUSED (packet);
         UNUSED (payload);
         UNUSED (offset);
         return Result::FAIL_SUPPORT;
      }
   };

} // namespace HF

#endif /* HF_CORE_H */
