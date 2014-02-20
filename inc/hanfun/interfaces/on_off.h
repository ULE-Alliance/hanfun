// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/on_off.h
 *
 * This file contains the definitions for the OnOff interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#ifndef HF_ITF_ON_OFF_H
#define HF_ITF_ON_OFF_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      struct OnOffServer;

      IAttribute *create_attribute (OnOffServer *server, uint8_t uid);

      /*!
       * On-Off Interface : Parent.
       *
       * This is the parent class for the On-Off interface implementation.
       */
      class OnOff:public Base <Interface::ON_OFF>
      {
         public:

         //! Command IDs.
         typedef enum
         {
            ON_CMD     = 0x01,       //! On Command ID.
            OFF_CMD    = 0x02,       //! Off Command ID.
            TOGGLE_CMD = 0x03,       //! Toggle Command ID.
         } CMD;

         //! Attributes
         typedef enum
         {
            STATE_ATTR    = 0x01,  //!< State attribute UID.
            __LAST_ATTR__ = STATE_ATTR,
         } Attributes;

         struct State:public Attribute <bool>
         {
            static constexpr uint8_t ID        = STATE_ATTR;
            static constexpr bool    WRITABBLE = false;

            private:

            State(bool state = false):
               Attribute <bool>(Interface::ON_OFF, ID, state, WRITABBLE)
            {}
         };

         static IAttribute * create_attribute(uint8_t uid)
         {
            return Interfaces::create_attribute( (OnOffServer *)nullptr, uid);
         }
      };

      /*!
       * On-Off Interface : Server side implementation.
       *
       * This class provides the server side of the On-Off interface.
       */
      class OnOffServer:public InterfaceRole <OnOff, Interface::SERVER_ROLE>
      {
         protected:

         /*!
          * Current server state.
          *
          *  * \c true : the interface is ON.
          *  * \c false: the interface is OFF.
          */
         bool _state;

         public:

         //! Constructor
         OnOffServer():_state (false) {}

         //! Destructor
         virtual ~OnOffServer() {}

         // ======================================================================
         // Events
         // ======================================================================
         //! \name Events
         //! @{

         /*!
          * Callback that is called when a \c ON_CMD message is received.
          */
         virtual void on ();

         /*!
          * Callback that is called when a \c OFF_CMD message is received.
          */
         virtual void off ();

         /*!
          * Callback that is called when a \c TOGGLE_CMD message is received.
          */
         virtual void toggle ();

         //! @}

         // =============================================================================
         // Get/Set API.
         // =============================================================================

         /*!
          * Setter : set the state attribute to given value.
          *
          * @param state   \c true the interface is on, \c false the interface is off.
          */
         void state (bool state);

         /*!
          * Getter : get the current state of the interface :
          *    * \c true the interface is ON.
          *    * \c false the interface is OFF.
          *
          * @retval  \c true if the interface is ON.
          * @retval  \c false if the interface is OFF.
          */
         bool state ();

         // =============================================================================
         // Attribute API.
         // =============================================================================

         IAttribute *attribute (uint8_t uid)
         {
            return Interfaces::create_attribute (this, uid);
         }

         friend IAttribute *Interfaces::create_attribute (OnOffServer *server, uint8_t uid);

         protected:

         //! \see AbstractInterface::handle_command
         Result handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset);

         //! \see AbstractInterface::attribute_uids
         attribute_uids_t attribute_uids (bool optional = false) const
         {
            UNUSED (optional);
            /* *INDENT-OFF* */
            return attribute_uids_t ({ OnOff::STATE_ATTR });
            /* *INDENT-ON* */
         }
      };

      /*!
       * On-Off Interface : Client side implementation.
       *
       * This class provides the client side of the On-Off interface.
       */
      class OnOffClient:public InterfaceRole <OnOff, Interface::CLIENT_ROLE>
      {
         public:

         // ======================================================================
         // Commands
         // ======================================================================
         // \name Commands
         //@{

         /*!
          * Send a \c OnOff::ON_CMD message to the device at the given address.
          *
          * @param addr    the address of the device to send the message to.
          */
         void on (Protocol::Address &addr);

         /*!
          * Send a \c OnOff::OFF_CMD message to the device at the given address.
          *
          * @param addr    the address of the device to send the message to.
          */
         void off (Protocol::Address &addr);

         /*!
          * Send a \c OnOff::TOGGLE_CMD message to the device at the given address.
          *
          * @param addr    the address of the device to send the message to.
          */
         void toggle (Protocol::Address &addr);

         //@}
         // =============================================================================
      };

   }  // namespace Interfaces

}   // namespace HF

#endif /* HF_ITF_ON_OFF_H */
