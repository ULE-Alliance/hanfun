// =============================================================================
/*!
 * @file       src/debug.cpp
 *
 * This file contains the implementation of the debug helper functions.
 *
 * @version    1.4.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include <iomanip>

#include "hanfun/debug.h"

using namespace HF::Debug;

// LCOV_EXCL_START

// =============================================================================
// Defines
// =============================================================================

/* *INDENT-OFF* */
#define ALIGN()      std::right << std::setfill (' ') << std::setw (log_width)
#define NL_ALIGN()   std::endl << ALIGN ()
/* *INDENT-ON* */

// =============================================================================
// Global variables.
// =============================================================================

static int8_t log_width = -1;

// =============================================================================
// Stream Helpers
// =============================================================================

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const uint8_t byte)
{
   if (stream.rdbuf() == std::cout.rdbuf() || stream.rdbuf() == std::cerr.rdbuf())
   {
      std::ios_base::fmtflags ff = stream.flags();
      char f                     = stream.fill('0');

      stream << Hex<uint8_t>(byte);

      stream << std::setfill(f) << std::setiosflags(ff);
   }
   else
   {
      stream << (int) byte;
   }

   return stream;
}

// =============================================================================
// Common
// =============================================================================

#include "hanfun/common.h"
#include "hanfun/uids.h"
#include "hanfun/interface.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Common::ByteArray &array)
{
   if (stream.rdbuf() == std::cout.rdbuf() || stream.rdbuf() == std::cerr.rdbuf())
   {
      std::ios_base::fmtflags ff = stream.flags();
      char f                     = stream.fill('0');

      stream << std::left;
      stream << "(" << Hex<uint16_t>(array.size()) << ") ";

      /* *INDENT-OFF* */
      std::for_each (array.begin (), array.end (), [&stream](uint8_t byte)
      {
         stream << Hex<uint8_t>(byte) << " ";
      });
      /* *INDENT-ON* */

      stream << std::setfill(f) << std::setiosflags(ff);
   }
   else
   {
      /* *INDENT-OFF* */
      std::for_each (array.begin (), array.end (), [&stream](uint8_t byte)
      {
         stream << byte;
      });
      /* *INDENT-ON* */
   }

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Common::Interface interface)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   stream << "R:" << static_cast<HF::Interface::Role>(interface.role)
          << " UID:" << static_cast<HF::Interface::UID>(interface.id);

   stream << std::setfill(f) << std::setiosflags(ff);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::UID::UID &uid)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill('0');

   switch (uid.raw()->type())
   {
      case HF::UID::NONE_UID:
      {
         stream << "uid: NONE";
         break;
      }
      case HF::UID::DECT_UID:
      {
         HF::UID::DECT *dect = (HF::UID::DECT *) uid.raw();
         stream << "dect: ";

         for (uint8_t i = 0; i < HF::UID::DECT::length(); i++)
         {
            stream << Hex<uint8_t>((*dect)[i]);
         }

         break;
      }
      case HF::UID::MAC_UID:
      {
         HF::UID::MAC *mac = (HF::UID::MAC *) uid.raw();
         stream << "mac: ";

         for (uint8_t i = 0; i < HF::UID::MAC::length() - 1; i++)
         {
            stream << Hex<uint8_t>((*mac)[i]) << ":";
         }

         stream << Hex<uint8_t>((*mac)[HF::UID::MAC::length() - 1]);

         break;
      }

      case HF::UID::URI_UID:
      {
         HF::UID::URI *uri = (HF::UID::URI *) uid.raw();
         stream << "uri: " << uri->str();
         break;
      }

      default:
         break;
   }

   stream << std::setfill(f) << std::setiosflags(ff);

   return stream;
}

// =============================================================================
// Interfaces
// =============================================================================

#include "hanfun/interface.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, HF::Interface::Role role)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   switch (role)
   {
      case HF::Interface::Role::SERVER_ROLE:
      {
         result = "Server";
         break;
      }
      case HF::Interface::Role::CLIENT_ROLE:
      {
         result = "Client";
         break;
      }
   }

   stream << Hex<uint8_t>((uint8_t) role) << " (" << result << ")";

   stream << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interface::UID uid)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   switch (uid)
   {
      case HF::Interface::UID::DEVICE_MANAGEMENT:
      {
         result = "Device Management";
         break;
      }
      case HF::Interface::UID::BIND_MANAGEMENT:
      {
         result = "Bind Management";
         break;
      }
      case HF::Interface::UID::GROUP_MANAGEMENT:
      {
         result = "Group Management";
         break;
      }
      case HF::Interface::UID::IDENTIFY:
      {
         result = "Identify";
         break;
      }
      case HF::Interface::UID::DEVICE_INFORMATION:
      {
         result = "Device Information";
         break;
      }
      case HF::Interface::UID::ATTRIBUTE_REPORTING:
      {
         result = "Attribute Reporting";
         break;
      }
      case HF::Interface::UID::TAMPER_ALERT:
      {
         result = "Tamper Alert";
         break;
      }
      case HF::Interface::UID::TIME:
      {
         result = "Time";
         break;
      }
      case HF::Interface::UID::POWER:
      {
         result = "Power";
         break;
      }
      case HF::Interface::UID::KEEP_ALIVE:
      {
         result = "Keep Alive";
         break;
      }
      case HF::Interface::UID::ALERT:
      {
         result = "Alert";
         break;
      }
      case HF::Interface::UID::ON_OFF:
      {
         result = "On-Off";
         break;
      }
      case HF::Interface::UID::LEVEL_CONTROL:
      {
         result = "Level Control";
         break;
      }
      case HF::Interface::UID::SIMPLE_POWER_METER:
      {
         result = "Simple Power Meter";
         break;
      }
      case HF::Interface::UID::ANY_UID:
      {
         result = "Any";
         break;
      }
      default:
      {
         if (uid >= HF::Interface::UID::RESERVED && uid <= HF::Interface::UID::MAX_UID)
         {
            result = "Proprietary";
         }

         break;
      }
   }

   stream << Hex<uint16_t>((uint16_t) uid) << " (" << result << ")";

   stream << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::Alert
// =============================================================================

#include "hanfun/interfaces/alert.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::Alert::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::Alert;

   switch (command)
   {
      case STATUS_CMD:
         result = "Status";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::Alert::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::Alert;

   switch (attribute)
   {
      case STATE_ATTR:
         result = "State";
         break;
      case ENABLE_ATTR:
         result = "Enabled";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);

   return stream;
}

// =============================================================================
// HF::Interfaces::LevelControl
// =============================================================================

#include "hanfun/interfaces/level_control.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::LevelControl::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::LevelControl;

   switch (command)
   {
      case SET_LEVEL_CMD:
         result = "Set Level";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::LevelControl::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::LevelControl;

   switch (attribute)
   {
      case LEVEL_ATTR:
         result = "Level";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::OnOff
// =============================================================================

#include "hanfun/interfaces/on_off.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::OnOff::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::OnOff;

   switch (command)
   {
      case ON_CMD:
         result = "On";
         break;
      case OFF_CMD:
         result = "Off";
         break;
      case TOGGLE_CMD:
         result = "Toggle";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::OnOff::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::OnOff;

   switch (attribute)
   {
      case STATE_ATTR:
         result = "State";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimplePowerMeter
// =============================================================================

#include "hanfun/interfaces/simple_power_meter.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimplePowerMeter::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimplePowerMeter;

   switch (command)
   {
      case REPORT_CMD:
         result = "Report/Measurement Reset";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimplePowerMeter::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimplePowerMeter;

   switch (attribute)
   {
      case ENERGY_ATTR:
      {
         result = "Energy";
         break;
      }
      case ENERGY_AT_RESET_ATTR:
      {
         result = "Energy at reset";
         break;
      }
      case TIME_AT_RESET_ATTR:
      {
         result = "Time at reset";
         break;
      }
      case POWER_ATTR:
      {
         result = "Power";
         break;
      }
      case AVG_POWER_ATTR:
      {
         result = "Avg. Power";
         break;
      }
      case AVG_POWER_INTERVAL_ATTR:
      {
         result = "Avg. Power Interval";
         break;
      }
      case VOLTAGE_ATTR:
      {
         result = "Voltage";
         break;
      }
      case CURRENT_ATTR:
      {
         result = "Current";
         break;
      }
      case FREQUENCY_ATTR:
      {
         result = "Frequency";
         break;
      }
      case POWER_FACTOR_ATTR:
      {
         result = "Power Factor";
         break;
      }
      case REPORT_INTERVAL_ATTR:
      {
         result = "Report Interval";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimpleThermostat
// =============================================================================

#include "hanfun/interfaces/simple_thermostat.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimpleThermostat::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleThermostat;

   switch (command)
   {
      case BOOST_START_CMD:
         result = "Boost Start";
         break;
      case BOOST_STOP_CMD:
         result = "Boost Stop";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleThermostat::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleThermostat;

   switch (attribute)
   {
      case SUPPORTED_MODES_ATTR:
         result = "Supported Modes";
         break;
      case HEAT_COOL_MODE_ATTR:
         result = "Heat/Cool Mode";
         break;
      case FAN_MODE_ATTR:
         result = "Fan Mode";
         break;
      case HEAT_MODE_TEMP_ATTR:
         result = "Heat Mode Temperature";
         break;
      case COOL_MODE_TEMP_ATTR:
         result = "Cool Mode Temperature";
         break;
      case AUTO_MODE_HEAT_TEMP_ATTR:
         result = "Heat/Cool Mode - Heat Temperature";
         break;
      case AUTO_MODE_COOL_TEMP_ATTR:
         result = "Heat/Cool Mode - Cool Temperature";
         break;
      case HEAT_MODE_TEMP_OFFSET_ATTR:
         result = "Heat Mode Temperature Offset";
         break;
      case COOL_MODE_TEMP_OFFSET_ATTR:
         result = "Cool Mode Temperature Offset";
         break;
      case BOOST_DURATION_ATTR:
         result = "Boost Duration";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimpleButton
// =============================================================================

#include "hanfun/interfaces/simple_button.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimpleButton::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleButton;

   switch (command)
   {
      case SHORT_PRESS_CMD:
         result = "Short Press";
         break;
      case LONG_PRESS_CMD:
         result = "Long Press";
         break;
      case EXTRA_LONG_PRESS_CMD:
         result = "Extra Long Press";
         break;
      case DOUBLE_CLICK_PRESS_CMD:
         result = "Double Click Press";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleButton::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleButton;

   switch (attribute)
   {
      case SHORT_PRESS_MAX_DURATION_ATTR:
         result = "Short Press Max Duration";
         break;
      case EXTRA_LONG_PRESS_MIN_DURATION_ATTR:
         result = "Extra Long Press Min Duration";
         break;
      case DOUBLE_CLICK_GAP_DURATION_ATTR:
         result = "Double Click Gap Duration";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimpleVisualEffects
// =============================================================================

#include "hanfun/interfaces/simple_visual_effects.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleVisualEffects::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleVisualEffects;

   switch (command)
   {
      case ON_CMD:
         result = "On";
         break;
      case OFF_CMD:
         result = "Off";
         break;
      case BLINK_CMD:
         result = "Blink";
         break;
      case FADE_CMD:
         result = "Fade";
         break;
      case BREATHE_CMD:
         result = "Breathe";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimpleAirPressure
// =============================================================================

#include "hanfun/interfaces/simple_air_pressure.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleAirPressure::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleAirPressure;

   switch (attribute)
   {
      case VALUE_ATTR:
         result = "Value";
         break;
      case MAXIMUM_ATTR:
         result = "Maximum";
         break;
      case MINIMUM_ATTR:
         result = "Minimum";
         break;
      case TOLERANCE_ATTR:
         result = "Tolerance";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::SimpleLightSensor
// =============================================================================

#include "hanfun/interfaces/simple_light_sensor.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleLightSensor::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::SimpleLightSensor;

   switch (attribute)
   {
      case VALUE_ATTR:
         result = "Value";
         break;
      case MINIMUM_ATTR:
         result = "Minimum";
         break;
      case MAXIMUM_ATTR:
         result = "Maximum";
         break;
      case TOLERANCE_ATTR:
         result = "Tolerance";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Interfaces::ColourControl
// =============================================================================

#include "hanfun/interfaces/colour_control.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::ColourControl::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::ColourControl;

   switch (command)
   {
      case MOVE_TO_HUE_CMD:
         result = "Move To Hue";
         break;
      case MOVE_HUE_CMD:
         result = "Move Hue";
         break;
      case STEP_HUE_CMD:
         result = "Step Hue";
         break;
      case MOVE_TO_SATURATION_CMD:
         result = "Move To Saturation";
         break;
      case MOVE_SATURATION_CMD:
         result = "Move Saturation";
         break;
      case STEP_SATURATION_CMD:
         result = "Step Saturation";
         break;
      case MOVE_TO_HUE_AND_SATURATION_CMD:
         result = "Move To Hue And Saturation";
         break;
      case MOVE_TO_XY_CMD:
         result = "Move To Xy";
         break;
      case MOVE_XY_CMD:
         result = "Move Xy";
         break;
      case STEP_XY_CMD:
         result = "Step Xy";
         break;
      case MOVE_TO_COLOUR_TEMPERATURE_CMD:
         result = "Move To Colour Temperature";
         break;
      case STOP_CMD:
         result = "Stop";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::ColourControl::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Interfaces::ColourControl;

   switch (attribute)
   {
      case SUPPORTED_ATTR:
         result = "Supported";
         break;
      case MODE_ATTR:
         result = "Mode";
         break;
      case HUE_AND_SATURATION_ATTR:
         result = "Hue And Saturation";
         break;
      case XY_ATTR:
         result = "XY";
         break;
      case COLOUR_TEMPERATURE_ATTR:
         result = "Colour Temperature";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// Core Services & Interfaces
// =============================================================================

#include "hanfun/core.h"

// =============================================================================
// HF::Core::AttributeReporting
// =============================================================================

#include "hanfun/core/attribute_reporting.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::AttributeReporting::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   stream << Hex<uint8_t>((uint8_t) command) << " (" << result << " [C])";

   using namespace HF::Core::AttributeReporting;

   switch (command)
   {
      // case PERIODIC_REPORT_CMD:
      case CREATE_PERIODIC_CMD:
      {
         result = "<Create> Periodic Report";
         break;
      }
      // case EVENT_REPORT_CMD:
      case CREATE_EVENT_CMD:
      {
         result = "<Create> Event Report";
         break;
      }
      case ADD_PERIODIC_ENTRY_CMD:
      {
         result = "Add periodic report entry";
         break;
      }
      case ADD_EVENT_ENTRY_CMD:
      {
         result = "Add event report entry";
         break;
      }
      case DELETE_REPORT_CMD:
      {
         result = "Delete report";
         break;
      }
      case GET_PERIODIC_ENTRIES:
      {
         result = "Get periodic entries";
         break;
      }
      case GET_EVENT_ENTRIES:
      {
         result = "Get event entries";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::AttributeReporting::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::AttributeReporting;

   switch (attribute)
   {
      case REPORT_COUNT_ATTR:
      {
         result = "Report entries count";
         break;
      }
      case PERIODIC_REPORT_COUNT_ATTR:
      {
         result = "Periodic report entries count";
         break;
      }
      case EVENT_REPORT_COUNT_ATTR:
      {
         result = "Event report entries count";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::BindManagement
// =============================================================================

#include "hanfun/core/bind_management.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::BindManagement::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::BindManagement;

   switch (command)
   {
      case ADD_BIND_CMD:
      {
         result = "Add bind entry";
         break;
      }
      case REMOVE_BIND_CMD:
      {
         result = "Remove bind entry";
         break;
      }
      case START_SESSION_CMD:
      {
         result = "Start Session - Read Registration Info";
         break;
      }
      case END_SESSION_CMD:
      {
         result = "End Session - Read Registration Info";
         break;
      }
      case GET_ENTRIES_CMD:
      {
         result = "Get Entries";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::BindManagement::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::BindManagement;

   switch (attribute)
   {
      case NUMBER_OF_ENTRIES_ATTR:
      {
         result = "Number of entries";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::DeviceInformation
// =============================================================================

#include "hanfun/core/device_information.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::DeviceInformation::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::DeviceInformation;

   switch (attribute)
   {
      case CORE_VERSION_ATTR:
      {
         result = "HF Core version";
         break;
      }
      case PROFILE_VERSION_ATTR:
      {
         result = "HF Profile version";
         break;
      }
      case INTERFACE_VERSION_ATTR:
      {
         result = "HF Interface version";
         break;
      }
      case EXTRA_CAP_ATTR:
      {
         result = "Extra capabilities";
         break;
      }
      case MIN_SLEEP_TIME_ATTR:
      {
         result = "Minimum sleep time";
         break;
      }
      case ACTUAL_RESP_TIME_ATTR:
      {
         result = "Actual response time";
         break;
      }
      case APP_VERSION_ATTR:
      {
         result = "Hardware version";
         break;
      }
      case HW_VERSION_ATTR:
      {
         result = "Hardware version";
         break;
      }
      case EMC_ATTR:
      {
         result = "Electronic Manufacture Code";
         break;
      }
      case DECT_ID_ATTR:
      {
         result = "RFPI / IPUI";
         break;
      }
      case MANUFACTURE_NAME_ATTR:
      {
         result = "Manufacture's name";
         break;
      }
      case LOCATION_ATTR:
      {
         result = "Location";
         break;
      }
      case ENABLED_ATTR:
      {
         result = "Device enabled";
         break;
      }
      case FRIENDLY_NAME_ATTR:
      {
         result = "Device friendly";
         break;
      }
      case UID_ATTR:
      {
         result = "Device UID";
         break;
      }
      case SERIAL_NUMBER_ATTR:
      {
         result = "Serial number";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::DeviceManagement
// =============================================================================

#include "hanfun/core/device_management.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::DeviceManagement::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::DeviceManagement;

   switch (command)
   {
      case REGISTER_CMD:
      {
         result = "Register device";
         break;
      }
      case DEREGISTER_CMD:
      {
         result = "De-register device";
         break;
      }
      case START_SESSION_CMD:
      {
         result = "Start Session";
         break;
      }
      case END_SESSION_CMD:
      {
         result = "End Session";
         break;
      }
      case GET_ENTRIES_CMD:
      {
         result = "Get Entries";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::DeviceManagement::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::DeviceManagement;

   switch (attribute)
   {
      case NUMBER_OF_ENTRIES_ATTR:
      {
         result = "Number of entries";
         break;
      }
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::RSSI
// =============================================================================

#include "hanfun/core/rssi.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::RSSI::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::RSSI;

   switch (attribute)
   {
      case RSSI_ATTR:
         result = "RSSI";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::SUOTA
// =============================================================================

#include "hanfun/core/suota.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::SUOTA::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::SUOTA;

   switch (command)
   {
      case NEW_VERSION_AVAILABLE_CMD /* CHECK_VERSION_CMD */:
         result = "New Version Available/Check Version";
         break;
      case UPGRADE_COMPLETE_CMD:
         result = "Upgrade Complete";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::Time
// =============================================================================

#include "hanfun/core/time.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::Time::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::Time;

   switch (attribute)
   {
      case TIME_ATTR:
         result = "Time";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::Time::Date &date)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill('0');

   using namespace HF::Core::Time;

   Date invalid;

   if (memcmp(&date, &invalid, sizeof(Date)) == 0)
   {
      stream << "Invalid Time";
   }
   else
   {
      stream << std::setw(4) << std::setfill('0') << YEARS_BASE + date.year << "-";
      stream << std::setw(2) << std::setfill('0') << date.month << "-";
      stream << std::setw(2) << std::setfill('0') << date.day << " ";
      stream << std::setw(2) << std::setfill('0') << date.hour << ":";
      stream << std::setw(2) << std::setfill('0') << date.minute << ":";
      stream << std::setw(2) << std::setfill('0') << date.second;
   }

   stream << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::GroupTable
// =============================================================================

#include "hanfun/core/group_table.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::GroupTable::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::GroupTable;

   switch (command)
   {
      case ADD_CMD:
         result = "Add";
         break;
      case REMOVE_CMD:
         result = "Remove";
         break;
      case REMOVE_ALL_CMD:
         result = "Remove All";
         break;
      case READ_ENTRIES_CMD:
         result = "Read Entries";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::GroupTable::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::GroupTable;

   switch (attribute)
   {
      case NUMBER_OF_ENTRIES_ATTR:
         result = "Number Of Entries";
         break;
      case NUMBER_OF_MAX_ENTRIES_ATTR:
         result = "Number Of Max Entries";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::BatchProgramManagement
// =============================================================================

#include "hanfun/core/batch_program_management.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::BatchProgramManagement::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::BatchProgramManagement;

   switch (command)
   {
      case DEFINE_PROGRAM_CMD:
         result = "Define Program";
         break;
      case INVOKE_PROGRAM_CMD:
         result = "Invoke Program";
         break;
      case DELETE_PROGRAM_CMD:
         result = "Delete Program";
         break;
      case DELETE_ALL_PROGRAMS_CMD:
         result = "Delete All Programs";
         break;
      case GET_PROGRAM_ACTIONS_CMD:
         result = "Get Program Actions";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream,
                         const HF::Core::BatchProgramManagement::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::BatchProgramManagement;

   switch (attribute)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
         result = "Maximum Number Of Entries";
         break;
      case NUMBER_OF_ENTRIES_ATTR:
         result = "Number Of Entries";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// HF::Core::EventScheduling
// =============================================================================

#include "hanfun/core/event_scheduling.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::Scheduling::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::Scheduling;

   switch (command)
   {
      case ACTIVATE_SCHEDULER_CMD:
         result = "Activate Scheduler";
         break;
      case DEFINE_CMD:
         result = "Define Event";
         break;
      case UPDATE_STATUS_CMD:
         result = "Update Event Status";
         break;
      case GET_ENTRY_CMD:
         result = "Get Event Entry";
         break;
      case DELETE_CMD:
         result = "Delete Event";
         break;
      case DELETE_ALL_CMD:
         result = "Delete All Events";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Core::Scheduling::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   std::string result         = "Unknown";

   using namespace HF::Core::Scheduling;

   switch (attribute)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
         result = "Maximum Number Of Entries";
         break;
      case NUMBER_OF_ENTRIES_ATTR:
         result = "Number Of Entries";
         break;
      case STATUS_ATTR:
         result = "Status";
         break;
      default:
         break;
   }

   stream << result << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// Protocol
// =============================================================================

#include "hanfun/protocol.h"

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Protocol::Address &address)
{
   stream << "M:" << address.mod << " "
          << "D:" << Hex<uint16_t>(address.device) << " "
          << "U:" << Hex<uint8_t>(address.unit);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Protocol::Message::Type type)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   using namespace HF::Protocol;

   std::string result = "Unknown";

   switch (type)
   {
      case Message::Type::COMMAND_REQ:
      {
         result = "COMMAND_REQ";
         break;
      }
      case Message::Type::COMMAND_RESP_REQ:
      {
         result = "COMMAND_RESP_REQ";
         break;
      }
      case Message::Type::COMMAND_RES:
      {
         result = "COMMAND_RES";
         break;
      }
      case Message::Type::GET_ATTR_REQ:
      {
         result = "GET_ATTR_REQ";
         break;
      }
      case Message::Type::GET_ATTR_RES:
      {
         result = "GET_ATTR_RES";
         break;
      }
      case Message::Type::SET_ATTR_REQ:
      {
         result = "SET_ATTR_REQ";
         break;
      }
      case Message::Type::SET_ATTR_RESP_REQ:
      {
         result = "SET_ATTR_RESP_REQ";
         break;
      }
      case Message::Type::SET_ATTR_RES:
      {
         result = "SET_ATTR_RES";
         break;
      }
      case Message::Type::GET_ATTR_PACK_REQ:
      {
         result = "GET_ATTR_PACK_REQ";
         break;
      }
      case Message::Type::GET_ATTR_PACK_RES:
      {
         result = "GET_ATTR_PACK_RES";
         break;
      }
      case Message::Type::SET_ATTR_PACK_REQ:
      {
         result = "SET_ATTR_PACK_REQ";
         break;
      }
      case Message::Type::SET_ATTR_PACK_RESP_REQ:
      {
         result = "SET_ATTR_PACK_RESP_REQ";
         break;
      }
      case Message::Type::SET_ATTR_PACK_RES:
      {
         result = "SET_ATTR_PACK_RES";
         break;
      }
      case Message::Type::ATOMIC_SET_ATTR_PACK_REQ:
      {
         result = "ATOMIC_SET_ATTR_PACK_REQ";
         break;
      }
      case Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ:
      {
         result = "ATOMIC_SET_ATTR_PACK_RESP_REQ";
         break;
      }
      case Message::Type::ATOMIC_SET_ATTR_PACK_RES:
      {
         result = "ATOMIC_SET_ATTR_PACK_RES";
         break;
      }
      default:
         break;
   }

   stream << Hex<uint8_t>((uint8_t) type) << " (" << result << ")";

   stream << std::setfill(f) << std::setiosflags(ff);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Protocol::Message &message)
{
   int8_t old_width = log_width;

   if (log_width < 0)
   {
      log_width = 12;
   }

   using namespace HF::Protocol;
   using namespace HF::Interfaces;
   using namespace HF::Core;

   stream << ALIGN();
   stream << "Reference | " << Hex<uint8_t>(message.reference) << NL_ALIGN()
          << "Type | " << static_cast<HF::Protocol::Message::Type>(message.type) << NL_ALIGN()
          << "Interface | " << message.itf << " M:" << Hex<uint8_t>(message.itf.member) << " (";

   switch (message.type)
   {
      case Message::Type::COMMAND_RESP_REQ:
      case Message::Type::COMMAND_REQ:
      case Message::Type::COMMAND_RES:
      {
         switch (message.itf.id)
         {
            /* Core Service & Interface [C] */
            case HF::Interface::ATTRIBUTE_REPORTING:
               stream << static_cast<AttributeReporting::CMD>(message.itf.member);
               break;
            case HF::Interface::BIND_MANAGEMENT:
               stream << static_cast<BindManagement::CMD>(message.itf.member);
               break;
            case HF::Interface::DEVICE_MANAGEMENT:
               stream << static_cast<DeviceManagement::CMD>(message.itf.member);
               break;
            case HF::Interface::GROUP_TABLE:
               stream << static_cast<GroupTable::CMD>(message.itf.member);
               break;
            case HF::Interface::SUOTA:
               stream << static_cast<SUOTA::CMD>(message.itf.member);
               break;
            case HF::Interface::BATCH_PROGRAM_MANAGEMENT:
               stream << static_cast<BatchProgramManagement::CMD>(message.itf.member);
               break;
            case HF::Interface::EVENT_SCHEDULING:
               stream << static_cast<Scheduling::CMD>(message.itf.member);
               break;
            /* Interfaces [C] */
            case HF::Interface::ALERT:
               stream << static_cast<Alert::CMD>(message.itf.member);
               break;
            case HF::Interface::LEVEL_CONTROL:
               stream << static_cast<LevelControl::CMD>(message.itf.member);
               break;
            case HF::Interface::ON_OFF:
               stream << static_cast<OnOff::CMD>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_POWER_METER:
               stream << static_cast<SimplePowerMeter::CMD>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_BUTTON:
               stream << static_cast<SimpleButton::CMD>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_VISUAL_EFFECTS:
               stream << static_cast<SimpleVisualEffects::CMD>(message.itf.member);
               break;
            case HF::Interface::COLOUR_CONTROL:
               stream << static_cast<ColourControl::CMD>(message.itf.member);
               break;
            /* Unknown [C] */
            default:
               stream << "Unknown";
               break;
         }

         stream << " [C]";

         break;
      }
      case Message::Type::GET_ATTR_REQ:
      case Message::Type::GET_ATTR_RES:
      case Message::Type::SET_ATTR_REQ:
      case Message::Type::SET_ATTR_RESP_REQ:
      case Message::Type::SET_ATTR_RES:
      {
         switch (message.itf.id)
         {
            /* Core Service & Interface [A] */
            case HF::Interface::ATTRIBUTE_REPORTING:
               stream << static_cast<AttributeReporting::Attributes>(message.itf.member);
               break;
            case HF::Interface::BIND_MANAGEMENT:
               stream << static_cast<BindManagement::Attributes>(message.itf.member);
               break;
            case HF::Interface::DEVICE_INFORMATION:
               stream << static_cast<DeviceInformation::Attributes>(message.itf.member);
               break;
            case HF::Interface::DEVICE_MANAGEMENT:
               stream << static_cast<DeviceManagement::Attributes>(message.itf.member);
               break;
            case HF::Interface::GROUP_TABLE:
               stream << static_cast<GroupTable::Attributes>(message.itf.member);
               break;
            case HF::Interface::RSSI:
               stream << static_cast<RSSI::Attributes>(message.itf.member);
               break;
            case HF::Interface::TIME:
               stream << static_cast<Time::Attributes>(message.itf.member);
               break;
            case HF::Interface::BATCH_PROGRAM_MANAGEMENT:
               stream << static_cast<BatchProgramManagement::Attributes>(message.itf.member);
               break;
            case HF::Interface::EVENT_SCHEDULING:
               stream << static_cast<Scheduling::Attributes>(message.itf.member);
               break;
            /* Interfaces [A] */
            case HF::Interface::ALERT:
               stream << static_cast<Alert::Attributes>(message.itf.member);
               break;
            case HF::Interface::LEVEL_CONTROL:
               stream << static_cast<LevelControl::Attributes>(message.itf.member);
               break;
            case HF::Interface::ON_OFF:
               stream << static_cast<OnOff::Attributes>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_POWER_METER:
               stream << static_cast<SimplePowerMeter::Attributes>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_BUTTON:
               stream << static_cast<SimpleButton::Attributes>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_AIR_PRESSURE:
               stream << static_cast<SimpleAirPressure::Attributes>(message.itf.member);
               break;
            case HF::Interface::SIMPLE_LIGHT_SENSOR:
               stream << static_cast<SimpleLightSensor::Attributes>(message.itf.member);
            case HF::Interface::COLOUR_CONTROL:
               stream << static_cast<ColourControl::Attributes>(message.itf.member);
               break;
            /* Unknown [A] */
            default:
               break;
         }

         stream << " [A]";

         break;
      }
      case Message::Type::GET_ATTR_PACK_REQ:
      case Message::Type::GET_ATTR_PACK_RES:
      case Message::Type::SET_ATTR_PACK_REQ:
      case Message::Type::SET_ATTR_PACK_RESP_REQ:
      case Message::Type::SET_ATTR_PACK_RES:
      case Message::Type::ATOMIC_SET_ATTR_PACK_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RES:
      {
         using namespace HF::Attributes;

         switch (message.itf.member)
         {
            case Pack::MANDATORY:
               stream << "Mandatory";
               break;
            case Pack::ALL:
               stream << "All";
               break;
            case Pack::DYNAMIC:
               stream << "Dynamic";
               break;
            default:
               stream << "Other";
               break;
         }

         stream << " [P]";

         break;
      }
      default:
         break;
   }

   stream << ")" << NL_ALIGN() << "Payload | " << message.payload;

   log_width = old_width;

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator<<(std::ostream &stream, const HF::Protocol::Packet &packet)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(0);

   int8_t old_width           = log_width;

   if (log_width < 0)
   {
      log_width = 15;
   }

   stream << std::right << std::setfill(' ') << std::setw(log_width);
   stream << "Packet | " << NL_ALIGN();

   stream << "Source | " << packet.source << NL_ALIGN();

   stream << "Destination | " << packet.destination << NL_ALIGN();

   stream << "Message | " << NL_ALIGN();
   stream << packet.message;

   stream << std::setfill(f) << std::setiosflags(ff);

   log_width = old_width;

   return stream;
}

// LCOV_EXCL_STOP
