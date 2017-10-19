// =============================================================================
/*!
 * @file       src/core/time.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Time service.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/time.h"

// =============================================================================
// Global variables
// =============================================================================

static uint8_t const month_days[] =
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
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Time;


// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return Time::create_attribute(uid);
   }
}

// =============================================================================
// Time::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Time::create_attribute(uint8_t uid)
{
   using namespace HF::Core::Time;

   Time::Attributes attr = static_cast<Time::Attributes>(uid);

   switch (attr)
   {
      case TIME_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::TIME, attr, Value::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Helper functions
// =============================================================================

// =============================================================================
// minutes_in_seconds
// =============================================================================
/*!
 * Calculate the number of seconds for the given number of @c minutes.
 *
 * @param [in] minutes  number of minutes to convert.
 *
 * @return the number of seconds for the the given @c minutes.
 */
// =============================================================================
__attribute_pure__
inline static uint32_t minutes_in_seconds(uint16_t minutes)
{
   return minutes * 60;
}

// =============================================================================
// hours_in_seconds
// =============================================================================
/*!
 * Calculate the number of seconds for the given number of @c hours.
 *
 * @param [in] hours  number of hours to convert.
 *
 * @return the number of seconds for the the given @c hours.
 */
// =============================================================================
__attribute_pure__
inline static uint32_t hours_in_seconds(uint16_t hours)
{
   return hours * minutes_in_seconds(60);
}

// =============================================================================
// days_in_seconds
// =============================================================================
/*!
 * Calculate the number of seconds for the given number of @c days.
 *
 * @param [in] days  number of days to convert.
 *
 * @return the number of seconds for the the given @c days.
 */
// =============================================================================
__attribute_pure__
inline static uint32_t days_in_seconds(uint16_t days)
{
   return days * hours_in_seconds(24);
}

// =============================================================================
// months_in_seconds
// =============================================================================
/*!
 * Calculate the number of seconds that have passed until the first second of the
 * given @month.
 *
 * @param [in] month  month number to accumulate the passed seconds to.
 * @param [in] leap   indication if the year that we are calculating the month's
 *                    seconds is leap or not.
 *
 * @return the number of seconds from January until the given month (excluded).
 */
// =============================================================================
__attribute_pure__
static uint32_t months_in_seconds(uint8_t month, bool leap)
{
   assert((month >= Time::MONTHS_MIN && month <= Time::MONTHS_MAX));

   uint32_t result = 0;

   for (uint8_t i = 0; i < month; ++i)
   {
      result += days_in_seconds(month_days[i]);
   }

   // In case of a leap year and month is bigger to February
   // add an extra day of seconds.
   if (leap && month > 2)
   {
      result += days_in_seconds(1);
   }

   return result;
}

// =============================================================================
// year_is_leap
// =============================================================================
/*!
 * Indicate if the given year counting from @c Time::YEARS_BASE is a leap year.
 *
 * @param [in] year  year after @c Time::YEARS_BASE.
 *
 * @retval  true  if Time::YEARS_BASE + year is a leap year.
 * @retval  false otherwise.
 */
// =============================================================================
__attribute_pure__
STATIC bool year_is_leap(uint8_t year)
{
   uint16_t temp = Time::YEARS_BASE + year;

   if ((temp % 4) == 0 && ((temp % 100) != 0 || (temp % 400) == 0))
   {
      return true;
   }
   else
   {
      return false;
   }
}

// =============================================================================
// years_in_seconds
// =============================================================================
/*!
 * Calculate the number of seconds from @c Time::YEARS_BASE to Jan 1st 00:00:00
 * of @c Time::YEARS_BASE + @c year.
 *
 * @param [in] year  the @c Time::YEARS_BASE + @c year to calculate the number of
 *                   seconds for.
 *
 * @return the number of seconds for the range of years requested.
 */
// =============================================================================
__attribute_pure__
static uint32_t years_in_seconds(uint8_t year)
{
   uint32_t result = 0;

   for (int i = 0; i < year; ++i)
   {
      result += days_in_seconds(year_is_leap(i) ? 366 : 365);
   }

   return result;
}

// =============================================================================
// Time::convert
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Time::convert(const HF::Core::Time::Date &date)
{
   // Check values in date.

   /* *INDENT-OFF* */
   HF_ASSERT((date.year <= Time::YEARS_MAX), {return Time::Value::INVALID;});

   HF_ASSERT((date.month >= Time::MONTHS_MIN && date.month <= Time::MONTHS_MAX),
             {return Time::Value::INVALID;});

   HF_ASSERT((date.day >= Time::DAYS_MIN && date.day <= Time::DAYS_MAX),
             {return Time::Value::INVALID;});

   HF_ASSERT(date.hour <= Time::HOURS_MAX, {return Time::Value::INVALID;});

   HF_ASSERT(date.minute <= Time::MINUTES_MAX, {return Time::Value::INVALID;});

   HF_ASSERT(date.second <= Time::SECONDS_MAX, {return Time::Value::INVALID;});
   /* *INDENT-ON* */

   uint32_t result = date.second;
   result         += minutes_in_seconds(date.minute);
   result         += hours_in_seconds(date.hour);
   result         += days_in_seconds(date.day - 1);
   result         += months_in_seconds(date.month, year_is_leap(date.year));
   result         += years_in_seconds(date.year);

   return result;
}

// =============================================================================
// Time::convert
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Core::Time::Date Time::convert(const HF::Core::Time::Value &value)
{
   return std::move(convert(value.get()));
}

// =============================================================================
// Time::convert
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Core::Time::Date Time::convert(uint32_t value)
{
   Date result = Time::Origin;

   for (result.year = YEARS_MIN;; ++(result.year))
   {
      uint32_t temp = (year_is_leap(result.year) ? 366 * (24 * 60 * 60) : 365 * (24 * 60 * 60));

      if (value < temp)
      {
         break;
      }
      else
      {
         value -= temp;
      }
   }

   for (result.month = MONTHS_MIN; result.month <= MONTHS_MAX; ++(result.month))
   {
      uint32_t temp = days_in_seconds(month_days[result.month]);
      temp += (result.month == 2 && year_is_leap(result.year) ? days_in_seconds(1) : 0);

      if (value < temp)
      {
         break;
      }
      else
      {
         value -= temp;
      }
   }

   std::div_t temp = std::div(value, 24 * 60 * 60);
   result.day    = (temp.quot == 0 ? DAYS_MIN : temp.quot + 1);
   temp          = std::div(temp.rem, 60 * 60);
   result.hour   = temp.quot;
   temp          = std::div(temp.rem, 60);
   result.minute = temp.quot;
   result.second = temp.rem;

   return std::move(result);
}
