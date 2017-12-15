// =============================================================================
/*!
 * @file       src/core/suota_server.cpp
 *
 * This file contains the implementation of the SUOTA interface : Server role.
 *
 * @version    1.5.0
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
// SUOTA Interface : Server Role
// =============================================================================

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   Common::Result result = Common::Result::OK;

   CMD cmd               = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case CHECK_VERSION_CMD /* NEW_VERSION_AVAILABLE_CMD */:
      {
         if (packet.message.isCommandResponse())
         {
            Protocol::Response response;

            if (response.unpack(payload, offset) == 0)
            {
               return Common::Result::FAIL_ARG;
            }

            new_version_available(packet.source, static_cast<NewVersionResponse>(response.code));
         }
         else
         {
            Version version;
            CheckVersionResponse response;

            if (version.unpack(payload, offset) != 0)
            {
               response = check_version(packet.source, version);
            }
            else
            {
               result   = Common::Result::FAIL_ARG;
               response = CheckVersionResponse(result);
            }

            Protocol::Message message(response.size());
            response.pack(message.payload);
            send(packet.source, message);
         }

         break;
      }

      case UPGRADE_COMPLETE_CMD:
      {
         UpgradeStatus status;
         uint16_t size = status.unpack(payload, offset);
         /* *INDENT-OFF* */
         HF_ASSERT(size != 0, { return Common::Result::FAIL_ARG; });
         /* *INDENT-ON* */

         upgrade_complete(packet.source, status);

         break;
      }

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return result;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::new_version_available
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::new_version_available(const Protocol::Address &addr, const Version &version)
{
   Protocol::Message message(version.size());

   version.pack(message.payload);

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = NEW_VERSION_AVAILABLE_CMD;

   send(addr, message);
}

// =============================================================================
// Events
// =============================================================================

// =============================================================================
// Server::new_version_available
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::new_version_available(const Protocol::Address &addr, NewVersionResponse result)
{
   UNUSED(addr);
   UNUSED(result);
}

// =============================================================================
// Server::check_version
// =============================================================================
/*!
 *
 */
// =============================================================================
CheckVersionResponse Server::check_version(const Protocol::Address &addr, const Version &version)
{
   UNUSED(addr);
   UNUSED(version);

   return CheckVersionResponse(CheckVersionResponse::NO_VERSION_AVAILABLE);
}

// =============================================================================
// Server::upgrade_complete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::upgrade_complete(const Protocol::Address &addr, const UpgradeStatus &status)
{
   UNUSED(addr);
   UNUSED(status);
}
