// =============================================================================
/*!
 * \file       tests/interfaces/test_on_off.cpp
 *
 * This is file contains the unit tests for the On-Off Interface implementation.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/on_off.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

// =============================================================================
// OnOff
// =============================================================================

//! Test Group for OnOff interface parent class.
TEST_GROUP (OnOff)
{
   class TestOnOff:public InterfaceParentHelper <OnOff::Base>
   {};

   TestOnOff interface;
};

//! \test OnOff::uid should return \c Interface::ON_OFF.
TEST (OnOff, UID)
{
   CHECK_EQUAL (Interface::ON_OFF, interface.uid ());
}

// =============================================================================
// OnOffClient
// =============================================================================

//! Test Group for OnOffClient interface class.
TEST_GROUP (OnOffClient)
{
   class TestOnOffClient:public InterfaceHelper <OnOff::Client>
   {};

   TestOnOffClient client;
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

//! \test Should send an ON_CMD message.
TEST (OnOffClient, On)
{
   mock ("Interface").expectOneCall ("sendMessage");

   client.on (addr);

   mock ("Interface").checkExpectations ();

   LONGS_EQUAL (Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL (client.uid (), client.sendMsg.itf.uid);
   LONGS_EQUAL (OnOff::ON_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! \test Should send an OFF_CMD message.
TEST (OnOffClient, Off)
{
   mock ("Interface").expectOneCall ("sendMessage");

   client.off (addr);

   mock ("Interface").checkExpectations ();

   LONGS_EQUAL (Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL (client.uid (), client.sendMsg.itf.uid);
   LONGS_EQUAL (OnOff::OFF_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! \test Should send an TOGGLE_CMD message.
TEST (OnOffClient, Toggle)
{
   mock ("Interface").expectOneCall ("sendMessage");

   client.toggle (addr);

   mock ("Interface").checkExpectations ();

   LONGS_EQUAL (Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL (client.uid (), client.sendMsg.itf.uid);
   LONGS_EQUAL (OnOff::TOGGLE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// OnOffServer
// =============================================================================

//! Test Group for OnOffServer interface class.
TEST_GROUP (OnOffServer)
{
   class DefaultTestOnOffServer:public InterfaceHelper <OnOff::Server>
   {};

   class TestOnOffServer:public InterfaceHelper <OnOff::Server>
   {
      /*!
       * Callback that is called when a \c ON_CMD message is received.
       */
      void on ()
      {
         mock ("OnOffServer").actualCall ("on");
      }

      /*!
       * Callback that is called when a \c OFF_CMD message is received.
       */
      virtual void off ()
      {
         mock ("OnOffServer").actualCall ("off");
      }

      /*!
       * Callback that is called when a \c TOGGLE_CMD message is received.
       */
      virtual void toggle ()
      {
         mock ("OnOffServer").actualCall ("toggle");
      }

   };

   TestOnOffServer  server;
   Protocol::Packet packet;
   ByteArray expected;

   TEST_SETUP ()
   {
      packet.message.itf.role   = Interface::SERVER_ROLE;
      packet.message.itf.uid    = server.uid ();
      packet.message.itf.member = 0xFF;
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

TEST (OnOffServer, State)
{
   CHECK_FALSE (server.state ());

   server.state (true);
   CHECK_TRUE (server.state ());

   server.state (false);
   CHECK_FALSE (server.state ());
}

TEST (OnOffServer, DefaultCallbacks)
{
   DefaultTestOnOffServer server;

   CHECK_FALSE (server.state ());

   server.on ();
   CHECK_TRUE (server.state ());

   server.off ();
   CHECK_FALSE (server.state ());

   server.toggle ();
   CHECK_TRUE (server.state ());

   server.toggle ();
   CHECK_FALSE (server.state ());
}

//! \test Should handle valid \c OnOff::ON_CMD message.
TEST (OnOffServer, Handle_Valid_On_Message)
{
   mock ("OnOffServer").expectOneCall ("on");

   packet.message.itf.member = OnOff::ON_CMD;

   Result result = server.handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("OnOffServer").checkExpectations ();
}

//! \test Should handle valid \c OnOff::OFF_CMD message.
TEST (OnOffServer, Handle_Valid_Off_Message)
{
   mock ("OnOffServer").expectOneCall ("off");

   packet.message.itf.member = OnOff::OFF_CMD;

   Result result = server.handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("OnOffServer").checkExpectations ();
}

//! \test Should handle valid \c OnOff::TOGGLE_CMD message.
TEST (OnOffServer, Handle_Valid_Toggle_Message)
{
   mock ("OnOffServer").expectOneCall ("toggle");

   packet.message.itf.member = OnOff::TOGGLE_CMD;

   Result result = server.handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("OnOffServer").checkExpectations ();
}

//! \test Should not handle message from invalid role.
TEST (OnOffServer, Handle_Invalid_Role)
{
   packet.message.itf.role = Interface::SERVER_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, server.handle (packet, expected, 3));
}

//! \test Should not handle message from invalid interface UID.
TEST (OnOffServer, Handle_Invalid_UID)
{
   packet.message.itf.uid = server.uid () + 1;

   CHECK_EQUAL (Result::FAIL_ID, server.handle (packet, expected, 3));
}

//! \test Should return attribute.
TEST (OnOffServer, Attribute)
{
   HF::Attributes::IAttribute *attr = server.attribute (OnOff::__LAST_ATTR__ + 1);

   CHECK_TRUE (attr == nullptr);

   attr = server.attribute (OnOff::STATE_ATTR);

   CHECK_TRUE (attr != nullptr);

   LONGS_EQUAL (OnOff::STATE_ATTR, attr->uid ());
   CHECK_FALSE (attr->isWritable ());

   LONGS_EQUAL (server.uid (), attr->interface ());

   delete attr;
}
