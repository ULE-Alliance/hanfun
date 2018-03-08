// =============================================================================
/*!
 * @file       inc/hanfun/core/batch_program_management.h
 *
 * This file contains the definitions for the Batch Program Management interface.
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

#ifndef HF_CORE_BATCH_PROGRAM_MANAGEMENT_H
#define HF_CORE_BATCH_PROGRAM_MANAGEMENT_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

#include <string>
#include <map>
#include <forward_list>

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace BatchProgramManagement
      {
         class IServer;
      }

      /*!
       * @ingroup batch_program_management_itf
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
      HF::Attributes::IAttribute *create_attribute(BatchProgramManagement::IServer *server,
                                                   uint8_t uid);

      /*!
       * This namespace contains the implementation of the Batch Program Management interface.
       */
      namespace BatchProgramManagement
      {
         /*!
          * @addtogroup batch_program_management_itf  Batch Program Management
          * @ingroup core
          *
          * This module contains the classes that define and implement the Batch Program Management interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            DEFINE_PROGRAM_CMD      = 0x01, //!< Define Program command %UID.
            INVOKE_PROGRAM_CMD      = 0x02, //!< Invoke Program command %UID.
            DELETE_PROGRAM_CMD      = 0x03, //!< Delete Program command %UID.
            DELETE_ALL_PROGRAMS_CMD = 0x04, //!< Delete All Programs command %UID.
            GET_PROGRAM_ACTIONS_CMD = 0x05, //!< Get Program Actions command %UID.
            __LAST_CMD__            = GET_PROGRAM_ACTIONS_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            MAXIMUM_NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Maximum Number Of %Entries attribute %UID.
            NUMBER_OF_ENTRIES_ATTR         = 0x02, //!< Number Of %Entries attribute %UID.
            __LAST_ATTR__                  = NUMBER_OF_ENTRIES_ATTR
         } Attributes;

         // =============================================================================
         // Basic memory struct
         // =============================================================================

         struct Action: public Protocol::Message
         {
            /**
             * Constructor
             *
             * @param [in] _uid         Identifier of the Unit to which this action is to be sent.
             * @param [in] _msg_type    Code that specifies how Interface Member is to be interpreted.
             * @param [in] _itf_type    Type of the interface to which this action is to be sent.
             * @param [in] _itf_uid     ID of the interface to which this action is to be sent.
             * @param [in] _itf_member  ID of the interface member to be invoked or acted upon.
             * @param [in] _payload     Content of the attribute or of the payload of the command.
             */
            Action(uint8_t _uid, HF::Protocol::Message::Type _msg_type,
                   uint8_t _itf_type, uint16_t _itf_uid, uint8_t _itf_member,
                   Common::ByteArray &_payload)
            {
               reference  = _uid;
               type       = _msg_type;
               itf.role   = _itf_type;
               itf.id     = _itf_uid;
               itf.member = _itf_member;
               payload    = _payload;
            }

            /**
             * Default Constructor.
             *
             * Mainly for using with the unpack method.
             */
            Action(void) = default;

            // =================================================================
            // Serializable API
            // =================================================================

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;
            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

         };

         struct Entry
         {
            uint8_t             pid;      //!< Program ID, unique per device
            std::string         name;     //!< Program Name
            std::vector<Action> actions;  //!< Actions list

            /**
             * Constructor
             *
             * @param [in] _pid       Program ID, unique per device
             * @param [in] _name      Program Name
             */
            Entry(uint8_t _pid, std::string _name):
               pid(_pid), name(_name)
            {}

            /**
             * Constructor
             *
             * @param [in] _pid       Program ID, unique per device
             * @param [in] _name      Program Name
             * @param [in] _actions   Actions list
             */
            Entry(uint8_t _pid, std::string _name, std::vector<Action> &_actions):
               pid(_pid), name(_name), actions(_actions)
            {}

            /**
             * Default Constructor.
             *
             * Mainly for using with the unpack method.
             */
            Entry(void) = default;

            static constexpr uint16_t START_PID     = 0x00;
            static constexpr uint16_t MAX_PID       = 0xFE;
            static constexpr uint16_t AVAILABLE_PID = 0xFF;

            // =================================================================
            // Serializable API
            // =================================================================
            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Program ID
                                                 + sizeof(uint8_t)   // Name Length
                                                 + sizeof(uint8_t);  // Number of actions

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         typedef Common::Pointer<Entry> EntryPtr;

         // =============================================================================
         // Messages
         // =============================================================================

         typedef Entry DefineProgram;

         struct DefineProgramResponse: public Protocol::Response
         {
            uint8_t pid; //!< Program ID, unique per device

            /**
             * Constructor
             *
             * @param [in] _code    the response code for the response.
             * @param [in] _pid      The Program ID of the response.
             */
            DefineProgramResponse(Common::Result _code = Common::Result::OK,
                                  uint8_t _pid = 0):
               Protocol::Response(_code), pid(_pid)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = Protocol::Response::min_size;     // Response Code

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         struct InvokeProgram
         {
            uint8_t pid;    //!< Program ID, unique per device

            /**
             * Constructor
             *
             * @param [in] _pid   Program ID, unique per device
             */
            InvokeProgram(uint8_t _pid = 0):
               pid(_pid)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);     // Program ID

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         typedef DefineProgramResponse InvokeProgramResponse;

         typedef InvokeProgram DeleteProgram;
         typedef InvokeProgramResponse DeleteProgramResponse;

         typedef Protocol::Response DeleteAllProgramsResponse;

         typedef InvokeProgram GetProgramActions;

         struct GetProgramActionsResponse: public Protocol::Response
         {
            Entry program;    //!< The program entry data

            /**
             * Constructor
             *
             * @param [in] _code    The response code for the response.
             * @param [in] _program The program entry data.
             */
            GetProgramActionsResponse(Common::Result _code,
                                      Entry &_program):
               Protocol::Response(_code), program(_program)
            {}

            GetProgramActionsResponse(Common::Result _code = Common::Result::OK):
               Protocol::Response(_code)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Maximum Number Of %Entries attribute for the
          * Batch Program Management interface.
          */
         struct MaximumNumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_NUMBER_OF_ENTRIES_ATTR; //!< Attribute %UID.
            static constexpr bool    WRITABLE = false;                          //!< Attribute Read/Write

            MaximumNumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::BATCH_PROGRAM_MANAGEMENT, ID, owner,
                                  value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Number Of %Entries attribute for the Batch Program Management interface.
          */
         struct NumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_ENTRIES_ATTR; //!< Attribute %UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            NumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::BATCH_PROGRAM_MANAGEMENT, ID, owner,
                                  value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Core::BatchProgramManagement::IServer *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::BatchProgramManagement::IServer *,uint8_t)
          *
          * @param [in] uid   attribute %%UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  nullptr if the attribute %%UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Batch Program Management - Persistent Storage API.
          */
         struct IEntries: public Common::IEntries<Entry>
         {
            using Common::IEntries<Entry>::save;

            /*!
             * Store the given @c entry to persistent storage.
             *
             * @param [in] pid      Program ID for the new entry.
             * @param [in] name     name for the new batch program.
             * @param [in] actions  Action list for the batch program.
             *
             * @retval  Common::Result::OK if the entry was saved,
             * @retval  Common::Result::FAIL_UNKNOWN otherwise.
             */
            virtual Common::Result save(const uint8_t pid, const std::string &name,
                                        std::vector<Action> &actions) = 0;

            using Common::IEntries<Entry>::destroy;

            /*!
             * Remove the program with the given @c pid ftom
             *
             * @param [in] pid   the Program ID to destroy.
             *
             * @retval Result::OK            if the entry was destroyed,
             * @retval Result::FAIL_ARG      if no entry with @c pid exists,
             * @retval Result::FAIL_UNKNOWN  otherwise.
             */
            virtual Common::Result destroy(const uint8_t pid) = 0;

            /*!
             * Erase all the DB entries.
             */
            virtual void clear(void) = 0;

            /*!
             * Find the program with the given @c pid.
             *
             * @param [in] pid  the Program ID to search for.
             *
             * @returns  pointer to the program with the given @c pid,
             *           @c nullptr otherwise.
             */
            virtual EntryPtr find(const uint8_t pid) const = 0;

            /*!
             * Find the program with the given @c name.
             *
             * @param [in] name  program name to search for.
             *
             * @returns  pointer to the program with the given @c name,
             *           @c nullptr otherwise.
             */
            virtual EntryPtr find(const std::string &name) const = 0;

            /*!
             * Return next available PID for the program.
             *
             * @return  the PID to use in the program entry, or
             *          @c Entry::AVAILABLE_PID if no PID is available.
             */
            virtual uint8_t next_pid() const = 0;
         };

         /*!
          * Default implementation of the persistence API.
          */
         struct Entries: public IEntries
         {
            typedef std::map<uint8_t, Entry> Container;
            typedef Container::iterator iterator;
            typedef Container::const_iterator const_iterator;
            typedef Container::value_type value_type;

            virtual ~Entries() {}

            uint16_t size() const;

            Common::Result save(const Entry &entry);

            Common::Result save(const uint8_t pid, const std::string &name,
                                std::vector<Action> &actions);

            Common::Result destroy(const uint8_t pid);

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @warning the reference passed into this method SHOULD NOT be considered
             *          valid if it was obtained by calling the find method.
             */
            Common::Result destroy(const Entry &entry);

            void clear(void);

            EntryPtr find(const uint8_t pid) const;

            EntryPtr find(const std::string &name) const;

            uint8_t next_pid() const;

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
          * Batch Program Management  %Interfaces::Interface : Parent.
          *
          * This is the parent class for the Batch Program Management interface implementation.
          */
         struct Base: public Service<HF::Interface::BATCH_PROGRAM_MANAGEMENT>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit): Service<HF::Interface::BATCH_PROGRAM_MANAGEMENT>(unit) {}
         };

         /*!
          * Batch Program Management %Interfaces::Interface : %Server side implementation.
          *
          * This class provides the server side of the Batch Program Management interface.
          */
         class IServer: public ServiceRole<BatchProgramManagement::Base,
                                           HF::Interface::SERVER_ROLE>
         {
            protected:

            uint8_t _maximum_number_of_entries; //!< Maximum Number Of %Entries

            public:

            //! Constructor
            IServer(Unit0 &unit): ServiceRole<BatchProgramManagement::Base,
                                              HF::Interface::SERVER_ROLE>(unit),
               _maximum_number_of_entries(std::numeric_limits<uint8_t>::max())
            {}

            //! Destructor
            virtual ~IServer() {}

            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c BatchProgramManagement::DEFINE_PROGRAM_CMD,
             * is received.
             *
             * @param [in] packet      the network packet received.
             * @param [in] msg         the DefineProgram message received.
             *
             * @retval Result::OK             if the request was processed,
             * @retval Result::FAIL_ARG       if the message contains invalid arguments to the request,
             * @retval Result::FAIL_RESOURCES if the new entry could not be created,
             * @retval Result::FAIL_UNKNOWN   otherwise.
             */
            virtual Common::Result define_program(const Protocol::Packet &packet,
                                                  DefineProgram &msg);

            /*!
             * Callback that is called when a @c BatchProgramManagement::INVOKE_PROGRAM_CMD,
             * is received.
             *
             * @param [in] packet      the network packet received.
             * @param [in] msg         the InvokeProgram message received.
             *
             * @retval Result::OK        if the request was processed,
             * @retval Result::FAIL_ARG  if the message contains invalid arguments to the request.
             */
            virtual Common::Result invoke_program(const Protocol::Packet &packet,
                                                  InvokeProgram &msg);
            /*!
             * Callback that is called when a @c BatchProgramManagement::DELETE_PROGRAM_CMD,
             * is received.
             *
             * @param [in] packet      the network packet received.
             * @param [in] msg         the DeleteProgram message received.
             *
             * @retval Result::OK        if the request was processed,
             * @retval Result::FAIL_ARG  if the message contains invalid arguments to the request.
             */
            virtual Common::Result delete_program(const Protocol::Packet &packet,
                                                  DeleteProgram &msg);

            /*!
             * Callback that is called when a @c BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD,
             * is received.
             *
             * @param [in] packet      the network packet received.
             *
             * @retval Result::OK
             */
            virtual Common::Result delete_all_programs(const Protocol::Packet &packet);

            /*!
             * Callback that is called when a @c BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD,
             * is received.
             *
             * @param [in] packet      the network packet received.
             * @param [in] msg         the get program actions message received.
             *
             * @retval Result::OK        if the request was processed,
             * @retval Result::FAIL_ARG  if the message contains invalid arguments to the request.
             */
            virtual Common::Result get_program_actions(const Protocol::Packet &packet,
                                                       GetProgramActions &msg);

            //! @}
            // ======================================================================


            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Get a reference to the current object implementing the persistence API,
             * for the device information.
             *
             * @return  reference to the current object for the persistence API.
             */
            virtual IEntries &entries() const = 0;

            /*!
             * Get the program entry given by @c pid.
             *
             * @param [in] pid  Program ID for the program to retrieve.
             *
             * @return  a pointer to the program entry if it exists,
             *          @c nullptr otherwise.
             */
            EntryPtr entry(const uint8_t pid) const
            {
               return entries().find(pid);
            }

            /*!
             * Get the program entry given by @c name.
             *
             * @param [in] name  program name of the program to retrieve.
             *
             * @return  a pointer to the program entry if it exists,
             *          @c nullptr otherwise.
             */
            EntryPtr entry(const std::string &name) const
            {
               return entries().find(name);
            }

            /*!
             * @copydoc IEntries::next_pid
             */
            uint8_t next_pid() const
            {
               return entries().next_pid();
            }

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Maximum Number Of Entries for the Batch Program Management server.
             *
             * @return  the current Maximum Number Of Entries.
             */
            uint8_t maximum_number_of_entries() const;

            /*!
             * Set the Maximum Number Of Entries for the Batch Program Management server.
             *
             * @param [in] __value the  Maximum Number Of Entries value to set the server to.
             */
            void maximum_number_of_entries(uint8_t __value);

            /*!
             * Get the Number Of Entries for the Batch Program Management server.
             *
             * @return  the current Number Of Entries.
             */
            uint8_t number_of_entries() const;

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);

         };

         /*!
          * Helper template to declare a Batch Program server with custom
          * entries backend.
          */
         template<typename _Entries = Entries>
         class Server: public IServer
         {
            public:

            Server(Unit0 &unit): IServer(unit) {}

            virtual ~Server() {}

            _Entries &entries() const
            {
               return const_cast<_Entries &>(_entries);
            }

            protected:

            _Entries _entries;
         };

         //! Helper typedef.
         typedef Server<> DefaultServer;

         /*!
          * Batch Program Management %Interfaces::Interface : %Client side implementation.
          *
          * This class provides the client side of the Batch Program Management interface.
          */
         struct Client: public ServiceRole<BatchProgramManagement::Base,
                                           HF::Interface::CLIENT_ROLE>
         {
            Client(Unit0 &unit): ServiceRole<BatchProgramManagement::Base,
                                             HF::Interface::CLIENT_ROLE>(unit) {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DEFINE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] pid        the program ID.
             * @param [in] name       the name for the program.
             * @param [in] actions    the list of actions that makes the program.
             *
             * @retval Result:OK         if the message could be created and sent,
             * @retval REsult::FAIL_ARG  otherwise.
             */
            Common::Result define_program(const Protocol::Address &addr, const uint8_t pid,
                                          const std::string name, std::vector<Action> &actions)
            {
               DefineProgram request(pid, name, actions);

               return define_program(addr, request);
            }

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DEFINE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] program     the program to send to the device.
             *
             * @retval Result:OK         if the message could be created and sent,
             * @retval REsult::FAIL_ARG  otherwise.
             */
            Common::Result define_program(const Protocol::Address &addr, Entry &program);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::INVOKE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] pid        the ID of the program to run.
             */
            void invoke_program(const Protocol::Address &addr, uint8_t pid);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] pid        the ID of the program to delete.
             */
            void delete_program(const Protocol::Address &addr, uint8_t pid);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD,
             * to the given network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void delete_all_programs(const Protocol::Address &addr);

#ifdef HF_CORE_BATCH_PROGRAM_MANAGEMENT_GET_PROGRAM_ACTIONS_CMD
            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD,
             * to the given network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] pid        the program ID to get the information.
             */
            void get_program_actions(const Protocol::Address &addr, uint8_t pid);
#endif

            //! @}
            // =============================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * This method is called when a response to a create program
             * is received.
             *
             * @param [in] response    the create program response that was received.
             */
            virtual void defined(DefineProgramResponse &response) = 0;

            /*!
             * This method is called when a response to a delete program
             * is received.
             *
             * @param [in] response    the deleted response that was received.
             */
            virtual void deleted(DeleteProgramResponse &response) = 0;

            /*!
             * This method is called when a response to a delete all programs
             * is received.
             *
             * @param [in] response    the deleted all response that was received.
             */
            virtual void deleted(DeleteAllProgramsResponse &response) = 0;

            /*!
             * This method is called when a response to a invoke program message
             * is received.
             *
             * @param [in] response    the invoke program response that was received.
             */
            virtual void invoked(InvokeProgramResponse &response) = 0;

            /*!
             * This method is called when a response to a get info message
             * is received.
             *
             * @param [in] response    the get program actions response that was received.
             */
            virtual void got_actions(GetProgramActionsResponse &response) = 0;

            //! @}
            // ======================================================================

            protected:

            uint16_t payload_size(Protocol::Message::Interface &itf) const;

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*! @} */

      }  // namespace BatchProgramManagement

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup batch_program_management_itf
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
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::BatchProgramManagement::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::BatchProgramManagement::Attributes attribute);
/*! @} */

#endif /* HF_CORE_BATCH_PROGRAM_MANAGEMENT__H */
