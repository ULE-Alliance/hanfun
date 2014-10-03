// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/level_control.h
 *
 * This file contains the definitions for the Level Control interface.
 *
 * \version    1.0.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_ITF_LEVEL_CONTROL_H
#define HF_ITF_LEVEL_CONTROL_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      namespace LevelControl
      {
         class Server;
      }

      HF::Attributes::IAttribute *create_attribute (LevelControl::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Level Control interface
       */
      namespace LevelControl
      {
         //! Attributes
         typedef enum
         {
            LEVEL_ATTR    = 0x01,      //!< State attribute UID.
            __LAST_ATTR__ = LEVEL_ATTR,
         } Attributes;

         struct Level:public HF::Attributes::Attribute <uint8_t>
         {
            static constexpr uint8_t ID        = LEVEL_ATTR;
            static constexpr bool    WRITABBLE = true;

            Level(uint8_t level = 0, HF::Interface *owner = nullptr):
               Attribute <uint8_t>(Interface::ALERT, ID, owner, level, WRITABBLE)
            {}
         };

         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * Level Control Interface : Parent.
          *
          * This is the parent class for the Level Control interface implementation.
          */
         struct Base:public Interfaces::Base <Interface::LEVEL_CONTROL>
         {
            protected:

            Base() {}

            using Interfaces::Base <Interface::LEVEL_CONTROL>::payload_size;

            size_t payload_size (Protocol::Message::Interface &itf) const
            {
               UNUSED (itf);
               return payload_size_helper <Level>();
            }
         };

         /*!
          * Level Control Interface : Server side implementation.
          *
          * This class provides the server side of the Level Control interface.
          */
         class Server:public InterfaceRole <LevelControl::Base, Interface::SERVER_ROLE>
         {
            protected:

            //! Current level value.
            uint8_t _level;

            public:

            //! Constructor
            Server():_level (0) {}

            // ======================================================================
            // API
            // ======================================================================

            /*!
             * Getter for the current level.
             *
             * @return  the current level.
             */
            uint8_t level ();

            /*!
             * Setter for the server level.
             *
             * @param new_level  the new level value to use.
             */
            void level (uint8_t new_level);

            // =============================================================================
            // Events
            // =============================================================================
            //! \name Events
            //! @{

            /*!
             * Callback for a \c SET_LEVEL_CMD message.
             *
             * @param [in] new_level    the new level value to use.
             */
            virtual void level_change (uint8_t new_level);

            //! @}
            // =============================================================================

            // =============================================================================
            // Attributes API
            // =============================================================================

            //! \see Interface::attribute
            HF::Attributes::IAttribute *attribute (uint8_t uid)
            {
               return Interfaces::create_attribute (this, uid);
            }

            //! \see AbstractInterface::attributes
            HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED (pack_id);
               /* *INDENT-OFF* */
               return HF::Attributes::UIDS ({ LevelControl::LEVEL_ATTR });
               /* *INDENT-ON* */
            }

            friend HF::Attributes::IAttribute *Interfaces::create_attribute (LevelControl::Server *server, uint8_t uid);

            protected:

            //! \see AbstractInterface::handle_attribute
            Common::Result handle_attribute (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);
         };

         /*!
          * Level Control Interface : Client side implementation.
          *
          * This class provides the client side of the Level Control interface.
          */
         class Client:public InterfaceRole <LevelControl::Base, Interface::CLIENT_ROLE>
         {
            public:

            // ======================================================================
            // Commands
            // ======================================================================
            // \name Commands
            //@{

            /*!
             * Send a \c SET_ATTR_REQ to the given address to set the level
             * at \c new_level.
             *
             * @param [in] addr        network address to send the message to.
             * @param [in] new_level   level value to send in the message.
             */
            void level (Protocol::Address &addr, uint8_t new_level);

            /*!
             * Send a \c SET_ATTR_REQ to broadcast network address to set the level
             * at \c new_level.
             *
             * @param [in] new_level    level value to send in the message.
             */
            void level (uint8_t new_level)
            {
               Protocol::Address addr;
               level (addr, new_level);
            }

            //@}
            // =============================================================================
         };

      }  // namespace LevelControl

   }  // namespace Interfaces

}   // namespace HF

#endif /* LEVEL_CONTROL_H_ */
