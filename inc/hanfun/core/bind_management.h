// =============================================================================
/*!
 * \file       inc/hanfun/core/bind_management.h
 *
 * This file contains the definitions for the core Bind Management Interface
 * of the HAN-FUN protocol.
 *
 * \version    0.3.2
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

         /*!
          * Add/Remove Bind message.
          */
         struct Message:public Entry
         {};

         // =============================================================================
         // API
         // =============================================================================

         // Forward declaration
         struct Server;

         /*!
          * This structure defines the API that devices using the BindManagement::Server,
          * must implement to store the BindManagement::Entries.
          */
         struct Entries
         {
            typedef std::set <Entry> Container;

            typedef typename Container::iterator Iterator;

            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Return the number of entries in the container.
             *
             * @return  the number of entries in the container.
             */
            size_t size () const;

            // =============================================================================
            // Query API
            // =============================================================================

            /*!
             * Return a reference of the iterator for the first element in the collection
             * containing the bind management entries in the system.
             *
             * @return a reference of the iterator for the first element.
             */
            Iterator begin () const;

            /*!
             * Return a reference of the iterator for the last element in the collection
             * containing the bind management entries in the system.
             *
             * @return a reference of the iterator for the last element.
             */
            Iterator end () const;

            /*!
             * Returns the start and end iterators for the bind management entries, that
             * exist for the given source address and interface.
             *
             * If no entries exist then the returned iterators are equal to the iterator
             * returned by BindManagement::Entries::end().
             *
             * @param source  the source address for the bind entry.
             * @param itf     the interface the bind entry refers to.
             *
             * @return  a pair containing the start and end iterators for entries requested.
             */
            std::pair <Iterator, Iterator> find (Protocol::Address const &source,
                                                 Common::Interface const &itf) const;

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
            const Entry *find (const Protocol::Address &source, const Common::Interface &itf,
                               const Protocol::Address &destination);

            /*!
             * Destroy the given \c entry in the persistent storage.
             *
             * @param [in] entry   reference to the bind entry to erase.
             *
             * @retval  Common::Result::OK if the entry was destroyed.
             * @retval  Common::Result::FAIL_ARG, otherwise.
             */
            Common::Result destroy (Entry const &entry);

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
             * @retval  Common::Result::OK if at least one entry was destroyed.
             * @retval  Common::Result::FAIL_ARG, otherwise.
             */
            Common::Result destroy (uint16_t address,
                                    Protocol::Address::Type type = Protocol::Address::DEVICE);

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
               Common::Result res = Common::Result::FAIL_ARG;

               for (Iterator itr = db.begin (); itr != db.end ();)
               {
                  if (func (*itr))
                  {
                     this->db.erase (itr++);
                     res = Common::Result::OK;
                  }
                  else
                  {
                     ++itr;
                  }
               }

               return res;
            }

            protected:

            /*!
             * Create a new bind entry in the persistent storage.
             *
             * @param [in] source         source address of the device.
             * @param [in] destination    destination address of the device.
             * @param [in] itf            interface to bind.
             *
             * @return a pair containing the result indicating if the entry was created and
             *         a pointer to the newlly created entry.
             */
            std::pair <Common::Result, const Entry *> create (Protocol::Address const &source,
                                                              Common::Interface const &itf,
                                                              Protocol::Address const &destination);

            friend class HF::Core::BindManagement::Server;

            private:

            Container db;
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
          * Bind Management interface : Server side.
          */
         struct Server:public ServiceRole <Abstract, HF::Interface::SERVER_ROLE>
         {
            //! This field contains the bind entries present in the system.
            Entries entries;

            Server(HF::Devices::Concentrator::IUnit0 &unit);

            virtual ~Server() {}

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
            HF::Devices::Concentrator::IUnit0 &unit0 ();

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
            virtual std::pair <Common::Result, const Entry *> add (const Protocol::Address &source,
                                                                   const Protocol::Address &destination,
                                                                   const Common::Interface &itf);

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
                                           const Common::Interface &itf);

            //! @}
            // ======================================================================

            protected:

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                           size_t offset);
         };

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
