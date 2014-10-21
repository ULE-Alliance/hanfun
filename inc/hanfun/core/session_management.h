// =============================================================================
/*!
 * \file       inc/hanfun/core/session_manager.h
 *
 * This file contains the definitions for the session management functionality.
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_SESSION_MANAGEMENT_H
#define HF_SESSION_MANAGEMENT_H

#include <algorithm>
#include <vector>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

namespace HF
{
   namespace Core
   {
      /*! Top level namespace for a the Session Management functionality. */
      namespace SessionManagement
      {
         typedef enum _CMD
         {
            START,   //!< Start a new session for a device.
            GET,     //!< Retrieve information.
            END,     //!> End the session for device.
         } CMD;

         // =============================================================================
         // Messages API
         // =============================================================================

         /*!
          * Start Read Session Command Message.
          */
         struct StartResponse: public Protocol::Response
         {
            uint16_t count; //!< Number of device entries.

            StartResponse (uint16_t count = 0) :
                  count(count)
            {
            }

            //! \see HF::Protocol::Response::size.
            size_t size () const
            {
               return Protocol::Response::size() + sizeof(count);
            }

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
                         {
               size_t start = offset;

               offset += Protocol::Response::pack(array, offset);
               offset += array.write(offset, count);

               return offset - start;
            }

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += Protocol::Response::unpack(array, offset);
               offset += array.read(offset, count);

               return offset - start;
            }
         };

         /*!
          * Get Entries Command Message.
          */
         struct GetEntriesMessage
         {
            uint16_t offset; //!< Start index for the first entry to be provided.
            uint8_t count;  //!< Number of entries to be sent in the response.

            constexpr static size_t min_size = sizeof(offset) + sizeof(count);

            GetEntriesMessage (uint16_t offset = 0, uint8_t count = 0) :
                  offset(offset), count(count)
            {
            }

            //! \see HF::Serializable::size.
            size_t size () const
            {
               return min_size;
            }

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write(offset, this->offset);
               offset += array.write(offset, this->count);

               return offset - start;
            }

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += array.read(offset, this->offset);
               offset += array.read(offset, this->count);

               return offset - start;
            }
         };

         template<typename _Entry>
         struct GetEntriesResponse: public Protocol::Response
         {
            std::vector<_Entry> entries;

            //! \see HF::Protocol::Response::size.
            size_t size () const
            {
               size_t result = Protocol::Response::size() + sizeof(uint8_t);

               std::for_each(entries.begin(), entries.end(), [&result](const _Entry &entry)
               {
                  result += entry.size();
               });

               return result;
            }

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += Protocol::Response::pack(array, offset);

               uint8_t count = entries.size();
               offset += array.write(offset, count);

               /* *INDENT-OFF* */
               std::for_each(entries.begin(), entries.end(),
                             [&offset, &array](const _Entry &entry)
                             {
                                offset += entry.pack (array, offset);
                             });
               /* *INDENT-ON* */

               return offset - start;
            }

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += Protocol::Response::unpack(array, offset);

               uint8_t count = 0;
               offset += array.read(offset, count);

               entries.clear();

               _Entry entry;
               for (uint8_t i = 0; i < count; i++)
               {
                  offset += entry.unpack(array, offset);
                  entries.push_back(entry);
               }

               return offset - start;
            }
         };

         template<>
         struct GetEntriesResponse<void>: public Protocol::Response
         {
            uint8_t count;

            GetEntriesResponse() : count(0)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const
            {
               size_t result = Protocol::Response::size() + sizeof(uint8_t);
               return result;
            }

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += Protocol::Response::pack(array, offset);

               uint8_t count = 0;
               offset += array.write(offset, count);

               return offset - start;
            }

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += Protocol::Response::unpack(array, offset);

               uint8_t count = 0;
               offset += array.read(offset, count);

               return offset - start;
            }
         };

         typedef GetEntriesResponse<void> GetEntriesEmptyResponse;

         // =============================================================================
         // Server API
         // =============================================================================

         struct IServer
         {
            /*!
             * Start a session for the device with the given \c address.
             *
             * Only one session is associated with any given device at any time.
             *
             * @param [in] address  device address to start the session for.
             */
            virtual void start_session (uint16_t address) = 0;

            /*!
             * Terminate the session associated with the device with the
             * given \c address.
             *
             * @param [in] address  device address to end the session for.
             */
            virtual void end_session (uint16_t address) = 0;

            /*!
             * Check if a session for the device with the given \c address exists.
             *
             * @param [in] address  device address to check is a session exists.
             *
             * @retval  true     if a session exists,
             * @retval  false    otherwise.
             */
            virtual bool exists (uint16_t address) const = 0;

            /*!
             * Check if the session for the device with the given \c address is
             * valid, i.e., the underling entries have not been modified since the
             * start of the session.
             *
             * @param [in] address  device address to check is the session is valid.
             *
             * @retval  true     if the session exists and is valid,
             * @retval  false    otherwise.
             */
            virtual bool is_valid (uint16_t address) const = 0;
         };

         /*!
          * Top-level parent for session management functionality.
          */
         class AbstractServer:public IServer
         {
            protected:

            struct Session
            {
               uint16_t address;
               bool valid;

               Session (uint16_t _address = HF::Protocol::BROADCAST_ADDR, bool _valid = false):
                  address(_address), valid(_valid)
               {}
            };

            typedef std::vector<Session>        Container;
            typedef Container::iterator         iterator;
            typedef Container::const_iterator   const_iterator;

            //! Session's database.
            Container sessions;

            // =============================================================================
            // API
            // =============================================================================

            public:

            AbstractServer():sessions(Container())
            {}

            virtual ~AbstractServer()
            {}

            void start_session (uint16_t address)
            {
               auto it = find (address);

               if (it == sessions.end())  // Session does not exist.
               {
                  sessions.push_back(Session(address, true));
               }
               else  // Session already exists. Update state.
               {
                  it->valid = true;
               }
            }

            void end_session (uint16_t address)
            {
               auto it = find(address);
               if (it != sessions.end())
               {
                  sessions.erase(it);
               }
            }

            bool exists (uint16_t address) const
            {
               return find(address) != sessions.end();
            }

            bool is_valid (uint16_t address) const
            {
               auto it = find(address);
               if (it != sessions.end())
               {
                  return it->valid;
               }
               return false;
            }

            void invalidate()
            {
               std::for_each(sessions.begin(), sessions.end(), [](Session &session){
                  session.valid = false;
               });
            }

            protected:

            Common::Result handle_command (CMD cmd, Protocol::Packet &packet, Common::ByteArray &payload,
                                            size_t offset = 0);

            iterator find(uint16_t address)
            {
               return std::find_if (sessions.begin (), sessions.end (),
                                     [address](const Session &session)
               {
                  return address == session.address;
               });
            }

            const_iterator find(uint16_t address) const
            {
               return std::find_if (sessions.cbegin (), sessions.cend (),
                                     [address](const Session &session)
               {
                  return address == session.address;
               });
            }

            size_t payload_size (CMD cmd) const;

            Common::Result check_offset (uint16_t offset, uint8_t &count, uint16_t size) const;

            virtual void send (const Protocol::Address &addr, Protocol::Message &message) = 0;

            virtual uint16_t entries_size() const = 0;

            virtual Common::Result entries(uint16_t offset, uint8_t count,
                                             Common::ByteArray &payload) = 0;

            Common::Result check_session (uint16_t address, Common::ByteArray &payload) const;
         };

         /*!
          * Wrapper over persistent storage APIs to invalidate sessions on
          * save and destroy.
          */
         template<typename Parent>
         class Entries:public Parent
         {
            AbstractServer &manager;

            public:

            typedef typename Parent::value_type  value_type;

            Entries(AbstractServer &_manager):Parent(),
                  manager(_manager)
            {}

            Entries(const Entries &other, AbstractServer &_manager):
               Parent(other), manager(_manager)
            {}

            virtual ~Entries()
            {}

            Common::Result save (const value_type &value)
            {
               auto res = Parent::save(value);
               manager.invalidate();
               return res;
            }

            Common::Result destroy (const value_type &value)
            {
               auto res = Parent::destroy(value);
               manager.invalidate();
               return res;
            }
         };

         /*!
          * Helper template to inject session management functionality into
          * services requiring it.
          */
         template<typename _Entries>
         struct Server:public AbstractServer
         {
            typedef Entries<_Entries> Container;

            Container &entries () const
            {
               return const_cast<Container &>(_entries);
            }

            protected:

            Server():AbstractServer(), _entries(*this)
            {}

            virtual ~Server()
            {}

            Server(const Server &other):
               AbstractServer(other), _entries(other._entries, *this)
            {}

            // =============================================================================
            // API
            // =============================================================================
            uint16_t entries_size() const
            {
               return static_cast<uint16_t>(_entries.size());
            }

            typedef typename _Entries::value_type value_type;

            Common::Result entries (uint16_t offset, uint8_t count,
                                     Common::ByteArray &payload)
            {
               GetEntriesResponse<value_type> response;

               response.code = check_offset (offset, count, entries_size());
               if (response.code == Common::Result::OK)
               {
                  auto start = _entries.begin();
                  std::advance(start, offset);

                  auto end = start;
                  std::advance(end, count);
                  std::copy (start, end, std::back_inserter(response.entries));
               }

               payload = Common::ByteArray(response.size());

               response.pack (payload);

               return response.code;
            }

            private:

            Container _entries;
         };

      }  // namespace SessionManagement

   }  // namespace Core

}  // namespace HF

#endif /* HF_SESSION_MANAGEMENT_H */
