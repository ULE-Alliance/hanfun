// =============================================================================
/*!
 * \file       tests/test_protocol.cpp
 *
 * This file contains the implementation of the unit tests for the protocol
 * layer in the HAN-FUN specification.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#include "test_helper.h"

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

using namespace std;
using namespace HF;
using namespace HF::Protocol;

// =============================================================================
// Message / Address
// =============================================================================

TEST_GROUP (Message_Address)
{
   ByteArray expected_1;
   ByteArray expected_2;

   Protocol::Address addr;

   TEST_SETUP ()
   {
      uint8_t exp_data_1[] = {0x00, 0x00, 0x00, 0xFA, 0xAA, 0x55, 0x00, 0x00, 0x00};
      expected_1 = ByteArray (exp_data_1, sizeof(exp_data_1));

      uint8_t exp_data_2[] = {0x00, 0x00, 0x00, 0x7A, 0xAA, 0x55, 0x00, 0x00, 0x00};
      expected_2 = ByteArray (exp_data_2, sizeof(exp_data_2));
   }
};

TEST (Message_Address, Default)
{
   LONGS_EQUAL (0, addr.mod);
   LONGS_EQUAL (Protocol::BROADCAST_ADDR, addr.device);
   LONGS_EQUAL (Protocol::BROADCAST_UNIT, addr.unit);
}

TEST (Message_Address, Size)
{
   LONGS_EQUAL (3, addr.size ());
}

TEST (Message_Address, Pack)
{
   addr.mod    = 1;
   addr.device = 0x7AAA;
   addr.unit   = 0x55;

   ByteArray array (addr.size () + 6);

   size_t    wsize = addr.pack (array, 3);
   LONGS_EQUAL (addr.size (), wsize);

   CHECK_EQUAL (expected_1, array);

   addr.mod    = 0;
   addr.device = 0x7AAA;
   addr.unit   = 0x55;

   wsize       = addr.pack (array, 3);
   LONGS_EQUAL (addr.size (), wsize);

   CHECK_EQUAL (expected_2, array);
}

TEST (Message_Address, Unpack)
{
   size_t rsize = addr.unpack (expected_1, 3);
   LONGS_EQUAL (addr.size (), rsize);

   LONGS_EQUAL (1, addr.mod);
   LONGS_EQUAL (0x7AAA, addr.device);
   LONGS_EQUAL (0x55, addr.unit);

   rsize = addr.unpack (expected_2, 3);
   LONGS_EQUAL (addr.size (), rsize);

   LONGS_EQUAL (0, addr.mod);
   LONGS_EQUAL (0x7AAA, addr.device);
   LONGS_EQUAL (0x55, addr.unit);
}

// =============================================================================
// Message / Interface
// =============================================================================

TEST_GROUP (Message_Interface)
{
   ByteArray expected_1;
   ByteArray expected_2;

   Message::Interface addr;

   TEST_SETUP ()
   {
      uint8_t exp_data_1[] = {0x00, 0x00, 0x00,
                              0xFA,
                              0xAA, // Server Role / Interface UID.
                              0x55, // Interface Member.
                              0x00, 0x00, 0x00};
      expected_1 = ByteArray (exp_data_1, sizeof(exp_data_1));

      uint8_t exp_data_2[] = {0x00, 0x00, 0x00,
                              0x7A,
                              0xAA, // Client Role / Interface UID.
                              0x55, // Interface Member.
                              0x00, 0x00, 0x00};
      expected_2 = ByteArray (exp_data_2, sizeof(exp_data_2));
   }
};

TEST (Message_Interface, Size)
{
   LONGS_EQUAL (3, addr.size ());
}

TEST (Message_Interface, Pack)
{
   addr.role   = 1;
   addr.uid    = 0x7AAA;
   addr.member = 0x55;

   ByteArray array (addr.size () + 6);

   size_t    wsize = addr.pack (array, 3);
   LONGS_EQUAL (addr.size (), wsize);

   CHECK_EQUAL (expected_1, array);

   addr.role   = 0;
   addr.uid    = 0x7AAA;
   addr.member = 0x55;

   wsize       = addr.pack (array, 3);
   LONGS_EQUAL (addr.size (), wsize);

   CHECK_EQUAL (expected_2, array);
}

TEST (Message_Interface, Unpack)
{
   size_t rsize = addr.unpack (expected_1, 3);
   LONGS_EQUAL (addr.size (), rsize);

   LONGS_EQUAL (1, addr.role);
   LONGS_EQUAL (0x7AAA, addr.uid);
   LONGS_EQUAL (0x55, addr.member);

   rsize = addr.unpack (expected_2, 3);
   LONGS_EQUAL (addr.size (), rsize);

   LONGS_EQUAL (0, addr.role);
   LONGS_EQUAL (0x7AAA, addr.uid);
   LONGS_EQUAL (0x55, addr.member);
}

// =============================================================================
// Message
// =============================================================================

TEST_GROUP (Message)
{
   struct TestMessage:public Message
   {
      using Message::length;
   };

   TestMessage message;

   ByteArray   expected;

   TEST_SETUP ()
   {
      uint8_t exp_data[] = {0x00, 0x00, 0x00,
                            0xAA,                      // Application Reference.
                            Message::COMMAND_RESP_REQ, // Message Type.
                            0xFA, 0xAA, 0x55,          // Interface Address.
                            0x01, 0xAA,                // Payload length.
                            0x00, 0x00, 0x00};
      expected = ByteArray (exp_data, sizeof(exp_data));
   }

   TEST_TEARDOWN ()
   {}
};

TEST (Message, Size)
{
   LONGS_EQUAL (0, message.length);
   LONGS_EQUAL (7, message.size ());

   Testing::Payload payload (42);

   message.payload = &payload;
   LONGS_EQUAL (7 + 42, message.size ());
}

TEST (Message, Pack)
{
   message.reference  = 0xAA;
   message.type       = Message::COMMAND_RESP_REQ;

   message.itf.role   = Interface::SERVER_ROLE;
   message.itf.uid    = 0x7AAA;
   message.itf.member = 0x55;

   size_t size = message.size ();

   Testing::Payload payload (0xFFAA);
   message.payload = &payload;

   ByteArray array (size + 6);

   size_t    wsize = message.pack (array, 3);
   LONGS_EQUAL (size + (0xFFAA & Protocol::MAX_PAYLOAD), wsize);

   CHECK_EQUAL (expected, array);
}

TEST (Message, Unpack)
{
   size_t rsize = message.unpack (expected, 3);
   LONGS_EQUAL (7, rsize);

   LONGS_EQUAL (0xAA, message.reference);
   LONGS_EQUAL (Message::COMMAND_RESP_REQ, message.type);

   LONGS_EQUAL (Interface::SERVER_ROLE, message.itf.role);
   LONGS_EQUAL (0x7AAA, message.itf.uid);
   LONGS_EQUAL (0x55, message.itf.member);

   LONGS_EQUAL (0x01AA, message.length);
}

// =============================================================================
// Packet
// =============================================================================

TEST_GROUP (Packet)
{
   class TestPayload:public Serializable
   {
      public:

      uint8_t data;

      size_t size () const
      {
         return sizeof(data);
      }

      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         size_t start = offset;

         offset += array.write (offset, this->data);

         return offset - start;
      }

      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         size_t start = offset;

         offset += array.read (offset, this->data);

         return offset - start;
      }
   };

   TestPayload *payload;
   Packet *packet;

   ByteArray expected;

   TEST_SETUP ()
   {
      uint8_t data[] = {0x00, 0x00, 0x00,
                        0x7A, 0xAA,           // Source device Address.
                        0x55,                 // Source unit.
                        0xF5, 0x55,           // Destination device Address.
                        0xAA,                 // Destination unit.
                        0x00, 0x00,           // Transport header.
                        0xCC,                 // Application Reference.
                        Message::COMMAND_REQ, // Message Type.
                        0xFA, 0xAA, 0x55,     // Interface Address.
                        0x00, 0x01,           // Payload length.
                        0xAB,                 // Payload data.
                        0x00, 0x00, 0x00};

      expected = ByteArray (data, sizeof(data));

      payload  = new TestPayload ();
      packet   = new Packet ();
   }

   TEST_TEARDOWN ()
   {
      delete packet;
      delete payload;
   }
};

TEST (Packet, Size)
{
   Protocol::Address addr;

   size_t size = 2 * addr.size () +       // Network address
                 sizeof(uint16_t) +       // Transport header.
                 packet->message.size (); // Payload size.

   LONGS_EQUAL (size, packet->size ());
}

TEST (Packet, Pack)
{
   packet->source.mod         = Protocol::Address::DEVICE_ADDR;
   packet->source.device      = 0x7AAA;
   packet->source.unit        = 0x55;

   packet->destination.mod    = Protocol::Address::GROUP_ADDR;
   packet->destination.device = 0x7555;
   packet->destination.unit   = 0xAA;

   packet->message.reference  = 0xCC;
   packet->message.type       = Message::COMMAND_REQ;
   packet->message.itf.role   = 1;
   packet->message.itf.uid    = 0x7AAA;
   packet->message.itf.member = 0x55;

   packet->message.payload    = payload;

   payload->data              = 0xAB;

   ByteArray array (packet->size () + 6);

   size_t    wsize = packet->pack (array, 3);
   LONGS_EQUAL (6 + 2 + 7 + 1, wsize);

   CHECK_EQUAL (expected, array);
}

TEST (Packet, Unpack)
{
   size_t rsize = packet->unpack (expected, 3);
   LONGS_EQUAL (6 + 2 + 7, rsize);

   LONGS_EQUAL (Protocol::Address::DEVICE_ADDR, packet->source.mod);
   LONGS_EQUAL (0x7AAA, packet->source.device);
   LONGS_EQUAL (0x55, packet->source.unit);

   LONGS_EQUAL (Protocol::Address::GROUP_ADDR, packet->destination.mod);
   LONGS_EQUAL (0x7555, packet->destination.device);
   LONGS_EQUAL (0xAA, packet->destination.unit);

   LONGS_EQUAL (0xCC, packet->message.reference);
   LONGS_EQUAL (Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL (1, packet->message.itf.role);
   LONGS_EQUAL (0x7AAA, packet->message.itf.uid);
   LONGS_EQUAL (0x55, packet->message.itf.member);

   payload->unpack (expected, 3 + rsize);

   LONGS_EQUAL (0xAB, payload->data);
}

// =============================================================================
// Response
// =============================================================================

TEST_GROUP (Response)
{
   Response  *response;

   ByteArray expected;

   TEST_SETUP ()
   {
      /* *INDENT-OFF* */
      uint8_t data[] = { 0x00, 0x00, 0x00,
                         Result::FAIL_ID,
                         0x00, 0x00, 0x00 };
      /* *INDENT-ON* */

      expected = ByteArray (data, sizeof(data));
      response = new Response ();
   }

   TEST_TEARDOWN ()
   {
      delete response;
   }
};

TEST (Response, Size)
{
   LONGS_EQUAL (1, response->size ());
}

TEST (Response, Pack)
{
   response->code = Result::FAIL_ID;

   size_t size = response->size ();

   ByteArray array (response->size () + 6);

   size_t    wsize = response->pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);
}

TEST (Response, Unpack)
{
   size_t rsize = response->unpack (expected, 3);
   LONGS_EQUAL (1, rsize);  // Response code.

   LONGS_EQUAL (Result::FAIL_ID, response->code);
}
