// =============================================================================
/*!
 * \file       inc/hanfun/gcc.h
 *
 * This file contains workarounds for the implementation on C++11 in older versions
 * of GCC.
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

#ifndef HF_GCC_H
#define HF_GCC_H

#if __GNUC__ > 4 ||                           \
   (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 ||   \
                      (__GNUC_MINOR__ == 6 && \
                       __GNUC_PATCHLEVEL__ >= 0)))
// Greater than or equal to 4.6.0
#else
   #define nullptr   NULL
#endif

#endif /* HF_GCC_H */
