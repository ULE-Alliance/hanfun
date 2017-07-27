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

#include "hanfun/core/group_table.h"

#include <string>
#include <map>
#include <forward_list>

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
          * This module contains the classes that define and implement the
          * Group Management service API.
          *
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
            uint16_t                  address; //!< Group Address

            constexpr static uint16_t NO_ADDR    = 0x0000;  //!< Empty Group Address.
            constexpr static uint16_t START_ADDR = 0x0001;  //!< First HAN-FUN Group Address.
            constexpr static uint16_t END_ADDR   = 0x7FFF;  //!< Last HAN-FUN Group Address.

            /*!
             * Constructor.
             *
             * @param [in] address     The group Address
             */
            GroupAddress(uint16_t address = 0)
            {
               // static_assert(START_ADDR <= address && address <= END_ADDR, "Group Address outside range");
               this->address = address;
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

         struct Group: public GroupAddress
         {
            std::string               name;    //!< Group Name
            std::vector<Member>       members; //!< Group Members

            constexpr static uint16_t MAX_MEMBERS = GroupAddress::END_ADDR -
                                                    GroupAddress::START_ADDR + 1;

            /*!
             * Constructor.
             *
             * @param [in] GroupAddr    Group Address
             * @param [in] GroupName	Group Name
             */
            Group(uint16_t address = 0, std::string name = ""):
               GroupAddress(address), name(name)
            {}

            std::vector<Member>::iterator find_member(const Member &member)
            {
               /* *INDENT-OFF* */
               auto it = std::find_if(members.begin(), members.end(),
                                      [&member](const Member &i)
               {
                  return member == i;
               });
               /* *INDENT-ON* */

               return it;
            }

            bool exists(const Member &member)
            {
               return !(find_member(member) == members.end());
            }

            bool exists(uint16_t device, uint8_t unit)
            {
               const Member member(device, unit);

               return !(find_member(member) == members.end());
            }

            bool add(uint16_t device, uint8_t unit)
            {
               const Member member(device, unit);

               return add(member);
            }

            bool add(const Member &member)
            {
               auto it = find_member(member);

               if (it == members.end())
               {
                  members.push_back(member);
                  return true;
               }

               return false;
            }

            bool remove(const Member &member)
            {
               const auto orig_size = members.size();

               auto it              = find_member(member);

               if (it != members.end())
               {
                  members.erase(it);

                  if (members.size() == orig_size - 1)
                  {
                     return true;
                  }

                  return false;
               }

               return false;
            }

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = GroupAddress::min_size  // Group Address
                                                 + sizeof(uint8_t)       // Group Name (Length)
                                                 + sizeof(uint16_t);     // Members Count

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

            // =============================================================================
            // Operators
            // =============================================================================

            //! Equals operator
            bool operator==(const Group &other) const
            {
               if (this->address == other.address)
               {
                  return true;
               }
               else
               {
                  return false;
               }
            }

            //! Not equals operator
            bool operator!=(const Group &other) const
            {
               return !(*this == other);
            }
         };

         typedef Common::Pointer<Group> GroupPtr;

         /*!
          * Message payload for a @c HF::GroupManagement::CREATE_CMD request.
          */
         struct CreateMessage
         {
            std::string name; //!< Group Name.

            /*!
             * Constructor.
             *
             * @param [in] name     Group Name
             */
            CreateMessage(std::string name = ""):
               name(name)
            {}

            // virtual ~CreateMessage();

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

         /*!
          * Message payload for a @c HF::GroupManagement::CREATE_CMD response.
          */
         struct CreateResponse: public Protocol::Response, GroupAddress
         {
            /*!
             * Constructor.
             *
             * @param [in] address     Group address
             */
            CreateResponse(uint16_t address):
               Protocol::Response(), GroupAddress(address)
            {}

            /*!
             * Constructor.
             *
             * @param [in] address     Group address
             * @param [in] code        Response code.
             */
            CreateResponse(Common::Result code = Common::Result::OK,
                           uint16_t address = GroupAddress::NO_ADDR):
               Protocol::Response(code), GroupAddress(address)
            {}

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

         //! Message payload for a @c HF::GroupManagement::DELETE_CMD request.
         typedef GroupAddress DeleteMessage;

         //! Message payload for a @c HF::GroupManagement::DELETE_CMD response.
         typedef Protocol::Response DeleteResponse;

         /*!
          * Parent class for the message payload for a @c HF::GroupManagement::ADD_CMD and
          * @c HF::GroupManagement::REMOVE_CMD requests.
          */
         struct Message: public GroupAddress, Protocol::Address
         {
            /*!
             * Constructor.
             *
             * @param [in] address     Group address
             */
            Message(uint16_t group, uint16_t device, uint8_t unit):
               GroupAddress(group), Protocol::Address(device, unit)
            {}

            /*!
             * Empty Constructor.
             *
             * Mainly used for the unpack function.
             */
            Message()
            {}

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

         //! Message payload for a @c HF::GroupManagement::ADD_CMD request.
         typedef Message AddMessage;

         //! Message payload for a @c HF::GroupManagement::ADD_CMD response.
         typedef Protocol::Response AddResponse;

         //! Message payload for a @c HF::GroupManagement::REMOVE_CMD request.
         typedef Message RemoveMessage;

         //! Message payload for a @c HF::GroupManagement::REMOVE_CMD response.
         typedef Protocol::Response RemoveResponse;

         //! Message payload for a @c HF::GroupManagement::GET_INFO_CMD request.
         typedef GroupAddress InfoMessage;

         //! Message payload for a @c HF::GroupManagement::GET_INFO_CMD response.
         struct InfoResponse: public Protocol::Response
         {
            std::string         name;
            std::vector<Member> members;

            /*!
             * Constructor.
             *
             * @param name       Group Name
             * @param members    Member vector
             */
            InfoResponse(const std::string &name, std::vector<Member> &members):
               Protocol::Response(), name(name), members(members)
            {}

            /*!
             * Constructor.
             *
             * @param name       Group Name
             * @param members    Member vector
             */
            InfoResponse(Common::Result code, const std::string &name,
                         std::vector<Member> &members):
               Protocol::Response(code), name(name), members(members)
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
             * Store the given @c entry to persistent storage.
             *
             * @param [in] address  HF group address for the new group.
             * @param [in] name     name for the new group.
             *
             * @retval  Common::Result::OK if the entry was saved,
             * @retval  Common::Result::FAIL_UNKNOWN otherwise.
             */
            virtual Common::Result save(uint16_t address, const std::string &name) = 0;

            /*!
             * Return the Device entry for the given address.
             *
             * @param [in] address    the device address.
             *
             * @retval  a pointer to the Device entry associated with the given address,
             * @retval  nullptr if the entry does not exist.
             */
            virtual GroupPtr find(uint16_t address) const        = 0;

            virtual GroupPtr find(const std::string &name) const = 0;

            /*!
             * Return next available address for registering a device.
             *
             * @return  the address to use in the next registration.
             */
            virtual uint16_t next_address() const = 0;
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

            uint16_t size() const;

            Common::Result save(const Group &entry);

            Common::Result save(uint16_t address, const std::string &name);

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @param [in] address     The @c Group address to destroy
             * @return
             */
            Common::Result destroy(const uint16_t &address);

            /*!
             * @copydoc HF::Common::IEntries::destroy
             *
             * @warning the reference passed into this method SHOULD NOT be considered
             *          valid if it was obtained by calling the find method.
             *
             */
            Common::Result destroy(const Group &entry);

            GroupPtr find(uint16_t address) const;

            GroupPtr find(const std::string &name) const;

            uint16_t next_address() const;

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

         // Forward declaration.
         struct IGroupTable;

         /*!
          * Group Management %Service : %Server side implementation.
          *
          * This class provides the server side of the Group Management interface.
          */
         class IServer: public ServiceRole<GroupManagement::Base, HF::Interface::SERVER_ROLE>
         {
            using Server = ServiceRole<GroupManagement::Base, HF::Interface::SERVER_ROLE>;

            public:

            //! Constructor
            IServer(Unit0 &unit): Server(unit) {}

            //! Destructor
            virtual ~IServer() {}

            // ======================================================================
            // API
            // ======================================================================

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
             * @param [in] group       Pointer to the group entry corresponding to the
             *                         created group.
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
               // TODO : Check this! Can we pass it by reference pointer???
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
            virtual void added(const GroupPtr &group, const Member &member)
            {
               UNUSED(group);
               UNUSED(member);
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
            virtual void removed(const GroupPtr &group, const Member &member)
            {
               UNUSED(group);
               UNUSED(member);
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
            virtual void got_info(const GroupPtr &group)
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
            virtual uint16_t next_address()
            {
               return entries().next_address();
            }

            //! @copydoc HF::Interface::handle
            Common::Result handle(Protocol::Packet &packet, Common::ByteArray &payload,
                                  uint16_t offset);

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

            /*!
             * Process the result of adding a device to a group.
             *
             * @param [in] addr        HF address of the device that requested the
             *                         @c HF::GroupManagement::ADD_CMD command.
             * @param [in] result      the result of the operation.
             * @param [in] request     the payload for the @c HF::GroupManagement::ADD_CMD command.
             * @param [in] reference   Application reference for the initial request.
             */
            void added(const Protocol::Address &addr, Common::Result result,
                       const AddMessage &request, uint8_t reference);

            virtual IGroupTable &group_table() const = 0;

            friend IGroupTable;
         };

         /*!
          * Interface to the HF::GroupTable API used to update the device's group tables
          * entries on add/remove events.
          *
          * Implementations of this interface are responsible for calling HF::IGroupTable::added,
          * when the device responds to the @c HF::GroupTable::ADD_CMD / @c HF::GroupTable::REMOVE_CMD
          * commands.
          */
         struct IGroupTable: protected GroupTable::Client
         {
            IGroupTable(IServer &_server): GroupTable::Client(_server.unit()),
               server(_server)
            {}

            using GroupTable::Client::uid;
            using GroupTable::Client::handle;
            using GroupTable::Client::remove;

            /*!
             * Send a GroupTable::ADD_CMD to the device given by the @c AddMessage request.
             *
             * @param source     HF address of the device that requested the @c HF::GroupManagement::ADD_CMD.
             * @param request    the payload for the @c HF::GroupManagement::ADD_CMD command.
             * @param reference  application reference of the message.
             */
            virtual void add(const Protocol::Address &source, const Message &request,
                             uint8_t reference);

            protected:

            IServer &server;

            using GroupTable::Client::added;

            void added(const Protocol::Address &dest, Common::Result result,
                       const AddMessage &request, uint8_t reference)
            {
               server.added(dest, result, request, reference);
            }
         };

         /*!
          * Default implementation of the @c IGroupTable interface.
          */
         struct GroupTableClient: public IGroupTable
         {
            using Entry = std::tuple<Protocol::Address, Message, uint8_t>;

            enum Fields
            {
               ADDRESS,
               MESSAGE,
               REFERENCE,
            };

            GroupTableClient(IServer &_server): IGroupTable(_server)
            {}

            //! @copydoc IGroupTable::add
            void add(const Protocol::Address &source, const AddMessage &request,
                     uint8_t reference);

            protected:

            std::forward_list<Entry> requests;

            using IGroupTable::added;

            //! @copydoc GroupTable::Client::added
            void added(const Protocol::Address &addr, const GroupTable::Response &response);
         };

         /*!
          * Helper class used to implement custom functionality to the group management
          * server side.
          */
         template<typename _Entries = Entries, typename _GroupTable = GroupTableClient>
         class Server: public IServer
         {
            protected:

            _Entries _entries;
            _GroupTable _group_table;

            public:

            /*!
             * Constructor.
             *
             * @param [in] unit  reference to the unit containing this service.
             */
            Server(Unit0 &unit): IServer(unit), _group_table(*this)
            {}

            virtual ~Server()
            {}

            //! @copydoc IServer::entries
            _Entries &entries() const
            {
               return const_cast<_Entries &>(_entries);
            }

            protected:

            //! @copydoc IServer::group_table
            _GroupTable &group_table() const
            {
               return const_cast<_GroupTable &>(_group_table);
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
             * Send a HAN-FUN message containing a @c GroupManagement::CREATE_CMD,
             * to the D:0/U:0 network address.
             *
             *  @param [in] name    The name of the group to be created.
             */
            void create(std::string name);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::DELETE_CMD,
             * to the D:0/U:0 network address.
             *
             *  @param [in] group      The group address to delete.
             */
            void remove(uint16_t group);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::ADD_CMD,
             * to the D:0/U:0 network address.
             *
             * @param [in] group      the group identifier to add the device.
             * @param [in] device     the device identifier to add the unit from.
             * @param [in] unit       the unit identifier of the given @c device to add from the given @c group.
             */
            void add(uint16_t group, uint16_t device, uint8_t unit);

            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::REMOVE_CMD,
             * to the D:0/U:0 network address.
             *
             * @param [in] group      the group identifier the device belongs to.
             * @param [in] device     the device identifier to delete the unit from.
             * @param [in] unit       the unit identifier of the given @c device to delete from the given @c group.
             */
            void remove(uint16_t group, uint16_t device, uint8_t unit);

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
            /*!
             * Send a HAN-FUN message containing a @c GroupManagement::GET_INFO_CMD,
             * to the D:0/U:0 network address.
             *
             *  @param [in] group      The group address for the info request.
             */
            void get_info(uint16_t group);
#endif

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * This method is called when a response to a create message
             * is received.
             *
             * @param [in] response    the create response that was received.
             */
            virtual void created(CreateResponse &response) = 0;

            /*!
             * This method is called when a response to a delete message
             * is received.
             *
             * @param [in] response    the create response that was received.
             */
            virtual void deleted(DeleteResponse &response) = 0;

            /*!
             * This method is called when a response to a add message
             * is received.
             *
             * @param [in] response    the create response that was received.
             */
            virtual void added(AddResponse &response) = 0;

            /*!
             * This method is called when a response to a remove message
             * is received.
             *
             * @param [in] response    the create response that was received.
             */
            virtual void removed(RemoveResponse &response) = 0;

            /*!
             * This method is called when a response to a get info message
             * is received.
             *
             * @param [in] response    the create response that was received.
             */
            virtual void got_info(InfoResponse &response) = 0;

            //! @}
            // ======================================================================

            protected:

            using ServiceRole::payload_size;

            uint16_t payload_size(Protocol::Message::Interface &itf) const;

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);

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
