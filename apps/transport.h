// =============================================================================
/*!
 * \file       apps/transport.h
 *
 * This file contains the declarations of the transport layer over libuv.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
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
      class Transport:public HF::Transport::Layer
      {
         protected:

         std::forward_list <HF::Transport::Endpoint *> endpoints;
         std::forward_list <HF::Transport::Link *> links;

         HF::UID::UID *_uid;

         uv_tcp_t socket;

         public:

         void initialize ();

         void destroy ();

         void add (HF::Transport::Endpoint *ep)
         {
            LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

            endpoints.push_front (ep);
            std::for_each (links.begin (), links.end (), [ep](HF::Transport::Link *link) {
                              ep->connected (link);
                           }
                          );
         }

         void remove (HF::Transport::Endpoint *ep = nullptr)
         {
            LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

            if (ep != nullptr)
            {
               endpoints.remove (ep);
            }
            else
            {
               endpoints.clear ();
            }
         }

         void add (HF::Transport::Link *link)
         {
            LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

            links.push_front (link);
            std::for_each (endpoints.begin (), endpoints.end (), [link](HF::Transport::Endpoint *ep) {
                              ep->connected (link);
                           }
                          );
         }

         void remove (HF::Transport::Link *link = nullptr)
         {
            LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

            if (link != nullptr)
            {
               std::for_each (endpoints.begin (), endpoints.end (), [link](HF::Transport::Endpoint *ep) {
                                 ep->disconnected (link);
                              }
                             );

               links.remove (link);
            }
            else
            {
               std::for_each (endpoints.begin (), endpoints.end (), [this](HF::Transport::Endpoint *ep) {
                                 std::for_each (links.begin (), links.end (), [ep](HF::Transport::Link *link) {
                                                   ep->disconnected (link);
                                                }
                                               );
                              }
                             );

               links.clear ();
            }
         }

         void receive (HF::Transport::Link *link, HF::Common::ByteArray &payload)
         {
            LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

            Protocol::Packet packet;
            packet.link = link;

            size_t offset = packet.unpack (payload);

            std::for_each (endpoints.begin (), endpoints.end (),
                           [&packet, &payload, &offset](HF::Transport::Endpoint *ep)
                           {
                              ep->receive (packet, payload, offset);
                           }
                          );
         }

         const HF::UID::UID *uid () const
         {
            return _uid;
         }

         void uid (HF::UID::UID *_uid)
         {
            this->_uid = _uid;
         }
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
