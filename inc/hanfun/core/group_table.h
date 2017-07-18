// =============================================================================
/*!
 * @file       inc/hanfun/core/group_table.h
 *
 * This file contains the definitions for the Group Table service.
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

#ifndef HF_CORE_GROUP_TABLE_H
#define HF_CORE_GROUP_TABLE_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace GroupTable
      {
         class IServer;
      }

      /*!
       * @ingroup group_table_itf
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
      HF::Attributes::IAttribute *create_attribute(GroupTable::IServer *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Group Table service.
       */
      namespace GroupTable
      {
         /*!
          * @addtogroup group_table_itf  Group Table service
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Group Table service API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            ADD_CMD          = 0x01, //!< Add command UID.
            REMOVE_CMD       = 0x02, //!< Remove command UID.
            REMOVE_ALL_CMD   = 0x03, //!< Remove All command UID.
            READ_ENTRIES_CMD = 0x04, //!< Read Entries command UID.
            __LAST_CMD__     = READ_ENTRIES_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            NUMBER_OF_ENTRIES_ATTR     = 0x01, //!< Number Of Entries attribute UID.
            NUMBER_OF_MAX_ENTRIES_ATTR = 0x02, //!< Number Of Max Entries attribute UID.
            __LAST_ATTR__              = NUMBER_OF_MAX_ENTRIES_ATTR
         } Attributes;

         /*!
          * This represents a group table entry data structure.
          */
         struct Entry
         {
            uint16_t group;   //!< Group address the entry belongs to.

            uint8_t  unit;    //!< Unit ID to route the group messages to.

            Entry(uint16_t _group = 0, uint8_t _unit = 0):
               group(_group), unit(_unit) {}

            Entry(const Entry &entry) = default;

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint16_t)  // Group Address
                                               + sizeof(uint8_t);  // Unit ID

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
          * Helper class to handle the Number Of Entries attribute for the Group Table service.
          */
         struct NumberOfEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            NumberOfEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::GROUP_TABLE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Number Of Max Entries attribute for the Group Table service.
          */
         struct NumberOfMaxEntries: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = NUMBER_OF_MAX_ENTRIES_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                      //!< Attribute Read/Write

            NumberOfMaxEntries(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::GROUP_TABLE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::GroupTable::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::GroupTable::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         // =============================================================================
         // Messages
         // =============================================================================

         struct Response: public Protocol::Response, public Entry
         {
            Response(Common::Result code = Common::Result::FAIL_UNKNOWN,
                     uint16_t group = 0x0000, uint8_t unit = 0x00):
               Protocol::Response(code), Entry(group, unit) {}

            Response(Common::Result code, const Entry &entry):
               Protocol::Response(code), Entry(entry) {}

            // =================================================================
            // Serializable API
            // =================================================================

            static constexpr uint16_t min_size = Protocol::Response::min_size
                                               + Entry::min_size;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return Protocol::Response::size() + Entry::size();
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint16_t start = offset;

               offset += Protocol::Response::pack(array, offset);
               offset += Entry::pack(array, offset);

               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint16_t start = offset;

               offset += Protocol::Response::unpack(array, offset);
               offset += Entry::unpack(array, offset);

               return offset - start;
            }
         };

         // =============================================================================
         // API
         // =============================================================================

         /*!
          * Group Table persistent storage API.
          */
         struct IEntries: public Common::IEntries<Entry>
         {
            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Clear all entries in database.
             */
            virtual void clear() = 0;

            /*!
             * Call the given function for all the entries with given @c group address.
             *
             * @param [in] group    group address to search for.
             */
            virtual void for_each(uint16_t group, std::function<void(const Entry &)> func) const = 0;
         };

         struct Entries: public IEntries
         {
            typedef std::vector<Entry> Container;

            //! @copydoc
            uint16_t size() const;

            Common::Result save(const Entry &entry);

            Common::Result destroy(const Entry&entry);

            void clear();

            void for_each(uint16_t group, std::function<void(const Entry &)> func) const;

            protected:

            bool any_of(uint16_t group, uint8_t unit) const;

            Container db;  //!< Group table entries database.
         };

         /*!
          * Group Table %Service : Parent.
          *
          * This is the parent class for the Group Table service implementation.
          */
         struct Base: public Service<HF::Interface::GROUP_TABLE>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit): Service<HF::Interface::GROUP_TABLE>(unit) {}
         };

         /*!
          * Group Table %Service : %Server side implementation.
          *
          * This class provides the server side of the Group Table interface.
          */
         class IServer: public ServiceRole<GroupTable::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            //! Number Of Max Entries
            uint8_t _number_of_max_entries = std::numeric_limits<uint8_t>::max();

            public:

            //! Constructor
            IServer(Unit0 &unit):
               ServiceRole<GroupTable::Base, HF::Interface::SERVER_ROLE>(unit) {}

            //! Destructor
            virtual ~IServer() {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c GroupTable::ADD_CMD,
             * is received.
             *
             * @param [in] addr   the network address the message was sent from.
             * @param [in] entry  the entry to add to the server.
             *
             * @retval Common::Result::OK                if the entry was added;
             * @retval Common::Result::FAIL_RESOURCES    if the @c number_of_max_entries would be exceed;
             * @retval Common::Result::FAIL_AUTH         if the request did not came from the Concentrator (0, 0).
             */
            virtual Common::Result add(const Protocol::Address &addr, const Entry &entry);

            /*!
             * Callback that is called when a @c GroupTable::REMOVE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to the message was received from.
             *
             * @retval Common::Result::OK          if the @c entry was removed;
             * @retval Common::Result::FAIL_ARG    if no entry exits that matches the given @c entry;
             * @retval Common::Result::FAIL_AUTH   if the request did not came from the Concentrator (0, 0).
             *
             */
            virtual Common::Result remove(const Protocol::Address &addr, const Entry &entry);

            /*!
             * Callback that is called when a @c GroupTable::REMOVE_ALL_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual Common::Result remove_all(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c GroupTable::READ_ENTRIES_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void read_entries(const Protocol::Address &addr);

            //! @}
            // ======================================================================

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Number Of Entries for the Group Table server.
             *
             * @return  the current Number Of Entries.
             */
            uint8_t number_of_entries() const;

            /*!
             * Get the Number Of Max Entries for the Group Table server.
             *
             * @return  the current Number Of Max Entries.
             */
            uint8_t number_of_max_entries() const;

            /*!
             * Set the Number Of Max Entries for the Group Table server.
             *
             * @param [in] __value the  Number Of Max Entries value to set the server to.
             */
            void number_of_max_entries(uint8_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

            // =============================================================================
            // Attribute API.
            // =============================================================================

            virtual IEntries &entries() const = 0;

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);


            uint16_t payload_size(Protocol::Message::Interface &itf) const;
         };

         template<typename _Entries = GroupTable::Entries>
         class Server: public IServer
         {
            public:

            //! Constructor
            Server(Unit0 &unit):
               IServer(unit) {}

            virtual ~Server() {}

            IEntries &entries() const
            {
               return *(const_cast<_Entries *>(&db));
            }

            protected:

            _Entries db;
         };

         typedef Server<> DefaultServer;

         /*!
          * Group Table %Service : %Client side implementation.
          *
          * This class provides the client side of the Group Table interface.
          */
         struct Client: public ServiceRole<GroupTable::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(Unit0 &unit): ServiceRole<GroupTable::Base, HF::Interface::CLIENT_ROLE>(unit) {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::ADD_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] entry      the group table entry to add.
             */
            void add(const Protocol::Address &addr, const Entry &entry);

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::ADD_CMD, to the given
             * @c device and the given @c entry.
             *
             * @param [in] device   the device address to send the entry to.
             * @param [in] entry    the group table entry to add.
             */
            void add(const uint16_t device, const Entry &entry)
            {
               Protocol::Address addr(device, 0);
               add(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::ADD_CMD, to the given
             * network address, for the given @c group and @c unit.
             *
             * @param [in] addr     the network address to send the message to.
             * @param [in] group    the group address for the entry to add.
             * @param [in] group    the unit ID for the entry to add.
             */
            void add(const Protocol::Address &addr, uint16_t group, uint8_t unit)
            {
               Entry entry(group, unit);
               add(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::ADD_CMD, to the given
             * @c device, for the given @c group and @c unit.
             *
             * @param [in] device   the device address to send the entry to.
             * @param [in] group    the group address for the entry to add.
             * @param [in] group    the unit ID for the entry to add.
             */
            void add(const uint16_t device, uint16_t group, uint8_t unit)
            {
               Protocol::Address addr(device, 0);
               Entry entry(group, unit);
               add(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] entry      the group table entry to remove.
             */
            void remove(const Protocol::Address &addr, const Entry &entry);

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_CMD, to the given
             * @c device and the given @c entry.
             *
             * @param [in] device   the device address to send the entry to.
             * @param [in] entry    the group table entry to remove.
             */
            void remove(const uint16_t device, const Entry &entry)
            {
               Protocol::Address addr(device, 0);
               remove(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_CMD, to the given
             * network address, for the given @c group and @c unit.
             *
             * @param [in] addr     the network address to send the message to.
             * @param [in] group    the group address for the entry to remove.
             * @param [in] group    the unit ID for the entry to remove.
             */
            void remove(const Protocol::Address &addr, uint16_t group, uint8_t unit)
            {
               Entry entry(group, unit);
               remove(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_CMD, to the given
             * @c device, for the given @c group and @c unit.
             *
             * @param [in] device   the device address to send the entry to.
             * @param [in] group    the group address for the entry to remove.
             * @param [in] group    the unit ID for the entry to remove.
             */
            void remove(const uint16_t device, uint16_t group, uint8_t unit)
            {
               Protocol::Address addr(device, 0);
               Entry entry(group, unit);
               remove(addr, entry);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_ALL_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void remove_all(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::REMOVE_ALL_CMD, to the device
             * with the given address.
             *
             * @param [in] device   the device's network address to send the message to.
             */
            void remove_all(uint16_t device)
            {
               Protocol::Address addr(device, 0);
               remove_all(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::READ_ENTRIES_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void read_entries(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c GroupTable::READ_ENTRIES_CMD,
             * to the D:0/U:0 network address.
             */
            void read_entries()
            {
               Protocol::Address addr(0, 0);
               read_entries(addr);
            }

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback for processing the response of a @c GroupTable::ADD_CMD.
             *
             * @param [in] addr        address for device that sent the response.
             * @param [in] response    the response received.
             */
            virtual void added(const Protocol::Address &addr, const GroupTable::Response &response);

            /*!
             * Callback for processing the response of a @c GroupTable::REMOVE_CMD.
             *
             * @param [in] addr        address for device that sent the response.
             * @param [in] response    the response received.
             */
            virtual void removed(const Protocol::Address &addr, const GroupTable::Response &response);

            /*!
             * Callback for processing the response of a @c GroupTable::REMOVE_ALL_CMD.
             *
             * @param [in] addr        address for device that sent the response.
             * @param [in] response    the response received.
             */
            virtual void removed_all(const Protocol::Address &addr,
                                     const Protocol::Response &response);

            //! @}
            // ======================================================================

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);


            uint16_t payload_size(Protocol::Message::Interface &itf) const;
         };

         // =============================================================================
         // Operators
         // =============================================================================

         inline bool operator==(const Entry &lhs, const Entry &rhs)
         {
            return lhs.group == rhs.group && lhs.unit == rhs.unit;
         }

         inline bool operator!=(const Entry &lhs, const Entry &rhs)
         {
            return !(lhs == rhs);
         }

         /*! @} */

      }  // namespace GroupTable

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup group_table_itf
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
std::ostream &operator<<(std::ostream &stream, const HF::Core::GroupTable::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::GroupTable::Attributes attribute);
/*! @} */

#endif /* HF_CORE_GROUP_TABLE_H */
