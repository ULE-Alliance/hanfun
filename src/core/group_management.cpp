// =============================================================================
/*!
 * @file       src/core/group_management.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Group Management service.
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
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return GroupManagement::create_attribute(uid);
   }
}

// =============================================================================
// GroupManagement::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *GroupManagement::create_attribute(uint8_t uid)
{
   using namespace HF::Core::GroupManagement;

   GroupManagement::Attributes attr = static_cast<GroupManagement::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_GROUPS_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::GROUP_MANAGEMENT, attr,
                                                       NumberOfGroups::WRITABLE);
      }

      default:
         return nullptr;
   }
}
