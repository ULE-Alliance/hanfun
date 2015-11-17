// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_thermostat.h
 *
 * This file contains the definitions for the Simple Thermostat interface.
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

#ifndef HF_ITF_SIMPLE_THERMOSTAT_H
#define HF_ITF_SIMPLE_THERMOSTAT_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleThermostat
      {
         class Server;
      }

      /*!
       * @ingroup on_off_itf
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
      HF::Attributes::IAttribute *create_attribute(SimpleThermostat::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Thermostat interface.
       */
      namespace SimpleThermostat
      {
         /*!
          * @addtogroup simple_humidity_itf  Simple Thermostat interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Thermostat interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            BOOST_START_CMD = 0x01,
            BOOST_STOP_CMD  = 0x02,
            __LAST_CMD__    = 0xFF,
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            SUPPORTED_MODES_ATTR       = 0x01, //!< Supported Modes
            HEAT_COOL_MODE_ATTR        = 0x02, //!< Heat/Cool Mode
            FAN_MODE_ATTR              = 0x03, //!< FAN Mode
            HEAT_MODE_TEMP_ATTR        = 0x04, //!< Heat Mode Temperature
            COOL_MODE_TEMP_ATTR        = 0x05, //!< Cool Mode Temperature
            AUTO_MODE_HEAT_TEMP_ATTR   = 0x06, //!< Heat/Cool Mode Heat Temperature
            AUTO_MODE_COOL_TEMP_ATTR   = 0x07, //!< Heat/Cool Mode Cool Temperature
            HEAT_MODE_TEMP_OFFSET_ATTR = 0x08, //!< Heat Mode Temperature Offset
            COOL_MODE_TEMP_OFFSET_ATTR = 0x09, //!< Cool Mode Temperature Offset
            BOOST_DURATION_ATTR        = 0x0A, //!< Boost Duration
            __LAST_ATTR__              = BOOST_DURATION_ATTR
         } Attributes;

         //! Masks for the Simple Thermostat supported modes.
         typedef enum _SupportedModesMasks
         {
            HEAT_MODE      = 0x01, //!< Heat only support.
            COOL_MODE      = 0x02, //!< Cool only support.
            AUTO_MODE      = 0x04, //!< Heat/Cool support.
            FAN_OFF_MODE   = 0x10, //!< Fan not working.
            FAN_ON_MODE    = 0x20, //!< Fan working.
            FAN_AUTO_MODE  = 0x40, //!< Fan has/is in automatic mode.
            ALL_MODES_MASK = HEAT_MODE | COOL_MODE | AUTO_MODE | FAN_ON_MODE |
                             FAN_OFF_MODE | FAN_AUTO_MODE,
         } SupportedModesMasks;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Supported Modes attribute for the Simple Thermostat interface.
          */
         struct SupportedModes: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID        = SUPPORTED_MODES_ATTR;  //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                 //!< Attribute Read/Write

            SupportedModes(uint8_t mode = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(Interface::SIMPLE_THERMOSTAT, ID, owner, mode, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Heat/Cool Mode attribute for the Simple Thermostat interface.
          */
         struct HeatCoolMode: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID        = HEAT_COOL_MODE_ATTR;  //!< Attribute UID.
            static constexpr bool    WRITABLE = true;                 //!< Attribute Read/Write

            HeatCoolMode(uint8_t mode = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(Interface::SIMPLE_THERMOSTAT, ID, owner, mode, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Fan Mode attribute for the Simple Thermostat interface.
          */
         struct FanMode: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID        = FAN_MODE_ATTR;  //!< Attribute UID.
            static constexpr bool    WRITABLE = true;           //!< Attribute Read/Write

            FanMode(uint8_t mode = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(Interface::SIMPLE_THERMOSTAT, ID, owner, mode, WRITABLE)
            {}
         };

         /*!
          * Helper template class to handle attributes for the Simple Thermostat interface.
          */
         template<uint8_t _ID>
         struct Attribute: public HF::Attributes::Attribute<int16_t>
         {
            static constexpr uint8_t ID        = _ID;  //!< Attribute UID.
            static constexpr bool    WRITABLE = true; //!< Attribute Read/Write

            Attribute(int16_t value = 0, HF::Interface *owner = nullptr):
               HF::Attributes::Attribute<int16_t>(Interface::SIMPLE_THERMOSTAT, ID, owner, value,
                                                  WRITABLE)
            {}
         };

         //! Helper class to handle the Heat Mode temperature attribute for the Simple Thermostat interface.
         typedef Attribute<HEAT_MODE_TEMP_ATTR> HeatModeTemperature;

         //! Helper class to handle the Cool Mode temperature attribute for the Simple Thermostat interface.
         typedef Attribute<COOL_MODE_TEMP_ATTR> CoolModeTemperature;

         //! Helper class to handle the Heat/Cool Mode heat temperature attribute for the Simple Thermostat interface.
         typedef Attribute<AUTO_MODE_HEAT_TEMP_ATTR> AutoModeHeatTemperature;

         //! Helper class to handle the Heat/Cool Mode cool temperature attribute for the Simple Thermostat interface.
         typedef Attribute<AUTO_MODE_COOL_TEMP_ATTR> AutoModeCoolTemperature;

         //! Helper class to handle the Heat Mode temperature offset attribute for the Simple Thermostat interface.
         typedef Attribute<HEAT_MODE_TEMP_OFFSET_ATTR> HeatModeTemperatureOffset;

         //! Helper class to handle the Cool Mode temperature offset attribute for the Simple Thermostat interface.
         typedef Attribute<COOL_MODE_TEMP_OFFSET_ATTR> CoolModeTemperatureOffset;

         /*!
          * Helper class to handle the Boost Duration attribute for the Simple Thermostat interface.
          */
         struct BoostDuration: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID        = BOOST_DURATION_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = true;                //!< Attribute Read/Write

            BoostDuration(uint8_t mode = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(Interface::SIMPLE_THERMOSTAT, ID, owner, mode, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleThermostat::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleThermostat::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Thermostat %Interface : Parent.
          *
          * This is the parent class for the Simple Thermostat interface implementation.
          */
         struct Base: public Interfaces::Base<Interface::SIMPLE_THERMOSTAT>
         {};

         /*!
          * Simple Thermostat %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Thermostat interface.
          */
         class Server: public InterfaceRole<SimpleThermostat::Base, Interface::SERVER_ROLE>
         {
            protected:

            uint8_t _supported_modes;              //!< Supported modes
            uint8_t _mode;                         //!< Current mode.

#if HF_ITF_STS_FAN_MODE
            uint8_t _fan_mode;                     //!< Current Fan Mode.
#endif
#if HF_ITF_STS_HEAT_MODE
            int16_t _heat_mode_temperature;        //!< Heat Mode temperature.
#endif
#if HF_ITF_STS_COOL_MODE
            int16_t _cool_mode_temperature;        //!< Cool Mode temperature.
#endif
#if HF_ITF_STS_AUTO_MODE
            int16_t _auto_mode_heat_temperature;   //!< Heat/Cool Mode Heat Temperature.
            int16_t _auto_mode_cool_temperature;   //!< Heat/Cool Mode Cool Temperature.
#endif
#if HF_ITF_STS_HEAT_MODE && HF_ITF_STS_HEAT_OFFSET_ATTR
            int16_t _heat_mode_temperature_offset; //!< Heat Mode Temperature Offset.
#endif
#if HF_ITF_STS_COOL_MODE && HF_ITF_STS_COOL_OFFSET_ATTR
            int16_t _cool_mode_temperature_offset; //!< Cool Mode Temperature Offset.
#endif
#if HF_ITF_STS_BOOST_CMD
            uint8_t _boost_duration;               //!< Boost Duration (min.)
#endif
            public:

            //! Constructor
            Server(const uint8_t __supported_modes = ALL_MODES_MASK):
               _supported_modes(__supported_modes)
            {}

            //! Destructor
            virtual ~Server() {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{
#if HF_ITF_STS_BOOST_CMD
            /*!
             * This method is called when a @c BOOST_START_CMD is received.
             *
             * @param [in] address  the HAN-FUN device address that initiated the command.
             */
            virtual Common::Result boost_start(const Protocol::Address &source)
            {
               UNUSED(source);
               return Common::Result::OK;
            }

            /*!
             * This method is called when a @c BOOST_STOP_CMD is received.
             *
             * @param [in] address  the HAN-FUN device address that initiated the command.
             */
            virtual Common::Result boost_stop(const Protocol::Address &source)
            {
               UNUSED(source);
               return Common::Result::OK;
            }
#endif
            //! @}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the supported modes for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @return  a mask of the supported modes.
             */
            uint8_t supported_modes() const;

            /*!
             * Set the supported modes for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @param [in] __supported_modes mask of modes supported by the server.
             */
            void supported_modes(uint8_t __supported_modes);

            /*!
             * Get the current Heat/Cool mode for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @return  the current Heat/Cool mode.
             */
            uint8_t mode() const;

            /*!
             * Set the current mode for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @param [in] __mode the current mode value to set the server to.
             */
            virtual void mode(uint8_t __mode);

#if HF_ITF_STS_FAN_MODE
            /*!
             * Get the current FAN mode for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @return  the current FAN mode.
             */
            uint8_t fan_mode() const;

            /*!
             * Set the current FAN mode for the Simple Thermostat server.
             *
             * @see HF::Interfaces::SimpleThermostat::SupportedModesMasks
             *
             * @param [in] __fan_mode     the FAN mode value to set the server to.
             */
            virtual void fan_mode(uint8_t __fan_mode);
#endif

#if HF_ITF_STS_HEAT_MODE
            /*!
             * Get the current temperature for the Heat mode.
             *
             * @return  the current temperature for the Heat mode.
             */
            int16_t heat_mode_temperature() const;

            /*!
             * Set the current temperature for the Heat mode.
             *
             * @param [in] __temperature  the temperature value to set for the Heat Mode.
             */
            virtual void heat_mode_temperature(int16_t __temperature);
#endif

#if HF_ITF_STS_COOL_MODE
            /*!
             * Get the current temperature for the Cool mode.
             *
             * @return  the current temperature for the Cool mode.
             */
            int16_t cool_mode_temperature() const;

            /*!
             * Set the current temperature for the Cool mode.
             *
             * @param [in] __temperature  the temperature value to set for the Cool Mode.
             */
            virtual void cool_mode_temperature(int16_t __temperature);
#endif

#if HF_ITF_STS_AUTO_MODE
            /*!
             * Get the current heat temperature for the Heat/Cool mode.
             *
             * @return  the current temperature for the Heat/Cool mode.
             */
            int16_t auto_mode_heat_temperature() const;

            /*!
             * Set the current heat temperature for the Cool mode.
             *
             * @param [in] __temperature  the heat temperature value to set for the Heat/Cool Mode.
             */
            virtual void auto_mode_heat_temperature(int16_t __temperature);

            /*!
             * Get the current cool temperature for the Heat/Cool mode.
             *
             * @return  the current cool temperature for the Heat/Cool mode.
             */
            int16_t auto_mode_cool_temperature() const;

            /*!
             * Set the current cool temperature for the Cool mode.
             *
             * @param [in] __temperature  the cool temperature value to set for the Heat/Cool Mode.
             */
            virtual void auto_mode_cool_temperature(int16_t __temperature);
#endif

#if HF_ITF_STS_HEAT_MODE && HF_ITF_STS_HEAT_OFFSET_ATTR
            /*!
             * Get the current temperature offset for the Heat mode.
             *
             * @return  the current temperature offset for the Heat mode.
             */
            int16_t heat_mode_temperature_offset() const;

            /*!
             * Set the current temperature for the Heat mode.
             *
             * @param [in] __offset  the temperature offset value to set for the Heat Mode.
             */
            virtual void heat_mode_temperature_offset(int16_t __offset);
#endif

#if HF_ITF_STS_COOL_MODE && HF_ITF_STS_COOL_OFFSET_ATTR
            /*!
             * Get the current temperature offset for the Cool mode.
             *
             * @return  the current temperature offset for the Cool mode.
             */
            int16_t cool_mode_temperature_offset() const;

            /*!
             * Set the current temperature for the Cool mode.
             *
             * @param [in] __offset  the temperature offset value to set for the Cool Mode.
             */
            virtual void cool_mode_temperature_offset(int16_t __offset);
#endif

#if HF_ITF_STS_BOOST_CMD
            /*!
             * Get the boost duration for the Boost command.
             *
             * @return  the current temperature offset for the Heat mode.
             */
            uint8_t boost_duration() const;

            /*!
             * Set the boost duration for the Boost command.
             *
             * @param [in] __duration  the duration in minutes for the boost command.
             */
            virtual void boost_duration(uint8_t __duration);
#endif

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid)
            {
               return Interfaces::create_attribute(this, uid);
            }

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;
#if HF_ITF_STS_BOOST_CMD
            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
#endif
         };

         /*!
          * Simple Thermostat %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Thermostat interface.
          */
         struct Client: public InterfaceRole<SimpleThermostat::Base, Interface::CLIENT_ROLE>
         {
            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{
#if HF_ITF_STS_BOOST_CMD
            /*!
             * This method sends a @c BOOST_START_CMD is message to the device with the given
             * address.
             *
             * @param [in] address  the HAN-FUN device address to send the message to.
             */
            void boost_start(const Protocol::Address &address);

            /*!
             * This method sends a @c BOOST_STOP_CMD is message to the device with the given
             * address.
             *
             * @param [in] address  the HAN-FUN device address to send the message to.
             */
            void boost_stop(const Protocol::Address &address);
#endif
            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{
            //!

#if HF_ITF_STS_BOOST_CMD
            /*!
             * This method is called when a @c BOOST_START_CMD response is received.
             *
             * @param [in] address  the HAN-FUN device address that responded to the command.
             * @param [in] result   the result of the command.
             */
            virtual void boost_start(const Protocol::Address &source, Common::Result result)
            {
               UNUSED(source);
               UNUSED(result);
            }

            /*!
             * This method is called when a @c BOOST_STOP_CMD response is received.
             *
             * @param [in] address  the HAN-FUN device address that responded to the command.
             * @param [in] result   the result of the command.
             */
            virtual void boost_stop(const Protocol::Address &source, Common::Result result)
            {
               UNUSED(source);
               UNUSED(result);
            }
#endif
            //! @}
            // =============================================================================

#if HF_ITF_STS_BOOST_CMD
            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
#endif
         };

         /*! @} */

      }  // namespace SimpleThermostat

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_humidity_itf
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
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::SimpleThermostat::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleThermostat::Attributes attribute);

/*! @} */

#endif /* HF_ITF_SIMPLE_THERMOSTAT_H */
