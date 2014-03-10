// =============================================================================
/*!
 * \file       tests/test_transport.cpp
 *
 * This file contains the implementation of the tests for the transport API.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

#include "test_helper.h"

#include "hanfun/transport.h"

using namespace HF;

// =============================================================================
// Helper Classes
// =============================================================================
namespace HF
{
   namespace Testing
   {
      struct Endpoint:public HF::Transport::Endpoint
      {
         typedef list <HF::Transport::Link *> links_t;

         links_t links;

         virtual void connected (HF::Transport::Link *link)
         {
            links.push_back (link);
         }

         virtual void disconnected (HF::Transport::Link *link)
         {
            links.remove (link);
         }

         void receive (Protocol::Packet &packet, ByteArray &payload, size_t offset)
         {
            UNUSED (packet);
            UNUSED (payload);
            UNUSED (offset);
         }
      };

      struct Endpoint2:public Endpoint
      {
         void receive (Protocol::Packet &packet, ByteArray &payload, size_t offset)
         {
            UNUSED (packet);
            UNUSED (payload);
            UNUSED (offset);
         }
      };

      struct Transport:public HF::Transport::Layer
      {
         typedef list <HF::Transport::Endpoint *> endpoints_t;

         typedef list <Link *> links_t;

         endpoints_t   endpoints;
         links_t       links;

         const HF::UID *_uid;

         Transport(const HF::UID *uid):
            _uid (uid)
         {}

         // =============================================================================
         // API
         // =============================================================================

         virtual void initialize ()
         {}

         void add (HF::Transport::Endpoint *ep)
         {
            endpoints.push_back (ep);
         }

         void initialize (Endpoint *ep)
         {
            initialize ();
            add (ep);
         }

         void remove (HF::Transport::Endpoint *ep = nullptr)
         {
            if (ep != nullptr)
            {
               endpoints.remove (ep);
            }
            else
            {
               endpoints.erase (endpoints.begin (), endpoints.end ());
            }
         }

         void destroy ()
         {
            /* *INDENT-OFF* */
            for_each(links.begin(), links.end(), [this](Link *link)
            {
               this->destroy_link(link);

            });
            /* *INDENT-ON* */

            remove (nullptr);
         }

         const HF::UID *uid () const
         {
            return _uid;
         }

         void create_link (HF::UID *uid)
         {
            Testing::Link *link = new Testing::Link (uid, this);
            links.push_back (link);

            /* *INDENT-OFF* */
            for_each(endpoints.begin(), endpoints.end(), [link](HF::Transport::Endpoint *ep)
            {
               ep->connected (link);
            });
            /* *INDENT-ON* */
         }

         void destroy_link (Link * &link)
         {
            /* *INDENT-OFF* */
            for_each(endpoints.begin(), endpoints.end(), [link](HF::Transport::Endpoint *ep)
            {
               ep->disconnected (link);
            });
            /* *INDENT-ON* */

            delete link;
            link = nullptr;
         }
      };

   }  // namespace Testing

} // namespace Testing

// =============================================================================
// Transport API Tests.
// =============================================================================

TEST_GROUP (Transport)
{
   Testing::Endpoint  ep1;
   Testing::Endpoint2 ep2;

   HF::URI tsp_uid;

   Testing::Transport *tsp;

   TEST_SETUP ()
   {
      tsp_uid = URI ("tsp://foobar@example.com");
      tsp     = new Testing::Transport (&tsp_uid);

      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      mock ("Link").checkExpectations ();
      tsp->destroy ();
      delete tsp;
      mock ().clear ();
   }
};

TEST (Transport, Initialize)
{
   LONGS_EQUAL (0, tsp->endpoints.size ());
   tsp->initialize (&ep1);
   LONGS_EQUAL (1, tsp->endpoints.size ());
   tsp->initialize (&ep2);
   LONGS_EQUAL (2, tsp->endpoints.size ());

   tsp->remove (&ep1);
   LONGS_EQUAL (1, tsp->endpoints.size ());
   tsp->initialize (&ep1);
   LONGS_EQUAL (2, tsp->endpoints.size ());

   tsp->destroy ();
   LONGS_EQUAL (0, tsp->endpoints.size ());
}

TEST (Transport, LinkSetup)
{
   tsp->initialize (&ep1);

   URI *uri = new URI ("dev://user1@example.com");

   tsp->create_link (uri);

   LONGS_EQUAL (1, ep1.links.size ());

   tsp->initialize (&ep2);

   uri = new URI ("dev://user2@example.com");
   tsp->create_link (uri);

   LONGS_EQUAL (2, ep1.links.size ());
   LONGS_EQUAL (1, ep2.links.size ());

   tsp->destroy_link (tsp->links.back ());

   LONGS_EQUAL (1, ep1.links.size ());
   LONGS_EQUAL (0, ep2.links.size ());

   tsp->destroy_link (tsp->links.front ());

   LONGS_EQUAL (0, ep1.links.size ());
   LONGS_EQUAL (0, ep2.links.size ());
}
