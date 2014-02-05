// =============================================================================
/*!
 * \file			inc/hanfun/core.h
 *
 * This file contains the forward declarations of the core services and interfaces
 * implementing classes.
 *
 * \author		Filipe Alves <filipe.alves@bithium.com>
 *
 * \version		x.x.x
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
   namespace Core
   {
      /*!
       * This is the interface common to all Services.
       */
      struct IService:public HF::Interface
      {
         //! The device this unit is associated with.
         virtual IDevice *device () = 0;
      };

      struct AbstractService:public IService, public HF::AbstractInterface, public HF::AbstractUnit
      {
         //! Id number of this unit on the device.
         uint8_t id () const
         {
            return 0;
         }

         //! \see HF::Interface::handle
         virtual Result handle (Protocol::Packet &packet, ByteArray &payload, size_t offset)
         {
            return AbstractInterface::handle (packet, payload, offset);
         }

         IDevice *device ()
         {
            return HF::AbstractUnit::device ();
         }

         virtual void periodic (uint32_t time)
         {
            UNUSED (time);
         }

         protected:

         AbstractService(IDevice *_device):
            HF::AbstractUnit (_device)
         {}
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

         void sendMessage (Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::sendMessage (addr, message);
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

   }  // namespace Core

} // namespace HF

#endif /* HF_CORE_H */
