// =============================================================================
/*!
 * @file       src/debug.cpp
 *
 * This file contains the implementation of the debug helper functions.
 *
 * @version    1.2.0
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include <iomanip>
#include <algorithm>

#include "hanfun.h"
#include "hanfun/debug.h"

using namespace HF::Debug;

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
std::ostream &operator <<(std::ostream &stream, const uint8_t byte)
{
   if (stream == std::cout || stream == std::cerr)
   {
      std::ios_base::fmtflags ff = stream.flags ();
      char f                     = stream.fill ('0');

      stream << Hex <uint8_t>(byte);

      stream << std::setfill (f) << std::setiosflags (ff);
   }
   else
   {
      stream << byte;
   }

   return stream;
}

// =============================================================================
// Common
// =============================================================================

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Common::ByteArray &array)
{
   if (stream == std::cout || stream == std::cerr)
   {
      std::ios_base::fmtflags ff = stream.flags ();
      char f                     = stream.fill ('0');

      stream << std::left;
      stream << "(" << Hex <uint16_t>(array.size ()) << ") ";

      /* *INDENT-OFF* */
      std::for_each (array.begin (), array.end (), [&stream](uint8_t byte)
      {
         stream << Hex<uint8_t>(byte) << " ";
      });
      /* *INDENT-ON* */

      stream << std::setfill (f) << std::setiosflags (ff);
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
std::ostream &operator <<(std::ostream &stream, const HF::Common::Interface interface)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

   stream << "R:" << static_cast <HF::Interface::Role>(interface.role)
          << " UID:" << static_cast <HF::Interface::UID>(interface.id);

   stream << std::setfill (f) << std::setiosflags (ff);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::UID::UID &uid)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill ('0');

   switch (uid.raw ()->type ())
   {
      case HF::UID::NONE_UID:
      {
         stream << "uid: NONE";
         break;
      }
      case HF::UID::DECT_UID:
      {
         HF::UID::DECT *dect = (HF::UID::DECT *) uid.raw ();
         stream << "dect: ";

         for (uint8_t i = 0; i < HF::UID::DECT::length (); i++)
         {
            stream << Hex <uint8_t>((*dect)[i]);
         }

         break;
      }
      case HF::UID::MAC_UID:
      {
         HF::UID::MAC *mac = (HF::UID::MAC *) uid.raw ();
         stream << "mac: ";

         for (uint8_t i = 0; i < HF::UID::MAC::length () - 1; i++)
         {
            stream << Hex <uint8_t>((*mac)[i]) << ":";
         }

         stream << Hex <uint8_t>((*mac)[HF::UID::MAC::length () - 1]);

         break;
      }

      case HF::UID::URI_UID:
      {
         HF::UID::URI *uri = (HF::UID::URI *) uid.raw ();
         stream << "uri: " << uri->str ();
         break;
      }

      default:
         break;
   }

   stream << std::setfill (f) << std::setiosflags (ff);

   return stream;
}

// =============================================================================
// Protocol
// =============================================================================

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Protocol::Address &address)
{
   stream << "M:" << address.mod << " "
          << "D:" << Hex <uint16_t>(address.device) << " "
          << "U:" << Hex <uint8_t>(address.unit);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Protocol::Message::Type type)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint8_t>((uint8_t) type) << " (" << result << ")";

   stream << std::setfill (f) << std::setiosflags (ff);

   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Protocol::Message &message)
{
   int8_t old_width = log_width;

   if (log_width < 0)
   {
      log_width = 12;
   }

   using namespace HF::Protocol;
   using namespace HF::Interfaces;

   stream << ALIGN ();
   stream << "Reference | " << Hex <uint8_t>(message.reference) << NL_ALIGN ()
          << "Type | " << static_cast <HF::Protocol::Message::Type>(message.type) << NL_ALIGN ()
          << "Interface | " << message.itf << "M:" << Hex <uint8_t>(message.itf.member);

   std::string member = nullptr;

   switch (message.type)
   {
      case Message::Type::COMMAND_RESP_REQ:
      case Message::Type::COMMAND_REQ:
      case Message::Type::COMMAND_RES:
      {
         switch (message.itf.id)
         {
            case HF::Interface::ALERT:
               stream << "(" << static_cast <Alert::CMD>(message.itf.member) << ")";
               break;
            case HF::Interface::LEVEL_CONTROL:
               stream << "(" << static_cast <LevelControl::CMD>(message.itf.member) << ")";
               break;
            case HF::Interface::ON_OFF:
               stream << "(" << static_cast <OnOff::CMD>(message.itf.member) << ")";
               break;
            case HF::Interface::SIMPLE_POWER_METER:
               stream << "(" << static_cast <SimplePowerMeter::CMD>(message.itf.member) << ")";
               break;
            default:
               break;
         }

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
            case HF::Interface::ALERT:
               stream << "(" << static_cast <Alert::Attributes>(message.itf.member) << ")";
               break;
            case HF::Interface::LEVEL_CONTROL:
               stream << "(" << static_cast <LevelControl::Attributes>(message.itf.member) << ")";
               break;
            case HF::Interface::ON_OFF:
               stream << "(" << static_cast <OnOff::Attributes>(message.itf.member) << ")";
               break;
            case HF::Interface::SIMPLE_POWER_METER:
               stream << "(" << static_cast <SimplePowerMeter::Attributes>(message.itf.member) << ")";
               break;
            default:
               break;
         }

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
               stream << "(Mandatory)";
               break;
            case Pack::ALL:
               stream << "(All)";
               break;
            case Pack::DYNAMIC:
               stream << "(Dynamic)";
               break;
            default:
               stream << "(Other)";
               break;
         }

         break;
      }
      default:
         break;
   }

   stream << NL_ALIGN () << "Payload | " << message.payload;

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
std::ostream &operator <<(std::ostream &stream, const HF::Protocol::Packet &packet)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (0);

   int8_t old_width           = log_width;

   if (log_width < 0)
   {
      log_width = 15;
   }

   stream << std::right << std::setfill (' ') << std::setw (log_width);
   stream << "Packet | " << NL_ALIGN ();

   stream << "Source | " << packet.source << NL_ALIGN ();

   stream << "Destination | " << packet.destination << NL_ALIGN ();

   stream << "Message | " << NL_ALIGN ();
   stream << packet.message;

   stream << std::setfill (f) << std::setiosflags (ff);

   log_width = old_width;

   return stream;
}

// =============================================================================
// Interfaces
// =============================================================================

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, HF::Interface::Role role)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint8_t>((uint8_t) role) << " (" << result << ")";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interface::UID uid)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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
      case HF::Interface::UID::GROUP_MANGEMENT:
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
      default:
      {
         if (uid >= HF::Interface::UID::RESERVED && uid < HF::Interface::UID::MAX_UID)
         {
            result = "Proprietary";
         }

         break;
      }
   }

   stream << Hex <uint16_t>((uint16_t) uid) << " (" << result << ")";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::Alert::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) command) << " (" << result << " [C])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::Alert::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) attribute) << " (" << result << " [A])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::LevelControl::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) command) << " (" << result << " [C])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::LevelControl::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) attribute) << " (" << result << " [A])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::OnOff::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) command) << " (" << result << " [C])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::OnOff::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) attribute) << " (" << result << " [A])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::SimplePowerMeter::CMD command)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) command) << " (" << result << " [C])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}

// =============================================================================
// operator <<
// =============================================================================
/*!
 *
 */
// =============================================================================
std::ostream &operator <<(std::ostream &stream, const HF::Interfaces::SimplePowerMeter::Attributes attribute)
{
   std::ios_base::fmtflags ff = stream.flags ();
   char f                     = stream.fill (' ');

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

   stream << Hex <uint16_t>((uint16_t) attribute) << " (" << result << " [A])";

   stream << std::setfill (f) << std::setiosflags (ff);
   return stream;
}
