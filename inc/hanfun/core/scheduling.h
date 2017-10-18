// =============================================================================
/*!
 * @file       inc/hanfun/core/scheduling.h
 *
 * This file contains the common definitions for the scheduling services.
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

#ifndef HF_CORE_SCHEDULING_H
#define HF_CORE_SCHEDULING_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

#include <string>
#include <map>
#include <forward_list>
#include <algorithm>

/*!
 * @ingroup Scheduling
 *
 * Helper macro to implement attribute setters for the Scheduling case.
 *
 * @param [in] _Type    helper class that wraps the attribute.
 * @param [in] _name    name of the attribute to generate the setter for.
 * @param [in] _value   name of the variable containing the new value.
 */
#define HF_SCHEDULING_SETTER_HELPER(_Type, _name, _value)                              \
   {                                                                                   \
      _Type::value_type old = this->_name;                                             \
                                                                                       \
      this->_name = _value;                                                            \
                                                                                       \
      _Type old_attr(static_cast<HF::Interface::UID>(this->uid()), old, this);         \
      _Type new_attr(static_cast<HF::Interface::UID>(this->uid()), this->_name, this); \
                                                                                       \
      notify(old_attr, new_attr);                                                      \
   }

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace Scheduling
      {
         class IServer;
      }

      /*!
       * @ingroup scheduling_common
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] itf_uid  interface's %UID to create the attribute object for.
       * @param [in] uid      attribute's %UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute %UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(Scheduling::IServer *server,
                                                   Interface::UID itf_uid, uint8_t uid);

      /*!
       * This namespace contains the implementation of the %Scheduling service.
       */
      namespace Scheduling
      {
         /*!
          * @addtogroup scheduling_common Scheduling Services (Common)
          * @ingroup core
          *
          * This module contains the common classes for the scheduling services API.
          *
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            ACTIVATE_SCHEDULER_CMD = 0x01,   //!< Activate Scheduler command %UID.
            DEFINE_EVENT_CMD       = 0x02,   //!< Define %Event command %UID.
            UPDATE_STATUS_CMD      = 0x03,   //!< Update %Event %Status command %UID.
            GET_ENTRY_CMD          = 0x04,   //!< Get %Event %Entry command %UID.
            DELETE_CMD             = 0x05,   //!< Delete %Event command %UID.
            DELETE_ALL_CMD         = 0x06,   //!< Delete All Events command %UID.
            __LAST_CMD__           = DELETE_ALL_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            MAXIMUM_NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Maximum Number Of %Entries attribute %UID.
            NUMBER_OF_ENTRIES_ATTR         = 0x02, //!< Number Of %Entries attribute %UID.
            STATUS_ATTR                    = 0x03, //!< %Status attribute %UID.
            __LAST_ATTR__                  = STATUS_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Maximum Number Of %Entries attribute for the
          * scheduling services.
          */
         struct MaximumNumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_NUMBER_OF_ENTRIES_ATTR; //!< Attribute %UID.
            static constexpr bool    WRITABLE = false;                          //!< Attribute Read/Write

            MaximumNumberOfEntries(Interface::UID itf, uint8_t value = 0,
                                   HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Number Of %Entries attribute for the scheduling services.
          */
         struct NumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_ENTRIES_ATTR; //!< Attribute %UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            NumberOfEntries(Interface::UID itf, uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the %Status attribute for the scheduling services.
          */
         struct Status: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = STATUS_ATTR; //!< Attribute %UID.
            static constexpr bool    WRITABLE = false;       //!< Attribute Read/Write

            Status(Interface::UID itf, uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Core::Scheduling::IServer *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::Scheduling::IServer *,uint8_t)
          *
          * @param [in] itf_uid  interface %UID to create the attribute object for.
          * @param [in] uid      attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute %UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(HF::Interface::UID itf_uid, uint8_t uid);

         /*!
          * %Base class for scheduling services entries.
          *
          * @tparam _Type the specific data for scheduling service entry.
          */
         template<typename _Type>
         struct Entry
         {
            uint8_t id;          //!< %Event ID. (Unique per device)
            uint8_t status;      //!< %Event %Status.
            _Type   time;        //!< Scheduler configuration.
            uint8_t pid;         //!< Program ID to be invoked.

            // Helper attribute - Not included on the message entry!
            uint32_t next_run;   //!< Next run timestamp.

            /*!
             * Constructor
             *
             * @param [in] _id      the entry ID.
             * @param [in] _status  the entry status.
             * @param [in] _t       the specific data for the scheduling service.
             * @param [in] _pid     the batch program ID to be run.
             */
            Entry(uint8_t _id, uint8_t _status, _Type _t, uint8_t _pid):
               id(_id), status(_status), time(_t), pid(_pid), next_run(time.first())
            {}

            Entry() = default;

            static constexpr uint16_t START_ID     = 0x00;  //!< Lower bound for the entries ID.
            static constexpr uint16_t MAX_ID       = 0xFE;  //!< Upper bound for the entries ID.
            static constexpr uint16_t AVAILABLE_ID = 0xFF;  //!< Special ID for system allocated ID.

            /*!
             * Check if the current entry is runnable at _time.
             *
             * @param [in] _time    the system time.
             *
             * @retval false     if the entry can't run.
             * @retval true      if the entry can run.
             */
            bool active(uint32_t _time) const
            {
               return (status == 0x01 &&
                       time.active(_time) &&
                       _time >= next_run);
            }

            /*!
             * Increment the next_run attribute.
             */
            void step(void)
            {
               next_run += time.step();
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)   // Event ID
                                               + sizeof(uint8_t)   // Event Status
                                               + _Type::min_size   // _Type min size
                                               + sizeof(uint8_t);  // Program ID

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint16_t start = offset;

               offset += array.write(offset, id);
               offset += array.write(offset, static_cast<uint8_t>((status << 7) & 0x80));
               offset += time.pack(array, offset);
               offset += array.write(offset, pid);

               return (offset - start);
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               uint16_t start = offset;
               uint16_t size;

               offset += array.read(offset, id);
               offset += array.read(offset, status);
               status  = (status >> 7) & 0x01;

               size    = time.unpack(array, offset);
               HF_ASSERT(size != 0, {return 0;});
               offset += size;

               offset += array.read(offset, pid);

               return (offset - start);
            }
         };

         //! Message payload for a @c Scheduling::ACTIVATE_SCHEDULER_CMD request.
         struct ActivateScheduler
         {
            bool status;   //!< Scheduler status

            ActivateScheduler(bool _status = false): status(_status)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);     // Status

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

         //! Response message payload for a @c Scheduling::ACTIVATE_SCHEDULER_CMD request.
         typedef Protocol::Response ActivateSchedulerResponse;

         /*!
          * Message payload for a @c Scheduling::DEFINE_EVENT_CMD request.
          *
          * @tparam _Type the specific data for scheduling service entry.
          */
         template<class _Type>
         using DefineEvent = Entry<_Type>;

         //! Response message payload for a @c Scheduling::DEFINE_EVENT_CMD request.
         struct DefineEventResponse: public HF::Protocol::Response
         {
            uint8_t event_id; //!< %Event ID.

            DefineEventResponse(Common::Result _code = Common::Result::OK,
                                uint8_t _event_id = 0x00):
               HF::Protocol::Response(_code), event_id(_event_id)
            {}

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               if (code != Common::Result::OK)
               {
                  return min_size;
               }

               return min_size + sizeof(uint8_t);
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         //! Message payload for a @c Scheduling::UPDATE_STATUS_CMD request.
         struct UpdateStatus
         {
            uint8_t event_id; //!< %Event ID.
            bool status;      //!< Scheduler status

            UpdateStatus(uint8_t _event_id = 0x00, bool _status = false):
               event_id(_event_id), status(_status)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Event ID
                                                 + sizeof(uint8_t);  // Status

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

         //! Response message payload for a @c Scheduling::UPDATE_STATUS_CMD request.
         typedef DefineEventResponse UpdateStatusResponse;

         //! Message payload for a @c Scheduling::GET_ENTRY_CMD request.
         struct GetEntry
         {
            uint8_t event_id; //!< %Event ID.

            GetEntry(uint8_t _event_id = 0x00):
               event_id(_event_id)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);     // Event ID

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

         /*!
          * Response message payload for a @c Scheduling::GET_ENTRY_CMD request.
          *
          * @tparam _Type the specific data for scheduling service entry.
          */
         template<typename _Type>
         struct GetEntryResponse: public HF::Protocol::Response
         {
            Entry<_Type> entry;   //!< &Entry data to return to client.

            GetEntryResponse(Common::Result _code,
                             Entry<_Type> _entry):
               HF::Protocol::Response(_code), entry(_entry)
            {}

            GetEntryResponse(Common::Result _code = Common::Result::FAIL_UNKNOWN):
               HF::Protocol::Response(_code)
            {}

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               if (code != Common::Result::OK)
               {
                  return min_size;
               }

               return min_size + Entry<_Type>::min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint8_t start = offset;

               offset += Protocol::Response::pack(array, offset);

               if (code != Common::Result::OK)
               {
                  goto _end;
               }

               offset += entry.pack(array, offset);

               _end:
               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               uint8_t start = offset;

               offset += Protocol::Response::unpack(array, offset);

               if (code != Common::Result::OK)
               {
                  goto _end;
               }

               HF_SERIALIZABLE_CHECK(array, offset, entry.size());
               offset += entry.unpack(array, offset);

               _end:
               return offset - start;
            }
         };

         //! Message payload for a @c Scheduling::DELETE_CMD request.
         typedef GetEntry DeleteEvent;

         //! Response message payload for a @c Scheduling::DELETE_CMD request.
         typedef DefineEventResponse DeleteEventResponse;

         //! Response Message payload for a @c Scheduling::DELETE_ALL_CMD request.
         typedef HF::Protocol::Response DeleteAllResponse;

         /*!
          * %Scheduling - Persistent Storage API.
          *
          * @tparam _Type the specific type of the scheduling entry.
          */
         template<typename _Type>
         struct IEntries: public Common::IEntries<Entry<_Type>>
         {
            typedef Entry<_Type> EntryType;                    //!< %Entry helper type.
            typedef Common::Pointer<EntryType> EntryPointer;   //!< %Entry pointer helper.

            /*!
             * Store the given @c entry to persistent storage.
             *
             * @param [in] entry    the entry itself for this event.
             *
             * @retval  Common::Result::OK if the entry was saved,
             * @retval  Common::Result::FAIL_UNKNOWN otherwise.
             */
            virtual Common::Result save(const EntryType &entry) = 0;

            /*!
             * Erase all the DB entries.
             */
            virtual void clear(void) = 0;

            /*!
             * Find the %Event with the given id.
             *
             * @param [in] id  %Event ID to search for.
             *
             * @returns  pointer to the entry with the given @c id,
             *           @c nullptr otherwise.
             */
            virtual EntryPointer find(uint8_t id) const = 0;

            /*!
             * Return next available id for event.
             *
             * @return  the id to use in the next event, or
             *          @c Scheduler::Entry::AVAILABLE_ID if no id is available.
             */
            virtual uint8_t next_id() const = 0;

            /*!
             * Call the given function with each entry in the DB as argument.
             *
             * @param [in] func function to apply to all entries in the store.
             */
            virtual void for_each(std::function<void(EntryType &e)> func) = 0;
         };

         /*!
          * Default implementation of the persistence API.
          *
          * @tparam _Type the specific type of the scheduling entry.
          */
         template<typename _Type>
         struct Entries: public IEntries<_Type>
         {
            typedef Entry<_Type> EntryType;
            typedef Common::Pointer<EntryType> EntryPointer;

            typedef typename std::map<uint8_t, EntryType> Container;
            typedef typename std::pair<uint8_t, EntryType> container_entry;
            typedef typename Container::iterator iterator;
            typedef typename Container::const_iterator const_iterator;
            typedef typename Container::value_type value_type;

            virtual ~Entries() = default;

            //! @copydoc HF::Common::IEntries::size
            uint16_t size() const
            {
               return db.size();
            }

            //! @copydoc HF::Common::IEntries::save
            Common::Result save(const EntryType &entry)
            {
               db.insert(db.end(), std::pair<uint8_t, EntryType>(entry.id, entry));

               return Common::Result::OK;
            }

            /*!
             * Store the given @c entry to persistent storage.
             *
             * @param [in] id       entry's ID.
             * @param [in] status   entry's status.
             * @param [in] time     entry's time.
             * @param [in] pid      entry's PID.
             *
             * @retval  Common::Result::OK if the entry was saved,
             * @retval  Common::Result::FAIL_UNKNOWN otherwise.
             */
            Common::Result save(uint8_t id, uint8_t status, _Type &time, uint8_t pid)
            {
               EntryType entry(id, status, time, pid);
               return save(entry);
            }

            /*!
             * Erase all the DB entries.
             */
            void clear(void)
            {
               db.clear();
            }

            /*!
             * @copybrief HF::Common::IEntries::destroy
             *
             * @param [in] id the @c %Event ID to destroy
             *
             * @retval  Common::Result::OK, if the entry was destroyed.
             * @retval  Common::Result::FAIL_ARG otherwise.
             */
            Common::Result destroy(const uint8_t &id)
            {
               auto count = db.erase(id);

               if (count != 1)
               {
                  return Common::Result::FAIL_ARG;
               }
               else
               {
                  return Common::Result::OK;
               }
            }

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @warning the reference passed into this method SHOULD NOT be considered
             *          valid if it was obtained by calling the find method.
             */
            Common::Result destroy(const EntryType &entry)
            {
               return destroy(entry.id);
            }

            //! @copydoc IEntries::find
            EntryPointer find(uint8_t id) const
            {
               auto it = db.find(id);

               if (it == db.end())
               {
                  return EntryPointer();
               }
               else
               {
                  return EntryPointer(const_cast<EntryType *>(&(*it).second));
               }
            }

            //! @copydoc IEntries::for_each
            void for_each(std::function<void(Entry<_Type>&e)> func)
            {
               for (iterator it = db.begin(); it != db.end(); ++it)
               {
                  func(const_cast<EntryType &>(it->second));
               }
            }

            //! @copydoc IEntries::next_id
            uint8_t next_id() const
            {
               uint8_t address = 0;

               if (db.size() > EntryType::MAX_ID)
               {
                  return EntryType::AVAILABLE_ID;
               }

               for (address = EntryType::START_ID; address <= EntryType::MAX_ID; ++address)
               {
                  if (db.find(address) == db.end())
                  {
                     return address;
                  }
               }

               return EntryType::AVAILABLE_ID;
            }

            /*!
             * Get an iterator to the start of the entries in this container.
             *
             * @return  iterator to the start of the entries present in this container.
             */
            iterator begin()
            {
               return db.begin();
            }

            /*!
             * Get an iterator to the end of the entries in this container.
             *
             * @return  iterator to the end of the entries present in this container.
             */
            iterator end()
            {
               return db.end();
            }

            /*!
             * Get a constant iterator to the start of the entries in this container.
             *
             * @return  constant iterator to the start of the entries present in this container.
             */
            const_iterator begin() const
            {
               return db.cbegin();
            }

            /*!
             * Get a constant iterator to the start of the entries in this container.
             *
             * @return  constant iterator to the start of the entries present in this container.
             */
            const_iterator end() const
            {
               return db.cend();
            }

            protected:

            //! Actual container for the entries.
            Container db;
         };


         /*!
          * %Scheduling %Service : Parent.
          *
          * This is the parent class for the %Scheduling service implementation.
          *
          * @tparam _ITF      interface %UID for the service.
          * @tparam _Parent   parent class for the service.
          */
         template<Interface::UID _ITF, typename _Parent>
         struct Base: public Service<_ITF, _Parent>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit):
               Service<_ITF, _Parent>(unit)
            {}
         };

         /*!
          * %Scheduling %Service : %Server side implementation.
          *
          * This class provides the server side of the %Scheduling interface.
          */
         class IServer: public ServiceRole<AbstractService, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint8_t _maximum_number_of_entries; //!< Maximum Number Of %Entries
            uint8_t _status;                    //!< %Status

            //! Constructor
            IServer(Unit0 &unit):
               ServiceRole<AbstractService, HF::Interface::SERVER_ROLE>(unit),
               _maximum_number_of_entries(std::numeric_limits<uint8_t>::max()),
               _status(0)
            {}

            public:

            //! Destructor
            virtual ~IServer()
            {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c Scheduling::ACTIVATE_SCHEDULER_CMD,
             * is received.
             *
             * @param [in] packet   the packet received.
             * @param [in] msg      the activate scheduler message received.
             *
             * @retval  Common::Result::OK, if the scheduler was enabled/disabled.
             * @retval  Common::Result::FAIL_ARG otherwise.
             */
            virtual Common::Result activate_scheduler(const Protocol::Packet &packet,
                                                      const ActivateScheduler &msg);

            //! @}
            // ======================================================================

            // ======================================================================
            // Get/Set API.
            // ======================================================================

            /*!
             * Get the Maximum Number Of %Entries for the %Scheduling server.
             *
             * @return  the current Maximum Number Of %Entries.
             */
            uint8_t maximum_number_of_entries() const;

            /*!
             * Set the Maximum Number Of %Entries for the %Scheduling server.
             *
             * @param [in] __value the  Maximum Number Of %Entries value to set the server to.
             */
            void maximum_number_of_entries(uint8_t __value);

            /*!
             * Get the Number Of %Entries for the %Scheduling server.
             *
             * @return  the current Number Of %Entries.
             */
            virtual uint8_t number_of_entries() const = 0;

            /*!
             * Get the %Status for the %Scheduling server.
             *
             * @return  the current Status.
             */
            uint8_t status() const;

            /*!
             * Set the %Status for the %Scheduling server.
             *
             * @param [in] __value the  %Status value to set the server to.
             */
            void status(uint8_t __value);

            // ======================================================================
            // Attribute API.
            // ======================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                            HF::Attributes::Pack::MANDATORY) const;

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*!
          * %Scheduling %Service : %Client side implementation.
          *
          * This class provides the client side of the %Scheduling interface.
          */
         class IClient: public Interfaces::InterfaceRole<Interfaces::AbstractInterface,
                                                         HF::Interface::CLIENT_ROLE>
         {
            public:

            virtual ~IClient()
            {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::ACTIVATE_SCHEDULER_CMD,
             * to the given network address.
             *
             * @param [in] addr     the network address to send the message to.
             * @param [in] itf_uid  interface UID to use in the message.
             * @param [in] enabled  enable/disable scheduler.
             */
            virtual void activate_scheduler(const Protocol::Address &addr,
                                            const Interface::UID itf_uid, bool enabled = true);

#ifdef HF_CORE_EVENT_SCHEDULING_UPDATE_EVENT_STATUS_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::UPDATE_EVENT_STATUS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] itf_uid    interface UID to use in the message.
             * @param [in] id         entry ID to delete.
             * @param [in] enabled    enable/disable entry.
             */
            virtual void update_event_status(const Protocol::Address &addr,
                                             const Interface::UID itf_uid,
                                             uint8_t id, bool enabled);
#endif

#ifdef HF_CORE_EVENT_SCHEDULING_GET_EVENT_ENTRY_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::GET_EVENT_ENTRY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] itf_uid    interface UID to use in the message.
             * @param [in] id         entry ID to delete.
             */
            virtual void get_event_entry(const Protocol::Address &addr,
                                         const Interface::UID itf_uid, uint8_t id);
#endif

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DELETE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] itf_uid    interface UID to use in the message.
             * @param [in] id         entry ID to delete.
             */
            virtual void delete_event(const Protocol::Address &addr,
                                      const Interface::UID itf_uid, uint8_t id);

#ifdef HF_CORE_EVENT_SCHEDULING_DELETE_ALL_EVENTS_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DELETE_ALL_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] itf_uid    interface UID to use in the message.
             */
            virtual void delete_all_events(const Protocol::Address &addr,
                                           const Interface::UID itf_uid);
#endif

            virtual void send(const Protocol::Address &addr, Protocol::Message &message) = 0;
            //! @}
            // ======================================================================
         };

         /*! @} */

      }  // namespace Scheduling

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup scheduling_common
 * @{
 */

// =============================================================================
// Stream Helpers
// =============================================================================
/*!
 * Convert the given @c command into a string and write it to the given @c stream.
 *
 * @param [in] stream   out stream to write the string to.
 * @param [in] command  role value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream, const HF::Core::Scheduling::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::Scheduling::Attributes attribute);
/*! @} */
#endif /* HF_CORE_SCHEDULING_H_ */
