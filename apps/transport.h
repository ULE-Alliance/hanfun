// =============================================================================
/*!
 * \file       apps/transport.h
 *
 * This file contains the declarations of the transport layer over libuv.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
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

         HF::UID::UID *_uid;

         public:

         Link(Transport *tsp, uv_stream_s *stream):
            HF::Transport::AbstractLink (), tsp (tsp), stream (stream), _uid (nullptr)
         {
            stream->data = this;
         }

         virtual ~Link()
         {
            delete _uid;
         }

         void send (HF::Common::ByteArray &array);

         HF::UID::UID const *uid () const
         {
            return _uid;
         }

         void uid (HF::UID::UID *_uid)
         {
            if (this->_uid != nullptr)
            {
               delete this->_uid;
            }

            this->_uid = _uid;
         }

         HF::Transport::Layer const *transport () const
         {
            return tsp;
         }
      };

   }  // namespace Application

}  // namespace HF

#endif /* HF_TRANSPORT_H */
