// =============================================================================
/*!
 * \file       inc/hanfun/interface.h
 *
 * This file contains the definitions common to all interfaces.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_INTERFACE_H
#define HF_INTERFACE_H

#include "hanfun/common.h"

#include "protocol.h"

using namespace HF::Protocol;

namespace HF
{
   /*!
    * Common interface for all Interfaces.
    */
   class Interface
   {
      public:

      /*!
       * Interface roles.
       */
      enum Role
      {
         CLIENT_ROLE = 0,   //!< Client Role.
         SERVER_ROLE = 1,   //!< Server Role.
      };

      /*!
       * Interfaces Unique Identifiers (UID).
       */
      enum UID
      {
         MAX_UID  = 0x7FFF               //!< Max interface UID value.
      };

   };

}  // namespace HF

#endif /* HF_INTERFACE_H */
