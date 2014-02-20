// =============================================================================
/*!
 * \file       tests/test_interfaces.cpp
 *
 * This file contains the implementation of the tests for the common functionalaty
 * of the interfaces.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interface.h"
#include "hanfun/attributes.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Testing;

// =============================================================================
// AbstractInterface
// =============================================================================

//! AbstractInterface test group.
TEST_GROUP (AbstractInterface)
{
   TestInterface *itf;
   Protocol::Packet packet;
   ByteArray payload;

   TEST_SETUP ()
   {
      itf                     = new TestInterface (Interface::SERVER_ROLE, 0x5AA5);

      packet.message.itf.uid  = itf->uid ();
      packet.message.itf.role = itf->role ();

      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      delete itf;

      mock ().clear ();
   }
};

/*!
 * \test Should handle request message coming from the same interface and
 *       from the same role.
 */
TEST (AbstractInterface, Handle_Request)
{
   packet.message.type = Message::COMMAND_REQ;

   CHECK_EQUAL (Result::OK, itf->handle (packet, payload, 0));

   itf->_role = Interface::CLIENT_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, itf->handle (packet, payload, 0));

   packet.message.itf.role = Interface::SERVER_ROLE;
   packet.message.itf.uid  = 0x7AAA;

   CHECK_EQUAL (Result::FAIL_ID, itf->handle (packet, payload, 0));
}

/*!
 * \test Should handle request message coming from the same interface and
 *       from the same role.
 */
TEST (AbstractInterface, Handle_RequestResp)
{
   packet.message.type = Message::COMMAND_RESP_REQ;

   CHECK_EQUAL (Result::OK, itf->handle (packet, payload, 0));

   itf->_role = Interface::CLIENT_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, itf->handle (packet, payload, 0));

   packet.message.itf.role = Interface::SERVER_ROLE;
   packet.message.itf.uid  = 0x7AAA;

   CHECK_EQUAL (Result::FAIL_ID, itf->handle (packet, payload, 0));
}

// =============================================================================
// AbstractInterface::Attributes
// =============================================================================

//! \test Should handle valid get attribute requests.
TEST (AbstractInterface, Handle_GetAttribute_Valid)
{
   packet.message.type       = Protocol::Message::GET_ATTR_REQ;
   packet.message.itf.member = TestInterface::ATTR1;

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 0);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);
   LONGS_EQUAL (TestInterface::ATTR1, itf->sendMsg.itf.member);

   AttributeResponse *attr_res = static_cast <AttributeResponse *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::OK, attr_res->code);

   CHECK_TRUE (attr_res->attribute != nullptr);

   CHECK_FALSE (attr_res->attribute->isWritable ());

   LONGS_EQUAL (TestInterface::ATTR1, attr_res->attribute->uid ());

   LONGS_EQUAL (itf->uid (), attr_res->attribute->interface ());
}

//! \test Should handle invalid get attribute requests.
TEST (AbstractInterface, Handle_GetAttribute_Invalid)
{
   packet.message.type       = Protocol::Message::GET_ATTR_REQ;
   packet.message.itf.member = TestInterface::ATTR3 + 4;

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 0);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);
   LONGS_EQUAL (packet.message.itf.member, itf->sendMsg.itf.member);

   AttributeResponse *attr_res = static_cast <AttributeResponse *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::FAIL_SUPPORT, attr_res->code);

   CHECK_TRUE (attr_res->attribute == nullptr);
}

//! \test Should handle valid set attribute requests.
TEST (AbstractInterface, Handle_SetAttribute_Valid)
{
   packet.message.type       = Protocol::Message::SET_ATTR_REQ;
   packet.message.itf.member = TestInterface::ATTR3;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A53, itf->attr3);

   mock ("Interface").expectNCalls (0, "sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (0x5AAA, itf->attr3);
}

//! \test Should handle invalid set attribute requests.
TEST (AbstractInterface, Handle_SetAttribute_Invalid)
{
   packet.message.type       = Protocol::Message::SET_ATTR_REQ;
   packet.message.itf.member = TestInterface::ATTR3 + 4;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A51, itf->attr1);

   mock ("Interface").expectNCalls (0, "sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (0x5A51, itf->attr1);
}

//! \test Should handle invalid set attribute requests.
TEST (AbstractInterface, Handle_SetAttribute_ReadOnly)
{
   packet.message.type       = Protocol::Message::SET_ATTR_REQ;
   packet.message.itf.member = TestInterface::ATTR1;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A51, itf->attr1);

   mock ("Interface").expectNCalls (0, "sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (0x5A51, itf->attr1);
}

//! \test Should handle valid set response required attribute requests.
TEST (AbstractInterface, Handle_SetAttributeResponse_Valid)
{
   packet.message.type       = Protocol::Message::SET_ATTR_RESP_REQ;
   packet.message.itf.member = TestInterface::ATTR3;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A53, itf->attr3);

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (0x5AAA, itf->attr3);
}

//! \test Should handle invalid set response required attribute requests.
TEST (AbstractInterface, Handle_SetAttributeResponse_Invalid)
{
   packet.message.type       = Protocol::Message::SET_ATTR_RESP_REQ;
   packet.message.itf.member = TestInterface::ATTR3 + 4;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A51, itf->attr1);

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);
   LONGS_EQUAL (packet.message.itf.member, itf->sendMsg.itf.member);

   Response *res = static_cast <Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::FAIL_SUPPORT, res->code);

   LONGS_EQUAL (0x5A51, itf->attr1);
}

//! \test Should handle invalid set response required attribute requests.
TEST (AbstractInterface, Handle_SetAttributeResponse_ReadOnly)
{
   packet.message.type       = Protocol::Message::SET_ATTR_RESP_REQ;
   packet.message.itf.member = TestInterface::ATTR1;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x5A, 0xAA, // Attribute value.
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   LONGS_EQUAL (0x5A51, itf->attr1);

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);
   LONGS_EQUAL (packet.message.itf.member, itf->sendMsg.itf.member);

   Response *res = static_cast <Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::FAIL_RO_ATTR, res->code);

   LONGS_EQUAL (0x5A51, itf->attr1);
}

//! \test Should handle valid get attribute pack requests for mandatory attributes.
TEST (AbstractInterface, Handle_GetAttributePack_Mandatory)
{
   packet.message.type       = Protocol::Message::GET_ATTR_PACK_REQ;
   packet.message.itf.member = GetAttributePack::Type::MANDATORY;

   uint8_t data[] = {0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);

   GetAttributePack::Response *attr_resp = static_cast <GetAttributePack::Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::OK, attr_resp->code);

   LONGS_EQUAL (1, attr_resp->attributes.size ());

   IAttribute *attr = attr_resp->attributes[TestInterface::ATTR1];

   CHECK_TRUE (attr == nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR2];

   CHECK_TRUE (attr == nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR3];

   CHECK_TRUE (attr != nullptr);
}

//! \test Should handle valid get attribute pack requests for all attributes.
TEST (AbstractInterface, Handle_GetAttributePack_All)
{
   packet.message.type       = Protocol::Message::GET_ATTR_PACK_REQ;
   packet.message.itf.member = GetAttributePack::Type::ALL;

   uint8_t data[] = {0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);

   GetAttributePack::Response *attr_resp = static_cast <GetAttributePack::Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::OK, attr_resp->code);

   LONGS_EQUAL (3, attr_resp->attributes.size ());

   IAttribute *attr = attr_resp->attributes[TestInterface::ATTR1];

   CHECK_TRUE (attr != nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR2];

   CHECK_TRUE (attr != nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR3];

   CHECK_TRUE (attr != nullptr);
}

//! \test Should handle valid get attribute pack requests.
TEST (AbstractInterface, Handle_GetAttributePack_Valid)
{
   packet.message.type       = Protocol::Message::GET_ATTR_PACK_REQ;
   packet.message.itf.member = GetAttributePack::Type::DYNAMIC;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x02, // Number of attribute uid's.
                           // Attribute uid's.
                     0x01, 0x03,
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);

   GetAttributePack::Response *attr_resp = static_cast <GetAttributePack::Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::OK, attr_resp->code);

   LONGS_EQUAL (2, attr_resp->attributes.size ());

   IAttribute *attr = attr_resp->attributes[TestInterface::ATTR1];

   CHECK_TRUE (attr != nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR2];

   CHECK_TRUE (attr == nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR3];

   CHECK_TRUE (attr != nullptr);
}

//! \test Should handle invalid get attribute requests.
TEST (AbstractInterface, Handle_GetAttributePack_Invalid)
{
   packet.message.type       = Protocol::Message::GET_ATTR_PACK_REQ;
   packet.message.itf.member = GetAttributePack::Type::DYNAMIC;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x03, // Number of attribute uid's.
                           // Attribute uid's.
                     0x01, 0x03, 0x04,
                     0x00, 0x00, 0x00};

   payload = ByteArray (data, sizeof(data));

   mock ("Interface").expectOneCall ("sendMessage");

   Result result = itf->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ().checkExpectations ();

   LONGS_EQUAL (itf->_uid, itf->sendMsg.itf.uid);
   LONGS_EQUAL (itf->_role, itf->sendMsg.itf.role);

   LONGS_EQUAL (Message::GET_ATTR_PACK_RES, itf->sendMsg.type);

   GetAttributePack::Response *attr_resp = static_cast <GetAttributePack::Response *>(itf->sendMsg.payload);

   CHECK_EQUAL (Result::FAIL_SUPPORT, attr_resp->code);

   LONGS_EQUAL (2, attr_resp->attributes.size ());

   IAttribute *attr = attr_resp->attributes[TestInterface::ATTR1];

   CHECK_TRUE (attr != nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR2];

   CHECK_TRUE (attr == nullptr);

   attr = attr_resp->attributes[TestInterface::ATTR3];

   CHECK_TRUE (attr != nullptr);
}
