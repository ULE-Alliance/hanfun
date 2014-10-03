// =============================================================================
/*!
 * \file       src/core/bind_management_server.cpp
 *
 * This file contains the implementation of the Bind Management : Server Role.
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

#include "hanfun/core/bind_management.h"
#include "hanfun/core/device_management.h"
#include "hanfun/devices.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::BindManagement;


// =============================================================================
// BindManagement::Server
// =============================================================================

Server::Server(HF::Devices::Concentrator::IUnit0 &unit):
   ServiceRole (unit)
{}

// =============================================================================
// BindManagement::Server::unit
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Devices::Concentrator::IUnit0 &Server::unit0 ()
{
   return static_cast <HF::Devices::Concentrator::IUnit0 &>(ServiceRole::unit ());
}

// =============================================================================
// BindManagement::Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                       size_t offset)
{
   Common::Result res      = Common::Result::FAIL_UNKNOWN;

   BindManagement::CMD cmd = static_cast <BindManagement::CMD>(packet.message.itf.member);

   Entry entry;

   switch (cmd)
   {
      case ADD_BIND_CMD:
      {
         entry.unpack (payload, offset);

         res = this->add (entry.source, entry.destination, entry.itf).first;

         break;
      }
      case REMOVE_BIND_CMD:
      {
         entry.unpack (payload, offset);

         res = this->remove (entry.source, entry.destination, entry.itf);

         break;
      }
      default:
         break;
   }

   Protocol::Response resp (res);

   Protocol::Message  response (packet.message, resp.size ());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = BindManagement::Server::uid ();
   response.itf.member = cmd;

   resp.pack (response.payload);

   send (packet.source, response);

   return res;
}

// =============================================================================
// BindManagement::Server::add
// =============================================================================
/*!
 *
 */
// =============================================================================
std::pair <Common::Result, const Entry *> Server::add (const Protocol::Address &source,
                                                       const Protocol::Address &destination,
                                                       const Common::Interface &itf)
{
   const Entry *entry = this->entries.find (source, itf, destination);

   // If the entry already exists, do nothing.
   if (entry != nullptr)
   {
      return std::make_pair (Common::Result::OK, entry);
   }

   // TODO Add support for group binding.
   if (destination.mod == HF::Protocol::Address::GROUP)
   {
      return std::make_pair (Common::Result::FAIL_SUPPORT, nullptr);
   }

   // Get device entries from device management.
   HF::Interface::Role role = static_cast <HF::Interface::Role>(itf.role);

   auto dst_dev             = unit0 ().device_management ()->entry (destination.device);

   if (dst_dev == nullptr)
   {
      return std::make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   // Check if the destination unit exist.
   /* *INDENT-OFF* */
   auto dst_unit_it = std::find_if(dst_dev->units.begin(), dst_dev->units.end(),
                                   [&destination](DeviceManagement::Unit &unit)
                                   {
                                      return unit.id == destination.unit;
                                   });
   /* *INDENT-ON* */

   if (dst_unit_it == dst_dev->units.end ())
   {
      return std::make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   // Check if destination unit has requested interface.
   if (!dst_unit_it->has_interface (itf.id, role))
   {
      return std::make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   // Skip source validation if catch all rule.
   if (!(source.device == HF::Protocol::BROADCAST_ADDR && source.unit == HF::Protocol::BROADCAST_UNIT))
   {
      auto src_dev = unit0 ().device_management ()->entry (source.device);

      if (src_dev == nullptr)
      {
         return std::make_pair (Common::Result::FAIL_ARG, nullptr);
      }

      // Check if the source unit exist.
      /* *INDENT-OFF* */
      auto src_unit_it = std::find_if(src_dev->units.begin(), src_dev->units.end(),
                                      [&source](DeviceManagement::Unit &unit)
      {
         return unit.id == source.unit;
      });
      /* *INDENT-ON* */

      if (src_unit_it == src_dev->units.end ())
      {
         return std::make_pair (Common::Result::FAIL_ARG, nullptr);
      }

      // Check if source has complementary interface.

      role = (role == Interface::CLIENT_ROLE ? Interface::SERVER_ROLE : Interface::CLIENT_ROLE);

      if (!src_unit_it->has_interface (itf.id, role))
      {
         return std::make_pair (Common::Result::FAIL_ARG, nullptr);
      }
   }

   return this->entries.create (source, itf, destination);
}

// =============================================================================
// BindManagement::Server::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::remove (const Protocol::Address &source,
                               const Protocol::Address &destination,
                               const Common::Interface &itf)
{
   const Entry *entry = this->entries.find (source, itf, destination);

   // If the entry does not exists, fail.
   if (entry == nullptr)
   {
      return Common::Result::FAIL_ARG;
   }

   return this->entries.destroy (*entry);
}
