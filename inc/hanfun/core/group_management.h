// =============================================================================
/*!
 * @file       inc/hanfun/core/group_management.h
 *
 * This file contains the definitions for the Group Management service.
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

#ifndef HF_CORE_GROUP_MANAGEMENT_H
#define HF_CORE_GROUP_MANAGEMENT_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

#include <string>
#include <map>

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace GroupManagement
      {
         class IServer;
      }

      /*!
       * @ingroup group_management_itf
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
      HF::Attributes::IAttribute *create_attribute(GroupManagement::IServer *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Group Management service.
       */
      namespace GroupManagement
      {
         /*!
          * @addtogroup group_management_itf  Group Management service
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Group Management service API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            CREATE_CMD   = 0x01, //!< Create command UID.
            DELETE_CMD   = 0x02, //!< Delete command UID.
            ADD_CMD      = 0x03, //!< Add command UID.
            REMOVE_CMD   = 0x04, //!< Remove command UID.
            GET_INFO_CMD = 0x05, //!< Get Info command UID.
            __LAST_CMD__ = GET_INFO_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            NUMBER_OF_GROUPS_ATTR = 0x01,   //!< Number Of Groups attribute UID.
            __LAST_ATTR__         = NUMBER_OF_GROUPS_ATTR
         } Attributes;


         struct GroupAddress
         {
            uint16_t address;    //!< Group Address

            constexpr static uint16_t START_ADDR = 0x0001;  //!< First HAN-FUN Group Address.
            constexpr static uint16_t END_ADDR   = 0x7FFF;  //!< Last HAN-FUN Group Address.

            /*!
             * Constructor.
             *
             * @param [in] address     The group Address
             */
            GroupAddress(uint16_t address = 0)
            {
               //static_assert(START_ADDR <= address && address <= END_ADDR, "Group Address outside range");
               this->address=address;
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint16_t);                         // Group Address

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };


         // =============================================================================
         // Group Member Entry Data structure
         // =============================================================================

         typedef Protocol::Address Member;

         // =============================================================================
         // Group Entry Data structure
         // =============================================================================

         struct Group:public GroupAddress
         {
            std::string name; 	//!< Group Name
            std::vector<Member> members; //!< Group Members

            constexpr static uint16_t MAX_MEMBERS = GroupAddress::END_ADDR -
                                                    GroupAddress::START_ADDR +1;

            /*!
             * Constructor.
             *
             * @param [in] GroupAddr 	Group Address
             * @param [in] GroupName	Group Name
             */
            Group(uint16_t address=0, std::string name=""):
               GroupAddress(address), name(name)
            {};

            std::vector<Member>::iterator find_member (const Member &member)
            {
               /* *INDENT-OFF* */
               std::vector<Member>::iterator it = std::find_if(members.begin(), members.end(), [member](const Member &i)
               {
                  return member == i;
               });
               /* *INDENT-ON* */

               return it;
            }


            bool add_member(const Member &member)
            {
               auto it = find_member(member);

               if(it == members.end())
               {
                  members.push_back(member);
                  return true;
               }
               return false;
            }

            bool remove_member ( const Member &member)
            {
               const auto orig_size = members.size();

               auto it = find_member(member);
               if(it != members.end())
               {
                  members.erase(it);

                  if (members.size() == orig_size-1)
                     return true;

                  return false;
               }
               return false;
            }

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = GroupAddress::min_size						         // Group Address
        	                                      + sizeof(uint8_t)                 	 	// Group Name (Length)
        	                                      + sizeof(uint16_t); 				        	// Members Count

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

            // =============================================================================
            // Operators
            // =============================================================================

            //!Equals operator
            bool operator == (const Group &other) const
            {
               if (this->address == other.address)
                  return true;
               else
                  return false;
            }
            //!Not equals operator
            bool operator != (const Group &other) const
            {
               return !(*this == other);
            }

         };

         typedef Common::Pointer<Group> GroupPtr;

//         struct Member{
//            uint16_t address; //!< Device Address
//            uint8_t unit_id; //!< Unit ID
//
//            constexpr static uint16_t START_ADDR = 0x0000;  //!< First HAN-FUN Device Address.
//            constexpr static uint16_t END_ADDR   = 0x7FFE;  //!< Last HAN-FUN Device Address.
//
//            /*!
//             * Constructor.
//             *
//             * @param [in] address		Device Address
//             * @param [in] unit_id		Unit Member ID
//             */
//            Member(uint16_t address, uint8_t unit_id):
//               address(address), unit_id(unit_id)
//            {};
//
//            // =============================================================================
//            // Serializable API
//            // =============================================================================
//
//            //! Minimum pack/unpack required data size.
//            static constexpr uint16_t min_size = sizeof(uint16_t)					      	// Member Address
//        	         	                              + sizeof(uint8_t);					 	// Member Unit ID
//
//            //! @copydoc HF::Common::Serializable::size
//            uint16_t size() const;
//
//            //! @copydoc HF::Common::Serializable::pack
//            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;
//
//            //! @copydoc HF::Common::Serializable::unpack
//            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
//
//         };

         struct CreateMessage
         {
            std::string name; //!< Group Name.

            /*!
             * Constructor.
             *
             * @param [in] name     Group Name
             */
            CreateMessage(std::string name=""):
               name(name)
            {};

            //virtual ~CreateMessage();

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);          // Group Name (length)

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

         };

         struct CreateResponse : public Protocol::Response, GroupAddress
         {

            /*!
             * Constructor.
             *
             * @param [in] address     Group address
             */
            CreateResponse(uint16_t address = 0):
               GroupAddress(address)
            {};

            //virtual ~CreateResponse();

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = Protocol::Response::min_size;            // Response Code


            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

         };

         typedef GroupAddress DeleteMessage;

         typedef Protocol::Response DeleteResponse;


         struct AddMessage : public GroupAddress, Protocol::Address
         {

            /*!
             * Constructor.
             *
             * @param [in] address     Group address
             */
            AddMessage(uint16_t group, uint16_t device, uint8_t unit):
               GroupAddress(group), Protocol::Address(device, unit)
            {};

            /*!
             * Empty Constructor.
             *
             * Mainly used for the unpack function.
             */
            AddMessage()
            {};

            //virtual ~AddMessage();

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = GroupAddress::min_size
                                               + Protocol::Address::min_size;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

         };

         typedef Protocol::Response AddResponse;

         typedef AddMessage RemoveMessage;

         typedef Protocol::Response RemoveResponse;


         typedef GroupAddress Info;

         struct InfoResponse: public Protocol::Response
         {
            std::string name;
            std::vector<Member> members;

            /*!
             * Constructor.
             *
             * @param name       Group Name
             * @param members    Member vector
             */
            InfoResponse(std::string name, std::vector<Member> members):
               name(name), members(members)
            {}

            /*!
             * Empty Constructor.
             *
             * Mainly to use with the unpack function.
             */
            InfoResponse()
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = Protocol::Response::min_size;


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
          * Helper class to handle the Number Of Groups attribute for the Group Management service.
          */
         struct NumberOfGroups: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_GROUPS_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                 //!< Attribute Read/Write

            NumberOfGroups(uint8_t value = 0, const HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::GROUP_MANAGEMENT, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::GroupManagement::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::GroupManagement::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);


         /*!
          * Device Management - Persistent Storage API.
          */
         struct IEntries: public Common::IEntries<Group>
         {
            /*!
             * Return the Device entry for the given address.
             *
             * @param [in] address    the device address.
             *
             * @retval  a pointer to the Device entry associated with the given address,
             * @retval  nullptr if the entry does not exist.
             */
            virtual GroupPtr find (uint16_t address) const = 0;

            virtual GroupPtr find (const std::string &name) const = 0;

            /*!
             * Return next available address for registering a device.
             *
             * @return  the address to use in the next registration.
             */
            virtual uint16_t next_address () const = 0;
         };


         /*!
          * Default implementation of the persistence API.
          */
         struct Entries: public IEntries
         {
            typedef std::map<uint16_t, Group> Container;
            typedef Container::iterator iterator;
            typedef Container::const_iterator const_iterator;
            typedef Container::value_type value_type;

            virtual ~Entries() {}

            uint16_t size () const;

            Common::Result save (const Group &entry);

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @param [in] address     The @c Group address to destroy
             * @return
             */
            Common::Result destroy (const uint16_t &address);

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @warning the reference passed into this method SHOULD NOT be considered
             *          valid if it was obtained by calling the find method.
             *
             */
            Common::Result destroy (const Group &entry);

            GroupPtr find (uint16_t address) const;

            GroupPtr find (const std::string &name) const;

            uint16_t next_address () const;

            /*!
             * Get an iterator to the start of the entries in this container.
             *
             * @return  iterator to the start of the entries present in this container.
             */
            iterator begin ()
            {
               return db.begin();
            }

            /*!
             * Get an iterator to the end of the entries in this container.
             *
             * @return  iterator to the end of the entries present in this container.
             */
            iterator end ()
            {
               return db.end();
            }

            /*!
             * Get a constant iterator to the start of the entries in this container.
             *
             * @return  constant iterator to the start of the entries present in this container.
             */
            const_iterator begin () const
            {
               return db.cbegin();
            }

            /*!
             * Get a constant iterator to the start of the entries in this container.
             *
             * @return  constant iterator to the start of the entries present in this container.
             */
            const_iterator end () const
            {
               return db.cend();
            }

            protected:

            //! Actual container for the entries.
            Container db;
         };

         /*!
          * Group Management %Service : Parent.
          *
          * This is the parent class for the Group Management service implementation.
          */
         struct Base: public Service<HF::Interface::GROUP_MANAGEMENT>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit): Service<HF::Interface::GROUP_MANAGEMENT>(unit) {}
         };

         /*!
          * Group Management %Service : %Server side implementation.
          *
          * This class provides the server side of the Group Management interface.
          */
         class IServer: public ServiceRole<GroupManagement::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint8_t _number_of_groups;   //!< Number Of Groups

            public:

            //! Constructor
            IServer(Unit0 &unit): ServiceRole<GroupManagement::Base,
                                             HF::Interface::SERVER_ROLE>(unit) {}

            //! Destructor
            virtual ~IServer() {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c GroupManagement::CREATE_CMD,
             * is received.
             *
             * @param [in] packet      The message Packet.
             * @param [in] msg         The @c CreateMessage sent.
             * @return                 The response code sent in response.
             */
            virtual Common::Result create(Protocol::Packet &packet, CreateMessage &msg);

            /*!
             * Indicate that a new group was created.
             *
             * @param [in] group       Pointer to the group entry corresponding to the created group.
             */
            virtual void created(const GroupPtr &group)
            {
               UNUSED(group);
            }

            /*!
             * Callback that is called when a @c GroupManagement::DELETE_CMD,
             * is received.
             *
             * @param [in] packet      The message Packet.
             * @param [in] msg         The @c DeleteMessage sent.
             * @return                 The response code sent in response.
             */
           virtual Common::Result remove(Protocol::Packet &packet, DeleteMessage &msg);

           /*!
            * Indicate that a group was deleted.
            *
            * @param [in] group     Copy of the group entry that was deleted.
            */
           virtual void deleted(Group group)
           {
              //TODO : Check this! Can we pass it by reference pointer???
              UNUSED(group);
           }

            /*!
             * Callback that is called when a @c GroupManagement::ADD_CMD,
             * is received.
             *
             * @param [in] packet      The message Packet.
             * @param [in] msg         The @c CreateMessage sent.
             * @return                 The response code sent in response.
             */
            virtual Common::Result add(Protocol::Packet &packet, const AddMessage &msg);

            /*!
             * Indicate that a new device/unit was added to an existing group.
             *
             * @param [in] group       Pointer to the group entry corresponding to the group.
             */
            virtual void added(const GroupPtr &group)
            {
               UNUSED(group);
            }

            /*!
             * Callback that is called when a @c GroupManagement::REMOVE_CMD,
             * is received.
             *
             * @param [in] packet      The message Packet.
             * @param [in] msg         The @c RemoveMessage sent.
             * @return                 The response code sent in response.
             */
            virtual Common::Result remove(Protocol::Packet &packet, const RemoveMessage &msg);

            /*!
             * Indicate that a device/unit was removed from an existing group.
             *
             * @param [in] group       Pointer to the group entry corresponding to the affected group.
             */
            virtual void removed (const GroupPtr &group)
            {
               UNUSED(group);
            }

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
            /*!
             * Callback that is called when a @c GroupManagement::GET_INFO_CMD,
             * is received.
             *
             * @param [in] packet      The message Packet.
             * @param [in] msg         The @c InfoMessage sent.
             * @return                 The response code sent in response.
             */
            virtual Common::Result get_info(Protocol::Packet &packet, const InfoMessage &msg);

            /*!
             * Indicate that group info was requested.
             *
             * @param [in] group       Pointer to the group entry corresponding to the affected group.
             */
            virtual void got_info (const GroupPtr &group)
            {
               UNUSED(group);
            }

#endif

            //! @}
            // ======================================================================

            // =============================================================================
            // API.
            // =============================================================================


            GroupPtr entry(const uint16_t address) const
            {
               return entries().find(address);
            }

            GroupPtr entry(const std::string &name) const
            {
               return entries().find(name);
            }

            /*!
             * Get a reference to the current object implementing the persistence API,
             * for the device information.
             *
             * @return  reference to the current object for the persistence API.
             */
            virtual IEntries &entries() const = 0;

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Number Of Groups for the Group Management server.
             *
             * @return  the current Number Of Groups.
             */
            uint8_t number_of_groups() const
            {
               return (uint8_t) entries().size();
            }

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED(pack_id);
               return HF::Attributes::UIDS {NUMBER_OF_GROUPS_ATTR};
            }

            /*!
             * Return next available address for device group.
             *
             * @return  the address to use in the next group.
             */
            virtual uint16_t next_address ()
            {
               return entries().next_address();
            }

            protected:

            /*!
             * Get the Number Of Groups for the Group Management server.
             *
             * @param [in] diff  number of groups added/removed.
             *
             * @return  the current Number Of Groups.
             */
            void number_of_groups_update(int8_t diff) const
            {
               uint8_t value = number_of_groups();

               NumberOfGroups old_attr(value - diff, this);
               NumberOfGroups new_attr(value, this);

               notify(old_attr, new_attr);
            }

            uint16_t payload_size(Protocol::Message::Interface &itf) const;

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         template<typename _Entries = Entries>
         class Server: public IServer
         {
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

            _Entries &entries() const
            {
               return const_cast<_Entries &>(_entries);
            }
         };

         typedef Server<> DefaultServer;

         /*!
          * Group Management %Service : %Client side implementation.
          *
          * This class provides the client side of the Group Management interface.
          */
         struct Client: public ServiceRole<GroupManagement::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(Unit0 &unit): ServiceRole<GroupManagement::Base,
                                             HF::Interface::CLIENT_ROLE>(unit) {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::CREATE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void create(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::CREATE_CMD,
             * to the D:0/U:0 network address.
             */
            void create()
            {
               Protocol::Address addr(0, 0);
               create(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::DELETE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void remove(const Protocol::Address &addr, uint16_t group);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::DELETE_CMD,
             * to the D:0/U:0 network address.
             */
            void remove(uint16_t group)
            {
               Protocol::Address addr(0, 0);
               remove(addr, group);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::ADD_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void add(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::ADD_CMD,
             * to the D:0/U:0 network address.
             */
            void add()
            {
               Protocol::Address addr(0, 0);
               add(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::REMOVE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] group      the group identifier the device belongs to.
             * @param [in] device     the device identifier to delete the unit from.
             * @param [in] unit       the unit identifier of the given @c device to delete from the given @c group.
             */
            void remove(const Protocol::Address &addr, uint16_t group, uint16_t device, uint8_t unit);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::REMOVE_CMD,
             * to the D:0/U:0 network address.
             */
            void remove(uint16_t group, uint16_t device, uint8_t unit)
            {
               Protocol::Address addr(0, 0);
               remove(addr, group, device, unit);
            }

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::GET_INFO_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void get_info(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::GET_INFO_CMD,
             * to the D:0/U:0 network address.
             */
            void get_info()
            {
               Protocol::Address addr(0, 0);
               get_info(addr);
            }
#endif

            //! @}
            // ======================================================================
         };

         /*! @} */

      }  // namespace GroupManagement

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup group_management_itf
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
std::ostream &operator<<(std::ostream &stream, const HF::Core::GroupManagement::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::GroupManagement::Attributes attribute);
/*! @} */

#endif /* HF_CORE_GROUP_MANAGEMENT_H */
