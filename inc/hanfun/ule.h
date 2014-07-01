// =============================================================================
/*!
 * \file       inc/hanfun/ule.h
 *
 * This file contains the common definitions for Bithium's HAN-FUN transport layer
 * over ULE.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_ULE_H
#define HF_ULE_H

#include "hanfun.h"

namespace HF
{
   /*!
    * Top level namespace for the HAN-FUN's ULE Transport layer.
    */
   namespace ULE
   {
      /*!
       * Enable/Disable registration mode.
       *
       * @param mode    true - enable registration;
       *                false - disable registration;
       *
       * @retval  true if the operation was successful;
       * @retval  false otherwise.
       */
      bool Registration (bool mode);

      /*!
       * De-register the device with the given HAN-FUN Address.
       *
       * @param address    HAN-FUN address of the device to de-register.
       */
      void Deregister (uint16_t address);

   }  // namespace ULE

}  // namespace HF

#endif /* HF_ULE_H */
