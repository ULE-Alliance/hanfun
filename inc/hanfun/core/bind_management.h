// =============================================================================
/*!
 * \file       inc/hanfun/core/bind_management.h
 *
 * This file contains the definitions for the core Bind Management Interface
 * of the HAN-FUN protocol.
 *
 * \version    1.0.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_BIND_MANAGEMENT_H
#define HF_BIND_MANAGEMENT_H

#include <set>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

#include "hanfun/core.h"

#include "hanfun/interface.h"

#include "hanfun/core/session_management.h"

// =============================================================================
// API
// =============================================================================

namespace HF
{
   // =============================================================================
   // Forward declarations.
   // =============================================================================

   namespace Devices
   {
      namespace Concentrator
      {
         struct IUnit0;

      }  // namespace Concentrator

   }  // namespace Devices

   // =============================================================================
   // API
   // =============================================================================

   namespace Core
   {
      /*!
       * This namespace contains the classes that implements
       * HAN-FUN's Core interface - Bind Management.
       */
      namespace BindManagement
      {
         //! Commands.
         typedef enum
         {
            ADD_BIND_CMD      = 0x01, //!< Add bind entry command.
            REMOVE_BIND_CMD   = 0x02, //!< Remove bind entry command.
            START_SESSION_CMD = 0x03, //!< TODO Start Session Read Registration Info.
            END_SESSION_CMD   = 0x04, //!< TODO End Session Read Registration Info.
            GET_ENTRIES_CMD   = 0x05, //!< TODO Get Entries Command.
         } CMD;

         //! Attributes.
         typedef enum
         {
            NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Number of entries attribute.
         } Attributes;

         /*!
          * This represents a bind entry data structure.
          */
         struct Entry
         {
            Protocol::Address source;           //!< Source Address.

            Protocol::Address destination;      //!< Destination Address.

            Common::Interface itf;              //!< Destination Interface.

            Entry()
            {}

            Entry(Protocol::Address _source, Common::Interface _itf):
               source (_source), itf (_itf)
            {}

            Entry(Protocol::Address _source, Common::Interface _itf, Protocol::Address _destination):
               source (_source), destination (_destination), itf (_itf)
            {}

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         typedef HF::Common::Pointer <const Entry> EntryPtr;

         /*!
          * Add/Remove Bind message.
          */
         struct Message:public Entry
         {};

         // =============================================================================
         // API
         // =============================================================================

         /*!
          * Bind Management persistent storage API.
          */
         struct IEntries:public Common::IEntries <Entry>
         {
            // =============================================================================
            // API
            // =============================================================================

            using Common::IEntries <Entry>::destroy;

            /*!
             * Destroy the entries in the persistent storage, that refer to the
             * the given address.
             *
             * This method destroys the entries that refer to the address both in
             * the source and in the destination address.
             *
             * @param [in] address   HF address to remove entries for.
             * @param [in] type      HF address type.
             *
             * @retval  Common::Result::OK and a copy of the destroyed entry if the entry was destroyed.
             * @retval  Common::Result::FAIL_ARG and \c nullptr otherwise.
             */
            virtual Common::Result destroy (uint16_t address,
                                            Protocol::Address::Type type = Protocol::Address::DEVICE) = 0;

            // =============================================================================
            // Query API
            // =============================================================================

            /*!
             * Return the Bind entry for the given parameters.
             *
             * @param [in] source         source address of the device.
             * @param [in] destination    destination address of the device.
             * @param [in] itf            interface to bind.
             *
             * @retval a pointer to bind entry for the given parameters,
             * @retval nullptr if bind entry does not exist.
             */
            virtual EntryPtr find (const Protocol::Address &source, const Common::Interface &itf,
                                   const Protocol::Address &destination) const = 0;

            /*!
             * Check if entries for the device with given \c source address and for the given
             * interface \c ifc exist.
             *
             * @param [in] source   device address to search for.
             * @param [in] itf      interface id to search for.
             *
             * @retval  true if entries exist,
             * @retval  false otherwise.
             */
            virtual bool any_of (Protocol::Address const &source,
                                 Common::Interface const &itf) const = 0;

            /*!
             * Call the given function for all the entries with given \c source address and
             * for the given interface \c ifc.
             *
             * @param [in] source   device address to search for.
             * @param [in] itf      interface id to search for.
             * @param [in] func     function to call for all the entries.
             */
            virtual void for_each (Protocol::Address const &source, Common::Interface const &itf,
                                   std::function <void(const Entry &)> func) const = 0;
         };

         /*!
          * Parent class for the Bind Management interface implementation.
          */
         struct Abstract:public Service <HF::Interface::BIND_MANAGEMENT>
         {
            Abstract(HF::Core::Unit0 &__unit):
               Service (__unit)
            {}
         };

         /*!
          * Bind Management interface : Client side.
          */
         struct Client:public ServiceRole <Abstract, HF::Interface::CLIENT_ROLE>
         {
            Client(HF::Core::Unit0 &unit):
               ServiceRole (unit)
            {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! \name Commands
            //! @{

            /*!
             * Send a ADD_BIND command.
             *
             * @param [in] source         the HAN-FUN source device.
             * @param [in] destination    the HAN-FUN destination device.
             * @param [in] itf            the interface to bind between source and destination.
             */
            void add (const Protocol::Address &source, const Protocol::Address &destination,
                      const Common::Interface &itf);

            /*!
             * Send a REMOVE_BIND command.
             *
             * @param [in] source         the HAN-FUN source device.
             * @param [in] destination    the HAN-FUN destination device.
             * @param [in] itf            the interface to unbind between source and destination.
             */
            void remove (const Protocol::Address &source, const Protocol::Address &destination,
                         const Common::Interface &itf);

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! \name Events
            //! @{

            /*!
             * This method is called when the response to a command is received.
             *
             * @param [in] cmd      the command that response refers to.
             * @param [in] response the result of the command.
             */
            virtual void response (const CMD cmd, const Protocol::Response &response);

            //! @}
            // ======================================================================

            protected:

            //!< \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                           size_t offset);
         };

         /*!
          * Bind Management interface : Server side API.
          */
         struct IServer:public ServiceRole <Abstract, HF::Interface::SERVER_ROLE>
         {
            virtual ~IServer() {}

            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Return a reference to the unit that this service belongs to.
             *
             * This is the same reference as AbstractService::unit, but static casted
             * to allow access to the the other interfaces.
             *
             * @return  a reference to the unit that holds this interface.
             */
            HF::Devices::Concentrator::IUnit0 &unit0 () const;

            // ======================================================================
            // Commands
            // ======================================================================
            //! \name Commands
            //! @{

            /*!
             * Create a new bind entry in the database.
             *
             * @param [in] source         the HAN-FUN source device.
             * @param [in] destination    the HAN-FUN destination device.
             * @param [in] itf            the interface to bind between source and destination.
             *
             * @return  Common::Result::OK if the bind was created or Result::FAIL_* otherwise.
             */
            virtual Common::Result add (const Protocol::Address &source,
                                        const Protocol::Address &destination,
                                        const Common::Interface &itf) = 0;

            /*!
             * Remove a bind entry from the database.
             *
             * @param [in] source         the HAN-FUN source device.
             * @param [in] destination    the HAN-FUN destination device.
             * @param [in] itf            the interface to bind between source and destination.
             *
             * @return  Common::Result::OK if the bind was removed or Result::FAIL_* otherwise.
             */
            virtual Common::Result remove (const Protocol::Address &source,
                                           const Protocol::Address &destination,
                                           const Common::Interface &itf) = 0;

            //! @}
            // ======================================================================

            /*!
             * Reference to the persistent storage implementation.
             *
             * @return  reference to the object responsible by the persistent storage of the
             *          bind entries.
             */
            virtual IEntries &entries () const = 0;

            /*!
             * Reference to the session management API.
             *
             * @return  reference to the object implementing the session management API for
             *          this bind management server.
             */
            virtual SessionManagement::IServer &sessions() = 0;

            protected:

            IServer(Unit0 &unit):
               ServiceRole <Abstract, HF::Interface::SERVER_ROLE>(unit)
            {}
         };

         struct AbstractServer:public IServer
         {
            virtual ~AbstractServer() {}

            // =============================================================================
            // API.
            // =============================================================================

            //! \see IServer::add
            Common::Result add (const Protocol::Address &source,
                                const Protocol::Address &destination,
                                const Common::Interface &itf);

            //! \see IServer::remove
            Common::Result remove (const Protocol::Address &source,
                                   const Protocol::Address &destination,
                                   const Common::Interface &itf);

            protected:

            AbstractServer(Unit0 &unit):IServer (unit)
            {}

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                           size_t offset);
         };

         struct Entries:public IEntries
         {
            typedef std::set <Entry> Container;
            typedef Container::iterator iterator;
            typedef Container::const_iterator const_iterator;
            typedef Container::value_type value_type;

            size_t size () const;

            Common::Result save (const Entry &entry);

            /*!
             * \see IEntries::destroy
             *
             * \warning the reference passed into this method SHOULD NOT be considered
             *          valid if it was obtained by calling the find method.
             */
            Common::Result destroy (const Entry &entry);

            Common::Result destroy (uint16_t address,
                                    Protocol::Address::Type type = Protocol::Address::DEVICE);

            // =============================================================================
            // Query API
            // =============================================================================

            EntryPtr find (const Protocol::Address &source, const Common::Interface &itf,
                           const Protocol::Address &destination) const;

            bool any_of (Protocol::Address const &source, Common::Interface const &itf) const;

            void for_each (Protocol::Address const &source, Common::Interface const &itf,
                           std::function <void(const Entry &)> func) const;

            iterator begin ()
            {
               return db.begin ();
            }

            iterator end ()
            {
               return db.end ();
            }

            const_iterator begin () const
            {
               return db.begin ();
            }

            iterator end () const
            {
               return db.end ();
            }

            /*!
             * Return a pair of iterators for the entries that match the given search
             * criteria.
             *
             * @param [in] source   device address to search for.
             * @param [in] itf      interface id to search for.
             *
             * @return  iterators to the entries found.
             */
            std::pair <iterator, iterator> find (Protocol::Address const &source,
                                                 Common::Interface const &itf) const;

            /*!
             * Destroy the entries in the persistent storage, for which the given
             * predicate function returns \c true.
             *
             * @param [in] func  predicate to call for each entry to check if it
             *                   should be deleted.
             *
             * @retval  Common::Result::OK if at least one entry was destroyed.
             * @retval  Common::Result::FAIL_ARG, otherwise.
             */
            template<typename Func>
            Common::Result destroy (Func func)
            {
               Common::Result result = Common::Result::FAIL_ARG;

               for (iterator itr = db.begin (); itr != db.end ();)
               {
                  if (func (*itr))
                  {
                     auto old = itr++;
                     destroy (*old);
                     result = Common::Result::OK;
                  }
                  else
                  {
                     ++itr;
                  }
               }

               return result;
            }

            protected:

            Container db;
         };

         /*!
          *
          */
         template<typename _Entries = Entries>
         struct Server:public AbstractServer, public SessionManagement::Server <_Entries>
         {
            typedef SessionManagement::Server <_Entries> SessionMgr;
            typedef typename SessionMgr::Container Container;

            Server(Unit0 &unit):AbstractServer (unit) //, SessionMgr()
            {}

            virtual ~Server()
            {}

            Container &entries () const
            {
               return SessionMgr::entries ();
            }

            SessionMgr &sessions ()
            {
               return *this;
            }

            using AbstractServer::send;

            void send (const Protocol::Address &addr, Protocol::Message &message)
            {
               AbstractServer::send (addr, message);
            }

            protected:

            _Entries _entries;

            //! \see AbstractServer::payload_size
            size_t payload_size (Protocol::Message::Interface &itf) const
            {
               switch (itf.member)
               {
                  case START_SESSION_CMD:
                     return SessionMgr::payload_size (SessionManagement::START);

                  case GET_ENTRIES_CMD:
                     return SessionMgr::payload_size (SessionManagement::GET);

                  case END_SESSION_CMD:
                     return SessionMgr::payload_size (SessionManagement::END);

                  default:
                     return AbstractService::payload_size (itf);
               }
            }

            //! \see AbstractServer::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
            {
               switch (packet.message.itf.member)
               {
                  case START_SESSION_CMD:
                     return SessionMgr::handle_command (SessionManagement::START, packet, payload,
                                                        offset);

                  case GET_ENTRIES_CMD:
                     return SessionMgr::handle_command (SessionManagement::GET, packet, payload,
                                                        offset);

                  case END_SESSION_CMD:
                     return SessionMgr::handle_command (SessionManagement::END, packet, payload,
                                                        offset);

                  default:
                     return AbstractServer::handle_command (packet, payload, offset);
               }
            }

            using SessionMgr::entries;
         };

         typedef Server <> DefaultServer;

         // =============================================================================
         // Operators
         // =============================================================================

         inline bool operator ==(const Entry &lhs, const Entry &rhs)
         {
            return lhs.source == rhs.source && lhs.destination == rhs.destination &&
                   lhs.itf == rhs.itf;
         }

         inline bool operator !=(const Entry &lhs, const Entry &rhs)
         {
            return !(lhs == rhs);
         }

         inline bool operator <(const Entry &lhs, const Entry &rhs)
         {
            return lhs.source < rhs.source || (lhs.source == rhs.source
                                               && (lhs.itf < rhs.itf || (lhs.itf == rhs.itf && lhs.destination < rhs.destination)));
         }

      }  // namespace BindManagement

   }  // namespace Core

}  // namespace HF

#endif /* HF_BIND_MANAGEMENT_H */
