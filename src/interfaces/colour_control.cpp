// =============================================================================
/*!
 * @file       src/interfaces/colour_control.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Colour Control interface.
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

#include "hanfun/interfaces/colour_control.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::ColourControl;

// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Interfaces::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return ColourControl::create_attribute(uid);
   }
}

// =============================================================================
// ColourControl::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *ColourControl::create_attribute(uint8_t uid)
{
   using namespace HF::Interfaces::ColourControl;

   ColourControl::Attributes attr = static_cast<ColourControl::Attributes>(uid);

   switch (attr)
   {
      case SUPPORTED_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::COLOUR_CONTROL, attr,
                                                       Supported::WRITABLE);
      }

      case MODE_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::COLOUR_CONTROL, attr,
                                                       Mode::WRITABLE);
      }

      case HUE_AND_SATURATION_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::COLOUR_CONTROL, attr,
                                                        HueAndSaturation::WRITABLE);
      }

      case XY_ATTR:
      {
         return new HF::Attributes::Attribute<uint32_t>(HF::Interface::COLOUR_CONTROL, attr,
                                                        Xy::WRITABLE);
      }

      case COLOUR_TEMPERATURE_ATTR:
      {
         return new HF::Attributes::Attribute<uint16_t>(HF::Interface::COLOUR_CONTROL, attr,
                                                        ColourTemperature::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// ColourControl::XY_Colour
// =============================================================================

// =============================================================================
// XY_Colour::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t XY_Colour::pack (Common::ByteArray& array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += array.write(offset,X);
   offset += array.write(offset,Y);

   return (offset - start);
}

// =============================================================================
// XY_Colour::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t XY_Colour::unpack (const Common::ByteArray& array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = array.read(offset,X);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size = array.read(offset,Y);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}
