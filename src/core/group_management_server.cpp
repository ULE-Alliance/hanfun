// =============================================================================
/*!
 * @file       src/core/group_management_server.cpp
 *
 * This file contains the implementation of the Group Management service : Server role.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/group_management.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupManagement;

// =============================================================================
// Group Management Interface : Server Role
// =============================================================================

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS Server::attributes(uint8_t pack_id) const
{
   HF::Attributes::UIDS result({NUMBER_OF_GROUPS_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {}

   return result;
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *IServer::attribute(uint8_t uid)
{
   using namespace HF::Core::GroupManagement;

   GroupManagement::Attributes attr = static_cast<GroupManagement::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_GROUPS_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (Server::*)(void) const) & Server::number_of_groups;
         auto setter = (void (Server::*)(uint8_t)) & Server::number_of_groups;

         return new Attribute(*this, attr, getter, setter, NumberOfGroups::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case CREATE_CMD:
      {
         create(packet.source);
         break;
      }

      case DELETE_CMD:
      {
         // FIXME Retrieve group identifier from message.
         remove(packet.source, 0x0000);
         break;
      }

      case ADD_CMD:
      {
         add(packet.source);
         break;
      }

      case REMOVE_CMD:
      {
         // FIXME Retrieve group/device/unit identifier from message.
         remove(packet.source, 0x0000, 0x0000, 0x00);
         break;
      }

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
      case GET_INFO_CMD:
      {
         get_info(packet.source);
         break;
      }
#endif

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::create
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::create(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::delete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::remove(const Protocol::Address &addr, uint16_t group)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(group);
}

// =============================================================================
// Server::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::add(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::remove(const Protocol::Address &addr, uint16_t group, uint16_t device, uint8_t unit)
{
   // FIXME Generated Stub.
   UNUSED(addr);

   UNUSED(group);
   UNUSED(device);
   UNUSED(unit);
}

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
// =============================================================================
// Server::get_info
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::get_info(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}
#endif


// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::number_of_groups
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::number_of_groups() const
{
   return _number_of_groups;
}

// =============================================================================
// Server::number_of_groups
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::number_of_groups(uint8_t __value)
{
   HF_SETTER_HELPER(NumberOfGroups, _number_of_groups, __value);
}
