// =============================================================================
/*!
 * \file       src/core/bind_management_server.cpp
 *
 * This file contains the implementation of the Bind Management : Server Role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
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

   return res;
}


// =============================================================================
// BindManagement::Server::add
// =============================================================================
/*!
 *
 */
// =============================================================================
pair <Common::Result, const Entry *> Server::add (const Protocol::Address &source,
                                                  const Protocol::Address &destination,
                                                  const Common::Interface &itf)
{
   const Entry *entry = this->entries.find (source, itf, destination);

   // If the entry already exists, do nothing.
   if (entry != nullptr)
   {
      return make_pair (Common::Result::OK, entry);
   }

   // TODO Add support for group binding.
   if (destination.mod == HF::Protocol::Address::GROUP)
   {
      return make_pair (Common::Result::FAIL_SUPPORT, nullptr);
   }

   // Get device entries from device management.
   DeviceManagement::Device *src_dev = unit0 ().device_management ()->entry (source.device);

   if (src_dev == nullptr)
   {
      return make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   DeviceManagement::Device *dst_dev = unit0 ().device_management ()->entry (destination.device);

   if (dst_dev == nullptr)
   {
      return make_pair (Common::Result::FAIL_ARG, nullptr);
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
      return make_pair (Common::Result::FAIL_ARG, nullptr);
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
      return make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   HF::Interface::Role role = static_cast <HF::Interface::Role>(itf.role);

   // Check if destination unit has requested interface.
   if (!dst_unit_it->has_interface (itf.id, role))
   {
      return make_pair (Common::Result::FAIL_ARG, nullptr);
   }

   // Check if source has complementary interface.

   role = (role == Interface::CLIENT_ROLE ? Interface::SERVER_ROLE : Interface::CLIENT_ROLE);

   if (!src_unit_it->has_interface (itf.id, role))
   {
      return make_pair (Common::Result::FAIL_ARG, nullptr);
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