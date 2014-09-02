// =============================================================================
/*!
 * \file       apps/transport.h
 *
 * This file contains the declarations of the transport layer over libuv.
 *
 * \version    1.0.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_APP_TRANSPORT_H
#define HF_APP_TRANSPORT_H

#include <forward_list>

#include "uv.h"

#include "hanfun.h"

// =============================================================================
// Defines
// =============================================================================

#define CHECK_STATUS()                                  \
   if (status != 0)                                     \
   {                                                    \
      print_error (uv_last_error (uv_default_loop ())); \
      exit (-1);                                        \
   }

// =============================================================================
// API
// =============================================================================

namespace HF
{
   namespace Application
   {
      class Transport:public HF::Devices::Concentrator::Transport
      {
         protected:

         uv_tcp_t socket;

         public:

         virtual ~Transport() {}

         void initialize ();

         void destroy ();
      };

      class Link:public HF::Transport::AbstractLink
      {
         protected:

         Transport *tsp;

         uv_stream_s *stream;

         public:

         Link(Transport *_tsp, uv_stream_s *stream):
            HF::Transport::AbstractLink (), tsp (_tsp), stream (stream)
         {
            stream->data = this;
         }

         virtual ~Link()
         {}

         void send (HF::Common::ByteArray &array);

         Transport *transport () const
         {
            return tsp;
         }
      };

   }  // namespace Application

}  // namespace HF

#endif /* HF_TRANSPORT_H */
