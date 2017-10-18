// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_keypad.h
 *
 * This file contains the definitions for the Simple Keypad interface.
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

#ifndef HF_ITF_SIMPLE_KEYPAD_H
#define HF_ITF_SIMPLE_KEYPAD_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleKeypad
      {
         class Server;
      }

      /*!
       * @ingroup simple_keypad_itf
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] uid      attribute's UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute %UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(SimpleKeypad::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Keypad interface.
       */
      namespace SimpleKeypad
      {
         /*!
          * @addtogroup simple_keypad_itf  Simple Keypad interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Keypad interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            KEYPRESSED_CMD = 0x01,  //!< Keypressed command UID.
            __LAST_CMD__   = KEYPRESSED_CMD
         } CMD;

         /*!
          * Message payload for a @c HF::SimpleKeypad::KEYPRESED request.
          */
         struct KeyPressed
         {
            uint32_t key_id;

            KeyPressed(uint32_t _key_id = 0): key_id(_key_id)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint32_t);

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleKeypad::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleKeypad::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @return  pointer to an attribute object or @c nullptr if the attribute %UID does not
          *          exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Keypad %Interface : Parent.
          *
          * This is the parent class for the Simple Keypad interface implementation.
          */
         struct Base: public Interface<HF::Interface::SIMPLE_KEYPAD>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::SIMPLE_KEYPAD>() {}
         };

         /*!
          * Simple Keypad %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Keypad interface.
          */
         class Server: public InterfaceRole<SimpleKeypad::Base, HF::Interface::SERVER_ROLE>
         {
            public:

            //! Constructor
            Server(): InterfaceRole<SimpleKeypad::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c SimpleKeypad::KEYPRESSED_CMD,
             * is received.
             *
             * @param [in] addr  the network address to send the message to.
             * @param [in] msg   the key pressed message received.
             */
            virtual void key_pressed(const Protocol::Address &addr, KeyPressed &msg);

            /*!
             * Callback that informs the app that a key was pressed on the client.
             *
             * @param[in] key_id    the id of the key pressed.
             *
             * @note this function must be implemented on the user application when defining
             *       the server.
             */
            virtual void key_received(const uint32_t key_id)
            {
               UNUSED(key_id);
            }

            //! @}
            // ======================================================================
            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*!
          * Simple Keypad %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Keypad interface.
          */
         struct Client: public InterfaceRole<SimpleKeypad::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<SimpleKeypad::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c SimpleKeypad::KEYPRESSED_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] key_id     the id of the key pressed.
             */
            void key_pressed(const Protocol::Address &addr, uint32_t key_id);

            //! @}
            // ======================================================================
         };

         /*! @} */

      }  // namespace SimpleKeypad

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_keypad_itf
 * @{
 */

// =============================================================================
// Stream Helpers
// =============================================================================

/*!
 * Convert the given @c command into a string and write it to the given @c stream.
 *
 * @param [in] stream   out stream to write the string to.
 * @param [in] command  role value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimpleKeypad::CMD command);

/*! @} */

#endif /* HF_ITF_SIMPLE_KEYPAD_H */
