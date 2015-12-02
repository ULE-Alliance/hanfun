// =============================================================================
/*!
 * @file       inc/hanfun/core/suota.h
 *
 * This file contains the definitions for the SUOTA interface.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_CORE_SUOTA_H
#define HF_CORE_SUOTA_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace SUOTA
      {
         class Server;
      }

      /*!
       * @ingroup suota_itf
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
      HF::Attributes::IAttribute *create_attribute(SUOTA::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the SUOTA interface.
       */
      namespace SUOTA
      {
         /*!
          * @addtogroup suota_itf  SUOTA interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the SUOTA interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            NEW_VERSION_AVAILABLE_CMD = 1, //!< New Version Available command UID.
            CHECK_VERSION_CMD         = 1, //!< Check Version command UID.
            UPGRADE_COMPLETE_CMD      = 2, //!< Upgrade Complete command UID.
            __LAST_CMD__              = UPGRADE_COMPLETE_CMD
         } CMD;

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::SUOTA::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::SUOTA::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * SUOTA %Interfaces::Interface : Parent.
          *
          * This is the parent class for the SUOTA interface implementation.
          */
         struct Base: public Interfaces::Interface<HF::Interface::SUOTA>
         {
            protected:

            //! Constructor
            Base(): Interfaces::Interface<HF::Interface::SUOTA>() {}
         };

         /*!
          * SUOTA %Interfaces::Interface : %Server side implementation.
          *
          * This class provides the server side of the SUOTA interface.
          */
         class Server: public Interfaces::InterfaceRole<SUOTA::Base, HF::Interface::SERVER_ROLE>
         {
            public:

            //! Constructor
            Server(): Interfaces::InterfaceRole<SUOTA::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::NEW_VERSION_AVAILABLE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void new_version_available(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::NEW_VERSION_AVAILABLE_CMD,
             * to the broadcast network address.
             */
            void new_version_available()
            {
               Protocol::Address addr;
               new_version_available(addr);
            }

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c SUOTA::CHECK_VERSION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void check_version(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c SUOTA::UPGRADE_COMPLETE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void upgrade_complete(const Protocol::Address &addr);

            //! @}
            // ======================================================================
            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*!
          * SUOTA %Interfaces::Interface : %Client side implementation.
          *
          * This class provides the client side of the SUOTA interface.
          */
         struct Client: public Interfaces::InterfaceRole<SUOTA::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): Interfaces::InterfaceRole<SUOTA::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

#ifdef HF_CORE_SUOTA_CHECK_VERSION_CMD
            /*!
             * Send a HAN-FUN message containing a @c SUOTA::CHECK_VERSION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void check_version(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::CHECK_VERSION_CMD,
             * to the broadcast network address.
             */
            void check_version()
            {
               Protocol::Address addr;
               check_version(addr);
            }
#endif

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::UPGRADE_COMPLETE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void upgrade_complete(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::UPGRADE_COMPLETE_CMD,
             * to the broadcast network address.
             */
            void upgrade_complete()
            {
               Protocol::Address addr;
               upgrade_complete(addr);
            }

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{
            //!

            /*!
             * Callback that is called when a @c SUOTA::NEW_VERSION_AVAILABLE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void new_version_available(const Protocol::Address &addr);

            //! @}
            // =============================================================================

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*! @} */

      }  // namespace SUOTA

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup suota_itf
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
std::ostream &operator<<(std::ostream &stream, const HF::Core::SUOTA::CMD command);

/*! @} */

#endif /* HF_CORE_SUOTA_H */
