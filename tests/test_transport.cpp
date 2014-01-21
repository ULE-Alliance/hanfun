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

      struct Link: public HF::Transport::Link
      {
         HF::UID _uid;
         HF::Transport * tsp;

         Link (HF::UID &uid, HF::Transport * tsp) :
               _uid(uid), tsp(tsp)
         {
         }

         virtual size_t write (ByteArray * array, size_t offset, size_t size)
         {
            UNUSED(array);
            UNUSED(offset);
            UNUSED(size);

            return mock("Link").actualCall("write").returnValue().getIntValue();
         }

         virtual size_t read (ByteArray * array, size_t offset, size_t size)
         {
            UNUSED(array);
            UNUSED(offset);
            UNUSED(size);

            return mock("Link").actualCall("read").returnValue().getIntValue();
         }

         virtual size_t available ()
         {
            return mock("Link").actualCall("available").returnValue().getIntValue();
         }

         virtual HF::UID uid ()
         {
            return _uid;
         }

         virtual HF::Transport * transport ()
         {
            return tsp;
         }
      };

      struct Endpoint: public HF::Transport::Endpoint
      {
         typedef list<HF::Transport::Link *> links_t;

         links_t links;

         virtual void connected (HF::Transport::Link * link)
         {
            links.push_back(link);
         }

         virtual void disconnected (HF::Transport::Link * link)
         {
            links.remove(link);
         }

         virtual void receive (HF::Transport::Link * link)
         {
            ByteArray array(link->available());
            link->read(&array, 0, array.size() );
         }
      };

      struct Endpoint2: public Endpoint
      {
         virtual void receive (HF::Transport::Link * link)
         {
            ByteArray array(link->available());
            link->read(&array, 4);
         }
      };

      struct Transport: public HF::Transport
      {
         typedef list<HF::Transport::Endpoint *> endpoints_t;

         typedef list<Link *> links_t;

         endpoints_t endpoints;
         links_t links;

         HF::UID _uid;

         Transport (const HF::UID &uid) :
               _uid(uid)
         {
         }

         ~Transport ()
         {
            for_each(links.begin(), links.end(), [endpoints](Link * link)
            {
               for_each( endpoints.begin(), endpoints.end(), [link](Endpoint * ep)
                     {
                        ep->disconnected(link);
                     });

               delete link;
            });
         }

         virtual void initialize (HF::Transport::Endpoint * ep)
         {
            endpoints.push_back(ep);
         }

         virtual void destroy (HF::Transport::Endpoint * ep = nullptr)
         {
            if (ep != nullptr)
            {
               endpoints.remove(ep);
            }
            else
            {
               endpoints.erase(endpoints.begin(), endpoints.end());
            }
         }

         virtual HF::UID uid () const
         {
            return _uid;
         }

         virtual void create_link (HF::UID &uid)
         {
            Testing::Link * link = new Testing::Link(uid, this);
            links.push_back(link);
            for (Endpoint *ep : endpoints)
            {
               ep->connected(link);
            }
         }

         virtual void destroy_link (Link * link)
         {
            for_each(endpoints.begin(), endpoints.end(), [link](Endpoint * ep)
            {
               ep->disconnected(link);
            });
         }
      };

   }  // namespace Testing

} // namespace Testing

// =============================================================================
// Transport API Tests.
// =============================================================================

TEST_GROUP(Transport)
{
   Testing::Endpoint  ep1;
   Testing::Endpoint2 ep2;

   HF::URI tsp_uid;

   Testing::Transport * tsp;

   TEST_SETUP()
   {
      tsp_uid = URI("tsp://foobar@example.com");
      tsp = new Testing::Transport(tsp_uid);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock("Link").checkExpectations();
      delete tsp;
      mock().clear();
   }
};

TEST(Transport, Initialize)
{
   LONGS_EQUAL(0, tsp->endpoints.size());
   tsp->initialize(&ep1);
   LONGS_EQUAL(1, tsp->endpoints.size());
   tsp->initialize(&ep2);
   LONGS_EQUAL(2, tsp->endpoints.size());

   tsp->destroy(&ep1);
   LONGS_EQUAL(1, tsp->endpoints.size());
   tsp->initialize(&ep1);
   LONGS_EQUAL(2, tsp->endpoints.size());

   tsp->destroy();
   LONGS_EQUAL(0, tsp->endpoints.size());
}

TEST(Transport, LinkSetup)
{
   tsp->initialize(&ep1);

   URI uri = URI("dev://user1@example.com");

   tsp->create_link(uri);

   LONGS_EQUAL(1, ep1.links.size());

   tsp->initialize(&ep2);

   uri = URI("dev://user2@example.com");
   tsp->create_link(uri);

   LONGS_EQUAL(2, ep1.links.size());
   LONGS_EQUAL(1, ep2.links.size());

   tsp->destroy_link(tsp->links.back());

   LONGS_EQUAL(1, ep1.links.size());
   LONGS_EQUAL(0, ep2.links.size());

   tsp->destroy_link(tsp->links.front());

   LONGS_EQUAL(0, ep1.links.size());
   LONGS_EQUAL(0, ep2.links.size());
}

TEST(Transport, Receive)
{
   tsp->initialize(&ep1);

   URI uri = URI("dev://user1@example.com");

   tsp->create_link(uri);

   mock("Link").strictOrder();
   mock("Link").expectOneCall("available").andReturnValue(10);
   mock("Link").expectOneCall("read").andReturnValue(10);
   ep1.receive( tsp->links.front() );
}

TEST(Transport, Receive2)
{
   tsp->initialize(&ep2);

   URI uri = URI("dev://user1@example.com");

   tsp->create_link(uri);

   mock("Link").strictOrder();
   mock("Link").expectNCalls( 2, "available" );
   mock("Link").expectOneCall("read").andReturnValue(10);
   ep2.receive( tsp->links.front() );
}
