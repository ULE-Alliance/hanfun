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
#include <math.h>


// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::ColourControl;

// =============================================================================
// Colour Control Interface : IServer Role
// =============================================================================

// =============================================================================
// IServer::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS IServer::attributes(uint8_t pack_id) const
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
// IServer::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *IServer::attribute(uint8_t uid)
{
   using namespace HF::Interfaces::ColourControl;

   ColourControl::Attributes attr = static_cast<ColourControl::Attributes>(uid);

   switch (attr)
   {
      case SUPPORTED_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::supported;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::supported;

         return new Attribute(*this, attr, getter, setter, Supported::WRITABLE);
      }

      case MODE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::mode;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::mode;

         return new Attribute(*this, attr, getter, setter, Mode::WRITABLE);
      }

#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
      case HUE_AND_SATURATION_ATTR:
      {
         typedef HF::Attributes::Attribute<HS_Colour, IServer> Attribute;

         auto getter = (HS_Colour (IServer::*)(void) const) & IServer::hue_and_saturation;
         auto setter = (void (IServer::*)(HS_Colour)) & IServer::hue_and_saturation;

         return new Attribute(*this, attr, getter, setter, HueAndSaturation::WRITABLE);
      }
#endif

#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
      case XY_ATTR:
      {
         typedef HF::Attributes::Attribute<XY_Colour, IServer> Attribute;

         auto getter = (XY_Colour (IServer::*)(void) const) & IServer::xy;
         auto setter = (void (IServer::*)(XY_Colour)) & IServer::xy;

         return new Attribute(*this, attr, getter, setter, Xy::WRITABLE);
      }
#endif

#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR
      case COLOUR_TEMPERATURE_ATTR:
      {
         typedef HF::Attributes::Attribute<uint16_t, IServer> Attribute;

         auto getter = (uint16_t (IServer::*)(void) const) & IServer::colour_temperature;
         auto setter = (void (IServer::*)(uint16_t)) & IServer::colour_temperature;

         return new Attribute(*this, attr, getter, setter, ColourTemperature::WRITABLE);
      }
#endif

      default:
         return nullptr;
   }
}

// =============================================================================
// IServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   Common::Result result = Common::Result::OK;

   switch (cmd)
   {
#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
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
#endif

#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
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
#endif

#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR

      case MOVE_TO_COLOUR_TEMPERATURE_CMD:
      {
         MoveToTemperatureMessage message;
         message.unpack(payload, offset);
         result = move_to_colour_temperature(packet.source, message);
         break;
      }
#endif

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
// IServer::move_to_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_to_hue(const Protocol::Address &addr, const MoveToHueMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_hue(const Protocol::Address &addr, const MoveHueMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::step_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::step_hue(const Protocol::Address &addr, const StepHueMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_to_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_to_saturation(const Protocol::Address &addr,
                                const MoveToSaturationMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_saturation(const Protocol::Address &addr,
                                           const MoveSaturationMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::step_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::step_saturation(const Protocol::Address &addr,
                                           const StepSaturationMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_to_hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_to_hue_and_saturation(const Protocol::Address &addr,
                                        const MoveToHueSaturationMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::HS_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_to_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_to_xy(const Protocol::Address &addr, const MoveToXYMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::XY_MODE);                         // Change mode to HS mode.

   return Common::Result::OK;
}

// =============================================================================
// IServer::move_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_xy(const Protocol::Address &addr, const MoveXYMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::XY_MODE);                         // Change mode to HS mode.

   return(Common::Result::OK);
}

// =============================================================================
// IServer::step_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::step_xy(const Protocol::Address &addr, const StepXYMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::XY_MODE);                         // Change mode to HS mode.

   return(Common::Result::OK);
}

// =============================================================================
// IServer::move_to_colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::move_to_colour_temperature(const Protocol::Address &addr,
                                        const MoveToTemperatureMessage &message)
{
   UNUSED(addr);
   UNUSED(message);

   mode(Mask::TEMPERATURE_MODE);                         // Change mode to HS mode.

   return(Common::Result::OK);
}

#ifdef HF_ITF_COLOUR_CONTROL_STOP_CMD
// =============================================================================
// IServer::stop
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::stop(const Protocol::Address &addr)
{
   UNUSED(addr);
   return(Common::Result::OK);
}
#endif

// =============================================================================
// Server
// =============================================================================

Server::Container Server::transitions;

// =============================================================================
// Server::transition
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Server::transition (void)
{
   auto time_step = transitions.front()->remaining_time;

   std::for_each(transitions.begin(), transitions.end(), [time_step](ITransition *entry)
   {
      entry->run(time_step);
   });

   auto cleanup = [](ITransition *t)
   {
      bool res = t->next();
      if(!res) delete t;
      return !res;
   };

   auto it = std::remove_if(transitions.begin(), transitions.end(), cleanup);

   transitions.erase(it, transitions.end());

   std::sort(transitions.begin(), transitions.end(), [ ]( const ITransition *lhs, const ITransition *rhs )
             {
                return lhs->remaining_time < rhs->remaining_time;
             });

   return has_transitions();
}

// =============================================================================
// Server::move_to_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::move_to_hue(const Protocol::Address &addr, const MoveToHueMessage &message)
{
   auto step = HS_Colour::get_travel_distance(message.direction,
                                                                   hue_and_saturation().hue,
                                                                   message.hue);

   Hue_Transition *new_transition = new Hue_Transition(*this,  // server
                                                       1);     // 100msec period

   if (message.time != 0)
   {
      new_transition->n_steps = message.time - 1;     // Run for time-1 (we will run once)
      new_transition->step = round(step/static_cast<float>(message.time));       // Step size.

      if (new_transition->step == 0)
      {
         delete new_transition;
         return Common::Result::FAIL_ARG;
      }
   }
   else
   {
      new_transition->n_steps = 0;                    // Run once
      new_transition->period = 0;                     // Run once
   }

   IServer::move_to_hue(addr, message);   // Common part

   new_transition->end = message.hue;

   new_transition->run(0);  //Run once immediately
   if (new_transition->next())
   {
      //If there are still iterations, add the transition to the list and inform the APP.
      add_transition(new_transition);
   }
   else
   {
      delete new_transition;
   }

   return Common::Result::OK;
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
   IServer::move_hue(addr, message);   // Common part

   auto step = (message.direction == Direction::UP ? message.rate: -message.rate);

   Hue_Transition_Continuous *new_transition = new Hue_Transition_Continuous(*this,
                                                                             10,    // 1 sec.
                                                                             step);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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
   IServer::step_hue(addr, message);   // Common part

   auto step = (message.direction == Direction::UP ? message.step_size : -message.step_size);

   Hue_Transition_Continuous *new_transition = new Hue_Transition_Continuous(*this,
                                                                             message.time,
                                                                             step);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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

   auto step = (message.direction == Direction::UP ? message.saturation : -message.saturation);

   Saturation_Transition *new_transition = new Saturation_Transition(*this,  // server
                                                       1);     // 100msec period

   if (message.time != 0)
   {
      new_transition->n_steps = message.time - 1; // Run for time-1 (adjust for base 0 instead of 1)
      new_transition->step = round(step / static_cast<float>(message.time)); // Step size.

      if (new_transition->step == 0)
      {
         delete new_transition;
         return Common::Result::FAIL_ARG;
      }

   }
   else
   {
      new_transition->n_steps = 0;                    // Run once
      new_transition->period = 0;                     // Run once
   }

   IServer::move_to_saturation(addr, message);   // Common part

   new_transition->end = message.saturation;

   new_transition->run(0);  //Run once immediately
   if (new_transition->next())
   {
      //If there are still iterations, add the transition to the list and inform the APP.
      add_transition(new_transition);
   }
   else
   {
      delete new_transition;
   }

   return Common::Result::OK;
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
   IServer::move_saturation(addr, message);   // Common part

   auto step = (message.direction == Direction::UP ? message.rate: -message.rate);

   Saturation_Transition_Continuous *new_transition = new Saturation_Transition_Continuous(*this,
                                                                                    10,    // 1 sec.
                                                                                    step);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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
   IServer::step_saturation(addr, message);   // Common part

   auto step = (message.direction == Direction::UP ? message.step_size : -message.step_size);

   Saturation_Transition_Continuous *new_transition = new Saturation_Transition_Continuous(*this,
                                                                             message.time,
                                                                             step);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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
   auto hue_step = HS_Colour::get_travel_distance<HS_Colour::HUE>(message.direction,
                                                                  hue_and_saturation().hue,
                                                                  message.colour.hue);
   auto sat_step = HS_Colour::get_travel_distance<HS_Colour::SATURATION>(message.direction,
                                                                         hue_and_saturation().saturation,
                                                                         message.colour.saturation);

   HS_Transition *new_transition = new HS_Transition(*this,  // server
                                                     1);     // 100msec period

   if (message.time != 0)
   {
      new_transition->n_steps = message.time - 1; // Run for time-1 (adjust for base 0 instead of 1)
      new_transition->hue_step = round(hue_step / static_cast<float>(message.time)); // Hue Step size.
      new_transition->sat_step = round(sat_step / static_cast<float>(message.time)); // Saturation Step size.

      if(new_transition->hue_step == 0 && new_transition->sat_step == 0)
      {
         delete new_transition;
         return Common::Result::FAIL_ARG;
      }
   }
   else
   {
      new_transition->n_steps = 0;                    // Run once
      new_transition->period = 0;                     // Run once
   }

   IServer::move_to_hue_and_saturation(addr, message);   // Common part

   new_transition->end = message.colour;

   new_transition->run(0);  //Run once immediately
   if (new_transition->next())
   {
      //If there are still iterations, add the transition to the list and inform the APP.
      add_transition(new_transition);
   }
   else
   {
      delete new_transition;
   }

   return Common::Result::OK;
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
   int32_t X_dist = message.colour.X - xy().X;
   int32_t Y_dist = message.colour.Y - xy().Y;

   XY_Transition *new_transition = new XY_Transition(*this,  // server
                                                       1);     // 100msec period

   if (message.time != 0)
   {
      new_transition->n_steps = message.time - 1;     // Run for time-1 (we will run once)
      new_transition->X_step = round(X_dist / static_cast<float>(message.time));       // Step size.
      new_transition->Y_step = round(Y_dist / static_cast<float>(message.time));       // Step size.

      if (new_transition->X_step == 0 && new_transition->Y_step == 0)
      {
         delete new_transition;
         return Common::Result::FAIL_ARG;
      }
   }
   else
   {
      new_transition->n_steps = 0;                    // Run once
      new_transition->period = 0;                     // Run once
   }

   IServer::move_to_xy(addr, message);   // Common part

   new_transition->end = message.colour;

   new_transition->run(0);  //Run once immediately
   if (new_transition->next())
   {
      //If there are still iterations, add the transition to the list and inform the APP.
      add_transition(new_transition);
   }
   else
   {
      delete new_transition;
   }

   return Common::Result::OK;
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
   IServer::move_xy(addr, message);   // Common part

   XY_Transition_Continuous *new_transition = new XY_Transition_Continuous(*this,
                                                                           10, // 1 sec.
                                                                           message.X_rate,
                                                                           message.Y_rate);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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
   IServer::step_xy(addr, message);   // Common part

   XY_Transition_Continuous *new_transition = new XY_Transition_Continuous(*this,
                                                                           message.time,
                                                                           message.X_step,
                                                                           message.Y_step);

   new_transition->run(0);  //Run once immediately
   add_transition(new_transition);

   return Common::Result::OK;
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
   int32_t dist = message.colour - colour_temperature();

   Temperature_Transition *new_transition = new Temperature_Transition(*this,  // server
                                                                       1);     // 100msec period

   if (message.time != 0)
   {
      new_transition->n_steps = message.time - 1;     // Run for time-1 (we will run once)
      new_transition->step = round(dist / static_cast<float>(message.time));       // Step size.

      if (new_transition->step == 0)
      {
         delete new_transition;
         return Common::Result::FAIL_ARG;
      }
   }
   else
   {
      new_transition->n_steps = 0;                    // Run once
      new_transition->period = 0;                     // Run once
   }

   IServer::move_to_colour_temperature(addr, message);   // Common part

   new_transition->end = message.colour;

   new_transition->run(0);  //Run once immediately
   if (new_transition->next())
   {
      //If there are still iterations, add the transition to the list and inform the APP.
      add_transition(new_transition);
   }
   else
   {
      delete new_transition;
   }

   return Common::Result::OK;
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
   UNUSED(addr);
   remove(*this);

   return Common::Result::OK;
}
#endif

// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// IServer::supported
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::supported() const
{
   return _supported;
}

// =============================================================================
// IServer::supported
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::supported(uint8_t __value)
{
   HF_SETTER_HELPER(Supported, _supported, __value);
}

// =============================================================================
// IServer::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::mode() const
{
   return _mode;
}

// =============================================================================
// IServer::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::mode(uint8_t __value)
{
   HF_SETTER_HELPER(Mode, _mode, __value);
}


#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
// =============================================================================
// IServer::hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
HS_Colour IServer::hue_and_saturation() const
{
   return _hue_and_saturation;
}

// =============================================================================
// IServer::hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::hue_and_saturation(HS_Colour __value)
{
   HF_SETTER_HELPER(HueAndSaturation, _hue_and_saturation, __value);
}
#endif


#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
// =============================================================================
// IServer::xy
// =============================================================================
/*!
 *
 */
// =============================================================================
XY_Colour IServer::xy() const
{
   return _xy;
}

// =============================================================================
// IServer::xy
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::xy(XY_Colour __value)
{
   HF_SETTER_HELPER(Xy, _xy, __value);
}
#endif


#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR
// =============================================================================
// IServer::colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t IServer::colour_temperature() const
{
   return _colour_temperature;
}

// =============================================================================
// IServer::colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::colour_temperature(uint16_t __value)
{
   HF_SETTER_HELPER(ColourTemperature, _colour_temperature, __value);
}
#endif


// =============================================================================
// ColourControl::Hue_Transition
// =============================================================================

// =============================================================================
// Hue_Transition::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Hue_Transition::run(uint16_t time)
{
   if( time!=0 && !ITransition::run(time))
   {
      return false;
   }

   if (n_steps != 0)
   {
      server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue + step,
                                          server.hue_and_saturation().saturation));
      n_steps--;
      return true;
   }
   else
   {
      server.hue_and_saturation(HS_Colour(end, server.hue_and_saturation().saturation));

      period=0;   //can be deleted
      return true;
   }
}

// =============================================================================
// Hue_Transition_Continuous::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Hue_Transition_Continuous::run(uint16_t time)
{
   if (time!=0 && !ITransition::run(time))
   {
      return false;
   }

   server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue + step,
                                             server.hue_and_saturation().saturation));
   return true;
}

// =============================================================================
// Saturation_Transition::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Saturation_Transition::run(uint16_t time)
{
   if( time!=0 && !ITransition::run(time))
   {
      return false;
   }

   if (n_steps != 0)
   {
      server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue,
                                          server.hue_and_saturation().saturation+step));
      n_steps--;
      return true;
   }
   else
   {
      server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue, end));

      period=0;   //can be deleted
      return true;
   }
}

// =============================================================================
// Saturation_Transition_Continuous::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Saturation_Transition_Continuous::run(uint16_t time)
{
   if (time!=0 && !ITransition::run(time))
   {
      return false;
   }

   server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue,
                                             server.hue_and_saturation().saturation + step));
   return true;
}


// =============================================================================
// HS_Transition::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool HS_Transition::run(uint16_t time)
{
   if( time!=0 && !ITransition::run(time))
   {
      return false;
   }

   if (n_steps != 0)
   {
      server.hue_and_saturation(HS_Colour(server.hue_and_saturation().hue+hue_step,
                                          server.hue_and_saturation().saturation+sat_step));
      n_steps--;
      return true;
   }
   else
   {
      server.hue_and_saturation(end);

      period=0;   //can be deleted
      return true;
   }
}


// =============================================================================
// XY_Transition::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool XY_Transition::run(uint16_t time)
{
   if( time!=0 && !ITransition::run(time))
   {
      return false;
   }

   if (n_steps != 0)
   {
      server.xy(XY_Colour(server.xy().X+X_step,
                          server.xy().Y+Y_step));
      n_steps--;
      return true;
   }
   else
   {
      server.xy(end);

      period=0;   //can be deleted
      return true;
   }
}

// =============================================================================
// XY_Transition_Continuous::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool XY_Transition_Continuous::run(uint16_t time)
{
   if (time!=0 && !ITransition::run(time))
   {
      return false;
   }

   server.xy(XY_Colour(server.xy().X + X_step,
                       server.xy().Y + Y_step));
   return true;
}


// =============================================================================
// Temperature_Transition::run
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Temperature_Transition::run(uint16_t time)
{
   if (time!=0 && !ITransition::run(time))
   {
      return false;
   }

   if (n_steps != 0)
   {
      server.colour_temperature(server.colour_temperature() + step);
      n_steps--;
      return true;
   }
   else
   {
      server.colour_temperature(end);

      period=0;   //can be deleted
      return true;
   }
}

