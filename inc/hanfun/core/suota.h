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

         // =============================================================================
         // Helper Classes
         // =============================================================================

         typedef enum _NewVersionResponse
         {
            UPGRADE_INITIATED    = 0x00, //!< Upgrade initiated
            INVALID_SOFTWARE     = 0x11, //!< Invalid software
            UNSUPPORTED_HARDWARE = 0x12, //!< Unsupported hardware
            BATTERY_TOO_LOW      = 0x13, //!< Battery too low
            FAIL_UNKNOWN         = Common::FAIL_UNKNOWN,
         } NewVersionResponse;

         struct Version
         {
            std::string sw_version;
            std::string hw_version;
            std::string url;

            /*!
             * Constructor.
             *
             * @param [in] _duration   number of miliseconds for the @c ON_CMD
             */
            Version(const std::string _sw_version = "",
                    const std::string _hw_version = "",
                    const std::string _url = ""):
               sw_version(_sw_version), hw_version(_hw_version), url(_url)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = 2 * sizeof(uint8_t);

            //! \see HF::Serializable::size.
            uint16_t size() const;

            //! \see HF::Serializable::pack.
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         // =============================================================================
         // Attributes API
         // =============================================================================

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
            void new_version_available(const Protocol::Address &addr, const Version &version);

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::NEW_VERSION_AVAILABLE_CMD,
             * to the broadcast network address.
             */
            void new_version_available(const Version &version)
            {
               Protocol::Address addr;
               new_version_available(addr, version);
            }

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c SUOTA::NEW_VERSION_AVAILABLE_CMD,
             * response is received.
             *
             * @param [in] addr     the network address received the message from.
             * @param [in] result   the result code for command.
             */
            virtual void new_version_available(const Protocol::Address &addr,
                                               NewVersionResponse result);

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
             * @warning  by default this method simply returns NewVersionResponse::FAIL_UNKNOWN
             *
             * @note Implementing classes should override this method to actually do something.
             *
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] version    the version parameters for the command.
             *
             * @returns    the response code for the command. @see NewVersionResponse
             */
            virtual NewVersionResponse new_version_available(const Protocol::Address &addr,
                                                             const Version &version);

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
