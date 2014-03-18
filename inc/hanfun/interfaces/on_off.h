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
      namespace OnOff
      {
         struct Server;
      }

      HF::Attributes::IAttribute *create_attribute (OnOff::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the On-Off interface.
       */
      namespace OnOff
      {
         //! Command IDs.
         typedef enum
         {
            ON_CMD     = 0x01,   //! On Command ID.
            OFF_CMD    = 0x02,   //! Off Command ID.
            TOGGLE_CMD = 0x03,   //! Toggle Command ID.
         } CMD;

         //! Attributes
         typedef enum
         {
            STATE_ATTR    = 0x01, //!< State attribute UID.
            __LAST_ATTR__ = STATE_ATTR,
         } Attributes;

         struct State:public HF::Attributes::Attribute <bool>
         {
            static constexpr uint8_t ID        = STATE_ATTR;
            static constexpr bool    WRITABBLE = false;

            private:

            State(bool state = false):
               Attribute <bool>(Interface::ON_OFF, ID, state, WRITABBLE)
            {}
         };

         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * On-Off Interface : Parent.
          *
          * This is the parent class for the On-Off interface implementation.
          */
         struct Base:public Interfaces::Base <Interface::ON_OFF>
         {};

         /*!
          * On-Off Interface : Server side implementation.
          *
          * This class provides the server side of the On-Off interface.
          */
         class Server:public InterfaceRole <OnOff::Base, Interface::SERVER_ROLE>
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
            Server():_state (false) {}

            //! Destructor
            virtual ~Server() {}

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

            HF::Attributes::IAttribute *attribute (uint8_t uid)
            {
               return Interfaces::create_attribute (this, uid);
            }

            friend HF::Attributes::IAttribute *Interfaces::create_attribute (OnOff::Server *server, uint8_t uid);

            protected:

            //! \see AbstractInterface::handle_command
            Result handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset);

            //! \see AbstractInterface::attributes
            HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED (pack_id);
               /* *INDENT-OFF* */
            return HF::Attributes::uids_t ({ OnOff::STATE_ATTR });
               /* *INDENT-ON* */
            }
         };

         /*!
          * On-Off Interface : Client side implementation.
          *
          * This class provides the client side of the On-Off interface.
          */
         struct Client:public InterfaceRole <OnOff::Base, Interface::CLIENT_ROLE>
         {
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
             * Send a \c OnOff::ON_CMD message to the broadcast address.
             */
            void on ()
            {
               Protocol::Address addr;
               on (addr);
            }

            /*!
             * Send a \c OnOff::OFF_CMD message to the device at the given address.
             *
             * @param addr    the address of the device to send the message to.
             */
            void off (Protocol::Address &addr);

            /*!
             * Send a \c OnOff::OFF_CMD message to the broadcast address.
             */
            void off ()
            {
               Protocol::Address addr;
               off (addr);
            }

            /*!
             * Send a \c OnOff::TOGGLE_CMD message to the device at the given address.
             *
             * @param addr    the address of the device to send the message to.
             */
            void toggle (Protocol::Address &addr);

            /*!
             * Send a \c OnOff::TOGGLE_CMD message to the broadcast address.
             */
            void toggle ()
            {
               Protocol::Address addr;
               toggle (addr);
            }

            //@}
            // =============================================================================
         };

      }  // namespace OnOff

   }  // namespace Interfaces

}   // namespace HF

#endif /* HF_ITF_ON_OFF_H */
