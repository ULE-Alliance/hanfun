// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_light_sensor.cpp
 *
 * This is file contains the unit tests for the Simple Light Sensor interface
 * implementation.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_light_sensor.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleLightSensor;

// =============================================================================
// Simple Light Sensor
// =============================================================================

//! Test Group for SimpleLightSensor interface parent class.
TEST_GROUP(SimpleLightSensor)
{
   struct SimpleLightSensorBase: public InterfaceParentHelper<SimpleLightSensor::Base>
   {};

   SimpleLightSensorBase interface;

   TEST_SETUP()
   {
      interface = SimpleLightSensorBase();

      mock("support").expectNoCall("assert");
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock("support").checkExpectations();
      mock().clear();
   }
};

//! @test SimpleLightSensor::uid should return @c HF::Interface::SIMPLE_LIGHT_SENSOR.
TEST(SimpleLightSensor, UID)
{
   LONGS_EQUAL(HF::Interface::SIMPLE_LIGHT_SENSOR, interface.uid());
}

//! @test Value support.
TEST(SimpleLightSensor, Value)
{
   HF::Attributes::IAttribute *attr = SimpleLightSensor::create_attribute(
      SimpleLightSensor::VALUE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Value::ID, attr->uid());
   CHECK_EQUAL(Value::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::SIMPLE_LIGHT_SENSOR, attr->interface());

   delete attr;
}

//! @test Minimum support.
TEST(SimpleLightSensor, Minimum)
{
   HF::Attributes::IAttribute *attr = SimpleLightSensor::create_attribute(
      SimpleLightSensor::MINIMUM_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Minimum::ID, attr->uid());
   CHECK_EQUAL(Minimum::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::SIMPLE_LIGHT_SENSOR, attr->interface());

   delete attr;
}

//! @test Maximum support.
TEST(SimpleLightSensor, Maximum)
{
   HF::Attributes::IAttribute *attr = SimpleLightSensor::create_attribute(
      SimpleLightSensor::MAXIMUM_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Maximum::ID, attr->uid());
   CHECK_EQUAL(Maximum::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::SIMPLE_LIGHT_SENSOR, attr->interface());

   delete attr;
}

//! @test Tolerance support.
TEST(SimpleLightSensor, Tolerance)
{
   HF::Attributes::IAttribute *attr = SimpleLightSensor::create_attribute(
      SimpleLightSensor::TOLERANCE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Tolerance::ID, attr->uid());
   CHECK_EQUAL(Tolerance::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::SIMPLE_LIGHT_SENSOR, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(SimpleLightSensor, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = SimpleLightSensor::create_attribute(
      SimpleLightSensor::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Simple Light Sensor Client
// =============================================================================

//! Test Group for Simple Light Sensor Client interface class.
TEST_GROUP(SimpleLightSensorClient)
{
   struct SimpleLightSensorClient: public InterfaceHelper<SimpleLightSensor::Client>
   {
      void read_resp(const Protocol::Address &addr,
                     const HF::Attributes::Attribute<uint32_t> &attr)
      {
         UNUSED(addr);

         int value = (int) attr.value();

         mock("SimpleLightSensor::Client").actualCall("read_resp")
            .withParameter("attr_uid", attr.uid())
            .withParameter("attr_value", value);
      }
   };

   SimpleLightSensorClient client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      client = SimpleLightSensorClient();

      mock("support").expectNoCall("assert");
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock("support").checkExpectations();
      mock().clear();
   }
};

/*!
 * @test Check if a GET_ATTR_PACK_REQ message is sent.
 */
TEST(SimpleLightSensorClient, ReadAll_Request)
{
   mock("Interface").expectOneCall("send");

   client.read_all(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(HF::Attributes::Pack::ALL, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::GET_ATTR_PACK_REQ, client.sendMsg.type);
}

/*!
 * @test Check if a GET_ATTR_REQ message is sent for the measured lux attribute.
 */
TEST(SimpleLightSensorClient, Read_Lux_Request)
{
   mock("Interface").expectOneCall("send");

   client.read<SimpleLightSensor::VALUE_ATTR>(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleTemperature::VALUE_ATTR, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::GET_ATTR_REQ, client.sendMsg.type);
}

/*!
 * @test Check if a GET_ATTR_REQ message is sent for the minimum measurable lux attribute.
 */
TEST(SimpleLightSensorClient, Read_Min_Lux_Request)
{
   mock("Interface").expectOneCall("send");

   client.read<SimpleLightSensor::MINIMUM_ATTR>(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleTemperature::MINIMUM_ATTR, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::GET_ATTR_REQ, client.sendMsg.type);
}

/*!
 * @test Check if a GET_ATTR_REQ message is sent for the maximum measurable Lux attribute.
 */
TEST(SimpleLightSensorClient, Read_Max_Lux_Request)
{
   mock("Interface").expectOneCall("send");

   client.read<SimpleLightSensor::MAXIMUM_ATTR>(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleTemperature::MAXIMUM_ATTR, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::GET_ATTR_REQ, client.sendMsg.type);
}

/*!
 * @test Check if a GET_ATTR_REQ message is sent for the tolerance attribute.
 */
TEST(SimpleLightSensorClient, Read_Tolerance_Request)
{
   mock("Interface").expectOneCall("send");

   client.read<SimpleLightSensor::TOLERANCE_ATTR>(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleTemperature::TOLERANCE_ATTR, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::GET_ATTR_REQ, client.sendMsg.type);
}

/*!
 * @test Check if a GET_ATTR_PACK_RES/Pack::ALL message is correctly parsed.
 */
TEST(SimpleLightSensorClient, ReadAll_Response)
{
   Protocol::Packet packet;

   packet.message.type       = Protocol::Message::GET_ATTR_PACK_RES;
   packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
   packet.message.itf.id     = HF::Interface::SIMPLE_LIGHT_SENSOR;
   packet.message.itf.member = HF::Attributes::Pack::ALL;

   ByteArray payload = ByteArray {0x00, 0x00, 0x00,
                                  Result::OK,         // Response result
                                  0x04,               // Number of attributes

                                  // Measured Temperature
                                  SimpleLightSensor::VALUE_ATTR,
                                  0x01, 0x02, 0x03, 0x04,

                                  // Minimum Measurable Temperature
                                  SimpleLightSensor::MINIMUM_ATTR,
                                  0x11, 0x12, 0x13, 0x14,

                                  // Maximum Measurable Temperature
                                  SimpleLightSensor::MAXIMUM_ATTR,
                                  0x21, 0x22, 0x23, 0x24,

                                  // Tolerance
                                  SimpleLightSensor::TOLERANCE_ATTR,
                                  0x31, 0x32, 0x33, 0x34,

                                  0x00, 0x00, 0x00};

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", (uint8_t) SimpleLightSensor::VALUE_ATTR)
      .withParameter("attr_value", (uint32_t) 0x01020304);

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", (uint8_t) SimpleLightSensor::MINIMUM_ATTR)
      .withParameter("attr_value", (uint32_t) 0x11121314);

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", (uint8_t) SimpleLightSensor::MAXIMUM_ATTR)
      .withParameter("attr_value", (uint32_t) 0x21222324);

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", (uint8_t) SimpleLightSensor::TOLERANCE_ATTR)
      .withParameter("attr_value", (uint32_t) 0x31323334);

   CHECK_EQUAL(Result::OK, client.handle(packet, payload, 3));

   mock("SimpleLightSensor::Client").checkExpectations();
}

/*!
 * @test Check if a GET_ATTR_RES message for the measured Lux is correctly parsed.
 */
TEST(SimpleLightSensorClient, Read_Lux_Response)
{
   Protocol::Packet packet;

   packet.message.type       = Protocol::Message::GET_ATTR_RES;
   packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
   packet.message.itf.id     = HF::Interface::SIMPLE_LIGHT_SENSOR;
   packet.message.itf.member = SimpleLightSensor::VALUE_ATTR;

   ByteArray payload = ByteArray {0x00, 0x00, 0x00,
                                  Result::OK,         // Response result
                                  0x12, 0x34, 0x56, 0x78,
                                  0x00, 0x00, 0x00};

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", SimpleLightSensor::VALUE_ATTR)
      .withParameter("attr_value", (uint32_t) 0x12345678);

   CHECK_EQUAL(Result::OK, client.handle(packet, payload, 3));

   mock("SimpleLightSensor::Client").checkExpectations();
}

/*!
 * @test Check if a GET_ATTR_RES message for the minimum measurable Lux
 * is correctly parsed.
 */
TEST(SimpleLightSensorClient, Read_Min_Lux_Response)
{
   Protocol::Packet packet;

   packet.message.type       = Protocol::Message::GET_ATTR_RES;
   packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
   packet.message.itf.id     = HF::Interface::SIMPLE_LIGHT_SENSOR;
   packet.message.itf.member = SimpleLightSensor::MINIMUM_ATTR;

   ByteArray payload = ByteArray {0x00, 0x00, 0x00,
                                  Result::OK,         // Response result
                                  0x12, 0x34, 0x56, 0x78,
                                  0x00, 0x00, 0x00};

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", SimpleLightSensor::MINIMUM_ATTR)
      .withParameter("attr_value", (uint32_t) 0x12345678);

   CHECK_EQUAL(Result::OK, client.handle(packet, payload, 3));

   mock("SimpleLightSensor::Client").checkExpectations();
}

/*!
 * @test Check if a GET_ATTR_RES message for the maximum measurable Lux
 * is correctly parsed.
 */
TEST(SimpleLightSensorClient, Read_Max_Lux_Response)
{
   Protocol::Packet packet;

   packet.message.type       = Protocol::Message::GET_ATTR_RES;
   packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
   packet.message.itf.id     = HF::Interface::SIMPLE_LIGHT_SENSOR;
   packet.message.itf.member = SimpleLightSensor::MAXIMUM_ATTR;

   ByteArray payload = ByteArray {0x00, 0x00, 0x00,
                                  Result::OK,         // Response result
                                  0x12, 0x34, 0x56, 0x78,
                                  0x00, 0x00, 0x00};

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", SimpleLightSensor::MAXIMUM_ATTR)
      .withParameter("attr_value", (uint32_t) 0x12345678);

   CHECK_EQUAL(Result::OK, client.handle(packet, payload, 3));

   mock("SimpleLightSensor::Client").checkExpectations();
}

/*!
 * @test Check if a GET_ATTR_RES message for the tolerance is correctly parsed.
 */
TEST(SimpleLightSensorClient, Read_Tolerance_Response)
{
   Protocol::Packet packet;

   packet.message.type       = Protocol::Message::GET_ATTR_RES;
   packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
   packet.message.itf.id     = HF::Interface::SIMPLE_LIGHT_SENSOR;
   packet.message.itf.member = SimpleLightSensor::TOLERANCE_ATTR;

   ByteArray payload = ByteArray {0x00, 0x00, 0x00,
                                  Result::OK,         // Response result
                                  0x12, 0x34, 0x56, 0x78,
                                  0x00, 0x00, 0x00};

   mock("SimpleLightSensor::Client").expectOneCall("read_resp")
      .withParameter("attr_uid", SimpleLightSensor::TOLERANCE_ATTR)
      .withParameter("attr_value", (uint32_t) 0x12345678);

   CHECK_EQUAL(Result::OK, client.handle(packet, payload, 3));

   mock("SimpleLightSensor::Client").checkExpectations();
}

// =============================================================================
// Simple Light Sensor Server
// =============================================================================

//! Test Group for Simple Light Sensor Server interface class.
TEST_GROUP(SimpleLightSensorServer)
{
   struct SimpleLightSensorServer: public InterfaceHelper<SimpleLightSensor::Server>
   {};

   SimpleLightSensorServer server;

   TEST_SETUP()
   {
      server = SimpleLightSensorServer();

      mock("support").expectNoCall("assert");
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock("support").checkExpectations();
      mock().clear();
   }
};


//! @test Value support.
TEST(SimpleLightSensorServer, Value)
{
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Value, false, value, 42, 142);
}

//! @test Minimum support.
TEST(SimpleLightSensorServer, Minimum)
{
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Minimum, false, minimum, 42, 142);
}

//! @test Maximum support.
TEST(SimpleLightSensorServer, Maximum)
{
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Maximum, false, maximum, 42, 142);
}

//! @test Tolerance support.
TEST(SimpleLightSensorServer, Tolerance)
{
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Tolerance, false, tolerance, 42, 142);
}
