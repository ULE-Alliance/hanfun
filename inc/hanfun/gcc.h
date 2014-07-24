// =============================================================================
/*!
 * \file       inc/hanfun/gcc.h
 *
 * This file contains workarounds for the implementation on C++11 in older versions
 * of GCC.
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
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
