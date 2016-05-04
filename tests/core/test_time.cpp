// =============================================================================
/*!
 * @file       tests/core/test_time.cpp
 *
 * This is file contains the unit tests for the Time service implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/time.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::Time;

// =============================================================================
// Helper
// =============================================================================

SimpleString StringFrom(const Time::Date &date)
{
   std::ostringstream out;
   out << date;
   return SimpleString(out.str().c_str());
}

static std::array<uint8_t, 13> const month_days =
{
   0,  // None
   31, // January
   28, // February
   31, // March
   30, // April
   31, // May
   30, // June
   31, // July
   31, // August
   30, // September
   31, // October
   30, // November
   31, // December
};

// =============================================================================
// Time
// =============================================================================

//! Test Group for Time service parent class.
TEST_GROUP(Time)
{
   struct TimeBase: public Time::Base
   {
      TimeBase(HF::Core::Unit0 &unit): Time::Base(unit) {}

      Interface::Role role() const
      {
         return Interface::SERVER_ROLE;
      }
   };

   Testing::Device *device;
   TimeBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new TimeBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;
      mock().clear();
   }
};

//! @test Time::uid should return @c HF::Interface::TIME.
TEST(Time, UID)
{
   LONGS_EQUAL(HF::Interface::TIME, service->uid());
}

TEST(Time, Convert_Date2Time_Bounds)
{
   Time::Date date;

   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.year   = Time::YEARS_MIN;
   date.month  = Time::MONTHS_MIN;
   date.day    = Time::DAYS_MIN;
   date.hour   = Time::HOURS_MIN;
   date.minute = Time::MINUTES_MIN;
   date.second = Time::SECONDS_MIN;

   LONGS_EQUAL(0x00000000, Time::convert(date));

   // Check for month values
   date.month = Time::MONTHS_MIN - 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.month = Time::MONTHS_MAX + 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.month = Time::MONTHS_MIN;

   // Check for day values
   date.day = Time::DAYS_MIN - 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.day = Time::DAYS_MAX + 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.day = Time::DAYS_MIN;

   // Check for hour values
   date.hour = Time::HOURS_MAX + 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.hour = Time::HOURS_MIN;

   // Check for minutes values
   date.minute = Time::MINUTES_MAX + 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.minute = Time::MINUTES_MIN;

   // Check for seconds values
   date.second = Time::SECONDS_MAX + 1;
   LONGS_EQUAL(Value::INVALID, Time::convert(date));

   date.second = Time::SECONDS_MIN;
}

extern bool year_is_leap(uint8_t year);

TEST(Time, YearLeap)
{
   std::vector<uint16_t> const leap_years =
   {
      2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040, 2044, 2048, 2052, 2056,
      2060, 2064, 2068, 2072, 2076, 2080, 2084, 2088, 2092, 2096, 2104, 2108, 2112, 2116, 2120,
      2124, 2128, 2132, 2136, 2140, 2144, 2148, 2152, 2156, 2160, 2164, 2168, 2172, 2176, 2180,
      2184, 2188, 2192, 2196, 2204, 2208, 2212, 2216, 2220, 2224, 2228, 2232, 2236, 2240, 2244,
      2248, 2252, 2256
   };

   for (uint16_t i = 0; i < 256; ++i)
   {
      bool expected =
         std::binary_search(leap_years.begin(), leap_years.end(), Time::YEARS_BASE + i);
      bool actual   = year_is_leap(i);

      CHECK_TRUE(expected == actual);
   }
}

/*!
 * Test the conversion from Date to timestamp.
 */
TEST(Time, Convert_Date2Time)
{
   Time::Date date;

   // Check seconds - 2000-01-01 00:00:42
   date        = Time::Origin;
   date.second = 42;
   LONGS_EQUAL(42, Time::convert(date));

   // Check minutes - 2000-01-01 00:42:00
   date        = Time::Origin;
   date.minute = 42;
   LONGS_EQUAL(42 * 60, Time::convert(date));

   // Check hours - 2000-01-01 12:00:00
   date      = Time::Origin;
   date.hour = 12;
   LONGS_EQUAL(12 * 60 * 60, Time::convert(date));

   // Check sum - 2000-01-01 12:34:56
   date        = Time::Origin;
   date.hour   = 12;
   date.minute = 34;
   date.second = 56;

   uint32_t sum = 12 * 60 * 60 + 34 * 60 + 56;
   LONGS_EQUAL(sum, Time::convert(date));

   // Check days - 2000-01-01 00:00:00
   date = Time::Origin;
   LONGS_EQUAL(0x00000000, Time::convert(date));

   date.day++; // 2000-01-02 00:00:00
   LONGS_EQUAL(1 * 24 * (60 * 60), Time::convert(date));

   date.day += 4; // 2000-01-06 00:00:00
   LONGS_EQUAL(5 * 24 * (60 * 60), Time::convert(date));

   // Check sum - 2000-01-19 12:34:56
   date        = Time::Origin;
   date.day    = 19;
   date.hour   = 12;
   date.minute = 34;
   date.second = 56;

   sum        += 18 * 24 * (60 * 60);
   LONGS_EQUAL(sum, Time::convert(date));

   // Check days - 2000-01-01 00:00:00
   date = Time::Origin;

   // Check years/months
   uint32_t expected = 0;
   uint32_t actual   = 0;
   uint32_t check    = 0;

   for (uint16_t year = YEARS_MIN; year <= YEARS_MAX; ++year)
   {
      bool leap = year_is_leap(year);
      date.year = year;

      for (uint8_t month = MONTHS_MIN; month <= MONTHS_MAX; ++month)
      {
         date.month = month;

         uint8_t days = month_days[month];

         if (leap && month == 2)
         {
            days++;
         }

         for (uint8_t day = DAYS_MIN; day <= days; ++day, expected += (24 * 60 * 60))
         {
            date.day = day;
            actual   = convert(date);
            check_index<uint32_t>(expected, actual, year << 16 | month << 8 | day,
                                  "Time", __FILE__, __LINE__);
         }
      }

      check += (leap ? 366 : 365) * (24 * 60 * 60);

      check_index<uint32_t>(check, expected, year, "Year", __FILE__, __LINE__);
   }
}

/*!
 * Test the conversion from timestamp to Date.
 *
 * @note this test assumes that the Date to timestamp is working.
 */
TEST(Time, Convert_Time2Date)
{
   Time::Date expected;
   Time::Date actual;
   uint32_t time = 0;

   // Check for start of epoch
   expected = Time::Origin;
   time     = Time::convert(expected);
   actual   = Time::convert(time);

   CHECK_EQUAL(expected, actual);

   // Check maximum value
   expected = Time::End;
   actual   = Time::convert(Time::Value::MAX);

   CHECK_EQUAL(expected, actual);

   for (uint16_t year = YEARS_MIN; year <= YEARS_MAX; ++year)
   {
      expected.year = year;
      bool leap = year_is_leap(year);

      for (uint8_t month = MONTHS_MIN; month <= MONTHS_MAX; ++month)
      {
         expected.month = month;

         uint8_t days = month_days[month];

         if (leap && month == 2)
         {
            days++;
         }

         for (uint8_t day = DAYS_MIN; day <= days; ++day)
         {
            expected.day = day;

            if (expected > Time::End)
            {
               break;
            }

            time   = Time::convert(expected);
            actual = Time::convert(time);

            check_index<Time::Date>(expected, actual, time, "Time", __FILE__, __LINE__);
         }
      }
   }
}

// =============================================================================
// Time Client
// =============================================================================

//! Test Group for Time Client service class.
TEST_GROUP(TimeClient)
{
   struct TimeClient: public Time::Client
   {
      TimeClient(HF::Core::Unit0 &unit):
         Time::Client(unit)
      {}

   };

   Testing::Device *device;
   TimeClient *client;

   TEST_SETUP()
   {
      device = new Testing::Device();
      client = new TimeClient(*(device->unit0()));
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;
      mock().clear();
   }
};

// =============================================================================
// Time Server
// =============================================================================

//! Test Group for Time Server service class.
TEST_GROUP(TimeServer)
{
   struct TimeServer: public InterfaceHelper<Time::Server>
   {
      TimeServer(HF::Core::Unit0 &unit):
         InterfaceHelper<Time::Server>(unit)
      {}

   };

   Testing::Device *device;
   TimeServer *server;

   TEST_SETUP()
   {
      device = new Testing::Device();
      server = new TimeServer(*(device->unit0()));
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete device;
      mock().clear();
   }
};

//! @test Time support.
TEST(TimeServer, Time)
{
   CHECK_ATTRIBUTE(TimeServer, Value, true, time, 42, 142);
}

//! @test Tick method.
TEST(TimeServer, Tick)
{
   server->time(42);
   server->tick();

   LONGS_EQUAL(43, server->time());

   server->time(42);
   server->tick(100);

   LONGS_EQUAL(142, server->time());

   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();

   server->time(Time::Value::INVALID - 100);
   server->tick(100);

   LONGS_EQUAL(Time::Value::INVALID, server->time());

   server->time(Time::Value::INVALID - 100);
   server->tick(101);

   LONGS_EQUAL(Time::Value::INVALID, server->time());

   mock("support").checkExpectations();
}
