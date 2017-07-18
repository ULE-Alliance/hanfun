// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_light_sensor.cpp
 *
 * This is file contains the unit tests for the Simple Light Sensor interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_light_sensor.h"

#include "test_helper.h"

using namespace HF;
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

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
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
   // TODO Add required unit tests.
   struct SimpleLightSensorClient: public InterfaceHelper<SimpleLightSensor::Client>
   {};

   SimpleLightSensorClient client;



   TEST_SETUP()
   {
      client = SimpleLightSensorClient();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// =============================================================================
// Simple Light Sensor Server
// =============================================================================

//! Test Group for Simple Light Sensor Server interface class.
TEST_GROUP(SimpleLightSensorServer)
{
   // TODO Add required unit tests.
   struct SimpleLightSensorServer: public InterfaceHelper<SimpleLightSensor::Server>
   {};

   SimpleLightSensorServer server;



   TEST_SETUP()
   {
      server = SimpleLightSensorServer();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Value support.
TEST(SimpleLightSensorServer, Value)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Value, false, value, 42, 142);
}

//! @test Minimum support.
TEST(SimpleLightSensorServer, Minimum)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Minimum, false, minimum, 42, 142);
}

//! @test Maximum support.
TEST(SimpleLightSensorServer, Maximum)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Maximum, false, maximum, 42, 142);
}

//! @test Tolerance support.
TEST(SimpleLightSensorServer, Tolerance)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleLightSensorServer, Tolerance, false, tolerance, 42, 142);
}
