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

// =============================================================================
// API
// =============================================================================

namespace HF
{
   class AbstractDevice;

   /*!
    * Namespace for the Core Services and Interfaces implementation.
    */
   namespace Core
   {
      class AbstractService:public IUnit, public AbstractInterface
      {
         //! Id number of this unit on the device.
         uint8_t id () const
         {
            return 0;
         }

         protected:

         AbstractDevice *_device;

         AbstractService(AbstractDevice *device):_device (device) {}

         void sendMessage (Message::Address &addr, Protocol::Message &message);
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

         Service(AbstractDevice *_device):AbstractService (_device) {}

         void sendMessage (Message::Address &addr, Message &message)
         {
            AbstractService::sendMessage (addr, message);
         }
      };

      /*!
       * Class template for all interfaces role implementations.
       */
      template<class Parent, Interface::Role _role>
      struct ServiceRole:public Parent
      {
         ServiceRole(AbstractDevice *device):Parent (device) {}

         //! \see Interface::role
         Interface::Role role () const
         {
            return _role;
         }
      };

      class DeviceManagementClient;

      class DeviceManagementServer;

   }  // namespace Core

}  // namespace HF


#endif /* HF_CORE_H */
