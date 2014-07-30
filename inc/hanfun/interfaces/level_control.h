// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/level_control.h
 *
 * This file contains the definitions for the Level Control interface.
 *
 * \version    0.3.2
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
         struct Server;
      }

      HF::Attributes::IAttribute *create_attribute (LevelControl::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Level Control interface
       */
      namespace LevelControl
      {
         //! Command IDs.
         typedef enum
         {
            SET_LEVEL_CMD = 0x01            //!< Set Level Command ID.
         } CMD;

         //! Attributes
         typedef enum
         {
            LEVEL_ATTR    = 0x01,      //!< State attribute UID.
            __LAST_ATTR__ = LEVEL_ATTR,
         } Attributes;

         struct Message
         {
            uint8_t level;

            Message(uint8_t level = 0):level (level) {}

            //! \see HF::Serializable::size.
            size_t size () const
            {
               return sizeof(level);
            }

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, level);

               return offset - start;
            }

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += array.read (offset, level);

               return offset - start;
            }
         };

         struct Level:public HF::Attributes::Attribute <uint8_t>
         {
            static constexpr uint8_t ID        = LEVEL_ATTR;
            static constexpr bool    WRITABBLE = false;

            private:

            Level(uint8_t level = 0):
               Attribute <uint8_t>(Interface::ALERT, ID, level, WRITABBLE)
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
               return payload_size_helper <Message>();
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

            friend HF::Attributes::IAttribute *Interfaces::create_attribute (LevelControl::Server *server, uint8_t uid);

            protected:

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

            //! \see AbstractInterface::attributes
            HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED (pack_id);
               /* *INDENT-OFF* */
               return HF::Attributes::uids_t ({ LevelControl::LEVEL_ATTR });
               /* *INDENT-ON* */
            }
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
             * Send \c SET_LEVEL_CMD message to the given network address and
             * the given level.
             *
             * @param [in] addr       network address to send the message to.
             * @param [in] new_level    level value to send in the message.
             */
            void level (Protocol::Address &addr, uint8_t new_level);

            /*!
             * Send \c SET_LEVEL_CMD message to the broadcast network address and
             * the given level.
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
