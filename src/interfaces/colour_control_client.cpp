// =============================================================================
/*!
 * @file       src/interfaces/colour_control_client.cpp
 *
 * This file contains the implementation of the Colour Control interface : Client role.
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
// Colour Control Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::move_to_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_to_hue(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_TO_HUE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_hue(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_HUE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::step_hue
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::step_hue(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = STEP_HUE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_to_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_to_saturation(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_TO_SATURATION_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_saturation(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_SATURATION_CMD;

   send(addr, message);
}

// =============================================================================
// Client::step_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::step_saturation(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = STEP_SATURATION_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_to_hue_and_saturation
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_to_hue_and_saturation(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_TO_HUE_AND_SATURATION_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_to_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_to_xy(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_TO_XY_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_xy(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_XY_CMD;

   send(addr, message);
}

// =============================================================================
// Client::step_xy
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::step_xy(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = STEP_XY_CMD;

   send(addr, message);
}

// =============================================================================
// Client::move_to_colour_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::move_to_colour_temperature(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = MOVE_TO_COLOUR_TEMPERATURE_CMD;

   send(addr, message);
}

// =============================================================================
// Client::stop
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::stop(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::COLOUR_CONTROL;
   message.itf.member = STOP_CMD;

   send(addr, message);
}
