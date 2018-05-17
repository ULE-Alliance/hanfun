// =============================================================================
/*!
 * @file       inc/hanfun/core/suota.h
 *
 * This file contains the definitions for the SUOTA interface.
 *
 * @version    1.5.3
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

#include "hanfun/common.h"

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
          * @addtogroup suota_itf  SUOTA
          * @ingroup core
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

         /*!
          * Result codes for a @c SUOTA::NEW_VERSION_AVAILABLE_CMD
          */
         typedef enum _NewVersionResponse
         {
            UPGRADE_INITIATED    = 0x00,                 //!< Upgrade initiated
            INVALID_SOFTWARE     = 0x11,                 //!< Invalid software
            UNSUPPORTED_HARDWARE = 0x12,                 //!< Unsupported hardware
            BATTERY_TOO_LOW      = 0x13,                 //!< Battery too low
            FAIL_UNKNOWN         = Common::FAIL_UNKNOWN, //!< FAIL_UNKNOWN
         } NewVersionResponse;

         /*!
          * Helper class that represents the information of a particular version.
          */
         struct Version
         {
            std::string sw_version;    //!< Current/Available software version.
            std::string hw_version;    //!< Current/Minimum required hardware version.
            std::string url;           //!< Update URL.

            /*!
             * Constructor.
             *
             * @param [in] _sw_version    software version string.
             * @param [in] _hw_version    hardware version string.
             * @param [in] _url           update URL string.
             */
            Version(const std::string _sw_version = "",
                    const std::string _hw_version = "",
                    const std::string _url = ""):
               sw_version(_sw_version), hw_version(_hw_version), url(_url)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = 2 * sizeof(uint8_t);

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const;

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * This class represents the message for the response of a
          * @c SUOTA::CHECK_VERSION_CMD.
          */
         struct CheckVersionResponse: public Protocol::Response, Version
         {
            /*!
             * Possible result codes for a SUOTA::CHECK_VERSION_CMD
             */
            typedef enum _Status
            {
               VERSION_AVAILABLE    = 0x00,                 //!< New version available.
               NO_VERSION_AVAILABLE = 0x11,                 //!< No newer version available.
               FAIL_INFRASTUCTURE   = 0x12,                 //!< Server or Network are down.
               FAIL_UNKNOWN         = Common::FAIL_UNKNOWN, //!< Fail: Unknown Error
            } Status;

            /*!
             * Constructor
             *
             * @param [in] _status        response status to send  (M)
             * @param [in] _sw_version    software version to send (M)
             * @param [in] _hw_version    hardware version to send (M)
             * @param [in] _url           update URL (O)
             */
            CheckVersionResponse(Status _status = FAIL_UNKNOWN, const std::string _sw_version = "",
                                 const std::string _hw_version = "", const std::string _url = ""):
               Protocol::Response(_status),
               Version(_sw_version, _hw_version, _url) {}

            CheckVersionResponse(Common::Result _status):
               Protocol::Response(_status), Version() {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = Protocol::Response::min_size + Version::min_size;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return Protocol::Response::size() + Version::size();
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint16_t start = offset;

               offset += Protocol::Response::pack(array, offset);
               offset += Version::pack(array, offset);

               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               uint16_t start = offset;

               uint16_t size  = Protocol::Response::unpack(array, offset);
               HF_ASSERT(size != 0, {return 0;});
               offset += size;
               size    = Version::unpack(array, offset);
               HF_ASSERT(size != 0, {return 0;});
               offset += size;

               return offset - start;
            }
         };

         /*!
          * This class contains the required parameters for a
          * @c SUOTA::UPGRADE_COMPLETE_CMD command.
          */
         struct UpgradeStatus
         {
            /*!
             * Possible status in a
             */
            typedef enum _Status
            {
               UPGRADE_SUCCESSFUL  = 0x00,                 //!< Upgrade successful
               UPGRADE_ABORTED     = 0x11,                 //!< Upgrade aborted
               GMEP_SESSION_ERROR  = 0x12,                 //!< GMEP session error
               INVALID_IMAGE       = 0x13,                 //!< Invalid image
               MEMORY_ACCESS_ERROR = 0x14,                 //!< Memory access error
               BATTERY_TOO_LOW     = 0x15,                 //!< Battery too low
               FAIL_UNKNOWN        = Common::FAIL_UNKNOWN, //!< Fail: Unknown Error
            } Code;

            Code        code;       //! Update status
            std::string sw_version; //! Software version after update.

            /*!
             * Constructor.
             *
             * @param [in] _code       update status to send in the command.
             * @param [in] _sw_version new software version of the update.
             */
            UpgradeStatus(Code _code = FAIL_UNKNOWN, std::string _sw_version = ""):
               code(_code), sw_version(_sw_version) {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t);

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return sizeof(uint8_t) +
                      (sw_version.empty() ? 0 :
                       Common::SerializableHelper<std::string>::size(sw_version));
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, size());

               uint16_t start = offset;

               offset += array.write(offset, static_cast<uint8_t>(code));

               if (!sw_version.empty())
               {
                  offset +=
                     Common::SerializableHelper<std::string>::pack(sw_version, array, offset);
               }

               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               uint16_t start = offset;

               uint8_t temp;
               offset += array.read(offset, temp);
               code    = static_cast<Code>(temp);

               offset += Common::SerializableHelper<std::string>::unpack(sw_version, array, offset);

               return offset - start;
            }
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
         struct Base: public Service<HF::Interface::SUOTA>
         {
            protected:

            //! Constructor
            Base(HF::Core::Unit0 &unit0): Service<HF::Interface::SUOTA>(unit0) {}
         };

         /*!
          * SUOTA %Interfaces::Interface : %Server side implementation.
          *
          * This class provides the server side of the SUOTA interface.
          */
         class Server: public ServiceRole<SUOTA::Base, HF::Interface::SERVER_ROLE>
         {
            public:

            //! Constructor
            Server(HF::Core::Unit0 &unit0):
               ServiceRole<SUOTA::Base, HF::Interface::SERVER_ROLE>(unit0) {}

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
             * @param [in] version    the version information to send in the message.
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
             * @warning the default implementation always responds with a
             *          @c CheckVersionResponse::NO_VERSION_AVAILABLE
             *
             * @param [in] addr     the network address received the message from.
             * @param [in] version  the version information for this command.
             *
             * @returns a structure containing the check version response.
             */
            virtual CheckVersionResponse check_version(const Protocol::Address &addr,
                                                       const Version &version);

            /*!
             * Callback that is called when a @c SUOTA::UPGRADE_COMPLETE_CMD,
             * is received.
             *
             * @param [in] addr     the network address received the message from.
             * @param [in] status   the status for the upgrade procedure.
             */
            virtual void upgrade_complete(const Protocol::Address &addr,
                                          const UpgradeStatus &status);

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
         struct Client: public ServiceRole<SUOTA::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(HF::Core::Unit0 &unit0):
               ServiceRole<SUOTA::Base, HF::Interface::CLIENT_ROLE>(unit0) {}

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
            void check_version(const Protocol::Address &addr, const Version &version);

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::CHECK_VERSION_CMD,
             * to the broadcast network address.
             */
            void check_version(const Version &version)
            {
               Protocol::Address addr;
               check_version(addr, version);
            }
#endif

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::UPGRADE_COMPLETE_CMD, to the given
             * network address.
             *
             * @param [in] addr     the network address to send the message to.
             * @param [in] status   upgrade operation status.
             * @param [in] version  version of the new software.
             */
            void upgrade_complete(const Protocol::Address &addr, UpgradeStatus::Code status,
                                  std::string version = "");

            /*!
             * Send a HAN-FUN message containing a @c SUOTA::UPGRADE_COMPLETE_CMD,
             * to the broadcast network address.
             *
             * @param [in] status   upgrade operation status.
             * @param [in] version  version of the new software.
             */
            void upgrade_complete(UpgradeStatus::Code status, std::string version = "")
            {
               Protocol::Address addr;
               upgrade_complete(addr, status, version);
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
             * @param [in] addr       the network address the message was received from.
             * @param [in] version    the version parameters for the command.
             *
             * @returns    the response code for the command. @see NewVersionResponse
             */
            virtual NewVersionResponse new_version_available(const Protocol::Address &addr,
                                                             const Version &version);

            /*!
             * Callback that is called when a response to a @c SUOTA::CHECK_VERSION_CMD,
             * is received.
             *
             * @param [in] addr       the network address the message was received from.
             * @param [in] response   the response received.
             */
            virtual void check_version(const Protocol::Address &addr,
                                       const CheckVersionResponse &response);

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
