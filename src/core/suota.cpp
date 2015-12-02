// =============================================================================
/*!
 * @file       src/core/suota.cpp
 *
 * This file contains the implementation of the common functionality for the
 * SUOTA interface.
 *
 * @version    x.x.x
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
      return SUOTA::create_attribute(uid);
   }
}

// =============================================================================
// SUOTA::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SUOTA::create_attribute(uint8_t uid)
{
   UNUSED(uid);
   return nullptr;
}

// =============================================================================
// Helper Classes
// =============================================================================


// =============================================================================
// Version::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Version::size() const
{
   return sizeof(uint8_t) + (!sw_version.empty() ? sw_version.size() * sizeof(uint8_t) : 0) +
          sizeof(uint8_t) + (!hw_version.empty() ? hw_version.size() * sizeof(uint8_t) : 0) +
          (!url.empty() ? sizeof(uint8_t) + url.size() * sizeof(uint8_t) : 0);
}

// =============================================================================
// Version::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Version::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   typedef Common::SerializableHelper<std::string> Helper;
   uint16_t start = offset;

   Helper helper(sw_version);
   offset += helper.pack(array, offset);

   helper  = Helper(hw_version);
   offset += helper.pack(array, offset);

   if (!url.empty())
   {
      helper  = Helper(url);
      offset += helper.pack(array, offset);
   }

   return offset - start;
}

// =============================================================================
// Version::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Version::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   typedef Common::SerializableHelper<std::string> Helper;
   uint16_t start = offset;

   Helper helper(sw_version);
   uint16_t size = helper.unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;
   sw_version = helper.data;

   helper  = Helper(hw_version);
   size    = helper.unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */
   offset += size;
   hw_version = helper.data;

   helper  = Helper(url);
   offset += helper.unpack(array, offset);

   url = helper.data;

   return offset - start;
}
