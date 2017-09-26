// =============================================================================
/*!
 * @file       inc/hanfun/core/scheduling.h
 *
 * This file contains the implementation of ...
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
#ifndef HF_CORE_SCHEDULING_H_
#define HF_CORE_SCHEDULING_H_

#include "hanfun/protocol.h"
#include "hanfun/core.h"


/*!
 * @ingroup Scheduling
 *
 * Helper macro to implement attribute setters for the Scheduling case.
 *
 * @param [in] _Type    helper class that wraps the attribute.
 * @param [in] _name    name of the attribute to generate the setter for.
 * @param [in] _value   name of the variable containing the new value.
 */
#define HF_SCHEDULING_SETTER_HELPER(_Type, _name, _value)                                 \
   {                                                                                      \
      _Type::value_type old = this->_name;                                                \
                                                                                          \
      this->_name = _value;                                                               \
                                                                                          \
      _Type old_attr(static_cast<HF::Interface::UID>(this->uid()),old, this);             \
      _Type new_attr(static_cast<HF::Interface::UID>(this->uid()),this->_name, this);     \
                                                                                          \
      notify(old_attr, new_attr);                                                         \
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
       * @ingroup scheduling_itf
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
      HF::Attributes::IAttribute *create_attribute(Scheduling::IServer *server,
                                                   Interface::UID itf_uid, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Scheduling service.
       */
      namespace Scheduling
      {
         /*!
          * @addtogroup event_scheduling_itf  Event Scheduling service
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Event Scheduling service API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            ACTIVATE_SCHEDULER_CMD = 0x01,   //!< Activate Scheduler command UID.
            DEFINE_CMD             = 0x02,   //!< Define Event command UID.
            UPDATE_STATUS_CMD      = 0x03,   //!< Update Event Status command UID.
            GET_ENTRY_CMD          = 0x04,   //!< Get Event Entry command UID.
            DELETE_CMD             = 0x05,   //!< Delete Event command UID.
            DELETE_ALL_CMD         = 0x06,   //!< Delete All Events command UID.
            __LAST_CMD__           = DELETE_ALL_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            MAXIMUM_NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Maximum Number Of Entries attribute UID.
            NUMBER_OF_ENTRIES_ATTR         = 0x02, //!< Number Of Entries attribute UID.
            STATUS_ATTR                    = 0x03, //!< Status attribute UID.
            __LAST_ATTR__                  = STATUS_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Maximum Number Of Entries attribute for the Event Scheduling service.
          */
         // template <Interface::UID _Itf_Type>
         struct MaximumNumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_NUMBER_OF_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                          //!< Attribute Read/Write

            MaximumNumberOfEntries(Interface::UID itf, uint8_t value = 0,
                                   HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Number Of Entries attribute for the Event Scheduling service.
          */
         struct NumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            NumberOfEntries(Interface::UID itf, uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Status attribute for the Event Scheduling service.
          */
         struct Status: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = STATUS_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;       //!< Attribute Read/Write

            Status(Interface::UID itf, uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(itf, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::EventScheduling::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::EventScheduling::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(HF::Interface::UID itf_uid, uint8_t uid);

         template<typename _Type>
         struct Entry
         {

            uint8_t id;          //!< Event ID. (Unique per device)
            uint8_t status;      //!< Event Status.
            _Type   time;        //!< Scheduler configuration.
            uint8_t pid;         //!< Program ID to be invoked.

            Entry(uint8_t _event_id, uint8_t _status, _Type _t, uint8_t _pid):
               id(_event_id), status(_status), time(_t), pid(_pid)
            {}

            Entry() = default;

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Event ID
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

         //! Message payload for a @c HF::Scheduling::ACTIVATE_SCHEDULER_CMD request.
         struct ActivateScheduler
         {
            uint8_t status;   //!< Scheduler status


            ActivateScheduler(uint8_t _status = 0x00): status(_status)
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

         //! Response message payload for a @c HF::Scheduling::ACTIVATE_SCHEDULER_CMD request.
         typedef Protocol::Response ActivateSchedulerResponse;

         //! Message payload for a @c HF::Scheduling::UPDATE_STATUS_CMD request.
         struct UpdateStatus
         {
            uint8_t event_id; //!< Event ID.
            uint8_t status;   //!< Scheduler status

            UpdateStatus(uint8_t _event_id = 0x00, uint8_t _status = 0x00):
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

         //! Response message payload for a @c HF::Scheduling::UPDATE_STATUS_CMD request.
         struct UpdateStatusResponse: public HF::Protocol::Response
         {
            uint8_t event_id; //!< Event ID.

            UpdateStatusResponse(Common::Result _code = Common::Result::OK,
                                 uint8_t _event_id = 0x00):
               HF::Protocol::Response(_code), event_id(_event_id)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);        // Response Code

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

         //! Message payload for a @c HF::Scheduling::GET_ENTRY_CMD request.
         struct GetEntry
         {
            uint8_t event_id; //!< Event ID.

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

         //! Response message payload for a @c HF::Scheduling::GET_ENTRY_CMD request.
         template<typename _Type>
         struct GetEntryResponse: public HF::Protocol::Response
         {
            Entry<_Type> entry;   //!< Entry

            GetEntryResponse(Common::Result _code,
                             Entry<_Type> _entry):
               HF::Protocol::Response(_code), entry(_entry)
            {}

            GetEntryResponse(Common::Result _code = Common::Result::FAIL_UNKNOWN):
               HF::Protocol::Response(_code)
            {}


            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);     // Respose code

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

         //! Message payload for a @c HF::Scheduling::DELETE_CMD request.
         typedef GetEntry DeleteEvent;

         //! Response message payload for a @c HF::Scheduling::DELETE_CMD request.
         typedef UpdateStatusResponse DeleteEventResponse;

         //! Response Message payload for a @c HF::Scheduling::DELETE_ALL_CMD request.
         typedef HF::Protocol::Response DeleteAllResponse;


         /*!
          * Scheduling %Service : Parent.
          *
          * This is the parent class for the Scheduling service implementation.
          */
         template<Interface::UID _Itf_Type>
         struct Base: public Service<_Itf_Type>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit):
               Service<_Itf_Type>(unit)
            {}
         };

         /*!
          * Scheduling %Service : %Server side implementation.
          *
          * This class provides the server side of the Scheduling interface.
          */
         class IServer: virtual public HF::Interface
         {
            protected:

            uint8_t _maximum_number_of_entries; //!< Maximum Number Of Entries
            uint8_t _number_of_entries;         //!< Number Of Entries
            uint8_t _status;                    //!< Status

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
             * @param [in] addr       the network address to send the message to.
             */
            virtual Common::Result activate_scheduler(const Protocol::Packet &packet,
                                                      ActivateScheduler &msg);

            /*!
             * Callback that is called when a @c Scheduling::DEFINE_EVENT_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void define_event(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c Scheduling::UPDATE_EVENT_STATUS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void update_event_status(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c Scheduling::GET_EVENT_ENTRY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void get_event_entry(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c Scheduling::DELETE_EVENT_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_event(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c Scheduling::DELETE_ALL_EVENTS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_all_events(const Protocol::Address &addr);

            //! @}
            // ======================================================================

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Maximum Number Of Entries for the Scheduling server.
             *
             * @return  the current Maximum Number Of Entries.
             */
            uint8_t maximum_number_of_entries() const;

            /*!
             * Set the Maximum Number Of Entries for the Scheduling server.
             *
             * @param [in] __value the  Maximum Number Of Entries value to set the server to.
             */
            void maximum_number_of_entries(uint8_t __value);

            /*!
             * Get the Number Of Entries for the Scheduling server.
             *
             * @return  the current Number Of Entries.
             */
            uint8_t number_of_entries() const;

            /*!
             * Set the Number Of Entries for the Scheduling server.
             *
             * @param [in] __value the  Number Of Entries value to set the server to.
             */
            void number_of_entries(uint8_t __value);

            /*!
             * Get the Status for the Scheduling server.
             *
             * @return  the current Status.
             */
            uint8_t status() const;

            /*!
             * Set the Status for the Scheduling server.
             *
             * @param [in] __value the  Status value to set the server to.
             */
            void status(uint8_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);

            virtual void notify(const HF::Attributes::IAttribute &old_value,
                                const HF::Attributes::IAttribute &new_value) const = 0;

            virtual void send(const Protocol::Address &addr, Protocol::Message &message,
                              Transport::Link *link) = 0;
         };

         /*!
          * Scheduling %Service : %Client side implementation.
          *
          * This class provides the client side of the Scheduling interface.
          */
         struct IClient
         {
            virtual ~IClient()
            {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::ACTIVATE_SCHEDULER_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void activate_scheduler(Interface::UID itf_uid, const Protocol::Address &addr,
                                            uint8_t _status);

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::ACTIVATE_SCHEDULER_CMD,
             * to the D:0/U:0 network address.
             */
            void activate_scheduler(Interface::UID itf_uid, uint8_t _status)
            {
               Protocol::Address addr(0, 0);
               activate_scheduler(itf_uid, addr, _status);
            }

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DEFINE_EVENT_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void define_event(Interface::UID itf_uid, const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DEFINE_EVENT_CMD,
             * to the D:0/U:0 network address.
             */
            void define_event(Interface::UID itf_uid)
            {
               Protocol::Address addr(0, 0);
               define_event(itf_uid, addr);
            }

#ifdef HF_CORE_EVENT_SCHEDULING_UPDATE_EVENT_STATUS_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::UPDATE_EVENT_STATUS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void update_event_status(Interface::UID itf_uid, const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::UPDATE_EVENT_STATUS_CMD,
             * to the D:0/U:0 network address.
             */
            void update_event_status(Interface::UID itf_uid)
            {
               Protocol::Address addr(0, 0);
               update_event_status(itf_uid, addr);
            }
#endif

#ifdef HF_CORE_EVENT_SCHEDULING_GET_EVENT_ENTRY_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::GET_EVENT_ENTRY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void get_event_entry(Interface::UID itf_uid, const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::GET_EVENT_ENTRY_CMD,
             * to the D:0/U:0 network address.
             */
            void get_event_entry(Interface::UID itf_uid)
            {
               Protocol::Address addr(0, 0);
               get_event_entry(itf_uid, addr);
            }
#endif

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DELETE_EVENT_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_event(Interface::UID itf_uid, const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DELETE_EVENT_CMD,
             * to the D:0/U:0 network address.
             */
            void delete_event(Interface::UID itf_uid)
            {
               Protocol::Address addr(0, 0);
               delete_event(itf_uid, addr);
            }

#ifdef HF_CORE_EVENT_SCHEDULING_DELETE_ALL_EVENTS_CMD
            /*!
             * Send a HAN-FUN message containing a @c Scheduling::DELETE_ALL_EVENTS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_all_events(Interface::UID itf_uid, const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c EventScheduling::DELETE_ALL_EVENTS_CMD,
             * to the D:0/U:0 network address.
             */
            void delete_all_events(Interface::UID itf_uid)
            {
               Protocol::Address addr(0, 0);
               delete_all_events(itf_uid, addr);
            }
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
 * @addtogroup scheduling_itf
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
