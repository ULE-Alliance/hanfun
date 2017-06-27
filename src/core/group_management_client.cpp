// =============================================================================
/*!
 * @file       src/core/group_management_client.cpp
 *
 * This file contains the implementation of the Group Management service : Client role.
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
// Group Management Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::create
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::create(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = CREATE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::delete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove(const Protocol::Address &addr, uint16_t group)
{
   // FIXME Generated Stub.
   UNUSED(group);

   /* *INDENT-OFF* */
   HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = DELETE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::add(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = ADD_CMD;

   send(addr, message);
}

// =============================================================================
// Client::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove(const Protocol::Address &addr, uint16_t group, uint16_t device, uint8_t unit)
{
   // FIXME Generated Stub.
   UNUSED(group);
   UNUSED(device);
   UNUSED(unit);

   /* *INDENT-OFF* */
   HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = REMOVE_CMD;

   send(addr, message);
}

#ifdef HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD
// =============================================================================
// Client::get_info
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::get_info(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::GROUP_MANAGEMENT;
   message.itf.member = GET_INFO_CMD;

   send(addr, message);
}
#endif
