// =============================================================================
/*!
 * @file       tests/interfaces/test_colour_control.cpp
 *
 * This is file contains the unit tests for the Colour Control interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/colour_control.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;
using namespace HF::Common;

using namespace HF::Interfaces::ColourControl;

// =============================================================================
// Colour Control Messages
// =============================================================================
TEST_GROUP(ColourControlMessages)
{

   ByteArray expected;
   ByteArray payload;
   TEST_SETUP()
   {
      expected = ByteArray();
      payload  = ByteArray();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test XY_Colour helper class basic test.
TEST(ColourControlMessages, XY_Colour)
{
   XY_Colour colour(0,10);

   expected = ByteArray(4);

   LONGS_EQUAL(4, colour.pack(expected));
   LONGS_EQUAL(4, colour.unpack(expected));
}

//! @test XY_Colour helper class basic test with wrong array size passed.
TEST(ColourControlMessages, XY_Colour_wrong_array_size)
{
   XY_Colour colour(0,10);

   expected = ByteArray(2);

   LONGS_EQUAL(0, colour.pack(expected,1));
   LONGS_EQUAL(0, colour.unpack(expected));
}

//! @test XY_Colour helper class size test.
TEST(ColourControlMessages, XY_Colour_size)
{
   XY_Colour colour(0x1234,0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // X value
                           0x56, 0x78     // Y value
                        });

   LONGS_EQUAL(expected.size(), colour.size());
   LONGS_EQUAL(4, colour.size());
}

//! @test XY_Colour helper class pack test.
TEST(ColourControlMessages, XY_Colour_pack)
{
   XY_Colour colour(0x1234,0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // X value
                           0x56, 0x78     // Y value
                        });
   payload = ByteArray(colour.size());

   LONGS_EQUAL(expected.size(), colour.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test XY_Colour helper class unpack test.
TEST(ColourControlMessages, XY_Colour_unpack)
{
   XY_Colour colour;

   payload = ByteArray({
                           0x12, 0x34,    // X value
                           0x56, 0x78     // Y value
                        });

   LONGS_EQUAL(payload.size(), colour.unpack(payload));
   LONGS_EQUAL(0x1234, colour.X);
   LONGS_EQUAL(0x5678, colour.Y);
}

/*! @test XY_Colour helper class unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, XY_Colour_unpack_incomplete)
{
   XY_Colour colour;

   payload = ByteArray({
                           0x12, 0x34,    // X value
                           0x56           // Y value
                        });

   LONGS_EQUAL(0, colour.unpack(payload));
   LONGS_EQUAL(0, colour.X);
   LONGS_EQUAL(0, colour.Y);
}

/*! @test XY_Colour helper class unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the colour values are maintained.
 */
TEST(ColourControlMessages, XY_Colour_unpack_incomplete_keep_values)
{
   XY_Colour colour(0x1234, 0x5678);

   payload = ByteArray({
                           0x12, 0x34,    // X value
                           0x56           // Y value
                        });

   LONGS_EQUAL(0, colour.unpack(payload));
   LONGS_EQUAL(0x1234, colour.X);
   LONGS_EQUAL(0x5678, colour.Y);
}

// ---- Move to hue Message ----

//! @test MoveToHue message basic test.
TEST(ColourControlMessages, MoveToHue)
{
   MoveToHueMessage message(100,Direction::UP, 0x10);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveToHue message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveToHue_wrong_array_size)
{
   MoveToHueMessage message(100,Direction::UP, 0x10);

   expected = ByteArray(2);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveToHue message size test.
TEST(ColourControlMessages, MoveToHue_size)
{
   MoveToHueMessage message(100,Direction::UP, 0x10);

   expected = ByteArray({
                           0x00, 0x64,    // Hue
                           Direction::UP, // Direction
                           0x00, 0x10     // Transition time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(5, message.size());
}

//! @test MoveToHue message pack test.
TEST(ColourControlMessages, MoveToHue_pack)
{
   MoveToHueMessage message(0x0118,Direction::UP, 0x1234);

   expected = ByteArray({
                              0x01, 0x18,    // Hue
                              Direction::UP, // Direction
                              0x12, 0x34     // Transition time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test MoveToHue message pack test.
 *
 * Invalid value passed to the hue field.
 */
TEST(ColourControlMessages, MoveToHue_pack_invalid_value)
{
   // -- On the constructor --

   MoveToHueMessage message(600,Direction::UP, 0x1234);

   expected = ByteArray({
                              0x01, 0x67,    // Hue
                              Direction::UP, // Direction
                              0x12, 0x34     // Transition time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);

   // -- Direct access --

   message.hue = 600;

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test MoveToHue message unpack test.
TEST(ColourControlMessages, MoveToHue_unpack)
{
   MoveToHueMessage message;

   payload = ByteArray({
                                 0x01, 0x18,    // Hue
                                 Direction::UP, // Direction
                                 0x12, 0x34     // Transition time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x0118, message.hue);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x1234, message.time);
}

/*! @test MoveToHue message unpack test.
 *
 * Invalid value passed to the hue field.
 */
TEST(ColourControlMessages, MoveToHue_unpack_invalid_value)
{
   MoveToHueMessage message;

   payload = ByteArray({
                                 0xFF, 0x18,    // Hue
                                 Direction::UP, // Direction
                                 0x12, 0x34     // Transition time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(359, message.hue);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x1234, message.time);
}

/*! @test MoveToHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveToHue_unpack_incomplete)
{
   MoveToHueMessage message;

      payload = ByteArray({
                                    0x01, 0x18,    // Hue
                                    Direction::UP, // Direction
                                    0x12           // Transition time (incomplete)
                           });

      LONGS_EQUAL(0, message.unpack(payload));
      LONGS_EQUAL(0, message.hue);
      CHECK_EQUAL(Direction::SHORTEST, message.direction);
      LONGS_EQUAL(0, message.time);
}

/*! @test MoveToHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveToHue_unpack_incomplete_keep_values)
{
   MoveToHueMessage message(0x0118,Direction::UP, 0x1234);

   payload = ByteArray({
                          0x00, 0x22,        // Hue
                          Direction::DOWN,   // Direction
                          0x21,              // Transition time (incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x0118, message.hue);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x1234, message.time);
}


// ---- Move hue Message ----

//! @test MoveHue message basic test.
TEST(ColourControlMessages, MoveHue)
{
   MoveHueMessage message(Direction::UP, 0x0118);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveHue message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveHue_wrong_array_size)
{
   MoveHueMessage message(Direction::UP, 0x10);

   expected = ByteArray(2);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveHue message size test.
TEST(ColourControlMessages, MoveHue_size)
{
   MoveHueMessage message(Direction::UP, 0x10);

   expected = ByteArray({
                           Direction::UP, // Direction
                           0x00, 0x10     // Rate
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(3, message.size());
}

//! @test MoveHue message pack test.
TEST(ColourControlMessages, MoveHue_pack)
{
   MoveHueMessage message(Direction::UP, 0x0118);

   expected = ByteArray({
                              Direction::UP, // Direction
                              0x01, 0x18     // Rate
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test MoveHue message pack test.
 *
 * Invalid value passed to the rate field.
 */
TEST(ColourControlMessages, MoveHue_pack_invalid_value)
{
   // -- On the constructor --

   MoveHueMessage message(Direction::UP, 600);

   expected = ByteArray({
                              Direction::UP, // Direction
                              0x01, 0x67     // Rate (MAX_Value)
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);

   // -- Direct access --

   message.rate = 600;

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.

   // -- Direct access - Direction --
   message.rate = 300;
   message.direction = Direction::LONGEST;   // Invalid Value.

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test MoveHue message unpack test.
TEST(ColourControlMessages, MoveHue_unpack)
{
   MoveHueMessage message;

   payload = ByteArray({
                                 Direction::UP, // Direction
                                 0x01, 0x18     // Rate
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x0118, message.rate);
}

/*! @test MoveHue message unpack test.
 *
 * Invalid value passed to the Rate and Direction field.
 */
TEST(ColourControlMessages, MoveHue_unpack_invalid_value)
{
   MoveHueMessage message;

   payload = ByteArray({
                                 Direction::UP, // Direction
                                 0xFF, 0x34     // Rate (over max)
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(359, message.rate);

   payload = ByteArray({
                                 Direction::SHORTEST, // Direction (Invalid Value)
                                 0x01, 0x18           // Rate
                        });

   LONGS_EQUAL(0, message.unpack(payload));     // Returns 0 as an error.
}

/*! @test MoveHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveHue_unpack_incomplete)
{
   MoveHueMessage message;

   payload = ByteArray({
                           Direction::UP, // Direction
                           0x01           // Rate (incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.rate);
}

/*! @test MoveHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveHue_unpack_incomplete_keep_values)
{
   MoveHueMessage message(Direction::UP, 0x0118);

   payload = ByteArray({
                          Direction::DOWN,   // Direction
                          0x01,              // Rate (incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x0118, message.rate);
}

// ---- Step hue Message ----

//! @test StepHue message basic test.
TEST(ColourControlMessages, StepHue)
{
   StepHueMessage message(0x5A, Direction::UP, 0xA5);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test StepHue message basic test with wrong array size passed.
TEST(ColourControlMessages, StepHue_wrong_array_size)
{
   StepHueMessage message(0x5A, Direction::UP, 0xA5);

   expected = ByteArray(2);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test StepHue message size test.
TEST(ColourControlMessages, StepHue_size)
{
   StepHueMessage message(0x5A, Direction::UP, 0xA5);

   expected = ByteArray({
                           0x5A,          // Step size
                           Direction::UP, // Direction
                           0xA5           // time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(3, message.size());
}

//! @test StepHue message pack test.
TEST(ColourControlMessages, StepHue_pack)
{
   StepHueMessage message(0x5A, Direction::UP, 0xA5);

   expected = ByteArray({
                           0x5A,          // Step size
                           Direction::UP, // Direction
                           0xA5           // time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test StepHue message pack test.
 *
 * Invalid value passed to the direction field.
 */
TEST(ColourControlMessages, StepHue_pack_invalid_value)
{
   // -- On the constructor --

   StepHueMessage message(0x5A, Direction::LONGEST, 0xA5); // Invalid Value (Direction).

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.

   // -- Direct access - Direction --

   message.direction = Direction::LONGEST;   // Invalid Value.

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test StepHue message unpack test.
TEST(ColourControlMessages, StepHue_unpack)
{
   StepHueMessage message;

   payload = ByteArray({
                           0x5A,          // Step size
                           Direction::UP, // Direction
                           0xA5           // time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x5A, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0xA5, message.time);
}

/*! @test StepHue message unpack test.
 *
 * Invalid value passed to the Direction field.
 */
TEST(ColourControlMessages, StepHue_unpack_invalid_value)
{
   StepHueMessage message;

   payload = ByteArray({
                           0x5A,                // Step size
                           Direction::SHORTEST, // Direction (Invalid Value)
                           0xA5                 // time
                        });

   LONGS_EQUAL(0, message.unpack(payload));     // Returns 0 as an error.
}

/*! @test StepHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, StepHue_unpack_incomplete)
{
   StepHueMessage message;

   payload = ByteArray({
                           0x5A,                // Step size
                           Direction::SHORTEST  // Direction (Invalid Value)
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.time);
}

/*! @test StepHue message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, StepHue_unpack_incomplete_keep_values)
{
   StepHueMessage message(0x5A, Direction::UP, 0xA5);

   payload = ByteArray({
                           0x2A,                // Step size
                           Direction::DOWN      // Direction (Invalid Value)
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x5A, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0xA5, message.time);
}

// ---- Move To Saturation Message ----

//! @test MoveToSaturation message basic test.
TEST(ColourControlMessages, MoveToSaturation)
{
   MoveToSaturationMessage message(0x5A, Direction::UP, 0x1234);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveToSaturation message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveToSaturation_wrong_array_size)
{
   MoveToSaturationMessage message(0x5A, Direction::UP, 0x1234);

   expected = ByteArray(2);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveToSaturation message size test.
TEST(ColourControlMessages, MoveToSaturation_size)
{
   MoveToSaturationMessage message(0x5A, Direction::UP, 0x1234);

   expected = ByteArray({
                           0x5A,          // Saturation
                           Direction::UP, // Direction
                           0x12, 0x34     // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(4, message.size());
}

//! @test MoveToSaturation message pack test.
TEST(ColourControlMessages, MoveToSaturation_pack)
{
   MoveToSaturationMessage message(0x5A, Direction::UP, 0x1234);

   expected = ByteArray({
                           0x5A,          // Saturation
                           Direction::UP, // Direction
                           0x12, 0x34     // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test MoveToSaturation message pack test.
 *
 * Invalid value passed to the direction field.
 */
TEST(ColourControlMessages, MoveToSaturation_pack_invalid_value)
{
   MoveToSaturationMessage message(0x5A, Direction::SHORTEST, 0x1234); // Invalid Value (Direction).

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test MoveToSaturation message unpack test.
TEST(ColourControlMessages, MoveToSaturation_unpack)
{
   MoveToSaturationMessage message;

   payload = ByteArray({
                           0x5A,          // Saturation
                           Direction::UP, // Direction
                           0x12, 0x34     // Time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x5A, message.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x1234, message.time);
}

/*! @test MoveToSaturation message unpack test.
 *
 * Invalid value passed to the Direction field.
 */
TEST(ColourControlMessages, MoveToSaturation_unpack_invalid_value)
{
   MoveToSaturationMessage message;

   payload = ByteArray({
                           0x5A,                // Saturation
                           Direction::SHORTEST, // Direction (Invalid Value)
                           0x12, 0x34           // Time
                        });

   LONGS_EQUAL(0, message.unpack(payload));     // Returns 0 as an error.
}

/*! @test MoveToSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveToSaturation_unpack_incomplete)
{
   MoveToSaturationMessage message;

   payload = ByteArray({
                           0x5A,                // Saturation
                           Direction::SHORTEST  // Direction (Invalid Value)
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.time);
}

/*! @test MoveToSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveToSaturation_unpack_incomplete_keep_values)
{
   MoveToSaturationMessage message(0x5A, Direction::UP, 0x1234);

   payload = ByteArray({
                           0x2A,                // Saturation
                           Direction::DOWN      // Direction (Invalid Value)
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x5A, message.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x1234, message.time);
}

// =============================================================================
// Colour Control
// =============================================================================

//! Test Group for ColourControl interface parent class.
TEST_GROUP(ColourControl)
{
   struct ColourControlBase: public InterfaceParentHelper<ColourControl::Base>
   {};

   ColourControlBase interface;

   TEST_SETUP()
   {
      interface = ColourControlBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test ColourControl::uid should return @c HF::Interface::COLOUR_CONTROL.
TEST(ColourControl, UID)
{
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, interface.uid());
}

//! @test Supported support.
TEST(ColourControl, Supported)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::SUPPORTED_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Supported::ID, attr->uid());
   CHECK_EQUAL(Supported::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Mode support.
TEST(ColourControl, Mode)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::MODE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Mode::ID, attr->uid());
   CHECK_EQUAL(Mode::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Hue And Saturation support.
TEST(ColourControl, HueAndSaturation)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(
      ColourControl::HUE_AND_SATURATION_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(HueAndSaturation::ID, attr->uid());
   CHECK_EQUAL(HueAndSaturation::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test XY support.
TEST(ColourControl, Xy)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::XY_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Xy::ID, attr->uid());
   CHECK_EQUAL(Xy::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Colour Temperature support.
TEST(ColourControl, ColourTemperature)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(
      ColourControl::COLOUR_TEMPERATURE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(ColourTemperature::ID, attr->uid());
   CHECK_EQUAL(ColourTemperature::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(ColourControl, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr =
      ColourControl::create_attribute(ColourControl::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Colour Control Client
// =============================================================================

//! Test Group for Colour Control Client interface class.
TEST_GROUP(ColourControlClient)
{
   // TODO Add required unit tests.
   struct ColourControlClient: public InterfaceHelper<ColourControl::Client>
   {};

   ColourControlClient client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      client = ColourControlClient();

      addr   = Protocol::Address(42);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Move To Hue support.
TEST(ColourControlClient, MoveToHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Hue support.
TEST(ColourControlClient, MoveHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Hue support.
TEST(ColourControlClient, StepHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Saturation support.
TEST(ColourControlClient, MoveToSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Saturation support.
TEST(ColourControlClient, MoveSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Saturation support.
TEST(ColourControlClient, StepSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Hue And Saturation support.
TEST(ColourControlClient, MoveToHueAndSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_hue_and_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Xy support.
TEST(ColourControlClient, MoveToXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Xy support.
TEST(ColourControlClient, MoveXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Xy support.
TEST(ColourControlClient, StepXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Colour Temperature support.
TEST(ColourControlClient, MoveToColourTemperature)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_colour_temperature(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Stop support.
TEST(ColourControlClient, Stop)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.stop(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STOP_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// Colour Control Server
// =============================================================================

//! Test Group for Colour Control Server interface class.
TEST_GROUP(ColourControlServer)
{
   // TODO Add required unit tests.
   struct ColourControlServer: public InterfaceHelper<ColourControl::Server>
   {
      void move_to_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue");
         InterfaceHelper<ColourControl::Server>::move_to_hue(addr);
      }

      void move_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_hue");
         InterfaceHelper<ColourControl::Server>::move_hue(addr);
      }

      void step_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_hue");
         InterfaceHelper<ColourControl::Server>::step_hue(addr);
      }

      void move_to_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_saturation");
         InterfaceHelper<ColourControl::Server>::move_to_saturation(addr);
      }

      void move_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_saturation");
         InterfaceHelper<ColourControl::Server>::move_saturation(addr);
      }

      void step_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_saturation");
         InterfaceHelper<ColourControl::Server>::step_saturation(addr);
      }

      void move_to_hue_and_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue_and_saturation");
         InterfaceHelper<ColourControl::Server>::move_to_hue_and_saturation(addr);
      }

      void move_to_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_xy");
         InterfaceHelper<ColourControl::Server>::move_to_xy(addr);
      }

      void move_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_xy");
         InterfaceHelper<ColourControl::Server>::move_xy(addr);
      }

      void step_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_xy");
         InterfaceHelper<ColourControl::Server>::step_xy(addr);
      }

      void move_to_colour_temperature(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_colour_temperature");
         InterfaceHelper<ColourControl::Server>::move_to_colour_temperature(addr);
      }

      void stop(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("stop");
         InterfaceHelper<ColourControl::Server>::stop(addr);
      }

   };

   ColourControlServer server;


   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = ColourControlServer();

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server.uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Supported support.
TEST(ColourControlServer, Supported)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(ColourControlServer, Supported, false, supported, 42, 142);
}

//! @test Mode support.
TEST(ColourControlServer, Mode)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(ColourControlServer, Mode, false, mode, 42, 142);
}

//! @test Hue And Saturation support.
TEST(ColourControlServer, HueAndSaturation)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, HueAndSaturation, false, hue_and_saturation, 42, 142);
}

//! @test XY support.
TEST(ColourControlServer, Xy)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, Xy, false, xy, 42, 142);
}

//! @test Colour Temperature support.
TEST(ColourControlServer, ColourTemperature)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, ColourTemperature, false, colour_temperature, 42, 142);
}

//! @test Move To Hue support.
TEST(ColourControlServer, MoveToHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_hue");

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Hue support.
TEST(ColourControlServer, MoveHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_hue");

   packet.message.itf.member = ColourControl::MOVE_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Hue support.
TEST(ColourControlServer, StepHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_hue");

   packet.message.itf.member = ColourControl::STEP_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Saturation support.
TEST(ColourControlServer, MoveToSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_saturation");

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Saturation support.
TEST(ColourControlServer, MoveSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_saturation");

   packet.message.itf.member = ColourControl::MOVE_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Saturation support.
TEST(ColourControlServer, StepSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_saturation");

   packet.message.itf.member = ColourControl::STEP_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Hue And Saturation support.
TEST(ColourControlServer, MoveToHueAndSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_hue_and_saturation");

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Xy support.
TEST(ColourControlServer, MoveToXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_xy");

   packet.message.itf.member = ColourControl::MOVE_TO_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Xy support.
TEST(ColourControlServer, MoveXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_xy");

   packet.message.itf.member = ColourControl::MOVE_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Xy support.
TEST(ColourControlServer, StepXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_xy");

   packet.message.itf.member = ColourControl::STEP_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Colour Temperature support.
TEST(ColourControlServer, MoveToColourTemperature)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_colour_temperature");

   packet.message.itf.member = ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Stop support.
TEST(ColourControlServer, Stop)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("stop");

   packet.message.itf.member = ColourControl::STOP_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}
