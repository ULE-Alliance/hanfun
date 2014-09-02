// =============================================================================
/*!
 * \file       apps/base_app.cpp
 *
 * This file contains an example for a HAN-FUN base application.
 *
 * \version    0.4.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

#include <unistd.h>

#include <forward_list>
#include <algorithm>

#include "hanfun.h"

#include "base.h"

#include "application.h"

#include "common.h"

#include "json/json.h"

// =============================================================================
// Defines
// =============================================================================

#ifndef HF_APP_CONFIG_FILE
   #define HF_APP_CONFIG_FILE   "./hanfun.json"
#endif

// =============================================================================
// Global Variables
// =============================================================================

static Base base;

// =============================================================================
// Commands
// =============================================================================

COMMAND (ListRegs, "lr", "lr:list registrations.")
{
   UNUSED (args);

   auto devices = base.unit0 ()->device_management ()->entries ();

   LOG (APP) << std::setfill (' ');
   LOG (APP) << "HAN-FUN" << "  Registered Devices (" << (int) devices.size () << "):" << NL;

   /* *INDENT-OFF* */
   std::for_each(devices.begin(), devices.end(), [](const HF::Core::DeviceManagement::Device *device)
   {
      LOG (APP) << std::setw (9) << (base.link (device->address) != nullptr ? "+ " : "- ");
      LOG (APP) << std::setw (5) << device->address << " | ";
      LOG (APP) << device->uid << NL;
   });
   /* *INDENT-ON* */
}

COMMAND (ListBinds, "lb", "lb:list binds.")
{
   UNUSED (args);

   HF::Core::BindManagement::Entries &entries = base.unit0 ()->bind_management ()->entries;

   LOG (APP) << "HAN-FUN Binds (" << entries.size () << "):" << NL;
   std::for_each (entries.begin (), entries.end (), [](const HF::Core::BindManagement::Entry &entry)
                  {
                     LOG (APP) << "       - "
                               << entry.source.device << ":" << (int) entry.source.unit << " -> "
                               << entry.destination.device << ":" << (int) entry.destination.unit << NL;
                  }
                 );
}

COMMAND (Register, "r", "r 1 x:register device x.|r 0:exit registration mode.")
{
   if (args.size () > 0 && args[0] == "0") //!< Disable Registration
   {
#ifdef HF_APP_EXT_REG

      if (HF::Application::Registration (false))
      {
         LOG (INFO) << "Disable Registration mode: SUCCESS" << NL;
      }
      else
      {
         LOG (WARN) << "Disable Registration mode: FAIL" << NL;
      }

#endif
   }
   else if (args.size () > 1 && args[0] == "1") //!< Enable Registration
   {
      uint16_t address = STRTOL (args[1]);

      if (base.unit0 ()->device_management ()->available (address) && address != 0 &&
          address < HF::Protocol::BROADCAST_ADDR)
      {
         base.unit0 ()->device_management ()->next_address (address);
#ifdef HF_APP_EXT_REG

         if (HF::Application::Registration (true))
         {
            LOG (INFO) << "Enable Registration mode: SUCCESS" << NL;
         }
         else
         {
            LOG (WARN) << "[HANFUN] Enable Registration mode: FAIL" << NL;
         }

#endif
         LOG (INFO) << "Next Registration will have address : " << (int) address << NL;
      }
      else
      {
         LOG (ERROR) << "Registration impossible: address " << (int) address
                     << "  not available." << NL;
      }

      return;
   }

   LOG (APP) << "r 0      : Disable Registration Mode." << NL;
   LOG (APP) << "r 1 x    : Enable Registration Mode (Register Device x)." << NL;
}

COMMAND (Deregister, "d", "d x:de-register device x.")
{
   if (args.size () < 1)
   {
      LOG (APP) << "d x      : deregister device x. " << NL;
      return;
   }

   uint16_t address = STRTOL (args[0]);

#ifdef HF_APP_EXT_REG
   // External de-registration.
   HF::Application::Deregister (address);
#endif

   /* HAN-FUN de-registration */
   bool res = base.unit0 ()->device_management ()->deregister (address);
   LOG (INFO) << "Device " << (int) address << " de-registration: "
              << (res ? "SUCCESS" : "FAIL") << " !" << NL;
}

COMMAND (Bind, "b", "b x y:associate device x with device y. (bind)")
{
   if (args.size () < 2)
   {
      LOG (APP) << "b x y    :associate device x with device y. (bind)";
   }

   uint16_t arg1 = STRTOL (args[0]);
   uint16_t arg2 = STRTOL (args[1]);

   uint8_t  err  = base.bind (arg1, arg2);

   switch (err)
   {
      case 0:
      {
         LOG (INFO) << "Bind created !" << NL;
         break;
      }
      case 1:
      {
         LOG (WARN) << "Bind already created !" << NL;
         break;
      }
      case 2:
      case 3:
      {
         LOG (ERROR) << "Bind impossible: " << NL;

         if ((err & 0x01) != 0)
         {
            LOG (ERROR) << "  - Second device does not exist !" << NL;
         }
         else
         {
            LOG (ERROR) << "  - Fist device does not exist !" << NL;
         }

         break;
      }
      default:
         LOG (ERROR) << "No match for bind !" << NL;
         break;
   }
}

COMMAND (Unbind, "u", "u x y:unbind device x with y.")
{
   UNUSED (args);

   if (args.size () < 2)
   {
      LOG (APP) << "u x y   : unbind device x with y." << NL;
   }

   uint16_t arg1 = STRTOL (args[0]);
   uint16_t arg2 = STRTOL (args[1]);

   if (base.unbind (arg1, arg2))
   {
      LOG (INFO) << "Bind: " << arg1 << " - " << arg2 << " removed !" << NL;
   }
   else
   {
      LOG (ERROR) << "Bind: does not exist !" << NL;
   }
}

// =============================================================================
// HF::Application::Initialize
// =============================================================================
/*!
 *
 */
// =============================================================================
void HF::Application::Initialize (HF::Transport::Layer &transport)
{
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   transport.initialize ();

   transport.add (&base);

   COMMAND_ADD (ListRegs);
   COMMAND_ADD (ListBinds);
   COMMAND_ADD (Register);
   COMMAND_ADD (Deregister);
   COMMAND_ADD (Bind);
   COMMAND_ADD (Unbind);

   Restore ();
}

// =============================================================================
// HF::Application::Save
// =============================================================================
/*!
 *
 */
// =============================================================================
void HF::Application::Save ()
{
   Json::Value root;
   Json::StyledWriter writer;
   std::ofstream ofs (HF_APP_CONFIG_FILE);

   base.unit0 ()->device_management ()->save (root["core"]["device_management"]);
   base.unit0 ()->bind_management ()->save (root["core"]["bind_management"]);

   if (ofs.is_open ())
   {
      ofs << root;
      ofs.close ();
   }

   Saved ();
}

// =============================================================================
// HF::Application::Restore
// =============================================================================
/*!
 *
 */
// =============================================================================
void HF::Application::Restore ()
{
   Json::Reader  reader;
   Json::Value   root;

   std::ifstream ifs (HF_APP_CONFIG_FILE);

   if (reader.parse (ifs, root, false) == false)
   {
      LOG (WARN) << "Reading configuration file !!" << reader.getFormattedErrorMessages () << NL;
   }
   else
   {
      base.unit0 ()->device_management ()->restore (root["core"]["device_management"]);
      base.unit0 ()->bind_management ()->restore (root["core"]["bind_management"]);
   }

   ifs.close ();

   Restored ();
}
