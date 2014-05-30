// =============================================================================
/*!
 * \file       apps/application.h
 *
 * This file contains the definitions for the HAN-FUN example applications.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#ifndef HF_APP_H
#define HF_APP_H

#include "hanfun.h"

namespace HF
{
   /*!
    * This namespace contains the declaration of the functions that implement the
    * HAN-FUN example applications.
    */
   namespace Application
   {
      /*!
       * This function implements the HAN-FUN example application for Base devices.
       *
       * This application provides commands to register, de-register, bind and un-bind
       * devices.
       *
       * @param [in] argc     number of elements in \c argv.
       * @param [in] argv     arguments received from the command line.
       * @param [in] layer    the HAN-FUN transport layer to use.
       *
       * @return  return value of the application.
       */
      int Base (int argc, char *argv[], HF::Transport::Layer &transport);

      /*!
       * This function implements the HAN-FUN example application for Node devices.
       *
       * This application provides commands to register, de-register the device.
       *
       * TODO This function is not implemented yet !
       *
       * @param [in] argc     number of elements in \c argv.
       * @param [in] argv     arguments received from the command line.
       * @param [in] layer    the HAN-FUN transport layer to use.
       *
       * @return  return value of the application.
       */
      int Node (int argc, char *argv[], HF::Transport::Layer *layer);

   }  // namespace Application

}  // namespace HF

#endif /* HF_APP_H */
