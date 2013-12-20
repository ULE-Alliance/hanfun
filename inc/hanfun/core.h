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
      class AbstractService
      {
         protected:

         AbstractDevice *device;

         AbstractService(AbstractDevice *_device):device (_device) {}

         void sendMessage (Message::Address &addr, Message &message);
      };

      /*!
       * Class template for all core services implementations.
       */
      template<Interface::UID _uid>
      class Service:public Interfaces::Base <_uid>, AbstractService
      {
         protected:

         Service(AbstractDevice *_device):AbstractService (_device) {}

         void sendMessage (Message::Address &addr, Message &message)
         {
            AbstractService::sendMessage (addr, message);
         }
      };

   }  // namespace Core

}  // namespace HF


#endif /* HF_CORE_H */
