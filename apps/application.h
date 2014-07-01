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

// =============================================================================
// Defines
// =============================================================================

// Debug Macros.

#define NL     std::endl

#define HF_LOG_LEVEL_NONE     0
#define HF_LOG_LEVEL_ERROR    1
#define HF_LOG_LEVEL_WARN     2
#define HF_LOG_LEVEL_INFO     3
#define HF_LOG_LEVEL_DEBUG    4
#define HF_LOG_LEVEL_TRACE    5

#ifndef HF_LOG_LEVEL
#define HF_LOG_LEVEL    HF_LOG_LEVEL_INFO
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_ERROR
#define ERROR  std::cerr.setstate(std::ios_base::goodbit); std::cerr << "[ERROR] "
#else
#define ERROR  std::cerr.setstate(std::ios_base::badbit); std::cerr
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_WARN
#define WARN   std::cerr.setstate(std::ios_base::goodbit); std::cerr << "[WARN ] "
#else
#define WARN   std::cerr.setstate(std::ios_base::badbit); std::cerr
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_INFO
#define INFO   std::cout.setstate(std::ios_base::goodbit); std::cout << "[INFO ] "
#else
#define INFO   std::cout.setstate(std::ios_base::badbit); std::cout
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_DEBUG
#define DEBUG  std::cout.setstate(std::ios_base::goodbit); std::cout << "[DEBUG] "
#else
#define DEBUG  std::cout.setstate(std::ios_base::badbit); std::cout
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_TRACE
#define TRACE  std::cout.setstate(std::ios_base::goodbit); std::cout << "[TRACE] "
#else
#define TRACE  std::cout.setstate(std::ios_base::badbit); std::cout
#endif

#define APP    std::cout

#define LOG(X) X

// =============================================================================
// API
// =============================================================================

namespace HF
{
   /*!
    * This namespace contains the declaration of the functions that implement the
    * HAN-FUN example applications.
    */
   namespace Application
   {
      /*!
       * Initialize the application.
       *
       * @param [in] transport   reference to the transport layer to use.
       */
      void Initialize (HF::Transport::Layer &transport);

      /*!
       * Handle the command.
       *
       * @param [in] command  string containing the command issued by the user.
       *
       * @retval  true  quit command requested,
       * @retval  false otherwise.
       */
      bool Handle (std::string command);

   }  // namespace Application

}  // namespace HF

#endif /* HF_APP_H */
