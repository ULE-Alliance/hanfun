// =============================================================================
/*!
 * \file       inc/hanfun/debug.h
 *
 * This file contains the prototypes of the debug functionality in HAN-FUN.
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

#ifndef HF_DEBUG_H
#define HF_DEBUG_H

#include <iostream>

// =============================================================================
// Defines
// =============================================================================

#define NL                   std::endl;std::cout.clear ();std::cerr.clear ()

#define HF_LOG_LEVEL_NONE    0
#define HF_LOG_LEVEL_ERROR   1
#define HF_LOG_LEVEL_WARN    2
#define HF_LOG_LEVEL_INFO    3
#define HF_LOG_LEVEL_DEBUG   4
#define HF_LOG_LEVEL_TRACE   5

#ifndef HF_LOG_LEVEL
   #define HF_LOG_LEVEL      HF_LOG_LEVEL_INFO
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_ERROR
   #define ERROR             std::cerr.clear ();std::cerr << "[ERROR] "
#else
   #define ERROR             std::cerr.setstate (std::ios_base::badbit);std::cerr
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_WARN
   #define WARN              std::cerr.clear ();std::cerr << "[WARN ] "
#else
   #define WARN              std::cerr.setstate (std::ios_base::badbit);std::cerr
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_INFO
   #define INFO              std::cout.clear ();std::cout << "[INFO ] "
#else
   #define INFO              std::cout.setstate (std::ios_base::badbit);std::cout
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_DEBUG
   #define DEBUG             std::cout.clear ();std::cout << "[DEBUG] "
#else
   #define DEBUG             std::cout.setstate (std::ios_base::badbit);std::cout
#endif

#if HF_LOG_LEVEL >= HF_LOG_LEVEL_TRACE
   #define TRACE             std::cout.clear ();std::cout << "[TRACE] "
#else
   #define TRACE             std::cout.setstate (std::ios_base::badbit);std::cout
#endif

#define APP                  std::cout.clear ();std::cout

#define LOG(X)   X

// =============================================================================
// Stream Helpers
// =============================================================================

// Forward declarations
namespace HF
{
   namespace Common
   {
      class ByteArray;

   }  // namespace Common

   namespace UID
   {
      class UID;

   }  // namespace UID

   namespace Protocol
   {
      struct Packet;

   }  // namespace Protocol

}  // namespace HF

std::ostream &operator <<(std::ostream &stream, const HF::Common::ByteArray &array);

std::ostream &operator <<(std::ostream &stream, const HF::UID::UID &uid);

std::ostream &operator <<(std::ostream &stream, const HF::Protocol::Packet &packet);

#endif /* HF_DEBUG_H */
