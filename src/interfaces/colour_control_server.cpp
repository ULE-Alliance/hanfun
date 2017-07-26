// =============================================================================
/*!
 * @file       src/interfaces/colour_control_server.cpp
 *
 * This file contains the implementation of the Colour Control interface : Server role.
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
// Colour Control Interface : Server Role
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
   HF::Attributes::UIDS result({SUPPORTED_ATTR, MODE_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {
#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
      result.push_back(HUE_AND_SATURATION_ATTR);
#endif
#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
      result.push_back(XY_ATTR);
#endif
#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR
      result.push_back(COLOUR_TEMPERATURE_ATTR);
#endif
   }

   return result;
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Server::attribute(uint8_t uid)
{
   using namespace HF::Interfaces::ColourControl;

   ColourControl::Attributes attr = static_cast<ColourControl::Attributes>(uid);

   switch (attr)
   {
      case SUPPORTED_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, Server> Attribute;

         auto getter = (uint8_t (Server::*)(void) const) & Server::supported;
         auto setter = (void (Server::*)(uint8_t)) & Server::supported;

         return new Attribute(*this, attr, getter, setter, Supported::WRITABLE);
      }

      case MODE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, Server> Attribute;

         auto getter = (uint8_t (Server::*)(void) const) & Server::mode;
         auto setter = (void (Server::*)(uint8_t)) & Server::mode;

         return new Attribute(*this, attr, getter, setter, Mode::WRITABLE);
      }

#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
      case HUE_AND_SATURATION_ATTR:
      {
         typedef HF::Attributes::Attribute<HS_Colour, Server> Attribute;

         auto getter = (HS_Colour (Server::*)(void) const) & Server::hue_and_saturation;
         auto setter = (void (Server::*)(HS_Colour)) & Server::hue_and_saturation;

         return new Attribute(*this, attr, getter, setter, HueAndSaturation::WRITABLE);
      }
#endif

#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
      case XY_ATTR:
      {
         typedef HF::Attributes::Attribute<XY_Colour, Server> Attribute;

         auto getter = (XY_Colour (Server::*)(void) const) & Server::xy;
         auto setter = (void (Server::*)(XY_Colour)) & Server::xy;

         return new Attribute(*this, attr, getter, setter, Xy::WRITABLE);
      }
#endif

#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR
      case COLOUR_TEMPERATURE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint16_t, Server> Attribute;

         auto getter = (uint16_t (Server::*)(void) const) & Server::colour_temperature;
         auto setter = (void (Server::*)(uint16_t)) & Server::colour_temperature;

         return new Attribute(*this, attr, getter, setter, ColourTemperature::WRITABLE);
      }
#endif

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
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   Common::Result result = AbstractInterface::check_payload_size(packet.message, payload, offset);

   if (result != Common::Result::OK)
   {
      return result;
   }

   switch (cmd)
   {
      case MOVE_TO_HUE_CMD:
      {
         MoveToHueMessage message;
         message.unpack(payload, offset);
         result = move_to_hue(packet.source, message);
         break;
      }

      case MOVE_HUE_CMD:
      {
         MoveHueMessage message;
         message.unpack(payload, offset);
         result = move_hue(packet.source, message);
         break;
      }

      case STEP_HUE_CMD:
      {
         StepHueMessage message;
         message.unpack(payload, offset);
         result = step_hue(packet.source, message);
         break;
      }

      case MOVE_TO_SATURATION_CMD:
      {
         MoveToSaturationMessage message;
         message.unpack(payload, offset);
         result = move_to_saturation(packet.source, message);
         break;
      }

      case MOVE_SATURATION_CMD:
      {
         MoveSaturationMessage message;
         message.unpack(payload, offset);
         result = move_saturation(packet.source, message);
         break;
      }

      case STEP_SATURATION_CMD:
      {
         StepSaturationMessage message;
         message.unpack(payload, offset);
         result = step_saturation(packet.source, message);
         break;
      }

      case MOVE_TO_HUE_AND_SATURATION_CMD:
      {
         MoveToHueSaturationMessage message;
         message.unpack(payload, offset);
         result = move_to_hue_and_saturation(packet.source, message);
         break;
      }

      case MOVE_TO_XY_CMD:
      {
         MoveToXYMessage message;
         message.unpack(payload, offset);
         result = move_to_xy(packet.source, message);
         break;
      }

      case MOVE_XY_CMD:
      {
         MoveXYMessage message;
         message.unpack(payload, offset);
         result = move_xy(packet.source, message);
         break;
      }

      case STEP_XY_CMD:
      {
         StepXYMessage message;
         message.unpack(payload, offset);
         result = step_xy(packet.source, message);
         break;
      }

      case MOVE_TO_COLOUR_TEMPERATURE_CMD:
      {
         MoveToTemperatureMessage message;
         message.unpack(payload, offset);
         result = move_to_colour_temperature(packet.source, message);
         break;
      }

#ifdef HF_ITF_COLOUR_CONTROL_STOP_CMD
      case STOP_CMD:
      {
         result = stop(packet.source);
         break;
      }
#endif

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return result;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::move_to_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_hue(const Protocol::Address &addr, const MoveToHueMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   Common::Result result = Common::Result::OK;
  return result;
}

// =============================================================================
// Server::move_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_hue(const Protocol::Address &addr, const MoveHueMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::step_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::step_hue(const Protocol::Address &addr, const StepHueMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::hue_callback
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Server::hue_callback(callback_args_t &arg)
{
   if (arg.hs.n_steps != 0)
   {
      hue_and_saturation(HS_Colour(hue_and_saturation().hue + arg.hs.step,
                                   hue_and_saturation().saturation));
      arg.hs.n_steps--;
      return 1;
   }
   else
   {
      hue_and_saturation(HS_Colour(arg.hs.end,hue_and_saturation().saturation));
      return 0;
   }
}

// =============================================================================
// Server::move_to_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_saturation(const Protocol::Address &addr,
                                const MoveToSaturationMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::move_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_saturation(const Protocol::Address &addr,
                                           const MoveSaturationMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::step_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::step_saturation(const Protocol::Address &addr,
                                           const StepSaturationMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::move_to_hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_hue_and_saturation(const Protocol::Address &addr,
                                        const MoveToHueSaturationMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::move_to_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_xy(const Protocol::Address &addr, const MoveToXYMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::move_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_xy(const Protocol::Address &addr, const MoveXYMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::step_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::step_xy(const Protocol::Address &addr, const StepXYMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

// =============================================================================
// Server::move_to_colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_colour_temperature(const Protocol::Address &addr,
                                        const MoveToTemperatureMessage &message)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   UNUSED(message);
   return(Common::Result::OK);
}

#ifdef HF_ITF_COLOUR_CONTROL_STOP_CMD
// =============================================================================
// Server::stop
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::stop(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
   return(Common::Result::OK);
}
#endif


// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::supported
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::supported() const
{
   return _supported;
}

// =============================================================================
// Server::supported
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::supported(uint8_t __value)
{
   HF_SETTER_HELPER(Supported, _supported, __value);
}

// =============================================================================
// Server::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::mode() const
{
   return _mode;
}

// =============================================================================
// Server::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::mode(uint8_t __value)
{
   HF_SETTER_HELPER(Mode, _mode, __value);
}


#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
// =============================================================================
// Server::hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
HS_Colour Server::hue_and_saturation() const
{
   return _hue_and_saturation;
}

// =============================================================================
// Server::hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::hue_and_saturation(HS_Colour __value)
{
   HF_SETTER_HELPER(HueAndSaturation, _hue_and_saturation, __value);
}
#endif


#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
// =============================================================================
// Server::xy
// =============================================================================
/*!
 *
 */
// =============================================================================
XY_Colour Server::xy() const
{
   return _xy;
}

// =============================================================================
// Server::xy
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::xy(XY_Colour __value)
{
   HF_SETTER_HELPER(Xy, _xy, __value);
}
#endif


#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR
// =============================================================================
// Server::colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Server::colour_temperature() const
{
   return _colour_temperature;
}

// =============================================================================
// Server::colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::colour_temperature(uint16_t __value)
{
   HF_SETTER_HELPER(ColourTemperature, _colour_temperature, __value);
}
#endif
