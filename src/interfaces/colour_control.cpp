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
HF::Attributes::IAttribute *Interfaces::create_attribute(IServer *server, uint8_t uid)
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
// ColourControl::HS_Colour
// =============================================================================

// =============================================================================
// HS_Colour::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t HS_Colour::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   HF_ASSERT(hue <= HUE_MAX, {return 0;});

   offset += array.write(offset, hue);
   offset += array.write(offset, saturation);

   return (offset - start);
}

// =============================================================================
// HS_Colour::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t HS_Colour::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size    = array.read(offset, hue);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   if (hue > HUE_MAX)
   {
      hue = HUE_MAX;
   }

   size    = array.read(offset, saturation);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
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
uint16_t XY_Colour::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += array.write(offset, X);
   offset += array.write(offset, Y);

   return (offset - start);
}

// =============================================================================
// XY_Colour::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t XY_Colour::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size    = array.read(offset, X);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, Y);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveToHueMessage
// =============================================================================

// =============================================================================
// MoveToHueMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToHueMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   HF_ASSERT(hue <= HUE_MAX, {return 0;});

   offset += array.write(offset, hue);
   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// MoveToHueMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToHueMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size    = array.read(offset, hue);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   if (hue > HUE_MAX)
   {
      hue = HUE_MAX;
   }

   size    = array.read(offset, direction);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveHueMessage
// =============================================================================

// =============================================================================
// MoveHueMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveHueMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(rate <= RATE_MAX, {return 0;});
   HF_ASSERT(direction <= DIRECTION_MAX, {return 0;});

   uint16_t start = offset;

   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, rate);


   return (offset - start);
}


// =============================================================================
// MoveHueMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveHueMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;
   uint8_t temp_dir;

   size = array.read(offset, temp_dir);
   HF_ASSERT(size != 0, {return 0;});
   HF_ASSERT(temp_dir <= DIRECTION_MAX, {return 0;});
   direction = static_cast<Direction>(temp_dir);
   offset   += size;

   size      = array.read(offset, rate);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   if (rate > RATE_MAX)
   {
      rate = RATE_MAX;
   }

   return (offset - start);
}

// =============================================================================
// ColourControl::StepHueMessage
// =============================================================================

// =============================================================================
// StepHueMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepHueMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(direction <= DIRECTION_MAX, {return 0;});

   uint16_t start = offset;

   offset += array.write(offset, step_size);
   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// StepHueMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepHueMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;
   uint8_t temp_dir;

   size = array.read(offset, step_size);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, temp_dir);
   HF_ASSERT(size != 0, {return 0;});
   HF_ASSERT(temp_dir <= DIRECTION_MAX, {return 0;});
   direction = static_cast<Direction>(temp_dir);
   offset   += size;

   size      = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveToSaturationMessage
// =============================================================================

// =============================================================================
// MoveToSaturationMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToSaturationMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(direction <= DIRECTION_MAX, {return 0;});

   uint16_t start = offset;

   offset += array.write(offset, saturation);
   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// MoveToSaturationMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToSaturationMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;
   uint8_t temp_dir;

   size = array.read(offset, saturation);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, temp_dir);
   HF_ASSERT(size != 0, {return 0;});
   HF_ASSERT(temp_dir <= DIRECTION_MAX, {return 0;});
   direction = static_cast<Direction>(temp_dir);
   offset   += size;

   size      = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveSaturationMessage
// =============================================================================

// =============================================================================
// MoveSaturationMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveSaturationMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(direction <= DIRECTION_MAX, {return 0;});

   uint16_t start = offset;

   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, rate);

   return (offset - start);
}

// =============================================================================
// MoveSaturationMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveSaturationMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;
   uint8_t temp_dir;

   size = array.read(offset, temp_dir);
   HF_ASSERT(size != 0, {return 0;});
   HF_ASSERT(temp_dir <= DIRECTION_MAX, {return 0;});
   direction = static_cast<Direction>(temp_dir);
   offset   += size;

   size      = array.read(offset, rate);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::StepSaturationMessage
// =============================================================================

// =============================================================================
// StepSaturationMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepSaturationMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(direction <= DIRECTION_MAX, {return 0;});

   uint16_t start = offset;

   offset += array.write(offset, step_size);
   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// StepSaturationMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepSaturationMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;
   uint8_t temp_dir;

   size = array.read(offset, step_size);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, temp_dir);
   HF_ASSERT(size != 0, {return 0;});
   HF_ASSERT(temp_dir <= DIRECTION_MAX, {return 0;});
   direction = static_cast<Direction>(temp_dir);
   offset   += size;

   size      = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveToHueSaturationMessage
// =============================================================================

// =============================================================================
// MoveToHueSaturationMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToHueSaturationMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = colour.pack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   offset += array.write(offset, static_cast<uint8_t>(direction));
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// MoveToHueSaturationMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToHueSaturationMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = colour.unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, direction);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveToXYMessage
// =============================================================================

// =============================================================================
// MoveToXYMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToXYMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += this->colour.pack(array, offset);
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// MoveToXYMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToXYMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = this->colour.unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveXYMessage
// =============================================================================

// =============================================================================
// MoveXYMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveXYMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += array.write(offset, X_rate);
   offset += array.write(offset, Y_rate);

   return (offset - start);
}

// =============================================================================
// MoveXYMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveXYMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = array.read(offset, X_rate);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, Y_rate);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::StepXYMessage
// =============================================================================

// =============================================================================
// StepXYMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepXYMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += array.write(offset, X_step);
   offset += array.write(offset, Y_step);
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// StepXYMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t StepXYMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = array.read(offset, X_step);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, Y_step);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}

// =============================================================================
// ColourControl::MoveToTemperatureMessage
// =============================================================================

// =============================================================================
// MoveToTemperatureMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToTemperatureMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += array.write(offset, colour);
   offset += array.write(offset, time);

   return (offset - start);
}

// =============================================================================
// MoveToTemperatureMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t MoveToTemperatureMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = array.read(offset, colour);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size    = array.read(offset, time);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}
