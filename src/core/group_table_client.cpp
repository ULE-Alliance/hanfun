// =============================================================================
/*!
 * @file       src/core/group_table_client.cpp
 *
 * This file contains the implementation of the Group Table service : Client role.
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

#include "hanfun/core/group_table.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupTable;

// =============================================================================
// Group Table Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

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

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
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
void Client::remove(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = REMOVE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::remove_all
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::remove_all(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = REMOVE_ALL_CMD;

   send(addr, message);
}

// =============================================================================
// Client::read_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::read_entries(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::GROUP_TABLE;
   message.itf.member = READ_ENTRIES_CMD;

   send(addr, message);
}
