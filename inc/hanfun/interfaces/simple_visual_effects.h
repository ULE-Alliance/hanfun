// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_visual_effects.h
 *
 * This file contains the definitions for the Simple Visual Effects interface.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_ITF_SIMPLE_VISUAL_EFFECTS_H
#define HF_ITF_SIMPLE_VISUAL_EFFECTS_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleVisualEffects
      {
         class Server;
      }

      /*!
       * @ingroup simple_visual_effects_itf
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] uid      attribute's UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(SimpleVisualEffects::Server *server,
                                                   uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Visual Effects interface.
       */
      namespace SimpleVisualEffects
      {
         /*!
          * @addtogroup simple_visual_effects_itf  Simple Visual Effects interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Visual Effects interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            ON_CMD       = 1, //!< On command UID.
            OFF_CMD      = 2, //!< Off command UID.
            BLINK_CMD    = 3, //!< Blink command UID.
            FADE_CMD     = 4, //!< Fade command UID.
            BREATHE_CMD  = 5, //!< Breathe command UID.
            __LAST_CMD__ = BREATHE_CMD
         } CMD;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleVisualEffects::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleVisualEffects::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Visual Effects %Interface : Parent.
          *
          * This is the parent class for the Simple Visual Effects interface implementation.
          */
         struct Base: public Interface<HF::Interface::SIMPLE_VISUAL_EFFECTS>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::SIMPLE_VISUAL_EFFECTS>() {}
         };

         /*!
          * Simple Visual Effects %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Visual Effects interface.
          */
         class Server: public InterfaceRole<SimpleVisualEffects::Base, HF::Interface::SERVER_ROLE>
         {
            public:

            //! Constructor
            Server(): InterfaceRole<SimpleVisualEffects::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Callback that is called when a @c SimpleVisualEffects::ON_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void on(const Protocol::Address &addr);

            /*!
             * Callback that is called when a @c SimpleVisualEffects::OFF_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void off(const Protocol::Address &addr);

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD
            /*!
             * Callback that is called when a @c SimpleVisualEffects::BLINK_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void blink(const Protocol::Address &addr);
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
            /*!
             * Callback that is called when a @c SimpleVisualEffects::FADE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void fade(const Protocol::Address &addr);
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
            /*!
             * Callback that is called when a @c SimpleVisualEffects::BREATHE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void breathe(const Protocol::Address &addr);
#endif

            //! @}
            // ======================================================================

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*!
          * Simple Visual Effects %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Visual Effects interface.
          */
         struct Client: public InterfaceRole<SimpleVisualEffects::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<SimpleVisualEffects::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_ON_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::ON_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void on(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::ON_CMD,
             * to the broadcast network address.
             */
            void on()
            {
               Protocol::Address addr;
               on(addr);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_OFF_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::OFF_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void off(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::OFF_CMD,
             * to the broadcast network address.
             */
            void off()
            {
               Protocol::Address addr;
               off(addr);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BLINK_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void blink(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BLINK_CMD,
             * to the broadcast network address.
             */
            void blink()
            {
               Protocol::Address addr;
               blink(addr);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::FADE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void fade(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::FADE_CMD,
             * to the broadcast network address.
             */
            void fade()
            {
               Protocol::Address addr;
               fade(addr);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BREATHE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void breathe(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BREATHE_CMD,
             * to the broadcast network address.
             */
            void breathe()
            {
               Protocol::Address addr;
               breathe(addr);
            }
#endif

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{
            //!

            //! @}
            // =============================================================================
         };

         /*! @} */

      }  // namespace SimpleVisualEffects

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_visual_effects_itf
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
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleVisualEffects::CMD command);

/*! @} */

#endif /* HF_ITF_SIMPLE_VISUAL_EFFECTS_H */
