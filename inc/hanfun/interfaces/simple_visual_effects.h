// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_visual_effects.h
 *
 * This file contains the definitions for the Simple Visual Effects interface.
 *
 * @version    1.5.1
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
          * @addtogroup simple_visual_effects_itf  Simple Visual Effects
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
         // Helper classes
         // =============================================================================

         /*!
          * This structure represents the parameters required for the @c ON_CMD effect.
          */
         struct OnEffect
         {
            uint16_t duration;   //!< How long to keep the visual indicator @c ON in miliseconds.

            /*!
             * Constructor.
             *
             * @param [in] _duration   number of miliseconds for the @c ON_CMD
             */
            OnEffect(uint16_t _duration = 0): duration(_duration) {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint16_t);

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               array.write(offset, duration);

               return min_size;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               array.read(offset, duration);

               return min_size;
            }
         };

         /*!
          * This structure represents the parameters required for the @c BLICK_CMD effect.
          */
         struct BlinkEffect
         {
            uint16_t duty_cycle_on    = 0;   //!< Number of miliseconds to keep visual indicator @c ON.
            uint16_t duty_cycle_off   = 0;   //!< Number of miliseconds to keep visual indicator @c OFF.
            uint16_t number_of_cycles = 1;   //!< Number of times to repeat the ON-OFF cycle.

            /*!
             * Constructor
             *
             * @param [in] _duty_cycle_on      Number of miliseconds to keep visual indicator @c ON.
             * @param [in] _duty_cycle_off     Number of miliseconds to keep visual indicator @c OFF
             * @param [in] _number_of_cycles   Number of times to repeat the ON-OFF cycle.
             */
            BlinkEffect(uint16_t _duty_cycle_on = 0, uint16_t _duty_cycle_off = 0,
                        uint16_t _number_of_cycles = 1): duty_cycle_on(_duty_cycle_on),
               duty_cycle_off(_duty_cycle_off), number_of_cycles(_number_of_cycles) {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = 3 * sizeof(uint16_t);

            //! Minimum value for @c number_of_cycles parameter.
            static constexpr uint16_t min_number_of_cycles = 1;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               HF_ASSERT(number_of_cycles >= min_number_of_cycles,
               {
                  return 0;
               });

               offset += array.write(offset, duty_cycle_on);
               offset += array.write(offset, duty_cycle_off);
               offset += array.write(offset, number_of_cycles);

               return min_size;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               offset += array.read(offset, duty_cycle_on);
               offset += array.read(offset, duty_cycle_off);
               offset += array.read(offset, number_of_cycles);

               HF_ASSERT(number_of_cycles >= min_number_of_cycles,
               {
                  return 0;
               })

               return min_size;
            }
         };

         /*!
          * This structure represents the parameters required for the @c FADE_CMD effect.
          */
         struct FadeEffect
         {
            uint8_t  start    = 0;  //!< Start brightness in percentage.
            uint8_t  end      = 0;  //!< End brightness in percentage.
            uint16_t duration = 0;  //!< Time in miliseconds to go from @c start to @c end brightness.

            FadeEffect(uint8_t _start = 0, uint8_t _end = 0, uint16_t _duration = 0):
               start(_start), end(_end), duration(_duration) {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = 2 * sizeof(uint8_t) + sizeof(uint16_t);

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               offset += array.write(offset, start);
               offset += array.write(offset, end);
               offset += array.write(offset, duration);

               return min_size;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               offset += array.read(offset, start);
               offset += array.read(offset, end);
               offset += array.read(offset, duration);

               return min_size;
            }
         };

         /*!
          * This structure represents the parameters required for the @c BREATHE_CMD effect.
          */
         struct BreatheEffect
         {
            uint8_t  start            = 0; //!< Start brightness in percentage.
            uint16_t start_hold       = 0; //!< Number of miliseconds to hold @c start brightness.
            uint16_t ste_duration     = 0; //!< Number of miliseconds to go from @c start to @c end brightness.

            uint8_t  end              = 0; //!< End brightness in percentage.
            uint16_t end_hold         = 0; //!< Number of miliseconds to hold @c end brightness.
            uint16_t ets_duration     = 0; //!< Number of miliseconds to go from @c end to @c start brightness.

            uint16_t number_of_cycles = 1;      //!< Number of times to repeat the Start-End-Start cycle.

            BreatheEffect(uint8_t _start = 0, uint16_t _start_hold = 0,
                          uint16_t _ste_duration = 0,
                          uint8_t _end = 0, uint16_t _end_hold = 0, uint16_t _ets_duration = 0,
                          uint16_t _number_of_cycles = 1):
               start(_start), start_hold(_start_hold), ste_duration(_ste_duration),
               end(_end), end_hold(_end_hold), ets_duration(_ets_duration),
               number_of_cycles(_number_of_cycles) {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = 2 * sizeof(uint8_t) + 5 * sizeof(uint16_t);

            //! Minimum value for @c number_of_cycles parameter.
            static constexpr uint16_t min_number_of_cycles = 1;

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               HF_ASSERT(number_of_cycles >= min_number_of_cycles,
               {
                  return 0;
               });

               offset += array.write(offset, start);
               offset += array.write(offset, start_hold);
               offset += array.write(offset, ste_duration);
               offset += array.write(offset, end);
               offset += array.write(offset, end_hold);
               offset += array.write(offset, ets_duration);
               offset += array.write(offset, number_of_cycles);

               return min_size;
            }

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
            {
               HF_SERIALIZABLE_CHECK(array, offset, min_size);

               offset += array.read(offset, start);
               offset += array.read(offset, start_hold);
               offset += array.read(offset, ste_duration);
               offset += array.read(offset, end);
               offset += array.read(offset, end_hold);
               offset += array.read(offset, ets_duration);
               offset += array.read(offset, number_of_cycles);

               HF_ASSERT(number_of_cycles >= min_number_of_cycles,
               {
                  return 0;
               });

               return min_size;
            }
         };

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
             * @param [in] effect     the parameters for the visual effect.
             */
            virtual void on(const Protocol::Address &addr, const OnEffect &effect);

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
             * @param [in] effect     the parameters for the visual effect.
             */
            virtual void blink(const Protocol::Address &addr, const BlinkEffect &effect);
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
            /*!
             * Callback that is called when a @c SimpleVisualEffects::FADE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] effect     the parameters for the visual effect.
             */
            virtual void fade(const Protocol::Address &addr, const FadeEffect &effect);
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
            /*!
             * Callback that is called when a @c SimpleVisualEffects::BREATHE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] effect     the parameters for the visual effect.
             */
            virtual void breathe(const Protocol::Address &addr, const BreatheEffect &effect);
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
             * @param [in] duration   number of milliseconds to keep the visual effect @c ON.
             */
            void on(const Protocol::Address &addr, uint16_t duration);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::ON_CMD,
             * to the broadcast network address.
             *
             * @param [in] duration   number of milliseconds to keep the visual effect @c ON.
             */
            void on(uint16_t duration)
            {
               Protocol::Address addr;
               on(addr, duration);
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
             * @param [in] effect     the parameters for the visual effect.
             */
            void blink(const Protocol::Address &addr, const BlinkEffect &effect);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BLINK_CMD,
             * to the broadcast network address.
             *
             * @param [in] effect     the parameters for the visual effect.
             */
            void blink(const BlinkEffect &effect)
            {
               Protocol::Address addr;
               blink(addr, effect);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::FADE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] effect     the parameters for the visual effect.
             */
            void fade(const Protocol::Address &addr, const FadeEffect &effect);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::FADE_CMD,
             * to the broadcast network address.
             */
            void fade(const FadeEffect &effect)
            {
               Protocol::Address addr;
               fade(addr, effect);
            }
#endif

#ifdef HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD
            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BREATHE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] effect     the parameters for the visual effect.
             */
            void breathe(const Protocol::Address &addr, const BreatheEffect &effect);

            /*!
             * Send a HAN-FUN message containing a @c SimpleVisualEffects::BREATHE_CMD,
             * to the broadcast network address.
             *
             * @param [in] effect     the parameters for the visual effect.
             */
            void breathe(const BreatheEffect &effect)
            {
               Protocol::Address addr;
               breathe(addr, effect);
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
