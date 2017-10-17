// =============================================================================
/*!
 * @file       inc/hanfun/core/weekly_scheduling.h
 *
 * This file contains the definitions for the Weekly Scheduling service.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_CORE_WEEKLY_SCHEDULING_H
#define HF_CORE_WEEKLY_SCHEDULING_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

#include "hanfun/core/scheduling.h"
#include "hanfun/core/time.h"

namespace HF
{
   namespace Core
   {
      namespace Scheduling
      {
         // Forward declaration
         namespace Weekly
         {
            struct IServer;
         }
      }

      /*!
       * @ingroup weekly_scheduling_itf
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
      HF::Attributes::IAttribute *create_attribute(Scheduling::Weekly::IServer *server, uint8_t uid);

      namespace Scheduling
      {
         /*!
          * This namespace contains the implementation of the Weekly Scheduling service.
          */
         namespace Weekly
         {
            typedef enum _Day_of_the_week
            {
               MONDAY = 0x00,
               TUESDAY,
               WEDNESDAY,
               THURSDAY,
               FRIDAY,
               SATURDAY,
               SUNDAY
            } Day_of_the_week;
            //! Specific part for the Weekly Scheduler of the @c HF::Scheduling::Entry.
            struct Day
            {

               uint8_t day;            //!< Day of Week.
               uint8_t hour;           //!< Hour of Day.
               uint8_t minute;         //!< Minute of Hour.

               Day(uint8_t _day, uint8_t _hour, uint8_t _minute):
                  day(_day), hour(_hour), minute(_minute)
               {}

               Day() = default;

               static constexpr int MINUTE = 60;
               static constexpr int HOUR = 60*MINUTE;
               static constexpr int DAY = 24*HOUR;
               static constexpr uint32_t WEEK = 7*DAY;

               static constexpr uint8_t DAY_OF_WEEK_BASE = SATURDAY; // Jan, 1st 2000 was on a Saturday (UTC)

               static Day convert (const uint32_t &timestamp)
               {
                  Day result;

                  std::div_t temp = std::div(timestamp, DAY);           // get the number of days
                                                                        // since the start

                  result.day = temp.quot % 7                            // n of days MOD 7
                             + DAY_OF_WEEK_BASE;

                  result.day = result.day % 7;

                  temp = std::div(temp.rem, HOUR);
                  result.hour = temp.quot;
                  temp = std::div(temp.rem, MINUTE);
                  result.minute = temp.quot;

                  return result;
               }

               /*!
                * Get the initial value for the next_run entry attribute.
                *
                * @return  the timestamp for the first run.
                */
               uint32_t first(uint32_t timestamp = 0) const
               {
                  uint32_t result = timestamp;

                  Day current(convert(timestamp));

                  result += (this->hour - current.hour) * 60 * 60;
                  result += (this->minute - current.minute) * 60;

                  result += (this->day - current.day)*24*60*60;

                  if(timestamp > result)
                  {
                     result += WEEK;
                  }

                  return result;
               }

               /*!
                * get the step between executions.
                * @return  the step time.
                */
               uint32_t step() const
               {
                  return WEEK;
               }

               bool active(uint32_t _time) const
               {
                  UNUSED(_time);
                  return true;
               }

               //! Minimum pack/unpack required data size.
               static constexpr uint16_t min_size = sizeof(uint8_t)     // Day of Week.
                                                    + sizeof(uint8_t)   // Hour of Day.
                                                    + sizeof(uint8_t);  // Minute of Hour.

               //! @copydoc HF::Common::Serializable::size
               uint16_t size() const
               {
                  return min_size;
               }

               //! @copydoc HF::Common::Serializable::pack
               uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

               //! @copydoc HF::Common::Serializable::unpack
               uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
            };

            typedef Scheduling::Entry<Day> Entry;

            //! Specific part for the Weekly Scheduler of the @c HF::Scheduling::GetEntryResonse.
            typedef Scheduling::GetEntryResponse<Day> GetEntryResponse;

            /*!
             * Helper class to handle the Maximum Number Of Entries attribute for the Weekly Scheduling service.
             */
            struct MaximumNumberOfEntries: public Scheduling::MaximumNumberOfEntries
            {
               MaximumNumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
                  Scheduling::MaximumNumberOfEntries(HF::Interface::WEEKLY_SCHEDULING,
                                                     value, owner)
               {}
            };

            /*!
             * Helper class to handle the Number Of Entries attribute for the Weekly Scheduling service.
             */
            struct NumberOfEntries: public Scheduling::NumberOfEntries
            {
               NumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
                  Scheduling::NumberOfEntries(HF::Interface::WEEKLY_SCHEDULING,
                                              value, owner)
               {}
            };

            /*!
             * Helper class to handle the Status attribute for the Weekly Scheduling service.
             */
            struct Status: public Scheduling::Status
            {
               Status(uint8_t value = 0, HF::Interface *owner = nullptr):
                  Scheduling::Status(HF::Interface::WEEKLY_SCHEDULING,
                                     value, owner)
               {}
            };

            /*!
             * @copybrief HF::Core::create_attribute (HF::Interfaces::Scheduling::Weekly::Server *,uint8_t)
             *
             * @see HF::Core::create_attribute (HF::Core::Scheduling::Weekly::Server *,uint8_t)
             *
             * @param [in] uid   attribute %UID to create the attribute object for.
             *
             * @retval  pointer to an attribute object
             * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
             */
            HF::Attributes::IAttribute *create_attribute(uint8_t uid);

            using IServerBase = Scheduling::Base<HF::Interface::WEEKLY_SCHEDULING,
                                                 Scheduling::IServer>;

            using IClientBase = Interfaces::Interface<HF::Interface::WEEKLY_SCHEDULING,
                                                      Scheduling::IClient>;

            /*!
             * Weekly Scheduling %Service : %Client side implementation.
             *
             * This class provides the client side of the Weekly Scheduling interface.
             */
            struct IClient: public IClientBase
            {
               //! Constructor
               IClient(): IClientBase()
               {}

               //! Destructor
               virtual ~IClient() = default;

               static constexpr HF::Interface::UID ITF = HF::Interface::WEEKLY_SCHEDULING;

               //! @copydoc HF::Core::Scheduling::activate_scheduler(Protocol::Address).
               virtual void activate_scheduler(const Protocol::Address &addr, uint8_t _status)
               {
                  Scheduling::IClient::activate_scheduler(addr, ITF, _status);
               }

               //! @copydoc HF::Core::Scheduling::define_event(Protocol::Address).
               virtual void define_event(const Protocol::Address &addr, uint8_t id, uint8_t status,
                                         Day &time, uint8_t pid);

#ifdef HF_CORE_WEEKLY_SCHEDULING_UPDATE_EVENT_STATUS_CMD
               //! @copydoc HF::Core::Scheduling::update_event_status(Protocol::Address).
               virtual void update_event_status(const Protocol::Address &addr,
                                                uint8_t id, uint8_t status)
               {
                  Scheduling::IClient::update_event_status(addr, ITF, id, status);
               }
#endif

#ifdef HF_CORE_WEEKLY_SCHEDULING_GET_EVENT_ENTRY_CMD
               //! @copydoc HF::Core::Scheduling::get_event_entry(Protocol::Address).
               virtual void get_event_entry(const Protocol::Address &addr, uint8_t id)
               {
                  Scheduling::IClient::get_event_entry(addr, ITF, id);
               }
#endif

               //! @copydoc HF::Core::Scheduling::delete_event(Protocol::Address).
               virtual void delete_event(const Protocol::Address &addr, uint8_t id)
               {
                  Scheduling::IClient::delete_event(addr, ITF, id);
               }

#ifdef HF_CORE_WEEKLY_SCHEDULING_DELETE_ALL_EVENTS_CMD
               //! @copydoc HF::Core::Scheduling::delete_all_events(Protocol::Address).
               virtual void delete_all_events(const Protocol::Address &addr)
               {
                  Scheduling::IClient::delete_all_events(addr, ITF);
               }
#endif

               using IClientBase::send;
            };

            /*!
             * Scheduling %Service : %Client side implementation.
             *
             * This class provides the client side of the Scheduling interface.
             */
            struct Client: public IClient
            {
               /*!
                * Constructor.
                *
                * @param [in] unit  reference to the unit containing this service.
                */
               Client(): IClient()
               {}

               virtual ~Client()
               {}
            };

            /*!
             * Scheduling %Service : %Server side implementation.
             *
             * This class provides the server side of the Scheduling interface.
             */
            struct IServer: public IServerBase
            {
               uint16_t uid() const
               {
                  return IServerBase::uid();
               }

               HF::Attributes::UIDS attributes(uint8_t uid = HF::Attributes::Pack::MANDATORY) const
               {
                  return Scheduling::IServer::attributes(uid);
               }

               uint8_t number_of_entries() const;

               /*!
                * Callback that is called when a @c Scheduling::DEFINE_EVENT_CMD,
                * is received.
                *
                * @param [in] addr       the network address to send the message to.
                */
               virtual Common::Result define_event(const Protocol::Packet &packet,
                                                   Scheduling::DefineEvent<Day> &msg);

               /*!
                * Callback that is called when a @c Scheduling::UPDATE_EVENT_STATUS_CMD,
                * is received.
                *
                * @param [in] addr       the network address to send the message to.
                */
               virtual Common::Result update_event_status(const Protocol::Packet &packet,
                                                          UpdateStatus &msg);

               /*!
                * Callback that is called when a @c Scheduling::GET_EVENT_ENTRY_CMD,
                * is received.
                *
                * @param [in] addr       the network address to send the message to.
                */
               virtual Common::Result get_event_entry(const Protocol::Packet &packet,
                                                      GetEntry &msg);

               /*!
                * Callback that is called when a @c Scheduling::DELETE_EVENT_CMD,
                * is received.
                *
                * @param [in] addr       the network address to send the message to.
                */
               virtual Common::Result delete_event(const Protocol::Packet &packet,
                                                   DeleteEvent &msg);

               /*!
                * Callback that is called when a @c Scheduling::DELETE_ALL_EVENTS_CMD,
                * is received.
                *
                * @param [in] addr       the network address to send the message to.
                */
               virtual Common::Result delete_all_events(const Protocol::Packet &packet);

               void periodic(uint32_t time);

               //! Constructor
               IServer(Unit0 &unit): IServerBase(unit)
               {}

               //! Destructor
               virtual ~IServer() = default;

               // =============================================================================
               // Entries API
               // =============================================================================

               /*!
                * Get a reference to the current object implementing the persistence API,
                * for the device information.
                *
                * @return  reference to the current object for the persistence API.
                */
               virtual IEntries<Day> &entries() const = 0;

               /*!
                * Get the Weekly Scheduling entry given by @c id.
                *
                * @param [in] id  event id of the event to retrieve.
                *
                * @return  a pointer to the event entry if it exists,
                *          @c nullptr otherwise.
                */
               Common::Pointer<Entry> entry(const uint8_t id) const
               {
                  return entries().find(id);
               }

               /*!
                * @copydoc IEntries::next_id
                */
               uint8_t next_id() const
               {
                  return entries().next_id();
               }

               using IServerBase::notify;

               // =============================================================================
               // Protected types and methods
               // =============================================================================

               protected:

               Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                             uint16_t offset);

            };

            /*!
             * Scheduling %Service : %Server side implementation.
             *
             * This class provides the server side of the Scheduling interface.
             */
            template<typename _Entries>
            struct Server: public IServer
            {
               static_assert(std::is_base_of<HF::Core::Scheduling::IEntries<Day>, _Entries>::value,
                             "_Entries must be of type HF::Core::Scheduling::IEntries<Day>");

               protected:

               _Entries _entries;

               public:

               /*!
                * Constructor.
                *
                * @param [in] unit  reference to the unit containing this service.
                */
               Server(Unit0 &unit): IServer(unit)
               {}

               virtual ~Server()
               {}

               //! @copydoc IServer::entries
               _Entries &entries() const
               {
                  return const_cast<_Entries &>(_entries);
               }
            };

            typedef Server<Entries<Day>> DefaultServer;

         } // namespace Weekly
      }    // namespace Scheduling
   }       // namespace Core
}          // namespace HF

#endif /* HF_CORE_WEEKLY_SCHEDULING_H */
