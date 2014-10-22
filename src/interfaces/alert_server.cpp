// =============================================================================
/*!
 * \file       src/interfaces/alert_server.cpp
 *
 * This file contains the implementation of the Alert interface : Server role.
 *
 * \version    1.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/alert.h"

using namespace HF;
using namespace HF::Protocol;
using namespace HF::Interfaces;

// =============================================================================
// Alert Interface : Server Role
// =============================================================================

//! Constructor
Alert::Server::Server()
{
   _state   = 0x0;
   _enabled = UINT32_MAX;
}

//! Destructor
Alert::Server::~Server()
{}

// =============================================================================
// Alert::Server::state
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Alert::Server::state ()
{
   return this->_state;
}

// =============================================================================
// Alert::Server::state
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Alert::Server::state (uint8_t index, bool state)
{
   if (index >= STATE_SIZE_BITS)
   {
      return false;
   }

   bool res = enabled (index);

   if (res)
   {
      uint32_t mask = 1U << index;

      uint32_t old  = this->_state;

      if (state)
      {
         this->_state |= mask;
      }
      else
      {
         this->_state &= ~mask;
      }

      State old_value (old, this);
      State new_value (this->_state, this);

      this->notify (old_value, new_value);
   }

   return res;
}

// =============================================================================
// Alert::Server::state
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Alert::Server::state (uint8_t index)
{
   if (index >= STATE_SIZE_BITS)
   {
      return false;
   }

   return (this->_state & (1U << index)) != 0;
}

// =============================================================================
// Alert::Server::clear
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::clear ()
{
   uint32_t old = this->_state;

   this->_state = 0;

   State old_value (old, this);
   State new_value (this->_state, this);

   notify (old_value, new_value);
}

// =============================================================================
// Alert::Server::enable
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::enable (uint8_t index)
{
   if (index > STATE_SIZE_BITS)
   {
      return;
   }

   uint32_t old = this->_enabled;

   this->_enabled |= (1U << index);

   Enable old_value (old, this);
   Enable new_value (this->_enabled, this);

   notify (old_value, new_value);
}

// =============================================================================
// Alert::Server::enableAll
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::enableAll ()
{
   uint32_t old = this->_enabled;

   this->_enabled = UINT32_MAX;

   Enable old_value (old, this);
   Enable new_value (this->_enabled, this);

   notify (old_value, new_value);
}

// =============================================================================
// Alert::Server::enabled
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Alert::Server::enabled ()
{
   return this->_enabled;
}

// =============================================================================
// Alert::Server::enabled
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Alert::Server::enabled (uint8_t index)
{
   if (index > STATE_SIZE_BITS)
   {
      return false;
   }

   return (this->_enabled & (1U << index)) != 0;
}

// =============================================================================
// Alert::Server::disable
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::disable (uint8_t index)
{
   if (index > STATE_SIZE_BITS)
   {
      return;
   }

   uint32_t old = this->_enabled;

   this->_enabled &= (~(1U << index));

   Enable old_value (old, this);
   Enable new_value (this->_enabled, this);

   notify (old_value, new_value);
}

// =============================================================================
// Alert::Server::disableAll
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::disableAll ()
{
   uint32_t old = this->_enabled;

   this->_enabled = 0;

   Enable old_value (old, this);
   Enable new_value (this->_enabled, this);

   notify (old_value, new_value);
}

// =============================================================================
// Alert::Server::disabled
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Alert::Server::disabled (uint8_t index)
{
   if (index > STATE_SIZE_BITS)
   {
      return true;
   }

   return (this->_enabled & (1U << index)) == 0;
}

// =============================================================================
// Alert::Server::status_cmd
// =============================================================================
/*!
 *
 */
// =============================================================================
Alert::Message *Alert::Server::create_status (uint16_t profile_uid)
{
   return new Alert::Message (profile_uid, this->_state);
}

// =============================================================================
// Alert::Server::status
// =============================================================================
/*!
 *
 */
// =============================================================================
void Alert::Server::status (Protocol::Address &addr, uint16_t profile_uid)
{
   Alert::Message alert_msg (profile_uid, this->_state);

   Protocol::Message message (alert_msg.size ());

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Alert::Server::uid ();
   message.itf.member = STATUS_CMD;

   alert_msg.pack (message.payload);

   send (addr, message);
}
