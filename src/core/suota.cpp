// =============================================================================
/*!
 * @file       src/core/suota.cpp
 *
 * This file contains the implementation of the common functionality for the
 * SUOTA interface.
 *
 * @version    1.4.2
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
// Defines
// =============================================================================

#if !defined(HF_CORE_DEV_INFO_APP_VERSION_ATTR) || \
   !defined(HF_CORE_DEV_INFO_HW_VERSION_ATTR) ||   \
   !defined(HF_CORE_DEV_INFO_MANUFACTURER_NAME_ATTR)
   #error "In order to use the SUOTA service, the Device Information service MUST                                                 \
   implement the APP_VERSION, HW_VERSION and MANUFACTURER_NAME attributes !                                                       \
   Please define HF_CORE_DEV_INFO_APP_VERSION_ATTR, HF_CORE_DEV_INFO_HW_VERSION_ATTR and HF_CORE_DEV_INFO_MANUFACTURER_NAME_ATTR, \
   in order to add support for these attributes, to the Device Information service.                                               \
   "
#endif

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

   offset += Helper::pack(sw_version, array, offset);
   offset += Helper::pack(hw_version, array, offset);

   if (!url.empty())
   {
      offset += Helper::pack(url, array, offset);
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

   uint16_t size  = Helper::unpack(sw_version, array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */
   offset += size;

   size    = Helper::unpack(hw_version, array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */
   offset += size;

   offset += Helper::unpack(url, array, offset);

   return offset - start;
}
