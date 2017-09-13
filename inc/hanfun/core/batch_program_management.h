// =============================================================================
/*!
 * @file       inc/hanfun/core/batch_program_management_interface.h
 *
 * This file contains the definitions for the Batch Program Management interface.
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

#ifndef HF_CORE_BATCH_PROGRAM_MANAGEMENT_H
#define HF_CORE_BATCH_PROGRAM_MANAGEMENT_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace BatchProgramManagement
      {
         class Server;
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
       * @param [in] uid      attribute's UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(BatchProgramManagement::Server *server,
                                                   uint8_t uid);

      /*!
       * This namespace contains the implementation of the Batch Program Management interface.
       */
      namespace BatchProgramManagement
      {
         /*!
          * @addtogroup batch_program_management_itf  Batch Program Management interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Batch Program Management interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            DEFINE_PROGRAM_CMD      = 0x01, //!< Define Program command UID.
            INVOKE_PROGRAM_CMD      = 0x02, //!< Invoke Program command UID.
            DELETE_PROGRAM_CMD      = 0x03, //!< Delete Program command UID.
            DELETE_ALL_PROGRAMS_CMD = 0x04, //!< Delete All Programs command UID.
            GET_PROGRAM_ACTIONS_CMD = 0x05, //!< Get Program Actions command UID.
            __LAST_CMD__            = GET_PROGRAM_ACTIONS_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            MAXIMUM_NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Maximum Number Of Entries attribute UID.
            NUMBER_OF_ENTRIES_ATTR         = 0x02, //!< Number Of Entries attribute UID.
            __LAST_ATTR__                  = NUMBER_OF_ENTRIES_ATTR
         } Attributes;

         // =============================================================================
         // Basic memory struct
         // =============================================================================

         struct Action : public Protocol::Message
         {

            /**
             * Constructor
             *
             * @param [in] _UID         Identifier of the Unit to which this action is to be sent.
             * @param [in] _msg_type    Code that specifies how Interface Member is to be interpreted.
             * @param [in] _itf_type    Type of the interface to which this action is to be sent.
             * @param [in] _itf_UID     ID of the interface to which this action is to be sent.
             * @param [in] _itf_member  ID of the interface member to be invoked or acted upon.
             * @param [in] _payload     Content of the attribute or of the payload of the command.
             *
             */
            Action (uint8_t _UID, HF::Protocol::Message::Type _msg_type,
                    uint8_t _itf_type, uint16_t _itf_UID, uint8_t _itf_member,
                    Common::ByteArray& _payload)
            {
               reference = _UID;
               type = _msg_type;
               itf.role = _itf_type;
               itf.id = _itf_UID;
               itf.member = _itf_member;
               payload = _payload;
            }

            /**
             * Default Constructor.
             *
             * Mainly for using with the unpack method.
             */
            Action (void) = default;

            // =================================================================
            // Serializable API
            // =================================================================
#if 0
            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size =   sizeof(uint8_t)   // UID
                                                 + sizeof(uint8_t)   // Message type
                                                 + sizeof(uint8_t)   // Interface type
                                                 + sizeof(uint16_t)  // Interface UID
                                                 + sizeof(uint8_t)   // Interface member
                                                 + sizeof(uint8_t);  // Payload size

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;
#endif
            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);

         };

         struct Entry
         {
            uint8_t ID;                   //!< Program ID, unique per device
            std::string name;             //!< Program Name
            std::vector<Action> actions;  //!< Actions list

            /**
             * Constructor
             *
             * @param [in] _ID        Program ID, unique per device
             * @param [in] _name      Program Name
             * @param [in] _actions   Actions list
             */
            Entry(uint8_t _ID, std::string _name, std::vector<Action>& _actions):
               ID(_ID), name(_name), actions(_actions)
            {}

            /**
             * Default Constructor.
             *
             * Mainly for using with the unpack method.
             */
            Entry(void) = default;

            // =================================================================
            // Serializable API
            // =================================================================
#if 1
            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size =   sizeof(uint8_t)   // Program ID
                                                 + sizeof(uint8_t)   // Name Length
                                                 + sizeof(uint8_t);  // Number of actions

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
#endif
         };

         // =============================================================================
         // Messages
         // =============================================================================

         typedef Entry DefineProgram;

         struct DefineProgramResponse : public Protocol::Response
         {
            uint8_t ID; //!< Program ID, unique per device

            /**
             * Constructor
             *
             * @param [in] _code    the response code for the response.
             * @param [in] _ID      The Program ID of the response.
             */
            DefineProgramResponse(Common::Result _code = Common::Result::OK,
                                    uint8_t _ID = 0):
                                       Protocol::Response(_code), ID(_ID)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = Protocol::Response::min_size;     // Response Code

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         struct InvokeProgram
         {
            uint8_t ID;    //!< Program ID, unique per device

            /**
             * Constructor
             *
             * @param [in] _ID   Program ID, unique per device
             */
            InvokeProgram(uint8_t _ID = 0):
               ID(_ID)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);     // Program ID

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         typedef DefineProgramResponse  InvokeProgramResponse;

         typedef InvokeProgram           DeleteProgram;
         typedef InvokeProgramResponse  DeleteProgramResponse;

         typedef Protocol::Response       DeleteAllPrograms;

         typedef InvokeProgram           GetProgramActions;

         struct GetProgramActionsResponse : public Protocol::Response
         {
            Entry program;    //!< The program entry data

            /**
             * Constructor
             *
             * @param [in] _code    The response code for the response.
             * @param [in] _program The program entry data.
             */
            GetProgramActionsResponse(Common::Result _code,
                                         Entry& _program) :
                                   Protocol::Response(_code), program(_program)
            {}

            GetProgramActionsResponse(Common::Result _code  = Common::Result::OK) :
               Protocol::Response(_code)
            {}

            // =================================================================
            // Serializable API
            // =================================================================

            //! Minimum pack/unpack required data size.
//            static constexpr uint16_t min_size = Protocol::Response::min_size
//                                                 + Entry::min_size;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Maximum Number Of Entries attribute for the Batch Program Management interface.
          */
         struct MaximumNumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_NUMBER_OF_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                          //!< Attribute Read/Write

            MaximumNumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::BATCH_PROGRAM_MANAGEMENT, ID, owner,
                                  value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Number Of Entries attribute for the Batch Program Management interface.
          */
         struct NumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            NumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::BATCH_PROGRAM_MANAGEMENT, ID, owner,
                                  value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::BatchProgramManagement::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::BatchProgramManagement::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Batch Program Management  %Interfaces::Interface : Parent.
          *
          * This is the parent class for the Batch Program Management interface implementation.
          */
         struct Base: public Interfaces::Interface<HF::Interface::BATCH_PROGRAM_MANAGEMENT>
         {
            protected:

            //! Constructor
            Base(): Interfaces::Interface<HF::Interface::BATCH_PROGRAM_MANAGEMENT>() {}
         };

         /*!
          * Batch Program Management %Interfaces::Interface : %Server side implementation.
          *
          * This class provides the server side of the Batch Program Management interface.
          */
         class Server: public Interfaces::InterfaceRole<BatchProgramManagement::Base,
                                                        HF::Interface::SERVER_ROLE>
         {
            protected:

            uint8_t _maximum_number_of_entries; //!< Maximum Number Of Entries
            uint8_t _number_of_entries;         //!< Number Of Entries

            public:

            //! Constructor
            Server(): Interfaces::InterfaceRole<BatchProgramManagement::Base,
                                                HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::INVOKE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void invoke_program(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::INVOKE_PROGRAM_CMD,
             * to the broadcast network address.
             */
            void invoke_program()
            {
               Protocol::Address addr;
               invoke_program(addr);
            }

            //! @}
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
             * @param [in] addr       the network address to send the message to.
             */
            virtual void define_program(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c BatchProgramManagement::DELETE_PROGRAM_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_program(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void delete_all_programs(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void get_program_actions(const Protocol::Address &addr);

            //! @}
            // ======================================================================

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

            /*!
             * Set the Number Of Entries for the Batch Program Management server.
             *
             * @param [in] __value the  Number Of Entries value to set the server to.
             */
            void number_of_entries(uint8_t __value);

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
          * Batch Program Management %Interfaces::Interface : %Client side implementation.
          *
          * This class provides the client side of the Batch Program Management interface.
          */
         struct Client: public Interfaces::InterfaceRole<BatchProgramManagement::Base,
                                                         HF::Interface::CLIENT_ROLE>
         {
            Client(): Interfaces::InterfaceRole<BatchProgramManagement::Base,
                                                HF::Interface::CLIENT_ROLE>() {}

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
             */
            void define_program(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DEFINE_PROGRAM_CMD,
             * to the broadcast network address.
             */
            void define_program()
            {
               Protocol::Address addr;
               define_program(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_PROGRAM_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void delete_program(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_PROGRAM_CMD,
             * to the broadcast network address.
             */
            void delete_program()
            {
               Protocol::Address addr;
               delete_program(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void delete_all_programs(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD,
             * to the broadcast network address.
             */
            void delete_all_programs()
            {
               Protocol::Address addr;
               delete_all_programs(addr);
            }

#ifdef HF_CORE_BATCH_PROGRAM_MANAGEMENT_GET_PROGRAM_ACTIONS_CMD
            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void get_program_actions(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD,
             * to the broadcast network address.
             */
            void get_program_actions()
            {
               Protocol::Address addr;
               get_program_actions(addr);
            }
#endif

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{
            //!

            /*!
             * Callback that is called when a @c BatchProgramManagement::INVOKE_PROGRAM_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void invoke_program(const Protocol::Address &addr);

            //! @}
            // =============================================================================

            protected:

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
