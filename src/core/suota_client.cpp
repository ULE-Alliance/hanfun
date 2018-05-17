// =============================================================================
/*!
 * @file       src/core/suota_client.cpp
 *
 * This file contains the implementation of the SUOTA interface : Client role.
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

#include "hanfun/core/suota.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::SUOTA;

// =============================================================================
// SUOTA Interface : Client Role
// =============================================================================

// =============================================================================
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   Common::Result result = Common::Result::OK;
   CMD cmd               = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case NEW_VERSION_AVAILABLE_CMD /* CHECK_VERSION_CMD */:
      {
         if (packet.message.isCommand())
         {
            Version version;
            uint16_t size = version.unpack(payload, offset);
            /* *INDENT-OFF* */
            HF_ASSERT(size != 0, {result = Common::Result::FAIL_ARG;});
            /* *INDENT-ON* */

            auto code = new_version_available(packet.source, version);

            Protocol::Response response;
            response.code = static_cast<Common::Result>(code);
            Protocol::Message message(packet.message, response.size());
            response.pack(message.payload);

            send(packet.source, message);
         }
         else
         {
            CheckVersionResponse response;
            uint16_t size = response.unpack(payload, offset);
            /* *INDENT-OFF* */
            HF_ASSERT(size != 0, {return Common::Result::FAIL_ARG;});
            /* *INDENT-ON* */

            check_version(packet.source, response);
         }

         break;
      }

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return result;
}

// =============================================================================
// Events
// =============================================================================

// =============================================================================
// Client::new_version_available
// =============================================================================
/*!
 *
 */
// =============================================================================
NewVersionResponse Client::new_version_available(const Protocol::Address &addr,
                                                 const Version &version)
{
   UNUSED(addr);
   UNUSED(version);

   return NewVersionResponse::FAIL_UNKNOWN;
}

// =============================================================================
// Client::check_version
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::check_version(const Protocol::Address &addr, const CheckVersionResponse &response)
{
   UNUSED(addr);
   UNUSED(response);
}

// =============================================================================
// Commands
// =============================================================================

#ifdef HF_CORE_SUOTA_CHECK_VERSION_CMD
// =============================================================================
// Client::check_version
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::check_version(const Protocol::Address &addr, const Version &version)
{
   Protocol::Message message(version.size());

   uint16_t size = version.pack(message.payload);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return;});
   /* *INDENT-ON* */

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = CHECK_VERSION_CMD;

   send(addr, message);
}
#endif

// =============================================================================
// Client::upgrade_complete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::upgrade_complete(const Protocol::Address &addr, UpgradeStatus::Code status,
                              std::string version)
{
   UpgradeStatus upgrade(status, version);

   Protocol::Message message(upgrade.size());

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = UPGRADE_COMPLETE_CMD;

   upgrade.pack(message.payload);

   send(addr, message);
}
