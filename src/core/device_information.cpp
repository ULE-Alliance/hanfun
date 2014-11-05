// =============================================================================
/*!
 * \file       src/core/device_information_server.cpp
 *
 * This file contains the implementation of the Device Information : Server Role.
 *
 * \version    1.1.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/device_information.h"

using namespace HF;
using namespace HF::Core;

// =============================================================================
// Defines
// =============================================================================

// =============================================================================
// API
// =============================================================================

// =============================================================================
// DeviceInformation::mandatory
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *DeviceInformation::mandatory ()
{
   Protocol::Message *result = new Protocol::Message ();

   result->type       = Protocol::Message::GET_ATTR_PACK_REQ;
   result->itf.id     = HF::Interface::DEVICE_INFORMATION;
   result->itf.role   = HF::Interface::SERVER_ROLE;
   result->itf.member = HF::Attributes::MANDATORY;

   return result;
}

// =============================================================================
// DeviceInformation::all
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *DeviceInformation::all ()
{
   Protocol::Message *result = new Protocol::Message ();


   result->type       = Protocol::Message::GET_ATTR_PACK_REQ;
   result->itf.id     = HF::Interface::DEVICE_INFORMATION;
   result->itf.role   = HF::Interface::SERVER_ROLE;
   result->itf.member = HF::Attributes::ALL;

   return result;
}

// =============================================================================
// DeviceInformation::get
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *DeviceInformation::get (HF::Attributes::UIDS &uids)
{
   HF::Protocol::GetAttributePack::Request *req = new HF::Protocol::GetAttributePack::Request (uids);

   Protocol::Message *result                    = new Protocol::Message ();

   result->type       = Protocol::Message::GET_ATTR_REQ;
   result->itf.id     = HF::Interface::DEVICE_INFORMATION;
   result->itf.role   = HF::Interface::SERVER_ROLE;
   result->itf.member = HF::Attributes::DYNAMIC;

   req->pack (result->payload);

   delete req;

   return result;
}

// =============================================================================
// DeviceInformation::get
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *DeviceInformation::get (uint8_t uid)
{
   Protocol::Message *result = new Protocol::Message ();

   result->type       = Protocol::Message::GET_ATTR_REQ;
   result->itf.id     = HF::Interface::DEVICE_INFORMATION;
   result->itf.role   = HF::Interface::SERVER_ROLE;
   result->itf.member = uid;

   return result;
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS DeviceInformation::Server::attributes (uint8_t pack_id) const
{
   HF::Attributes::UIDS result;

   switch (pack_id)
   {
      case HF::Attributes::Pack::ALL:
      {
         result.push_back (ENABLED_ATTR);
         result.push_back (MANUFACTURE_NAME_ATTR);
         result.push_back (EMC_ATTR);
      }
      case HF::Attributes::Pack::MANDATORY:
      {
         result.push_back (UID_ATTR);
         result.push_back (INTERFACE_VERSION_ATTR);
         result.push_back (PROFILE_VERSION_ATTR);
         result.push_back (CORE_VERSION_ATTR);
         break;
      }
      default:
         break;
   }

   return result;
}

// =============================================================================
// DeviceInformation::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *DeviceInformation::create_attribute (uint8_t uid)
{
   return Core::create_attribute ((DeviceInformation::Server *) nullptr, uid);
}
