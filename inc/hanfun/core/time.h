// =============================================================================
/*!
 * @file       inc/hanfun/core/time.h
 *
 * This file contains the definitions for the Time service.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_CORE_TIME_H
#define HF_CORE_TIME_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace Time
      {
         class Server;
      }

      /*!
       * @ingroup time_itf
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] uid      attribute's UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(Time::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Time service.
       */
      namespace Time
      {
         /*!
          * @addtogroup time_itf  Time service
          * @ingroup core
          *
          * This module contains the classes that define and implement the Time service API.
          * @{
          */

         //! Attributes
         typedef enum _Attributes
         {
            TIME_ATTR     = 1, //!< Time attribute UID.
            __LAST_ATTR__ = TIME_ATTR
         } Attributes;

         //! Start year for time calculations - 2000
         static constexpr uint16_t YEARS_BASE = 2000;

         //! Minimum year value for time calculations.
         static constexpr uint8_t YEARS_MIN = 0;

         //! Maximum year value for time calculations
         static constexpr uint8_t YEARS_MAX = 136;

         //! Minimum month value for time calculations - January
         static constexpr uint8_t MONTHS_MIN = 1;

         //! Maximum month value for time calculations - December
         static constexpr uint8_t MONTHS_MAX = 12;

         //! Minimum day value for time calculations - 1st
         static constexpr uint8_t DAYS_MIN = 1;

         //! Maximum day value for time calculations - 31st
         static constexpr uint8_t DAYS_MAX = 31;

         //! Minimum hour value for time calculations - Midnight
         static constexpr uint8_t HOURS_MIN = 0;

         //! Maximum hour value for time calculations
         static constexpr uint8_t HOURS_MAX = 23;

         //! Minimum minute value for time calculations - 0 minutes
         static constexpr uint8_t MINUTES_MIN = 0;

         //! Maximum minute value for time calculations - 59 minutes
         static constexpr uint8_t MINUTES_MAX = 59;

         //! Minimum seconds value for time calculations - 0 seconds
         static constexpr uint8_t SECONDS_MIN = 0;

         //! Maximum seconds value for time calculations - 59 seconds
         static constexpr uint8_t SECONDS_MAX = 59;

         struct Date;

         /*!
          * Convert the date given by @c date to the corresponding time value.
          *
          * @param [in] date  reference to the date to convert from.
          *
          * @return  the time that corresponds to the given date or Value::INVALID in case of
          *          error.
          */
         uint32_t convert(const Date &date);

         /*!
          * Split the time given by @c value to the year, month, day, hour, minute and second
          * values.
          *
          * @param [in] time  value to convert.
          *
          * @return a Date structure containing the time value split into the proper fields.
          */
         Date convert(uint32_t time);

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Time attribute for the Time service.
          */
         struct Value: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint32_t INVALID  = 0xFFFFFFFF;  //!< Invalid value for time.
            static constexpr uint32_t MIN      = 0x00000000;  //!< Minimum value for time.
            static constexpr uint32_t MAX      = INVALID - 1; //!< Maximum value for time.

            static constexpr uint8_t  ID       = TIME_ATTR; //!< Attribute UID.
            static constexpr bool     WRITABLE = true;      //!< Attribute Read/Write

            Value(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::TIME, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Split the time given by @c value to the year, month, day, hour, minute and second
          * values.
          *
          * @param [in] value reference to the attribute containing the time value to convert.
          *
          * @return  a Date structure containing the time value split into the proper fields.
          */
         Date convert(const Value &value);

         /*!
          * Helper class to parse the time value into a human readable string.
          */
         struct Date
         {
            uint8_t year;     //!< Year : 20(00) - 2(255)
            uint8_t month;    //!< Month: 1 - 12
            uint8_t day;      //!< Day  : 1 - 31

            uint8_t hour;     //!< Hours   : 0 - 23
            uint8_t minute;   //!< Minutes : 0 - 59
            uint8_t second;   //!< Seconds : 0 - 59

            Date(uint8_t _year = 0, uint8_t _month = 0, uint8_t _day = 0,
                 uint8_t _hour = 0, uint8_t _minute = 0, uint8_t _second = 0):
               year(_year), month(_month), day(_day), hour(_hour),
               minute(_minute), second(_second)
            {}

            Date(uint32_t _time)
            {
               *this = convert(_time);
            }
         };

         static Date const Origin = Date(Value::MIN);

         static Date const End    = Date(Value::MAX);

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::Time::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::Time::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Time %Service : Parent.
          *
          * This is the parent class for the Time service implementation.
          */
         struct Base: public Service<HF::Interface::TIME>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit): Service<HF::Interface::TIME>(unit) {}
         };

         /*!
          * Time %Service : %Server side implementation.
          *
          * This class provides the server side of the Time interface.
          */
         class Server: public ServiceRole<Time::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint32_t _time;   //!< Time value

            public:

            //! Constructor
            Server(Unit0 &unit):
               ServiceRole<Time::Base, HF::Interface::SERVER_ROLE>(unit),
               _time(Value::INVALID) {}

            //! Destructor
            virtual ~Server() {}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Time for the Time server.
             *
             * @return  the current Time.
             */
            virtual uint32_t time() const;

            /*!
             * Set the Time for the Time server.
             *
             * @param [in] __value the  Time value to set the server to.
             */
            void time(uint32_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Increment time by the amount given by @c count seconds.
             *
             * @param [in] count number of seconds to increment the time count by.
             */
            void tick(uint32_t count = 1)
            {
               HF_ASSERT(Value::INVALID - _time > count, {_time = Value::INVALID;
                                                          return;});
               _time += count;
            }
         };

         /*!
          * Time %Service : %Client side implementation.
          *
          * This class provides the client side of the Time interface.
          */
         struct Client: public ServiceRole<Time::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(Unit0 &unit): ServiceRole<Time::Base, HF::Interface::CLIENT_ROLE>(unit) {}

            virtual ~Client() {}
         };

         /*! @} */

      }  // namespace Time

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup time_itf
 * @{
 */

inline bool operator==(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return (memcmp(&lhs, &rhs, sizeof(HF::Core::Time::Date)) == 0);
}

inline bool operator!=(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return !(lhs == rhs);
}

inline bool operator<(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return memcmp(&lhs, &rhs, sizeof(HF::Core::Time::Date)) < 0;
}

inline bool operator<=(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return memcmp(&lhs, &rhs, sizeof(HF::Core::Time::Date)) <= 0;
}

inline bool operator>(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return memcmp(&lhs, &rhs, sizeof(HF::Core::Time::Date)) > 0;
}

inline bool operator>=(const HF::Core::Time::Date &lhs, const HF::Core::Time::Date &rhs)
{
   return memcmp(&lhs, &rhs, sizeof(HF::Core::Time::Date)) >= 0;
}

// =============================================================================
// Stream Helpers
// =============================================================================


/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return reference to the @c stream.
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::Time::Attributes attribute);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] date        date value to convert to a string.
 *
 * @return reference to the @c stream.
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::Time::Date &date);
/*! @} */

#endif /* HF_CORE_TIME_H */
