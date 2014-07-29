// =============================================================================
/*!
 * \file       apps/node_app.cpp
 *
 * This file contains the implementation of the example application for a
 * HAN-FUN Node.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <unistd.h>

#include <forward_list>
#include <algorithm>

#include "node.h"

#include "common.h"

#include "application.h"

// =============================================================================
// Global Variables.
// =============================================================================

static Node node;

static SimpleLight *simple_light   = nullptr;

static SimpleSwitch *simple_switch = nullptr;

// =============================================================================
// Commands
// =============================================================================

COMMAND (Register, "r", "r:register device")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   node.unit0.device_management ()->register_device ();
}

COMMAND (Address, "a", "a:device address")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   LOG (APP) << "Device Address : " << node.address () << NL;
}

COMMAND (SimpleLight, "sl", "sl:set device as a simple light")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (simple_switch != nullptr)
   {
      delete simple_switch;
   }

   if (simple_light == nullptr)
   {
      simple_light = new SimpleLight (1, node);
   }

   LOG (APP) << "Device is now a Simple Light !" << NL;
}

COMMAND (SimpleSwitch, "ss", "ss:set device as a simple switch")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (simple_light != nullptr)
   {
      delete simple_light;
   }

   if (simple_switch == nullptr)
   {
      simple_switch = new SimpleSwitch (1, node);
   }

   LOG (APP) << "Device is now a Simple Switch !" << NL;
}

COMMAND (On, "on", "on:On Command")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (simple_switch != nullptr)
   {
      simple_switch->on ();
   }
   else
   {
      LOG (ERROR) << "Simple Switch not configured !" << NL;
   }
}

COMMAND (Off, "off", "off:Off Command")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (simple_switch != nullptr)
   {
      simple_switch->off ();
   }
   else
   {
      LOG (ERROR) << "Simple Switch not configured !" << NL;
   }
}

COMMAND (Toggle, "toggle", "toggle:Toggle Command")
{
   UNUSED (args);
   LOG (TRACE) << __PRETTY_FUNCTION__ << NL;

   if (simple_switch != nullptr)
   {
      simple_switch->toggle ();
   }
   else
   {
      LOG (ERROR) << "Simple Switch not configured !" << NL;
   }
}

// =============================================================================
// API
// =============================================================================

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

   transport.add (&node);

   COMMAND_ADD (Register);
   COMMAND_ADD (Address);
   COMMAND_ADD (SimpleLight);
   COMMAND_ADD (SimpleSwitch);
   COMMAND_ADD (On);
   COMMAND_ADD (Off);
   COMMAND_ADD (Toggle);

   HF::Application::Restore ();
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
   Restored ();
}
