// =============================================================================
/*!
 * \file       tests/interfaces/test_simple_power_meter.cpp
 *
 * This file contains the implementation of the tests for the Simple Metering
 * Interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#include "hanfun/interfaces/simple_power_meter.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

// =============================================================================
// SimplePowerMeter
// =============================================================================

//! Test Group for SimplePowerMeter interface parent class.
TEST_GROUP (SimplePowerMeter)
{
   class TestSimplePowerMeter:public InterfaceParentHelper <SimplePowerMeter>
   {};

   TestSimplePowerMeter interface;
};

//! \test SimplePowerMeter::uid should return \c Interface::SIMPLE_POWER_METER.
TEST (SimplePowerMeter, UID)
{
   CHECK_EQUAL (Interface::SIMPLE_POWER_METER, interface.uid ());
}

// =============================================================================
// SimplePowerMeter / Measurement
// =============================================================================

//! Test Group for SimplePowerMeter::Measurement class.
TEST_GROUP (SimplePowerMeter_Measurement)
{
   SimplePowerMeter::Measurement measurement;

   ByteArray expected;

   TEST_SETUP ()
   {
      measurement = SimplePowerMeter::Measurement ();

      uint8_t data[] = {0x00,                              0x00, 0x00,
                        SimplePowerMeter::MICRO_PRECISION,
                        0xFF,                              0x5A, 0xA5,0xCC,
                        0x00,                              0x00, 0x00};

      expected = ByteArray (data, sizeof(data));
   }
};

//! \test Should report correct size.
TEST (SimplePowerMeter_Measurement, Size)
{
   LONGS_EQUAL (5, measurement.size ());
}

//! \test Should pack the measurement correctly.
TEST (SimplePowerMeter_Measurement, Pack)
{
   measurement.unit  = SimplePowerMeter::MICRO_PRECISION;
   measurement.value = 0xFF5AA5CC;

   ByteArray array (measurement.size () + 6);

   size_t    wsize = measurement.pack (array, 3);

   LONGS_EQUAL (measurement.size (), wsize);

   CHECK_EQUAL (expected, array);
}

//! \test Should unpack the measurement correctly.
TEST (SimplePowerMeter_Measurement, Unpack)
{
   LONGS_EQUAL (SimplePowerMeter::BASE_PRECISION, measurement.unit);
   LONGS_EQUAL (0, measurement.value);

   size_t rsize = measurement.unpack (expected, 3);

   LONGS_EQUAL (measurement.size (), rsize);

   LONGS_EQUAL (SimplePowerMeter::MICRO_PRECISION, measurement.unit);
   LONGS_EQUAL (0xFF5AA5CC, measurement.value);
}

// =============================================================================
// SimplePowerMeter / Report
// =============================================================================

//! Test Group for SimplePowerMeter::Measurement class.
TEST_GROUP (SimplePowerMeter_Report)
{
   SimplePowerMeter::Report report;

   TEST_SETUP ()
   {
      report = SimplePowerMeter::Report ();
   }
};

//! \test Should report correct size.
TEST (SimplePowerMeter_Report, Size)
{
   LONGS_EQUAL (1 + 10 + 8 * 5 + 2 + 1, report.size ());
}

// Data used to check SimplePowerMeter::Report::pack.
static const uint8_t pack_data[] =
{
   0x00,                              0x00, 0x00,

   0x0A,                              // Number of attributes.

   SimplePowerMeter::ENERGY_ATTR,     // Energy measurement.
   SimplePowerMeter::BASE_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC1,

   SimplePowerMeter::ENERGY_AT_RESET_ATTR, // Energy measurement at last reset.
   SimplePowerMeter::MILI_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC2,

   SimplePowerMeter::TIME_AT_RESET_ATTR, // Device time measurement at last reset.
   SimplePowerMeter::UPTIME,
   0xFF,                              0x5A, 0xA5,0xC3,

   SimplePowerMeter::POWER_ATTR,      // Instantaneous Power measurement.
   SimplePowerMeter::MICRO_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC4,

   SimplePowerMeter::AVG_POWER_ATTR,  // Average Power measurement.
   SimplePowerMeter::NANO_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC5,

   SimplePowerMeter::AVG_POWER_INTERVAL_ATTR, // Average Power Interval.
   0x5A,                              0xA5,

   SimplePowerMeter::POWER_FACTOR_ATTR, // Power Factor.
   0xAA,

   SimplePowerMeter::VOLTAGE_ATTR,    // Voltage measurement.
   SimplePowerMeter::MEGA_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC6,

   SimplePowerMeter::CURRENT_ATTR,    // Current measurement.
   SimplePowerMeter::GIGA_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC7,

   SimplePowerMeter::FREQUENCY_ATTR,
   SimplePowerMeter::TERA_PRECISION,  // Frequency measurement.
   0xFF,                              0x5A, 0xA5,0xC8,
   0x00,                              0x00, 0x00
};

//! \test Should pack the measurement correctly.
TEST (SimplePowerMeter_Report, Pack)
{
   ByteArray expected (pack_data, sizeof(pack_data));

   uint32_t  value = 0xFF5AA5C1;

   report.energy.unit        = SimplePowerMeter::BASE_PRECISION;
   report.energy.value       = value++;

   report.last_energy.unit   = SimplePowerMeter::MILI_PRECISION;
   report.last_energy.value  = value++;

   report.last_time.unit     = SimplePowerMeter::UPTIME;
   report.last_time.value    = value++;

   report.power.unit         = SimplePowerMeter::MICRO_PRECISION;
   report.power.value        = value++;

   report.avg_power.unit     = SimplePowerMeter::NANO_PRECISION;
   report.avg_power.value    = value++;

   report.avg_power_interval = 0x5AA5;

   report.power_factor       = 0xAA;

   report.voltage.unit       = SimplePowerMeter::MEGA_PRECISION;
   report.voltage.value      = value++;

   report.current.unit       = SimplePowerMeter::GIGA_PRECISION;
   report.current.value      = value++;

   report.frequency.unit     = SimplePowerMeter::TERA_PRECISION;
   report.frequency.value    = value++;

   ByteArray array (report.size () + 6);

   size_t    wsize = report.pack (array, 3);

   LONGS_EQUAL (report.size (), wsize);

   CHECK_EQUAL (expected, array);
}

// Data used to check SimplePowerMeter::Report::unpack.
static const uint8_t unpack_data[] =
{
   0x00,                              0x00, 0x00,

   0x0A,                              // Number of attributes.

   SimplePowerMeter::TIME_AT_RESET_ATTR, // Device time measurement at last reset.
   SimplePowerMeter::UPTIME,
   0xFF,                              0x5A, 0xA5,0xC3,

   SimplePowerMeter::ENERGY_ATTR,     // Energy measurement.
   SimplePowerMeter::BASE_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC1,

   SimplePowerMeter::FREQUENCY_ATTR,
   SimplePowerMeter::TERA_PRECISION,  // Frequency measurement.
   0xFF,                              0x5A, 0xA5,0xC8,

   SimplePowerMeter::POWER_ATTR,      // Instantaneous Power measurement.
   SimplePowerMeter::MICRO_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC4,

   SimplePowerMeter::CURRENT_ATTR,    // Current measurement.
   SimplePowerMeter::GIGA_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC7,

   SimplePowerMeter::ENERGY_AT_RESET_ATTR, // Energy measurement at last reset.
   SimplePowerMeter::MILI_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC2,

   SimplePowerMeter::AVG_POWER_INTERVAL_ATTR, // Average Power Interval.
   0x5A,                              0xA5,

   SimplePowerMeter::POWER_FACTOR_ATTR, // Power Factor.
   0xAA,

   SimplePowerMeter::VOLTAGE_ATTR,    // Voltage measurement.
   SimplePowerMeter::MEGA_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC6,

   SimplePowerMeter::AVG_POWER_ATTR,  // Average Power measurement.
   SimplePowerMeter::NANO_PRECISION,
   0xFF,                              0x5A, 0xA5,0xC5,

   0x00,                              0x00, 0x00,
};

//! \test Should unpack the report correctly.
TEST (SimplePowerMeter_Report, Unpack)
{
   ByteArray array (unpack_data, sizeof(unpack_data));

   size_t    rsize = report.unpack (array, 3);

   LONGS_EQUAL (report.size (), rsize);

   uint32_t value = 0xFF5AA5C1;

   LONGS_EQUAL (SimplePowerMeter::BASE_PRECISION, report.energy.unit);
   LONGS_EQUAL (value++, report.energy.value);

   LONGS_EQUAL (SimplePowerMeter::MILI_PRECISION, report.last_energy.unit);
   LONGS_EQUAL (value++, report.last_energy.value);

   LONGS_EQUAL (SimplePowerMeter::UPTIME, report.last_time.unit);
   LONGS_EQUAL (value++, report.last_time.value);

   LONGS_EQUAL (SimplePowerMeter::MICRO_PRECISION, report.power.unit);
   LONGS_EQUAL (value++, report.power.value);

   LONGS_EQUAL (SimplePowerMeter::NANO_PRECISION, report.avg_power.unit);
   LONGS_EQUAL (value++, report.avg_power.value);

   LONGS_EQUAL (0x5AA5, report.avg_power_interval);

   LONGS_EQUAL (0xAA, report.power_factor);

   LONGS_EQUAL (SimplePowerMeter::MEGA_PRECISION, report.voltage.unit);
   LONGS_EQUAL (value++, report.voltage.value);

   LONGS_EQUAL (SimplePowerMeter::GIGA_PRECISION, report.current.unit);
   LONGS_EQUAL (value++, report.current.value);

   LONGS_EQUAL (SimplePowerMeter::TERA_PRECISION, report.frequency.unit);
   LONGS_EQUAL (value, report.frequency.value);
}

// =============================================================================
// SimplePowerMeterServer
// =============================================================================

//! Test Group for SimplePowerMeterServer interface class.
TEST_GROUP (SimplePowerMeterServer)
{
   class TestSimplePowerMeterServer:public InterfaceHelper <SimplePowerMeterServer>
   {
      public:

      using SimplePowerMeterServer::report;
   };

   TestSimplePowerMeterServer *server;

   SimplePowerMeter::Measurement initial;
   SimplePowerMeter::Measurement other;
   SimplePowerMeter::Measurement temp;

   TEST_SETUP ()
   {
      server        = new TestSimplePowerMeterServer ();

      initial.unit  = SimplePowerMeter::BASE_PRECISION;
      initial.value = 0;

      other.unit    = SimplePowerMeter::MEGA_PRECISION;
      other.value   = 42;

      temp          = other;

      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();

      delete server;
   }

   void check_equal (SimplePowerMeter::Measurement expected, SimplePowerMeter::Measurement actual, uint32_t line)
   {
      CHECK_EQUAL_LOCATION (expected.unit, actual.unit, __FILE__, line);
      CHECK_EQUAL_LOCATION (expected.value, actual.value, __FILE__, line);
   }

   void check_not_equal (SimplePowerMeter::Measurement &expected, SimplePowerMeter::Measurement actual, uint32_t line)
   {
      CHECK_LOCATION_FALSE (expected.unit == actual.unit, "CHECK_NOT_EQUAL", "expected.unit == actual.unit", __FILE__, line);
      CHECK_LOCATION_FALSE (expected.value == actual.value, "CHECK_NOT_EQUAL", "expected.value == actual.value", __FILE__, line);
   }
};

#define check_equal(_expected, _actual)       check_equal (_expected, _actual, __LINE__)
#define check_not_equal(_expected, _actual)   check_not_equal (_expected, _actual, __LINE__)

// =============================================================================
// SimplePowerMeterServer : Getter/Setter
// =============================================================================

TEST (SimplePowerMeterServer, energy)
{
   check_equal (initial, server->energy ());

   server->energy (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->energy ());

   check_equal (temp, server->energy ());
}

TEST (SimplePowerMeterServer, last_energy)
{
   check_equal (initial, server->last_energy ());

   server->last_energy (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->last_energy ());

   check_equal (temp, server->last_energy ());
}

TEST (SimplePowerMeterServer, last_time)
{
   initial.unit = SimplePowerMeter::UPTIME;
   other.unit   = SimplePowerMeter::UPTIME;
   temp         = other;

   check_equal (initial, server->last_time ());

   server->last_time (other);

   other.unit   = SimplePowerMeter::UTC;
   other.value += 42;

   check_not_equal (other, server->last_time ());

   check_equal (temp, server->last_time ());
}

TEST (SimplePowerMeterServer, power)
{
   check_equal (initial, server->power ());

   server->power (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->power ());

   check_equal (temp, server->power ());
}

TEST (SimplePowerMeterServer, avg_power)
{
   check_equal (initial, server->avg_power ());

   server->avg_power (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->avg_power ());

   check_equal (temp, server->avg_power ());
}

TEST (SimplePowerMeterServer, avg_power_interval)
{
   CHECK_EQUAL (0, server->avg_power_interval ());

   server->avg_power_interval (42);

   CHECK_EQUAL (42, server->avg_power_interval ());
}

TEST (SimplePowerMeterServer, voltage)
{
   check_equal (initial, server->voltage ());

   server->voltage (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->voltage ());

   check_equal (temp, server->voltage ());
}

TEST (SimplePowerMeterServer, current)
{
   check_equal (initial, server->current ());

   server->current (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->current ());

   check_equal (temp, server->current ());
}

TEST (SimplePowerMeterServer, frequency)
{
   check_equal (initial, server->frequency ());

   server->frequency (other);

   other.unit  += 1;
   other.value += 42;

   check_not_equal (other, server->frequency ());

   check_equal (temp, server->frequency ());
}

TEST (SimplePowerMeterServer, power_factor)
{
   CHECK_EQUAL (0, server->power_factor ());

   server->power_factor (42);

   CHECK_EQUAL (42, server->power_factor ());
}

TEST (SimplePowerMeterServer, report_interval)
{
   CHECK_EQUAL (0, server->report_interval ());

   server->report_interval (42);

   CHECK_EQUAL (42, server->report_interval ());
}

// =============================================================================
// SimplePowerMeterServer : API
// =============================================================================

//! \test Should create a report with the correct values.
TEST (SimplePowerMeterServer, report)
{
   SimplePowerMeter::Measurement energy;
   SimplePowerMeter::Measurement last_energy;
   SimplePowerMeter::Measurement last_time;
   SimplePowerMeter::Measurement power;
   SimplePowerMeter::Measurement avg_power;
   uint16_t power_interval;
   SimplePowerMeter::Measurement voltage;
   SimplePowerMeter::Measurement current;
   SimplePowerMeter::Measurement frequency;

   uint8_t  power_factor;
   uint16_t report_interval;

   uint32_t value = 1;
   uint8_t  unit  = SimplePowerMeter::MILI_PRECISION;

   last_time.value   = value++;
   last_time.unit    = SimplePowerMeter::UPTIME;

   last_energy.value = value++;
   last_energy.unit  = unit++;

   energy.value      = value++;
   energy.unit       = unit++;

   power.value       = value++;
   power.unit        = unit++;

   avg_power.value   = value++;
   avg_power.unit    = unit++;

   power_interval    = value++;

   voltage.value     = value++;
   voltage.unit      = unit++;

   current.value     = value++;
   current.unit      = unit++;

   frequency.value   = value++;
   frequency.unit    = unit++;

   power_factor      = value++;
   report_interval   = value;

   server->energy (energy);
   server->last_energy (last_energy);
   server->last_time (last_time);

   server->power (power);
   server->avg_power (avg_power);
   server->avg_power_interval (power_interval);

   server->voltage (voltage);
   server->current (current);
   server->frequency (frequency);

   server->power_factor (power_factor);
   server->report_interval (report_interval);

   SimplePowerMeter::Report *report = server->report ();

   CHECK (report != nullptr);

   check_equal (server->energy (), report->energy);                // Energy.

   check_equal (server->last_energy (), report->last_energy);      // Energy at Last Reset.
   check_equal (server->last_time (), report->last_time);          // Time at Last Reset.

   check_equal (server->power (), report->power);                  // Instantaneous Power.
   check_equal (server->avg_power (), report->avg_power);          // Average Power.

   // Average Power Interval.
   CHECK_EQUAL (server->avg_power_interval (), report->avg_power_interval);

   check_equal (server->voltage (), report->voltage);              // Voltage.
   check_equal (server->current (), report->current);              // Current.
   check_equal (server->frequency (), report->frequency);          // Frequency.

   CHECK_EQUAL (server->power_factor (), report->power_factor);    // Power Factor.

   // FIXME check_equal( report->, server->() );   // Report Interval.

   delete report;
}

//! \test Should send message on periodic event.
TEST (SimplePowerMeterServer, periodic)
{
   uint16_t time = 1234;

   server->report_interval (13);

   mock ("Interface").expectOneCall ("sendMessage");

   server->periodic (time);

   mock ("Interface").checkExpectations ();

   CHECK_FALSE (server->sendMsg.payload == nullptr);

   CHECK_EQUAL (Interface::CLIENT_ROLE, server->sendMsg.itf.role);
   CHECK_EQUAL (server->uid (), server->sendMsg.itf.uid);
   CHECK_EQUAL (SimplePowerMeter::REPORT_CMD, server->sendMsg.itf.member);

   delete server->sendMsg.payload;
   server->sendMsg.payload = nullptr;

   time                   += 10;
   server->periodic (time);
   CHECK_TRUE (server->sendMsg.payload == nullptr);

   mock ("Interface").expectOneCall ("sendMessage");

   time += 10;
   server->periodic (time);
   CHECK_FALSE (server->sendMsg.payload == nullptr);

   mock ("Interface").checkExpectations ();
}

//! \test Should not send message on periodic event disabled.
TEST (SimplePowerMeterServer, periodic_disabled)
{
   uint16_t time = 1234;

   server->report_interval (0);

   server->periodic (time);
   CHECK_TRUE (server->sendMsg.payload == nullptr);

   time += 10;
   server->periodic (time);
   CHECK_TRUE (server->sendMsg.payload == nullptr);
}


// =============================================================================
// SimplePowerMeterClient
// =============================================================================

//! Test Group for SimplePowerMeterClient interface class.
TEST_GROUP (SimplePowerMeterClient)
{
   class TestSimplePowerMeterClient:public InterfaceHelper <SimplePowerMeterClient>
   {
      public:

      void report (Report &report)
      {
         UNUSED (report);
         mock ("SimplePowerMeterClient").actualCall ("report");
      }
   };

   TestSimplePowerMeterClient client;
   ByteArray expected;

   Protocol::Packet packet;

   TEST_SETUP ()
   {
      client                    = TestSimplePowerMeterClient ();

      expected                  = ByteArray (unpack_data, sizeof(unpack_data));

      packet.message.itf.role   = Interface::CLIENT_ROLE;
      packet.message.itf.uid    = client.uid ();
      packet.message.itf.member = SimplePowerMeter::REPORT_CMD;

      packet.message.length     = expected.size ();
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

//! \test Should handle valid message.
TEST (SimplePowerMeterClient, Handle_Valid_Message)
{
   mock ("SimplePowerMeterClient").expectOneCall ("report");

   Result result = client.handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("SimplePowerMeterClient").checkExpectations ();
}

//! \test Should not handle message from invalid role.
TEST (SimplePowerMeterClient, Handle_Invalid_Role)
{
   packet.message.itf.role = Interface::SERVER_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, client.handle (packet, expected, 3));
}

//! \test Should not handle message from invalid interface UID.
TEST (SimplePowerMeterClient, Handle_Invalid_UID)
{
   packet.message.itf.uid = client.uid () + 1;

   CHECK_EQUAL (Result::FAIL_ID, client.handle (packet, expected, 3));
}

//! \test FIXME Should not handle message with invalid payload size.
IGNORE_TEST (SimplePowerMeterClient, Handle_Invalid_Payload_Size)
{
   SimplePowerMeter::Report report;
   packet.message.length = report.size () - 1;

   CHECK_EQUAL (Result::FAIL_ARG, client.handle (packet, expected, 3));
}

//! \test FIXME Should not handle message with not enough payload.
IGNORE_TEST (SimplePowerMeterClient, Handle_Invalid_Payload)
{
   CHECK_EQUAL (Result::FAIL_ARG, client.handle (packet, expected, 10));
}
