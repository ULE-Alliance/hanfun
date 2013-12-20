// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/alert.h
 *
 * This file contains the definitions for the Alert interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_ITF_ALERT_H
#define HF_ITF_ALERT_H

#include "hanfun/common.h"

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

// =============================================================================
// Defines
// =============================================================================

#define STATE_SIZE_BITS   (sizeof(uint32_t) * 8)

// =============================================================================
// API
// =============================================================================

namespace HF
{
   namespace Interfaces
   {
      /*!
       * Alert Interface : Parent.
       *
       * This is the parent class for the Alert interface implementation.
       */
      struct Alert:public Base <Interface::ALERT>
      {
         //! Command IDs.
         enum
         {
            STATUS_CMD = 0x01       //! Alert Status Command ID.
         };

         struct Message:public Serializable
         {
            uint16_t type;        //!< Unit Type that generated the message.
            uint32_t state;       //!< Current state of the server.

            Message(uint16_t type = 0, uint32_t state = 0);

            //! \see HF::Protocol::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };
      };

      /*!
       * Alert Interface : Server side implementation.
       *
       * This class provides the server side of the Alert interface.
       */
      class AlertServer:public InterfaceRole <Alert, Interface::SERVER_ROLE>
      {
         protected:

         uint32_t _state;        //!< Alerts state
         uint32_t _enabled;      //!< Alerts enable state

         public:

         //! Constructor.
         AlertServer();

         //! Destructor
         virtual ~AlertServer();

         // ======================================================================
         // State API
         // ======================================================================

         /*!
          * Return the current state for all the alerts as a bitmask.
          *
          * @return  the bitmask of the alert states.
          */
         uint32_t state ();

         /*!
          * Set the alert given by \c index to \c value.
          *
          * The alert entry \b MUST be enabled in order for the state to be set.
          *
          * @param [in] index   index of the alert to set : <tt>[0,31]</tt>.
          * @param [in] value   state to set the index to.
          *
          * @return  \c true if the alert state was set or \c false otherwise.
          */
         bool state (uint8_t index, bool value);

         /*!
          * Get the state of the alert given by \c index.
          *
          * \note This will return \c false if \c index is greater than 31.
          *
          * @param [in] index   index of the alert to get the state from : [0,31].
          *
          * @return  true if the alert is active, false otherwise.
          */
         bool state (uint8_t index);

         /*!
          * Clear all the alerts.
          */
         void clear ();

         // ======================================================================
         // Enable API
         // ======================================================================

         /*!
          * Enable alert at \c index.
          *
          * @param [in] index   index to enable the alert : [0,31].
          */
         void enable (uint8_t index);

         /*!
          * Enable all alerts.
          */
         void enableAll ();

         /*!
          * Check if alert at \c index is enabled.
          *
          * @param [in] index   index of the alert to get the enabled state : [0,31].
          *
          * @retval  \c true if the alert at \c index is enabled,
          * @retval  \c false otherwise.
          */
         bool enabled (uint8_t index);

         /*!
          * Return the bitmask of the enable state of the alerts.
          *
          * @return  the bitmask of the enable state of the alerts.
          */
         uint32_t enabled ();

         /*!
          * Disable the alert at \c index.
          *
          * @param [in] index   index of the alert to disable : [0,31].
          */
         void disable (uint8_t index);

         /*!
          * Check if alert at \c index is disabled.
          *
          * @param [in] index   index of the alert to get the enable state : [0,31].
          *
          * @retval  \c true if the alert at \c index is disabled
          * @retval  \c false otherwise
          */
         bool disabled (uint8_t index);

         /*!
          * Disable all alerts.
          */
         void disableAll ();

         // ======================================================================
         // Commands
         // ======================================================================
         //! \name Commands
         //! @{

         /*!
          * Send a HAN-FUN message containing a \c Alert::STATUS_CMD, to the given
          * network address.
          *
          * @param [in] addr       the network address to send the message to.
          * @param [in] unit_type  the unit type ( \see hf_profile_uid_t ) sending the message.
          */
         void status (Protocol::Message::Address &addr, uint16_t unit_type);

         //! @}
         // ======================================================================

         protected:

         /*!
          * Create a HAN-FUN message containing a \c Alert::STATUS_CMD, to be sent to the
          * network.
          *
          * @param unit_type     unit type (i.e. profile) source for the alert status message.
          *
          * @return              a pointer to a \c Alert::Message, or \c nullptr if the message can't
          *                      be allocated.
          */
         Alert::Message *status (uint16_t unit_type);
      };

      /*!
       * Alert Interface : Client side implementation.
       *
       * This class provides the client side of the Alert interface.
       */
      struct AlertClient:public InterfaceRole <Alert, Interface::CLIENT_ROLE>
      {
         //! \see Interface::handle.
         bool handle (Protocol::Message &message, ByteArray &payload, size_t offset);

         // ======================================================================
         // Events
         // ======================================================================
         //! \name Events
         //! @{

         /*!
          * Callback function called when a status update message is received
          * from an Alert server.
          */
         virtual void status (Message &message) = 0;

         //! @}
         // ======================================================================
      };

   }  // namespace Interfaces

}  // namespace HF

/*! @} */

#endif /* HF_ITF_ALERT_H */