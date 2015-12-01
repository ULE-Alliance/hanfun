// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_button.h
 *
 * This file contains the definitions for the Simple Button interface.
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

#ifndef HF_ITF_SIMPLE_BUTTON_H
#define HF_ITF_SIMPLE_BUTTON_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleButton
      {
         class Server;
      }

      /*!
       * @ingroup simple_button_itf
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
      HF::Attributes::IAttribute *create_attribute(SimpleButton::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Button interface.
       */
      namespace SimpleButton
      {
         /*!
          * @addtogroup simple_button_itf  Simple Button interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Button interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            SHORT_PRESS_CMD        = 1, //!< Short Press command UID.
            LONG_PRESS_CMD         = 2, //!< Long Press command UID.
            EXTRA_LONG_PRESS_CMD   = 3, //!< Extra Long Press command UID.
            DOUBLE_CLICK_PRESS_CMD = 4, //!< Double Click Press command UID.
            __LAST_CMD__           = DOUBLE_CLICK_PRESS_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            SHORT_PRESS_MAX_DURATION_ATTR      = 1, //!< Short Press Max Duration attribute UID.
            EXTRA_LONG_PRESS_MIN_DURATION_ATTR = 2, //!< Extra Long Press Min Duration attribute UID.
            DOUBLE_CLICK_GAP_DURATION_ATTR     = 3, //!< Double Click Gap Duration attribute UID.
            __LAST_ATTR__                      = DOUBLE_CLICK_GAP_DURATION_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Short Press Max Duration attribute for the Simple Button interface.
          */
         struct ShortPressMaxDuration: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = SHORT_PRESS_MAX_DURATION_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = true;                          //!< Attribute Read/Write

            ShortPressMaxDuration(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Extra Long Press Min Duration attribute for the Simple Button interface.
          */
         struct ExtraLongPressMinDuration: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = EXTRA_LONG_PRESS_MIN_DURATION_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = true;                               //!< Attribute Read/Write

            ExtraLongPressMinDuration(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Double Click Gap Duration attribute for the Simple Button interface.
          */
         struct DoubleClickGapDuration: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = DOUBLE_CLICK_GAP_DURATION_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = true;                           //!< Attribute Read/Write

            DoubleClickGapDuration(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_BUTTON, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleButton::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleButton::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Button %Interface : Parent.
          *
          * This is the parent class for the Simple Button interface implementation.
          */
         struct Base: public Interface<HF::Interface::SIMPLE_BUTTON>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::SIMPLE_BUTTON>() {}
         };

         /*!
          * Simple Button %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Button interface.
          */
         class Server: public InterfaceRole<SimpleButton::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint16_t _short_press_max_duration;      //!< Short Press Max Duration
            uint16_t _extra_long_press_min_duration; //!< Extra Long Press Min Duration

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_GAP_DURATION_ATTR
            uint16_t _double_click_gap_duration;   //!< Double Click Gap Duration
#endif

            public:

            //! Constructor
            Server(): InterfaceRole<SimpleButton::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::SHORT_PRESS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void short_press(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::SHORT_PRESS_CMD,
             * to the broadcast network address.
             */
            void short_press()
            {
               Protocol::Address addr;
               short_press(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::LONG_PRESS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void long_press(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::LONG_PRESS_CMD,
             * to the broadcast network address.
             */
            void long_press()
            {
               Protocol::Address addr;
               long_press(addr);
            }

#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::EXTRA_LONG_PRESS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void extra_long_press(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::EXTRA_LONG_PRESS_CMD,
             * to the broadcast network address.
             */
            void extra_long_press()
            {
               Protocol::Address addr;
               extra_long_press(addr);
            }
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::DOUBLE_CLICK_PRESS_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void double_click_press(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c SimpleButton::DOUBLE_CLICK_PRESS_CMD,
             * to the broadcast network address.
             */
            void double_click_press()
            {
               Protocol::Address addr;
               double_click_press(addr);
            }
#endif

            //! @}
            // ======================================================================


            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Short Press Max Duration for the Simple Button server.
             *
             * @return  the current Short Press Max Duration.
             */
            uint16_t short_press_max_duration() const;

            /*!
             * Set the Short Press Max Duration for the Simple Button server.
             *
             * @param [in] __value the  Short Press Max Duration value to set the server to.
             */
            virtual void short_press_max_duration(uint16_t __value);

            /*!
             * Get the Extra Long Press Min Duration for the Simple Button server.
             *
             * @return  the current Extra Long Press Min Duration.
             */
            uint16_t extra_long_press_min_duration() const;

            /*!
             * Set the Extra Long Press Min Duration for the Simple Button server.
             *
             * @param [in] __value the  Extra Long Press Min Duration value to set the server to.
             */
            virtual void extra_long_press_min_duration(uint16_t __value);

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_GAP_DURATION_ATTR
            /*!
             * Get the Double Click Gap Duration for the Simple Button server.
             *
             * @return  the current Double Click Gap Duration.
             */
            uint16_t double_click_gap_duration() const;

            /*!
             * Set the Double Click Gap Duration for the Simple Button server.
             *
             * @param [in] __value the  Double Click Gap Duration value to set the server to.
             */
            virtual void double_click_gap_duration(uint16_t __value);
#endif

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

         };

         /*!
          * Simple Button %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Button interface.
          */
         struct Client: public InterfaceRole<SimpleButton::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<SimpleButton::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}

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
            //!

#ifdef HF_ITF_SIMPLE_BUTTON_SHORT_PRESS_CMD
            /*!
             * Callback that is called when a @c SimpleButton::SHORT_PRESS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void short_press(const Protocol::Address &addr);
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_LONG_PRESS_CMD
            /*!
             * Callback that is called when a @c SimpleButton::LONG_PRESS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void long_press(const Protocol::Address &addr);
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD
            /*!
             * Callback that is called when a @c SimpleButton::EXTRA_LONG_PRESS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void extra_long_press(const Protocol::Address &addr);
#endif

#ifdef HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD
            /*!
             * Callback that is called when a @c SimpleButton::DOUBLE_CLICK_PRESS_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual void double_click_press(const Protocol::Address &addr);
#endif

            //! @}
            // =============================================================================
            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         /*! @} */

      }  // namespace SimpleButton

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_button_itf
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
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimpleButton::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleButton::Attributes attribute);
/*! @} */

#endif /* HF_ITF_SIMPLE_BUTTON_H */
