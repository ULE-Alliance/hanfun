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

// ---- Hue and Saturation colour ----

//! @test HS_Colour helper class basic test.
TEST(ColourControlMessages, HS_Colour)
{
   HS_Colour colour(0,10);

   expected = ByteArray(3);

   LONGS_EQUAL(3, colour.pack(expected));
   LONGS_EQUAL(3, colour.unpack(expected));
}

//! @test HS_Colour helper class basic test with wrong array size passed.
TEST(ColourControlMessages, HS_Colour_wrong_array_size)
{
   HS_Colour colour(0,10);

   expected = ByteArray(2);

   LONGS_EQUAL(0, colour.pack(expected,1));
   LONGS_EQUAL(0, colour.unpack(expected));
}

//! @test HS_Colour helper class size test.
TEST(ColourControlMessages, HS_Colour_size)
{
   HS_Colour colour(0x1234,0x56);

   expected = ByteArray({
                           0x12, 0x34,    // Hue value
                           0x56           // Saturation value
                        });

   LONGS_EQUAL(expected.size(), colour.size());
   LONGS_EQUAL(3, colour.size());
}

//! @test HS_Colour helper class pack test.
TEST(ColourControlMessages, HS_Colour_pack)
{
   HS_Colour colour(0x0123,0x45);

   expected = ByteArray({
                           0x01, 0x23,    // Hue value
                           0x45           // Saturation value
                        });
   payload = ByteArray(colour.size());

   LONGS_EQUAL(expected.size(), colour.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test HS_Colour helper class unpack test.
TEST(ColourControlMessages, HS_Colour_unpack)
{
   HS_Colour colour;

   payload = ByteArray({
                        0x01, 0x23,    // Hue value
                        0x45           // Saturation value
                        });

   LONGS_EQUAL(payload.size(), colour.unpack(payload));
   LONGS_EQUAL(0x0123, colour.hue);
   LONGS_EQUAL(0x45, colour.saturation);
}

/*! @test HS_Colour helper class unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
IGNORE_TEST(ColourControlMessages, HS_Colour_unpack_incomplete)
{
   HS_Colour colour;

   payload = ByteArray({
                        0x12, 0x34     // Hue value
                                       // Saturation value
                        });

   LONGS_EQUAL(0, colour.unpack(payload));
   LONGS_EQUAL(0, colour.hue);
   LONGS_EQUAL(0, colour.saturation);
}

/*! @test HS_Colour helper class unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the colour values are maintained.
 */
TEST(ColourControlMessages, HS_Colour_unpack_incomplete_keep_values)
{
   HS_Colour colour(0x0111, 0x22);

   payload = ByteArray({
                           0x01, 0x23     // X value
                                          // Y value
                        });

   LONGS_EQUAL(0, colour.unpack(payload));
   LONGS_EQUAL(0x0111, colour.hue);
   LONGS_EQUAL(0x22, colour.saturation);
}

/*!
 * @test HS_Colour invert_angle test
 */
TEST(ColourControlMessages, HS_Colour_invert_angle)
{
   LONGS_EQUAL( 0,   HS_Colour::invert_angle(-360));
   LONGS_EQUAL( 45,  HS_Colour::invert_angle(-315));
   LONGS_EQUAL( 90,  HS_Colour::invert_angle(-270));
   LONGS_EQUAL( 135, HS_Colour::invert_angle(-225));
   LONGS_EQUAL( 180, HS_Colour::invert_angle(-180));
   LONGS_EQUAL( 225, HS_Colour::invert_angle(-135));
   LONGS_EQUAL( 270, HS_Colour::invert_angle(-90));
   LONGS_EQUAL( 315, HS_Colour::invert_angle(-45));
   LONGS_EQUAL( 0,   HS_Colour::invert_angle( 0));
   LONGS_EQUAL(-315, HS_Colour::invert_angle( 45));
   LONGS_EQUAL(-270, HS_Colour::invert_angle( 90));
   LONGS_EQUAL(-225, HS_Colour::invert_angle( 135));
   LONGS_EQUAL(-180, HS_Colour::invert_angle( 180));
   LONGS_EQUAL(-135, HS_Colour::invert_angle( 225));
   LONGS_EQUAL(-90,  HS_Colour::invert_angle( 270));
   LONGS_EQUAL(-45,  HS_Colour::invert_angle( 315));
   LONGS_EQUAL(0,   HS_Colour::invert_angle( 360));
}

//! @test HS_Colour get hue travel distance function.
TEST(ColourControlMessages, HS_Colour_Get_Hue_travel_distance)
{
   // Both on the same quadrant. Short distance = CW
   LONGS_EQUAL(50,   HS_Colour::get_hue_travel_distance(Direction::UP,        100, 150));
   LONGS_EQUAL(-310, HS_Colour::get_hue_travel_distance(Direction::DOWN,      100, 150));
   LONGS_EQUAL(50,   HS_Colour::get_hue_travel_distance(Direction::SHORTEST,  100, 150));
   LONGS_EQUAL(-310, HS_Colour::get_hue_travel_distance(Direction::LONGEST,   100, 150));

   // Different quadrants. Short distance = CW
   LONGS_EQUAL(30,   HS_Colour::get_hue_travel_distance(Direction::UP,        340, 10));
   LONGS_EQUAL(-330, HS_Colour::get_hue_travel_distance(Direction::DOWN,      340, 10));
   LONGS_EQUAL(30,   HS_Colour::get_hue_travel_distance(Direction::SHORTEST,  340, 10));
   LONGS_EQUAL(-330, HS_Colour::get_hue_travel_distance(Direction::LONGEST,   340, 10));

   // Different quadrants. Short distance = CCW
   LONGS_EQUAL(330,  HS_Colour::get_hue_travel_distance(Direction::UP,        10, 340));
   LONGS_EQUAL(-30,  HS_Colour::get_hue_travel_distance(Direction::DOWN,      10, 340));
   LONGS_EQUAL(-30,  HS_Colour::get_hue_travel_distance(Direction::SHORTEST,  10, 340));
   LONGS_EQUAL(330,  HS_Colour::get_hue_travel_distance(Direction::LONGEST,   10, 340));
}

// ---- XY colour ----

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
IGNORE_TEST(ColourControlMessages, XY_Colour_unpack_incomplete)
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


// ---- Move Saturation Message ----

//! @test MoveSaturation message basic test.
TEST(ColourControlMessages, MoveSaturation)
{
   MoveSaturationMessage message(Direction::UP, 0x12);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveSaturation message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveSaturation_wrong_array_size)
{
   MoveSaturationMessage message(Direction::UP, 0x12);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveSaturation message size test.
TEST(ColourControlMessages, MoveSaturation_size)
{
   MoveSaturationMessage message(Direction::UP, 0x12);

   expected = ByteArray({
                           Direction::UP, // Direction
                           0x12           // Rate
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(2, message.size());
}

//! @test MoveSaturation message pack test.
TEST(ColourControlMessages, MoveSaturation_pack)
{
   MoveSaturationMessage message(Direction::UP, 0x12);

   expected = ByteArray({
                           Direction::UP, // Direction
                           0x12           // Rate
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test MoveSaturation message pack test.
 *
 * Invalid value passed to the direction field.
 */
TEST(ColourControlMessages, MoveSaturation_pack_invalid_value)
{
   MoveSaturationMessage message(Direction::SHORTEST, 0x12); // Invalid Value (Direction).

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test MoveSaturation message unpack test.
TEST(ColourControlMessages, MoveSaturation_unpack)
{
   MoveSaturationMessage message;

   payload = ByteArray({
                           Direction::UP, // Direction
                           0x12           // Rate
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x12, message.rate);
}

/*! @test MoveSaturation message unpack test.
 *
 * Invalid value passed to the Direction field.
 */
TEST(ColourControlMessages, MoveSaturation_unpack_invalid_value)
{
   MoveSaturationMessage message;

   payload = ByteArray({
                           Direction::SHORTEST, // Direction (Invalid Value)
                           0x12                 // Rate
                        });

   LONGS_EQUAL(0, message.unpack(payload));     // Returns 0 as an error.
}

/*! @test MoveSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveSaturation_unpack_incomplete)
{
   MoveSaturationMessage message;

   payload = ByteArray({
                           Direction::SHORTEST  // Direction (Invalid Value)
                                                // Rate (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.rate);
}

/*! @test MoveSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveSaturation_unpack_incomplete_keep_values)
{
   MoveSaturationMessage message(Direction::UP, 0x12);

   payload = ByteArray({
                           Direction::DOWN      // Direction (Invalid Value)
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x12, message.rate);
}


// ---- Step Saturation Message ----

//! @test StepSaturation message basic test.
TEST(ColourControlMessages, StepSaturation)
{
   StepSaturationMessage message(0x12, Direction::UP, 0x34);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test StepSaturation message basic test with wrong array size passed.
TEST(ColourControlMessages, StepSaturation_wrong_array_size)
{
   StepSaturationMessage message(0x12, Direction::UP, 0x34);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test StepSaturation message size test.
TEST(ColourControlMessages, StepSaturation_size)
{
   StepSaturationMessage message(0x12, Direction::UP, 0x34);

   expected = ByteArray({
                           0x12,          // Step
                           Direction::UP, // Direction
                           0x34           // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(3, message.size());
}

//! @test StepSaturation message pack test.
TEST(ColourControlMessages,StepSaturation_pack)
{
   StepSaturationMessage message(0x12, Direction::UP, 0x34);

   expected = ByteArray({
                           0x12,          // Step
                           Direction::UP, // Direction
                           0x34           // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test StepSaturation message pack test.
 *
 * Invalid value passed to the direction field.
 */
TEST(ColourControlMessages, StepSaturation_pack_invalid_value)
{
   StepSaturationMessage message(0x12, Direction::SHORTEST, 0x34); // Invalid Value (Direction).

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test StepSaturation message unpack test.
TEST(ColourControlMessages, StepSaturation_unpack)
{
   StepSaturationMessage message;

   payload = ByteArray({
                           0x12,          // Step
                           Direction::UP, // Direction
                           0x34           // Time
   });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x12, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x34, message.time);
}

/*! @test StepSaturation message unpack test.
 *
 * Invalid value passed to the Direction field.
 */
TEST(ColourControlMessages, StepSaturation_unpack_invalid_value)
{
   StepSaturationMessage message;

   payload = ByteArray({
                           0x12,                // Step
                           Direction::SHORTEST, // Direction (invalid)
                           0x34                 // Time
                        });

   LONGS_EQUAL(0, message.unpack(payload));     // Returns 0 as an error.
}

/*! @test StepSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, StepSaturation_unpack_incomplete)
{
   StepSaturationMessage message;

   payload = ByteArray({
                           0x12,          // Step
                           Direction::UP  // Direction
                                          // Time (missing)
                        });
   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.time);
}

/*! @test StepSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, StepSaturation_unpack_incomplete_keep_values)
{
   StepSaturationMessage message(0x12, Direction::UP, 0x34);

   payload = ByteArray({
                           0x21,                // Step
                           Direction::DOWN      // Direction
                                                // time (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x12, message.step_size);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x34, message.time);
}


// ---- Move To Hue & Saturation Message ----

//! @test MoveToHueSaturation message basic test.
TEST(ColourControlMessages, MoveToHueSaturation)
{
   MoveToHueSaturationMessage message(HS_Colour(0x0123, 0x45), Direction::UP, 0x6789);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveToHueSaturation message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveToHueSaturation_wrong_array_size)
{
   MoveToHueSaturationMessage message(HS_Colour(0x0123, 0x45), Direction::UP, 0x6789);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveToHueSaturation message size test.
TEST(ColourControlMessages, MoveToHueSaturation_size)
{
   MoveToHueSaturationMessage message(HS_Colour(0x0123, 0x45), Direction::UP, 0x6789);

   expected = ByteArray({
                           0x01, 0x23,    // Hue
                           0x45,          // Saturation
                           Direction::UP, // Direction
                           0x67, 0x89     // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(6, message.size());
}

//! @test MoveToHueSaturation message pack test.
TEST(ColourControlMessages,MoveToHueSaturation_pack)
{
   MoveToHueSaturationMessage message(HS_Colour(0x0123, 0x45), Direction::UP, 0x6789);

   expected = ByteArray({
                           0x01, 0x23,    // Hue
                           0x45,          // Saturation
                           Direction::UP, // Direction
                           0x67, 0x89     // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

/*! @test MoveToHueSaturation message pack test.
 *
 * Invalid value passed to the Hue field.
 */
TEST(ColourControlMessages, MoveToHueSaturation_pack_invalid_value)
{
   // --- HUE invalid value ---

   // -- On the constructor --
   MoveToHueSaturationMessage message(HS_Colour(0xFF23, 0x45), Direction::UP, 0x6789); // Invalid Value (Hue).

   expected = ByteArray({
                           0x01, 0x67,    // Hue
                           0x45,          // Saturation
                           Direction::UP, // Direction
                           0x67, 0x89     // Transition time
                           });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));  // Constructor corrected the Hue error!
   CHECK_EQUAL(expected, payload);

   // -- Direct access --

   message.colour.hue = 600;

   payload = ByteArray(message.size());

   LONGS_EQUAL(0, message.pack(payload));    // Returns 0 as an error.
}

//! @test MoveToHueSaturation message unpack test.
TEST(ColourControlMessages, MoveToHueSaturation_unpack)
{
   MoveToHueSaturationMessage message;

   payload = ByteArray({
                        0x01, 0x23,    // Hue
                        0x45,          // Saturation
                        Direction::UP, // Direction
                        0x67, 0x89     // Transition time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x0123, message.colour.hue);
   LONGS_EQUAL(0x45, message.colour.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x6789, message.time);
}

/*! @test MoveToHueSaturation message unpack test.
 *
 * Invalid value passed to the Hue field.
 */
TEST(ColourControlMessages, MoveToHueSaturation_unpack_invalid_value)
{
   MoveToHueSaturationMessage message;

   payload = ByteArray({
                        0xFF, 0x23,    // Hue
                        0x45,          // Saturation
                        Direction::UP, // Direction
                        0x67, 0x89     // Transition time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(359, message.colour.hue);
   LONGS_EQUAL(0x45, message.colour.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x6789, message.time);
}

/*! @test MoveToHueSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveToHueSaturation_unpack_incomplete)
{
   MoveToHueSaturationMessage message;

   payload = ByteArray({
                        0x01, 0x23,    // Hue
                        0x45,          // Saturation
                        Direction::UP, // Direction
                        0x67           // Transition time   (Incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.colour.hue);
   LONGS_EQUAL(0, message.colour.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0, message.time);
}

/*! @test MoveToHueSaturation message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveToHueSaturation_unpack_incomplete_keep_values)
{
   MoveToHueSaturationMessage message(HS_Colour(0x0123, 0x45), Direction::UP, 0x6789);

   payload = ByteArray({
                        0x01, 0x00,    // Hue
                        0xFF,          // Saturation
                        Direction::UP, // Direction
                        0x00           // Transition time   (Incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x0123, message.colour.hue);
   LONGS_EQUAL(0x45, message.colour.saturation);
   CHECK_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(0x6789, message.time);
}


// ---- Move To XY Message ----

//! @test MoveToXY message basic test.
TEST(ColourControlMessages, MoveToXY)
{
   MoveToXYMessage message(XY_Colour(0x1234,0x5678), 0x9ABC);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveToXY message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveToXY_wrong_array_size)
{
   MoveToXYMessage message(XY_Colour(0x1234,0x5678), 0x9ABC);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveToXY message size test.
TEST(ColourControlMessages, MoveToXY_size)
{
   MoveToXYMessage message(XY_Colour(0x1234,0x5678), 0x9ABC);

   expected = ByteArray({
                           0x12, 0x34,    // X
                           0x56, 0x78,    // Y
                           0x9A, 0xBC     // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(6, message.size());
}

//! @test MoveToXY message pack test.
TEST(ColourControlMessages,MoveToXY_pack)
{
   MoveToXYMessage message(XY_Colour(0x1234,0x5678), 0x9ABC);

   expected = ByteArray({
                           0x12, 0x34,    // X
                           0x56, 0x78,    // Y
                           0x9A, 0xBC     // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test MoveToXY message unpack test.
TEST(ColourControlMessages, MoveToXY_unpack)
{
   MoveToXYMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // X
                           0x56, 0x78,    // Y
                           0x9A, 0xBC     // Time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x1234, message.colour.X);
   LONGS_EQUAL(0x5678, message.colour.Y);
   LONGS_EQUAL(0x9ABC, message.time);
}

/*! @test MoveToXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveToXY_unpack_incomplete)
{
   MoveToXYMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // X
                           0x56, 0x78,    // Y
                           0x9A           // Time (incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.colour.X);
   LONGS_EQUAL(0, message.colour.Y);
   LONGS_EQUAL(0, message.time);
}

/*! @test MoveToXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveToXY_unpack_incomplete_keep_values)
{
   MoveToXYMessage message(XY_Colour(0x1234,0x5678), 0x9ABC);

   payload = ByteArray({
                           0xCB, 0xA9,    // X
                           0x87, 0x65,    // Y
                           0x43           // Time (incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x1234, message.colour.X);
   LONGS_EQUAL(0x5678, message.colour.Y);
   LONGS_EQUAL(0x9ABC, message.time);
}


// ---- Move XY Message ----

//! @test MoveXY message basic test.
TEST(ColourControlMessages, MoveXY)
{
   MoveXYMessage message(0x1234,0x5678);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveXY message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveXY_wrong_array_size)
{
   MoveXYMessage message(0x1234,0x5678);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveXY message size test.
TEST(ColourControlMessages, MoveXY_size)
{
   MoveXYMessage message(0x1234,0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // Rate of X
                           0x56, 0x78     // Rate of Y
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(4, message.size());
}

//! @test MoveXY message pack test.
TEST(ColourControlMessages, MoveXY_pack)
{
   MoveXYMessage message(0x1234,0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // Rate of X
                           0x56, 0x78     // Rate of Y
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test MoveXY message unpack test.
TEST(ColourControlMessages, MoveXY_unpack)
{
   MoveXYMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // Rate of X
                           0x56, 0x78     // Rate of Y
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x1234, message.X_rate);
   LONGS_EQUAL(0x5678, message.Y_rate);
}

/*! @test MoveXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveXY_unpack_incomplete)
{
   MoveXYMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // Rate of X
                           0x56,          // Rate of Y (Incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.X_rate);
   LONGS_EQUAL(0, message.Y_rate);
}

/*! @test MoveXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveXY_unpack_incomplete_keep_values)
{
   MoveXYMessage message(0x1234,0x5678);

   payload = ByteArray({
                           0xCB, 0xA9,    // X
                           0x87,          // Y
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x1234, message.X_rate);
   LONGS_EQUAL(0x5678, message.Y_rate);
}


// ---- Step XY Message ----

//! @test StepXY message basic test.
TEST(ColourControlMessages, StepXY)
{
   StepXYMessage message(0x1234,0x5678, 0x9A);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test StepXY message basic test with wrong array size passed.
TEST(ColourControlMessages, StepXY_wrong_array_size)
{
   StepXYMessage message(0x1234,0x5678, 0x9A);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test StepXY message size test.
TEST(ColourControlMessages, StepXY_size)
{
   StepXYMessage message(0x1234,0x5678, 0x9A);

   expected = ByteArray({
                           0x12, 0x34,    // Step of X
                           0x56, 0x78,    // Step of Y
                           0x9A           // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(5, message.size());
}

//! @test StepXY message pack test.
TEST(ColourControlMessages, StepXY_pack)
{
   StepXYMessage message(0x1234,0x5678, 0x9A);

   expected = ByteArray({
                           0x12, 0x34,    // Step of X
                           0x56, 0x78,    // Step of Y
                           0x9A           // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test StepXY message unpack test.
TEST(ColourControlMessages, StepXY_unpack)
{
   StepXYMessage message;

   payload = ByteArray({
                        0x12, 0x34,    // Step of X
                        0x56, 0x78,    // Step of Y
                        0x9A           // Time
                        });

   LONGS_EQUAL(payload.size(), message.unpack(payload));
   LONGS_EQUAL(0x1234,  message.X_step);
   LONGS_EQUAL(0x5678,  message.Y_step);
   LONGS_EQUAL(0x9A,    message.time);
}

/*! @test StepXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, StepXY_unpack_incomplete)
{
   StepXYMessage message;

   payload = ByteArray({
                        0x12, 0x34,    // Step of X
                        0x56, 0x78     // Step of Y
                                       // Time        (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0,  message.X_step);
   LONGS_EQUAL(0,  message.Y_step);
   LONGS_EQUAL(0,    message.time);
}

/*! @test StepXY message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, StepXY_unpack_incomplete_keep_values)
{
   StepXYMessage message(0x1234,0x5678, 0x9A);

   payload = ByteArray({
                        0x87, 0x65,    // Step of X
                        0x43, 0x21     // Step of Y
                                       // Time        (Missing)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0x1234,  message.X_step);
   LONGS_EQUAL(0x5678,  message.Y_step);
   LONGS_EQUAL(0x9A,    message.time);
}


// ---- Move To Colour Message ----

//! @test MoveToTemperature message basic test.
TEST(ColourControlMessages, MoveToTemperature)
{
   MoveToTemperatureMessage message(0x1234, 0x5678);

   expected = ByteArray(message.size());

   LONGS_EQUAL(message.size(), message.pack(expected));
   LONGS_EQUAL(message.size(), message.unpack(expected));
}

//! @test MoveToTemperature message basic test with wrong array size passed.
TEST(ColourControlMessages, MoveToTemperature_wrong_array_size)
{
   MoveToTemperatureMessage message(0x1234, 0x5678);

   expected = ByteArray(1);

   LONGS_EQUAL(0, message.pack(expected));
   LONGS_EQUAL(0, message.unpack(expected));
}

//! @test MoveToTemperature message size test.
TEST(ColourControlMessages, MoveToTemperature_size)
{
   MoveToTemperatureMessage message(0x1234, 0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // Colour Temperature
                           0x56, 0x78,    // Time
                        });

   LONGS_EQUAL(expected.size(), message.size());
   LONGS_EQUAL(4, message.size());
}

//! @test MoveToTemperature message pack test.
TEST(ColourControlMessages, MoveToTemperature_pack)
{
   MoveToTemperatureMessage message(0x1234, 0x5678);

   expected = ByteArray({
                           0x12, 0x34,    // Colour Temperature
                           0x56, 0x78,    // Time
                        });

   payload = ByteArray(message.size());

   LONGS_EQUAL(expected.size(), message.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test MoveToTemperature message unpack test.
TEST(ColourControlMessages, MoveToTemperature_unpack)
{
   MoveToTemperatureMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // Colour Temperature
                           0x56, 0x78,    // Time
                        });

   LONGS_EQUAL(payload.size(),   message.unpack(payload));
   LONGS_EQUAL(0x1234,           message.colour);
   LONGS_EQUAL(0x5678,           message.time);
}

/*! @test MoveToTemperature message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 */
TEST(ColourControlMessages, MoveToTemperature_unpack_incomplete)
{
   MoveToTemperatureMessage message;

   payload = ByteArray({
                           0x12, 0x34,    // Colour Temperature
                           0x56           // Time        (Incomplete)
                        });

   LONGS_EQUAL(0, message.unpack(payload));
   LONGS_EQUAL(0, message.colour);
   LONGS_EQUAL(0, message.time);
}

/*! @test MoveToTemperature message unpack test.
 *
 * Incomplete payload passed to the unpack function.
 * Test if the values are maintained.
 */
TEST(ColourControlMessages, MoveToTemperature_unpack_incomplete_keep_values)
{
   MoveToTemperatureMessage message(0x1234, 0x5678);

   payload = ByteArray({
                           0x65, 0x43,   // Colour Temperature
                           0x21          // Time        (Incomplete)
                        });

   LONGS_EQUAL(0,       message.unpack(payload));
   LONGS_EQUAL(0x1234,  message.colour);
   LONGS_EQUAL(0x5678,  message.time);
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
   mock("Interface").expectOneCall("send");

   client.move_to_hue(addr, 100, Direction::UP, 10);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   MoveToHueMessage message;
   message.unpack(client.sendMsg.payload);

   LONGS_EQUAL(100,           message.hue);
   LONGS_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(10,            message.time);
}

//! @test Move Hue support.
TEST(ColourControlClient, MoveHue)
{
   mock("Interface").expectOneCall("send");

   client.move_hue(addr, Direction::UP, 10);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   MoveHueMessage message;
   message.unpack(client.sendMsg.payload);

   LONGS_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(10, message.rate);
}

//! @test Step Hue support.
TEST(ColourControlClient, StepHue)
{
   mock("Interface").expectOneCall("send");

   client.step_hue(addr, 10, Direction::UP, 10);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   StepHueMessage message;
   message.unpack(client.sendMsg.payload);

   LONGS_EQUAL(10, message.step_size);
   LONGS_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(10, message.time);

}

//! @test Move To Saturation support.
TEST(ColourControlClient, MoveToSaturation)
{
   mock("Interface").expectOneCall("send");

   client.move_to_saturation(addr, 10, Direction::UP, 20);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   MoveToSaturationMessage message;
   message.unpack(client.sendMsg.payload);

   LONGS_EQUAL(10, message.saturation);
   LONGS_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(20, message.time);
}

//! @test Move Saturation support.
TEST(ColourControlClient, MoveSaturation)
{
   mock("Interface").expectOneCall("send");

   client.move_saturation(addr, Direction::UP, 10);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   MoveSaturationMessage message;
   message.unpack(client.sendMsg.payload);

   LONGS_EQUAL(Direction::UP, message.direction);
   LONGS_EQUAL(10, message.rate);
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
      Common::Result move_to_hue(const Protocol::Address &addr,
                       const MoveToHueMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue");
         return InterfaceHelper<ColourControl::Server>::move_to_hue(addr, message);
      }

      Common::Result move_hue(const Protocol::Address &addr,
                    const MoveHueMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_hue");
         return InterfaceHelper<ColourControl::Server>::move_hue(addr, message);
      }

      Common::Result step_hue(const Protocol::Address &addr,
                    const  StepHueMessage &message) override
      {
         mock("ColourControl::Server").actualCall("step_hue");
         return InterfaceHelper<ColourControl::Server>::step_hue(addr, message);
      }

      Common::Result move_to_saturation(const Protocol::Address &addr,
                              const MoveToSaturationMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_to_saturation");
         return InterfaceHelper<ColourControl::Server>::move_to_saturation(addr, message);
      }

      Common::Result move_saturation(const Protocol::Address &addr,
                           const MoveSaturationMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_saturation");
         return InterfaceHelper<ColourControl::Server>::move_saturation(addr, message);
      }

      Common::Result step_saturation(const Protocol::Address &addr,
                           const StepSaturationMessage &message) override
      {
         mock("ColourControl::Server").actualCall("step_saturation");
         return InterfaceHelper<ColourControl::Server>::step_saturation(addr, message);
      }

      Common::Result move_to_hue_and_saturation(const Protocol::Address &addr,
                                      const MoveToHueSaturationMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue_and_saturation");
         return InterfaceHelper<ColourControl::Server>::move_to_hue_and_saturation(addr, message);
      }

      Common::Result move_to_xy(const Protocol::Address &addr,
                      const MoveToXYMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_to_xy");
         return InterfaceHelper<ColourControl::Server>::move_to_xy(addr, message);
      }

      Common::Result move_xy(const Protocol::Address &addr,
                   const MoveXYMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_xy");
         return InterfaceHelper<ColourControl::Server>::move_xy(addr, message);
      }

      Common::Result step_xy(const Protocol::Address &addr,
                   const StepXYMessage &message) override
      {
         mock("ColourControl::Server").actualCall("step_xy");
         return InterfaceHelper<ColourControl::Server>::step_xy(addr, message);
      }

      Common::Result move_to_colour_temperature(const Protocol::Address &addr,
                                      const MoveToTemperatureMessage &message) override
      {
         mock("ColourControl::Server").actualCall("move_to_colour_temperature");
         return InterfaceHelper<ColourControl::Server>::move_to_colour_temperature(addr, message);
      }

      Common::Result stop(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("stop");
         return InterfaceHelper<ColourControl::Server>::stop(addr);
      }

      void notify (const HF::Attributes::IAttribute &old_value,
                   const HF::Attributes::IAttribute &new_value) const
      {
         mock("Interface").actualCall("notify")
               .withParameterOfType("IAttribute", "old", &old_value)
               .withParameterOfType("IAttribute", "new", &new_value);
      }

      void changed(void)
      {
         mock("ColourControl::Server").actualCall("changed");
      }

      void add_transition(ITransition *T)
      {
         mock("ColourControl::Server").actualCall("add_transition");
         return InterfaceHelper<ColourControl::Server>::add_transition(T);
      }

      void remove(IServer const &itf)
      {
         mock("ColourControl::Server").actualCall("remove");
         return InterfaceHelper<ColourControl::Server>::remove(itf);
      }

      Server::Container &transitions()
      {
         return Server::transitions;
      }

      void clear_transitions(void)
      {
         for(ITransition *entry: Server::transitions)
         {
            delete entry;
         }

         Server::transitions = Server::Container();
      }
   };


   ColourControlServer server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;


   TEST_SETUP()
   {
      server                    = ColourControlServer();

      addr = Protocol::Address(42, 0);
      link = Testing::Link();

      packet                    = Protocol::Packet();
      packet.source             = addr;
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server.uid();
      packet.message.itf.member = 0xFF;

      packet.message.type = Protocol::Message::COMMAND_REQ;
      packet.link = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      payload = ByteArray(0);
      server.clear_transitions();
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
   //CHECK_OPT_ATTRIBUTE(ColourControlServer, HueAndSaturation, false, hue_and_saturation, 42, 142);
}

//! @test XY support.
TEST(ColourControlServer, Xy)
{
   // FIXME Generated Stub.
   //CHECK_OPT_ATTRIBUTE(ColourControlServer, Xy, false, xy, 42, 142);
}

//! @test Colour Temperature support.
TEST(ColourControlServer, ColourTemperature)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, ColourTemperature, false, colour_temperature, 42, 142);
}

//! @test Hue_transition test.
TEST(ColourControlServer, Hue_Transition)
{
   server.hue_and_saturation(HS_Colour(100,50));
   Hue_Transition transition(server,
                             1,                    // Period
                             10,                   // Step
                             0,                    // Number of steps
                             110);                 // Final value

   CHECK_TRUE(transition.run(1));   // check if the transition ran
   CHECK_FALSE(transition.next());  // it should be no more transitions...

   LONGS_EQUAL(110, server.hue_and_saturation().hue);
}

//! @test Hue_transition_Continuous test.
TEST(ColourControlServer, Hue_Transition_Continuous)
{
   server.hue_and_saturation(HS_Colour(100,50));
   Hue_Transition_Continuous transition(server,
                             10,                   // Period
                             10);                  // Step

   CHECK_FALSE(transition.run(1));              // check if the transition didn't ran
   LONGS_EQUAL(9, transition.remaining_time);
   CHECK_TRUE(transition.run(9));               // Now it ran
   CHECK_TRUE(transition.next());               // it should continue...

   LONGS_EQUAL(110, server.hue_and_saturation().hue);
}

//! @test Hue_transition test.
TEST(ColourControlServer, Saturation_Transition)
{
   server.hue_and_saturation(HS_Colour(100,50));
   Saturation_Transition transition(server,
                                    1,                    // Period
                                    10,                   // Step
                                    0,                    // Number of steps
                                    110);                 // Final value

   CHECK_TRUE(transition.run(1));   // check if the transition ran
   CHECK_FALSE(transition.next());  // it should be no more transitions...

   LONGS_EQUAL(110, server.hue_and_saturation().saturation);
}

//! @test Hue_transition_Continuous test.
TEST(ColourControlServer, Saturation_Transition_Continuous)
{
   server.hue_and_saturation(HS_Colour(100,50));
   Saturation_Transition_Continuous transition(server,
                                               10,                   // Period
                                               10);                  // Step

   CHECK_FALSE(transition.run(1));              // check if the transition didn't ran
   LONGS_EQUAL(9, transition.remaining_time);
   CHECK_TRUE(transition.run(9));               // Now it ran
   CHECK_TRUE(transition.next());               // it should continue...

   LONGS_EQUAL(60, server.hue_and_saturation().saturation);
}


//! @test Move To Hue support instantaneously.
TEST(ColourControlServer, MoveToHue_Instantly)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   MoveToHueMessage received(150,Direction::UP,0);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);

   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(150, 50), &server);

   mock("ColourControl::Server").expectOneCall("move_to_hue");
   mock("Interface").expectOneCall("notify")
            .withParameterOfType("IAttribute", "new", &mode_new)
            .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
            .withParameterOfType("IAttribute", "old", &HS_old)
            .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   LONGS_EQUAL(150, server.hue_and_saturation().hue);
   LONGS_EQUAL(0, server.transitions().size());          // no transitions on the servers

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();
}

//! @test Move To Hue support with time frame.
TEST(ColourControlServer, MoveToHue_With_Time)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   MoveToHueMessage received(150,Direction::UP,5);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE,&server);

   HueAndSaturation HS_old(HS_Colour(100,50),&server);
   HueAndSaturation HS_new(HS_Colour(110,50),&server);

   mock("ColourControl::Server").expectOneCall("move_to_hue");
   mock("ColourControl::Server").expectOneCall("changed");

   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));
   LONGS_EQUAL(110, server.hue_and_saturation().hue);
   LONGS_EQUAL(1, server.transitions().size());

   mock("Interface").checkExpectations();
   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Hue support direction UP.
TEST(ColourControlServer, MoveToHue_Check_step_DIR_UP)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   MoveToHueMessage received(150,Direction::UP,5);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(3, static_cast<Hue_Transition *>(server.transitions().at(0))->n_steps);
   LONGS_EQUAL(10, static_cast<Hue_Transition *>(server.transitions().at(0))->step);

}

//! @test Move To Hue support Direction DOWN.
TEST(ColourControlServer, MoveToHue_Check_step_DIR_Down)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   MoveToHueMessage received(150,Direction::DOWN,5);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(3, static_cast<Hue_Transition *>(server.transitions().at(0))->n_steps);
   LONGS_EQUAL(-62, static_cast<Hue_Transition *>(server.transitions().at(0))->step);
}

//! @test Move To Hue support Direction SHORTEST.
TEST(ColourControlServer, MoveToHue_Check_step_DIR_Shortest)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   MoveToHueMessage received(150,Direction::SHORTEST,5);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(3, static_cast<Hue_Transition *>(server.transitions().at(0))->n_steps);
   LONGS_EQUAL(10, static_cast<Hue_Transition *>(server.transitions().at(0))->step);
}

//! @test Move To Hue support Direction LONGEST.
TEST(ColourControlServer, MoveToHue_Check_step_DIR_Longest)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   MoveToHueMessage received(150,Direction::LONGEST,5);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(3, static_cast<Hue_Transition *>(server.transitions().at(0))->n_steps);
   LONGS_EQUAL(-62, static_cast<Hue_Transition *>(server.transitions().at(0))->step);

   server.clear_transitions();
}

//! @test Move To Hue no support.
TEST(ColourControlServer, MoveToHue_No_Support)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //No HS Support

   MoveToHueMessage received(150,Direction::UP,0);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   mock("ColourControl::Server").expectOneCall("move_to_hue");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectNoCall("notify");

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   CHECK_EQUAL(Common::Result::FAIL_SUPPORT, server.handle(packet, payload, 0));

   LONGS_EQUAL(0, server.transitions().size());

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();
}

//! @test Move Hue support.
TEST(ColourControlServer, MoveHue)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   MoveHueMessage received(Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE,&server);
   HueAndSaturation HS_old(HS_Colour(100,50),&server);
   HueAndSaturation HS_new(HS_Colour(110,50),&server);

   mock("ColourControl::Server").expectOneCall("move_hue");
   mock("ColourControl::Server").expectOneCall("changed");
   mock("Interface").expectOneCall("notify")
            .withParameterOfType("IAttribute", "new", &mode_new)
            .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
           .withParameterOfType("IAttribute", "old", &HS_old)
           .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_HUE_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->period);
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->step);
}

//! @test Move Hue no support.
TEST(ColourControlServer, MoveHue_no_support)
{
   server.hue_and_saturation(HS_Colour(100,50));
   server.supported(ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //No HS Support

   MoveHueMessage received(Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   mock("ColourControl::Server").expectOneCall("move_hue");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectNoCall("notify");

   packet.message.itf.member = ColourControl::MOVE_HUE_CMD;

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server.handle(packet, payload, 0));
   LONGS_EQUAL(0, server.transitions().size());

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();
}



//! @test Step Hue support.
TEST(ColourControlServer, StepHue)
{
   server.hue_and_saturation(HS_Colour(100, 50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE
                    +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   StepHueMessage received(10, Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);
   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(110, 50), &server);

   mock("ColourControl::Server").expectOneCall("step_hue");
   mock("ColourControl::Server").expectOneCall("changed");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::STEP_HUE_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->period);
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->step);
}

//! @test Step Hue no support.
TEST(ColourControlServer, StepHue_no_support)
{
   server.hue_and_saturation(HS_Colour(100, 50));
   server.supported(ColourControl::Mask::XY_MODE+
                    ColourControl::Mask::TEMPERATURE_MODE);    //no HS Support

   StepHueMessage received(10, Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   mock("ColourControl::Server").expectOneCall("step_hue");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectNoCall("notify");

   packet.message.itf.member = ColourControl::STEP_HUE_CMD;

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();
}

//! @test Move To Saturation support.
TEST(ColourControlServer, MoveToSaturation_Instantly)
{
   server.hue_and_saturation(HS_Colour(100, 50));

   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE
                    +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   MoveToSaturationMessage received(10, Direction::UP, 0);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);
   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(100, 60), &server);

   mock("ColourControl::Server").expectOneCall("move_to_saturation");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(0, server.transitions().size());
}

//! @test Move To Saturation support.
TEST(ColourControlServer, MoveToSaturation_with_time)
{
   server.hue_and_saturation(HS_Colour(100, 50));

   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE
                    +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   MoveToSaturationMessage received(10, Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);
   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(100, 51), &server);

   mock("ColourControl::Server").expectOneCall("move_to_saturation");
   mock("ColourControl::Server").expectOneCall("changed");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(1, static_cast<Saturation_Transition *>(server.transitions().at(0))->period);
   LONGS_EQUAL(1, static_cast<Saturation_Transition *>(server.transitions().at(0))->step);
   LONGS_EQUAL(8, static_cast<Saturation_Transition *>(server.transitions().at(0))->n_steps);

}

//! @test Move To Saturation Direction DOWN support.
TEST(ColourControlServer, MoveToSaturation_with_time_DIR_DOWN)
{
   server.hue_and_saturation(HS_Colour(100, 50));

   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE
                    +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   MoveToSaturationMessage received(10, Direction::DOWN, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);
   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(100, 49), &server);

   mock("ColourControl::Server").expectOneCall("move_to_saturation");
   mock("ColourControl::Server").expectOneCall("changed");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(1, static_cast<Saturation_Transition *>(server.transitions().at(0))->period);
   LONGS_EQUAL(-1, static_cast<Saturation_Transition *>(server.transitions().at(0))->step);
   LONGS_EQUAL(8, static_cast<Saturation_Transition *>(server.transitions().at(0))->n_steps);

}

//! @test Move To Saturation no support.
TEST(ColourControlServer, MoveToSaturation_no_suport)
{
   server.hue_and_saturation(HS_Colour(100, 50));

   server.supported(ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //no HS Support

   MoveToSaturationMessage received(10, Direction::DOWN, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   mock("ColourControl::Server").expectOneCall("move_to_saturation");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectNoCall("notify");

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

}

//! @test Move Saturation support.
TEST(ColourControlServer, MoveSaturation)
{
   server.hue_and_saturation(HS_Colour(100, 50));
   server.supported(ColourControl::Mask::HS_MODE +
                    ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //HS Support

   MoveSaturationMessage received(Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   Mode mode_new(Mask::HS_MODE, &server);
   HueAndSaturation HS_old(HS_Colour(100, 50), &server);
   HueAndSaturation HS_new(HS_Colour(100, 60), &server);

   mock("ColourControl::Server").expectOneCall("move_saturation");
   mock("ColourControl::Server").expectOneCall("changed");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "new", &mode_new)
         .ignoreOtherParameters();
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &HS_old)
         .withParameterOfType("IAttribute", "new", &HS_new);

   packet.message.itf.member = ColourControl::MOVE_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server.transitions().size());
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->period);
   LONGS_EQUAL(10, static_cast<Hue_Transition_Continuous *>(server.transitions().at(0))->step);

}

//! @test Move Saturation support.
TEST(ColourControlServer, MoveSaturation_no_suport)
{
   server.hue_and_saturation(HS_Colour(100, 50));
   server.supported(ColourControl::Mask::XY_MODE +
                    ColourControl::Mask::TEMPERATURE_MODE);    //no-HS Support

   MoveSaturationMessage received(Direction::UP, 10);
   payload = ByteArray(received.size());
   received.pack(payload);                         //pack it

   mock("ColourControl::Server").expectOneCall("move_saturation");
   mock("ColourControl::Server").expectNoCall("changed");
   mock("Interface").expectNoCall("notify");

   packet.message.itf.member = ColourControl::MOVE_SATURATION_CMD;

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server.handle(packet, payload, 0));

   mock("ColourControl::Server").checkExpectations();
   mock("Interface").checkExpectations();
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
}

// =============================================================================
// Server transitions
// =============================================================================

//! @test Add_Transition support.
TEST(ColourControlServer, Add_Transition)
{
   Hue_Transition_Continuous *test = new Hue_Transition_Continuous(server,10, 5);

   LONGS_EQUAL(0, server.transitions().size());

   mock("ColourControl::Server").expectOneCall("add_transition");

   server.add_transition(test);
   LONGS_EQUAL(1, server.transitions().size());

   mock("ColourControl::Server").checkExpectations();
}

//! @test Remove_Transition support.
TEST(ColourControlServer, Remove_Transition)
{
   Hue_Transition_Continuous *test = new Hue_Transition_Continuous(server,10, 5);


   mock("ColourControl::Server").expectOneCall("add_transition");
   server.add_transition(test);

   mock("ColourControl::Server").expectOneCall("remove");
   server.remove(server);

   LONGS_EQUAL(0, server.transitions().size());

   mock("ColourControl::Server").checkExpectations();
}

//! @test Transition support.
TEST(ColourControlServer, Transition_delete)
{
   ColourControlServer server1;
   ColourControlServer server2;

   server1.hue_and_saturation(HS_Colour(100,50));
   server2.hue_and_saturation(HS_Colour(100,50));

   Hue_Transition_Continuous *test1 = new Hue_Transition_Continuous(server1,1, 5);
   Hue_Transition            *test2 = new Hue_Transition(server2, 1, 5, 2, 111);

   server1.add_transition(test1);
   server2.add_transition(test2);

   LONGS_EQUAL(2, server.transitions().size());

   for(uint8_t i =0; i<10; ++i)
   {
      std::string loc = std::to_string(i);
      loc = "For i=" + loc;

      server.transition();

      if(i<2)
      {
         LONGS_EQUAL_TEXT(2, server.transitions().size(),loc.c_str());
      }
      else
      {
         LONGS_EQUAL_TEXT(1, server.transitions().size(),loc.c_str());
      }
   }

   LONGS_EQUAL(1, server.transitions().size());
   POINTERS_EQUAL(test1, server.transitions().front());

   LONGS_EQUAL(150, server1.hue_and_saturation().hue);
   LONGS_EQUAL(111, server2.hue_and_saturation().hue);

}

//! @test Transition support.
TEST(ColourControlServer, Transition_order)
{
   ColourControlServer server1;
   ColourControlServer server2;

   server1.hue_and_saturation(HS_Colour(100,50));
   server2.hue_and_saturation(HS_Colour(100,50));

   Hue_Transition_Continuous *test1 = new Hue_Transition_Continuous(server1,2, 5);
   Hue_Transition_Continuous *test2 = new Hue_Transition_Continuous(server2,3, 5);

   server1.add_transition(test1);
   server2.add_transition(test2);

   LONGS_EQUAL(2, server.transitions().size());

   POINTERS_EQUAL(test1, server.transitions().front());

   server.transition();                                  // Swap test1 and test2

   POINTERS_EQUAL(test2, server.transitions().front());

   server.transition();                                  // Swap test2 and test1

   POINTERS_EQUAL(test1, server.transitions().front());

}
