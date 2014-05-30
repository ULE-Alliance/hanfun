// =============================================================================
/*!
 * \file       tests/interfaces/test_level_control.cpp
 *
 * This is file contains the unit tests for the Level Control Interface implementation.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/level_control.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Interfaces;

using namespace HF::Testing;

// =============================================================================
// LevelControl
// =============================================================================

TEST_GROUP (LevelControl)
{
   class TestLevelControl:public InterfaceParentHelper <LevelControl::Base>
   {};

   TestLevelControl interface;
};

TEST (LevelControl, UID)
{
   CHECK_EQUAL (HF::Interface::LEVEL_CONTROL, interface.uid ());
}

// =============================================================================
// LevelControl / Messages
// =============================================================================

//! LevelControl::Message test group.
TEST_GROUP (LevelControl_Message)
{
   LevelControl::Message *message;

   ByteArray expected;

   TEST_SETUP ()
   {
      message  = new LevelControl::Message ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0xAA,  // Level value.
                            0x00, 0x00, 0x00};
   }

   TEST_TEARDOWN ()
   {
      delete message;
   }

};

//! \test LevelControl::Message::size should return the correct value.
TEST (LevelControl_Message, Size)
{
   LONGS_EQUAL (sizeof(uint8_t), message->size ());

   BYTES_EQUAL (0, message->level);
}

//! \test LevelControl::Message::pack should write the correct values to the ByteArray.
TEST (LevelControl_Message, Pack)
{
   message->level = 0xAA;

   ByteArray array (message->size () + 6);

   size_t    wsize = message->pack (array, 3);

   LONGS_EQUAL (message->size (), wsize);

   CHECK_EQUAL (expected, array);
}

//! \test LevelControl::Message::unpack should read the correct values from the ByteArray.
TEST (LevelControl_Message, Unpack)
{
   size_t rsize = message->unpack (expected, 3);

   LONGS_EQUAL (message->size (), rsize);

   BYTES_EQUAL (0xAA, message->level);
}

// =============================================================================
// LevelControlClient
// =============================================================================

//! Test Group for LevelControlClient interface class.
TEST_GROUP (LevelControlClient)
{
   class TestLevelControlClient:public InterfaceHelper <LevelControl::Client>
   {};

   TestLevelControlClient client;
   Protocol::Address addr;

   TEST_SETUP ()
   {
      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

//! \test Should send an LevelControl::SET_LEVEL_CMD message.
TEST (LevelControlClient, Level)
{
   mock ("Interface").expectOneCall ("sendMessage");

   client.level (addr, 0x42);

   mock ("Interface").checkExpectations ();

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL (client.uid (), client.sendMsg.itf.id);
   LONGS_EQUAL (LevelControl::SET_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Message msg;

   msg.unpack (client.sendMsg.payload);

   BYTES_EQUAL (0x42, msg.level);
}

// =============================================================================
// LevelControlServer Tests
// =============================================================================;

TEST_GROUP (LevelControlServer)
{
   class TestLevelControlServer:public InterfaceHelper <LevelControl::Server>
   {
      public:

      void level_change (uint8_t new_level)
      {
         mock ("LevelControlServer").actualCall ("level_change");
         LevelControl::Server::level_change (new_level);
      }
   };

   TestLevelControlServer server;

   Protocol::Packet packet;
   ByteArray expected;

   TEST_SETUP ()
   {
      mock ("LevelControlServer").ignoreOtherCalls ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0xAA,  // Level value.
                            0x00, 0x00, 0x00};

      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server.uid ();
      packet.message.itf.member = LevelControl::SET_LEVEL_CMD;

      packet.message.length     = expected.size ();
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

TEST (LevelControlServer, Level)
{
   CHECK_EQUAL (0, server.level ());
   server.level (42);
   CHECK_EQUAL (42, server.level ());
}

//! \test Should handle valid message.
TEST (LevelControlServer, Handle_Valid_Message)
{
   mock ("LevelControlServer").expectOneCall ("level_change");

   Result result = server.handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   LONGS_EQUAL (0xAA, server.level ());

   mock ("LevelControlServer").checkExpectations ();
}

//! \test Should not handle message from invalid role.
TEST (LevelControlServer, Handle_Invalid_Role)
{
   packet.message.itf.role = HF::Interface::CLIENT_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, server.handle (packet, expected, 3));
}

//! \test Should not handle message from invalid interface UID.
TEST (LevelControlServer, Handle_Invalid_UID)
{
   packet.message.itf.id = server.uid () + 1;

   CHECK_EQUAL (Result::FAIL_ID, server.handle (packet, expected, 3));
}

//! \test Should not handle message with invalid payload size.
TEST (LevelControlServer, Handle_Invalid_Payload_Size)
{
   LevelControl::Message level_msg;
   packet.message.length = level_msg.size () - 1;

   CHECK_EQUAL (Result::FAIL_ARG, server.handle (packet, expected, 3));
}

//! \test Should not handle message with not enough payload / offset.
TEST (LevelControlServer, Handle_Invalid_Payload)
{
   CHECK_EQUAL (Result::FAIL_ARG, server.handle (packet, expected, 10));
}

//! \test Should return attribute.
TEST (LevelControlServer, Attribute)
{
   HF::Attributes::IAttribute *attr = server.attribute (LevelControl::__LAST_ATTR__ + 1);

   CHECK_TRUE (attr == nullptr);

   attr = server.attribute (LevelControl::LEVEL_ATTR);

   CHECK_TRUE (attr != nullptr);

   LONGS_EQUAL (LevelControl::LEVEL_ATTR, attr->uid ());
   CHECK_FALSE (attr->isWritable ());

   LONGS_EQUAL (server.uid (), attr->interface ());

   delete attr;
}
