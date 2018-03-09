// =============================================================================
/*!
 * @file       inc/hanfun/core/weekly_scheduling.h
 *
 * This file contains the definitions for the %Weekly %Scheduling service.
 *
 * @version    1.5.2
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
       * @ingroup weekly_scheduling
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] uid      attribute's %UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute %UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(Scheduling::Weekly::IServer *server,
                                                   uint8_t uid);

      namespace Scheduling
      {
         //! This namespace contains the implementation of the %Weekly %Scheduling service.
         namespace Weekly
         {
            /*!
             * @addtogroup weekly_scheduling  Weekly Scheduling
             * @ingroup core
             *
             * This module contains the classes that implement the %Weekly %Scheduling service.
             *
             * @{
             */

            //! %Day of the Week enumeration.
            typedef enum _Days
            {
               MONDAY = 0x00,
               TUESDAY,
               WEDNESDAY,
               THURSDAY,
               FRIDAY,
               SATURDAY,
               SUNDAY
            } Days;

            //! Specific part for the %Weekly Scheduler of the @c HF::Scheduling::Entry.
            struct Day
            {
               uint8_t day;            //!< %Day of Week.
               uint8_t hour;           //!< Hour of %Day.
               uint8_t minute;         //!< Minute of Hour.

               Day(uint8_t _day, uint8_t _hour, uint8_t _minute):
                  day(_day), hour(_hour), minute(_minute)
               {}

               Day() = default;

               static constexpr int      MINUTE = 60;
               static constexpr int      HOUR   = 60 * MINUTE;
               static constexpr int      DAY    = 24 * HOUR;
               static constexpr uint32_t WEEK   = 7 * DAY;

               // Jan, 1st 2000 was on a Saturday (UTC)
               static constexpr uint8_t DAY_OF_WEEK_BASE = SATURDAY;

               static Day               convert(const uint32_t &timestamp)
               {
                  Day result;

                  std::div_t temp = std::div(timestamp, DAY);           // get the number of days
                                                                        // since the start

                  result.day = temp.quot % 7                            // n of days MOD 7
                               + DAY_OF_WEEK_BASE;

                  result.day    = result.day % 7;

                  temp          = std::div(temp.rem, HOUR);
                  result.hour   = temp.quot;
                  temp          = std::div(temp.rem, MINUTE);
                  result.minute = temp.quot;

                  return result;
               }

               /*!
                * Get the initial value for the next_run entry attribute.
                *
                * @param [in] timestamp   the current system time.
                *
                * @return  the timestamp for the first run.
                */
               uint32_t first(uint32_t timestamp = 0) const
               {
                  uint32_t result = timestamp;

                  Day current(convert(timestamp));

                  result += (this->hour - current.hour) * 60 * 60;
                  result += (this->minute - current.minute) * 60;

                  result += (this->day - current.day) * 24 * 60 * 60;

                  if (timestamp > result)
                  {
                     result += WEEK;
                  }

                  return result;
               }

               /*!
                * Get the step between executions.
                *
                * @return  the step time.
                */
               uint32_t step() const
               {
                  return WEEK;
               }

               /*!
                *
                * @param _time
                * @return
                */
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

            //! Specific part for the %Weekly Scheduler of the @c HF::Scheduling::Entry.
            typedef Scheduling::Entry<Day> Entry;

            //! Specific part for the %Weekly Scheduler of the @c HF::Scheduling::GetEntryResonse.
            typedef Scheduling::GetEntryResponse<Day> GetEntryResponse;

            /*!
             * Helper class to handle the Maximum Number Of %Entries attribute
             * for the %Weekly %Scheduling service.
             */
            struct MaximumNumberOfEntries: public Scheduling::MaximumNumberOfEntries
            {
               MaximumNumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
                  Scheduling::MaximumNumberOfEntries(HF::Interface::WEEKLY_SCHEDULING,
                                                     value, owner)
               {}
            };

            /*!
             * Helper class to handle the Number Of %Entries attribute
             * for the %Weekly %Scheduling service.
             */
            struct NumberOfEntries: public Scheduling::NumberOfEntries
            {
               NumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
                  Scheduling::NumberOfEntries(HF::Interface::WEEKLY_SCHEDULING,
                                              value, owner)
               {}
            };

            /*!
             * Helper class to handle the %Status attribute
             * for the %Weekly %Scheduling service.
             */
            struct Status: public Scheduling::Status
            {
               Status(bool value = false, HF::Interface *owner = nullptr):
                  Scheduling::Status(HF::Interface::WEEKLY_SCHEDULING,
                                     value, owner)
               {}
            };

            /*!
             * @copybrief HF::Core::create_attribute (HF::Core::Scheduling::Weekly::IServer *,uint8_t)
             *
             * @see HF::Core::create_attribute (HF::Core::Scheduling::Weekly::IServer *,uint8_t)
             *
             * @param [in] uid   attribute %UID to create the attribute object for.
             *
             * @return  pointer to an attribute object or @c nullptr if the attribute %UID does not
             *          exist.
             */
            HF::Attributes::IAttribute *create_attribute(uint8_t uid);

            typedef Scheduling::Base<HF::Interface::WEEKLY_SCHEDULING,
                                     Scheduling::IServer> IServerBase;

            typedef Interfaces::Interface<HF::Interface::WEEKLY_SCHEDULING,
                                          Scheduling::IClient> IClientBase;

            /*!
             * %Weekly %Scheduling %Service : %Client side implementation.
             *
             * This class provides the client side of the %Weekly %Scheduling interface.
             */
            struct IClient: public IClientBase
            {
               //! Constructor
               IClient(): IClientBase()
               {}

               //! Destructor
               virtual ~IClient() {}

               static constexpr HF::Interface::UID ITF = HF::Interface::WEEKLY_SCHEDULING;

               /*!
                * @copybrief HF::Core::Scheduling::IClient::activate_scheduler
                *
                * @param [in] addr     the network address to send the message to.
                * @param [in] enabled  enable/disable scheduler.
                */
               virtual void activate_scheduler(const Protocol::Address &addr, bool enabled)
               {
                  Scheduling::IClient::activate_scheduler(addr, ITF, enabled);
               }

               /*!
                * Send a HAN-FUN message containing a @c Scheduling::DEFINE_CMD,
                * to the given network address.
                *
                * @param [in] addr     the network address to send the message to.
                * @param [in] id       the ID for the event entry.
                * @param [in] status   the event entry status.
                * @param [in] time     the time to set the event entry to.
                * @param [in] pid      the batch program to call on event.
                */
               virtual void define_event(const Protocol::Address &addr, uint8_t id, uint8_t status,
                                         Day &time, uint8_t pid);

#ifdef HF_CORE_WEEKLY_SCHEDULING_UPDATE_EVENT_STATUS_CMD
               /*!
                * @copybrief HF::Core::Scheduling::IClient::update_event_status
                *
                * @param [in] addr     the network address to send the message to.
                * @param [in] id       entry ID to delete.
                * @param [in] enabled  enable/disable entry.
                */
               virtual void update_event_status(const Protocol::Address &addr,
                                                uint8_t id, bool enabled)
               {
                  Scheduling::IClient::update_event_status(addr, ITF, id, enabled);
               }
#endif

#ifdef HF_CORE_WEEKLY_SCHEDULING_GET_EVENT_ENTRY_CMD
               /*!
                * @copybrief HF::Core::Scheduling::IClient::get_event_entry
                *
                * @param [in] addr  the network address to send the message to.
                * @param [in] id    entry ID to delete.
                */
               virtual void get_event_entry(const Protocol::Address &addr, uint8_t id)
               {
                  Scheduling::IClient::get_event_entry(addr, ITF, id);
               }
#endif
               /*!
                * @copybrief HF::Core::Scheduling::IClient::delete_event
                *
                * @param [in] addr  the network address to send the message to.
                * @param [in] id    entry ID to delete.
                */
               virtual void delete_event(const Protocol::Address &addr, uint8_t id)
               {
                  Scheduling::IClient::delete_event(addr, ITF, id);
               }

#ifdef HF_CORE_WEEKLY_SCHEDULING_DELETE_ALL_EVENTS_CMD
               /*!
                * @copybrief HF::Core::Scheduling::IClient::delete_all_events
                *
                * @param [in] addr  the network address to send the message to.
                */
               virtual void delete_all_events(const Protocol::Address &addr)
               {
                  Scheduling::IClient::delete_all_events(addr, ITF);
               }
#endif

               using IClientBase::send;
            };

            /*!
             * %Scheduling %Service : %Client side implementation.
             *
             * This class provides the client side of the %Scheduling interface.
             */
            struct Client: public IClient
            {
               /*!
                * Constructor.
                */
               Client(): IClient()
               {}

               virtual ~Client()
               {}
            };

            /*!
             * %Scheduling %Service : %Server side implementation.
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
                * @param [in] packet   the packet received.
                * @param [in] msg      the define event message received.
                *
                * @retval  Common::Result::OK               if the entry was created;
                * @retval  Common::Result::FAIL_ARG         if the entry ID already exists;
                * @retval  Common::Result::FAIL_RESOURCES   if the entry could not be created;
                * @retval  Common::Result::FAIL_UNKNOWN     otherwise.
                */
               virtual Common::Result define_event(const Protocol::Packet &packet,
                                                   Scheduling::Entry<Day> &msg);

               /*!
                * Callback that is called when a @c Scheduling::UPDATE_STATUS_CMD,
                * is received.
                *
                * @param [in] packet   the packet received.
                * @param [in] msg      the update event status message received.
                *
                * @retval  Common::Result::OK         if the entry was updated;
                * @retval  Common::Result::FAIL_ARG   if the entry does not exists;
                */
               virtual Common::Result update_event_status(const Protocol::Packet &packet,
                                                          const UpdateStatus &msg);

               /*!
                * Callback that is called when a @c Scheduling::GET_ENTRY_CMD,
                * is received.
                *
                * @param [in] packet   the packet received.
                * @param [in] msg      the get event message received.
                *
                * @retval  Common::Result::OK         if the entry exists,
                * @retval  Common::Result::FAIL_ARG   otherwise.
                */
               virtual Common::Result get_event_entry(const Protocol::Packet &packet,
                                                      const GetEntry &msg);

               /*!
                * Callback that is called when a @c Scheduling::DELETE_CMD,
                * is received.
                *
                * @param [in] packet   the packet received.
                * @param [in] msg      the delete event message received.
                *
                * @retval  Common::Result::OK         if the entry was deleted,
                * @retval  Common::Result::FAIL_ARG   otherwise.
                */
               virtual Common::Result delete_event(const Protocol::Packet &packet,
                                                   const DeleteEvent &msg);

               /*!
                * Callback that is called when a @c Scheduling::DELETE_ALL_CMD,
                * is received.
                *
                * @param [in] packet   the packet received.
                *
                * @retval Common::Result::OK
                */
               virtual Common::Result delete_all_events(const Protocol::Packet &packet);

               void periodic(uint32_t time);

               //! Constructor
               IServer(Unit0 &unit): IServerBase(unit)
               {}

               //! Destructor
               virtual ~IServer() {}

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
                * Get the %Weekly %Scheduling entry given by @c id.
                *
                * @param [in] id  event id of the event to retrieve.
                *
                * @retval pointer   to the event entry if it exists,
                * @retval nullptr   otherwise.
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
             * %Scheduling %Service : %Server side implementation.
             *
             * This class provides the server side of the %Scheduling interface.
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

            /*!
             * %Weekly %Scheduling %Service : %Server side with
             * default persistence implementation.
             */
            typedef Server<Entries<Day>> DefaultServer;

            /*! @} */

         } // namespace Weekly
      }    // namespace Scheduling
   }       // namespace Core
}          // namespace HF

#endif /* HF_CORE_WEEKLY_SCHEDULING_H */
