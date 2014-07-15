// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/simple_power_meter.h
 *
 * This file contains the definitions for the Simple Power Meter interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_ITF_SIMPLE_POWER_METER_H
#define HF_ITF_SIMPLE_POWER_METER_H

#include <array>

#include "hanfun/common.h"

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

// =============================================================================
// Configuration
// =============================================================================

#if HF_ITF_SPM_REPORT_CMD

   #if !HF_ITF_SPM_REPORT_INTERVAL_ATTR
      #define HF_ITF_SPM_REPORT_INTERVAL_ATTR   1
   #endif

#endif

#if HF_ITF_SPM_RESET_CMD

   #if !HF_ITF_SPM_ENERGY_ATTR
      #undef HF_ITF_SPM_ENERGY_ATTR
      #define HF_ITF_SPM_ENERGY_ATTR            1
   #endif

   #if !HF_ITF_SPM_ENERGY_AT_RESET_ATTR
      #undef HF_ITF_SPM_ENERGY_AT_RESET_ATTR
      #define HF_ITF_SPM_ENERGY_AT_RESET_ATTR   1
   #endif

   #if !HF_ITF_SPM_TIME_AT_RESET_ATTR
      #undef HF_ITF_SPM_TIME_AT_RESET_ATTR
      #define HF_ITF_SPM_TIME_AT_RESET_ATTR     1
   #endif

#endif

// =============================================================================
// API
// =============================================================================

namespace HF
{
   namespace Interfaces
   {
      namespace SimplePowerMeter
      {
         struct Server;
      }

      HF::Attributes::IAttribute *create_attribute (SimplePowerMeter::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Power Meter interface.
       */
      namespace SimplePowerMeter
      {
         //! Commands IDs.
         typedef enum
         {
            REPORT_CMD            = 0x01,          //!< Report command ID.
            MEASUREMENT_RESET_CMD = 0x01           //!< Measurement Reset command ID.
         } CMD;

         /*!
          * Attributes IDs.
          */
         typedef enum
         {
            ENERGY_ATTR             = 0x01,          //!< Energy Attribute ID.
            ENERGY_AT_RESET_ATTR    = 0x02,          //!< Energy at Last Reset Attribute ID.
            TIME_AT_RESET_ATTR      = 0x03,          //!< Time at Last Reset Attribute ID.
            POWER_ATTR              = 0x04,          //!< Instantaneous Power Attribute ID.
            AVG_POWER_ATTR          = 0x05,          //!< Average Power Attribute ID.
            AVG_POWER_INTERVAL_ATTR = 0x06,          //!< Average Power Interval Attribute ID.
            VOLTAGE_ATTR            = 0x07,          //!< Voltage Attribute ID.
            CURRENT_ATTR            = 0x08,          //!< Current Attribute ID.
            FREQUENCY_ATTR          = 0x09,          //!< Frequency Attribute ID.
            POWER_FACTOR_ATTR       = 0x0A,          //!< Power Factor Attribute ID.
            REPORT_INTERVAL_ATTR    = 0x0B,          //!< Report Interval Attribute ID.
            __LAST_ATTR__           = REPORT_INTERVAL_ATTR,
         } Attributes;

         /*!
          * This represents a measurement for a given attribute.
          */
         struct Measurement
         {
            uint8_t  unit;               //!< Measurement precision/type.
            uint32_t value;              //!< Measurement value.

            //! \see HF::Serializable::size.
            size_t size () const
            {
               return sizeof(uint8_t) +            // Precision size.
                      sizeof(uint32_t);            // Value size.
            }

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, static_cast <uint8_t>(unit));
               offset += array.write (offset, value);

               return offset - start;
            }

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t  start = offset;

               uint8_t temp;
               offset += array.read (offset, temp);

               unit    = static_cast <Common::Precision>(temp);

               offset += array.read (offset, value);

               return offset - start;
            }
         };

         /*!
          * Simple Meter report.
          *
          * TODO This needs a way to dynamically add only the attributes
          *       that are needed to the report.
          */
         struct Report
         {
            Measurement energy;                       //!< Energy measurement.

            Measurement last_energy;                  //!< Energy measurement at last reset.
            Measurement last_time;                    //!< Device time measurement at last reset.

            Measurement power;                        //!< Instantaneous Power measurement.
            Measurement avg_power;                    //!< Average Power measurement.

            uint16_t    avg_power_interval;           //!< Average Power Interval.

            uint8_t     power_factor;                 //!< Power Factor.

            Measurement voltage;                      //!< Voltage measurement.
            Measurement current;                      //!< Current measurement.
            Measurement frequency;                    //!< Frequency measurement.

            /*!
             * This array contains an indication of with attributes should packed or
             * with were unpacked.
             */
            array <bool, __LAST_ATTR__ + 1> enabled;

            Report();

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * Simple Power Meter Interface : Parent.
          *
          * This is the parent class for the Simple Power Meter interface implementation.
          */
         struct Base:public Interfaces::Base <Interface::SIMPLE_POWER_METER>
         {
            protected:

            Base() {}

            using Interfaces::Base <Interface::SIMPLE_POWER_METER>::payload_size;

            size_t payload_size (Protocol::Message::Interface &itf) const
            {
               UNUSED (itf);
               return payload_size_helper <Report>();
            }
         };

         /*!
          * Simple Power Meter Interface : Server side implementation.
          *
          * This class provides the server side of the Simple Power Meter interface.
          */
         class Server:public InterfaceRole <SimplePowerMeter::Base, Interface::SERVER_ROLE>
         {
            protected:

            /* Attributes */
#if HF_ITF_SPM_ENERGY_ATTR
            Measurement _energy;           //!< Energy measurement attribute.
#endif

#if HF_ITF_SPM_ENERGY_AT_RESET_ATTR
            Measurement _last_energy;      //!< Energy measurement at last reset attribute.
#endif

#if HF_ITF_SPM_TIME_AT_RESET_ATTR
            Measurement _last_time;        //!< Device time measurement at last reset attribute.
#endif

#if HF_ITF_SPM_POWER_ATTR
            Measurement _power;            //!< Instantaneous Power measurement attribute.
#endif

#if HF_ITF_SPM_AVG_POWER_ATTR
            Measurement _avg_power;        //!< Average Power measurement attribute.
#endif

#if HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR
            uint16_t _avg_power_interval;  //!< Average Power Interval attribute.
#endif

#if HF_ITF_SPM_POWER_FACTOR_ATTR
            uint8_t _power_factor;         //!< Power Factor attribute.
#endif

#if HF_ITF_SPM_VOLTAGE_ATTR
            Measurement _voltage;          //!< Voltage measurement attribute.
#endif

#if HF_ITF_SPM_CURRENT_ATTR
            Measurement _current;          //!< Current measurement attribute.
#endif

#if HF_ITF_SPM_FREQUENCY_ATTR
            Measurement _frequency;        //!< Frequency measurement attribute.
#endif

#if HF_ITF_SPM_REPORT_CMD
            uint16_t _report_interval;     //!< Report Interval attribute.

            uint32_t _last_periodic;       //! Time in seconds of the last report message sent.
#endif

            public:

            //! Constructor
            Server();

            // =============================================================================
            // API
            // =============================================================================

            //! \see Interface::periodic
            void periodic (uint32_t time);

            // ======================================================================
            // Getters & Setters
            // ======================================================================
            //! \name Getters & Setters
            //! @{

#if HF_ITF_SPM_ENERGY_ATTR
            /*!
             * Retrieve the energy measurement value for the meter.
             *
             * @return  the current measurement for the energy attribute.
             */
            Measurement energy ()
            {
               return _energy;
            }

            /*!
             * Set the energy measurement value for the meter.
             *
             * @param value   the measurement to set as current.
             */
            void energy (Measurement &value)
            {
               _energy = value;
            }
#endif

#if HF_ITF_SPM_ENERGY_AT_RESET_ATTR
            /*!
             * Retrieve the energy measurement value at the last reset.
             *
             * @return     the energy measurement value at the last reset.
             */
            Measurement last_energy ()
            {
               return _last_energy;
            }

            /*!
             * Set the energy measurement value at the last reset.
             *
             * @param value     the energy measurement value.
             */
            void last_energy (Measurement &value)
            {
               _last_energy = value;
            }
#endif

#if HF_ITF_SPM_TIME_AT_RESET_ATTR
            /*!
             * Retrieve the device time value at the last reset.
             *
             * @return      the device time value at the last reset.
             */
            Measurement last_time ()
            {
               return _last_time;
            }

            /*!
             * Set the device time value at the last reset.
             *
             * @param value     the device time value.
             */
            void last_time (Measurement &value)
            {
               _last_time = value;
            }
#endif

#if HF_ITF_SPM_POWER_ATTR
            /*!
             * Retrieve the instantaneous power measurement value for the meter.
             *
             * @return  the current value for the instantaneous power attribute.
             */
            Measurement power ()
            {
               return _power;
            }

            /*!
             * Set the instantaneous power measurement value for the meter.
             *
             * @param value   the measurement value to set the instantaneous power attribute to.
             */
            void power (Measurement &value)
            {
               _power = value;
            }
#endif

#if HF_ITF_SPM_AVG_POWER_ATTR
            /*!
             * Retrieve the average power measurement value for the meter.
             *
             * @return  the current measurement for the average power attribute.
             */
            Measurement avg_power ()
            {
               return _avg_power;
            }

            /*!
             * Set the average power measurement value for the meter.
             *
             * @param value   the measurement value to set the average power attribute to.
             */
            void avg_power (Measurement &value)
            {
               _avg_power = value;
            }
#endif

#if HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR
            /*!
             * Retrieve the average power interval value for the meter.
             *
             * @return  the current value for the average power interval attribute.
             */
            uint16_t avg_power_interval ()
            {
               return _avg_power_interval;
            }

            /*!
             * Set the average power interval measurement value for the meter.
             *
             * @param value   the measurement value to set the average power interval attribute to.
             */
            void avg_power_interval (uint16_t value)
            {
               _avg_power_interval = value;
            }
#endif

#if HF_ITF_SPM_POWER_FACTOR_ATTR
            /*!
             * Retrieve the power factor value for the meter.
             *
             * @return  the value for the power factor attribute.
             */
            uint8_t power_factor ()
            {
               return _power_factor;
            }

            /*!
             * Set the power factor measurement value for the meter.
             *
             * @param value   the measurement value to set the power factor attribute to.
             */
            void power_factor (uint8_t value)
            {
               _power_factor = value;
            }
#endif

#if HF_ITF_SPM_VOLTAGE_ATTR
            /*!
             * Retrieve the voltage measurement value for the meter.
             *
             * @return  the current measurement for the voltage attribute.
             */
            Measurement voltage ()
            {
               return _voltage;
            }

            /*!
             * Set the voltage measurement value for the meter.
             *
             * @param value   the measurement value to set the voltage attribute to.
             */
            void voltage (Measurement &value)
            {
               _voltage = value;
            }
#endif

#if HF_ITF_SPM_CURRENT_ATTR
            /*!
             * Retrieve the current measurement value for the meter.
             *
             * @return  the measurement for the current attribute.
             */
            Measurement current ()
            {
               return _current;
            }

            /*!
             * Set the current measurement value for the meter.
             *
             * @param value   the measurement value to set the current attribute to.
             */
            void current (Measurement &value)
            {
               _current = value;
            }
#endif

#if HF_ITF_SPM_FREQUENCY_ATTR
            /*!
             * Retrieve the frequency measurement value for the meter.
             *
             * @return  the measurement for the frequency attribute.
             */
            Measurement frequency ()
            {
               return _frequency;
            }

            /*!
             * Set the frequency energy measurement value for the meter.
             *
             * @param value   the measurement value to set the frequency attribute to.
             */
            void frequency (Measurement &value)
            {
               _frequency = value;
            }
#endif

#if HF_ITF_SPM_REPORT_CMD
            /*!
             * Retrieve the report interval value for the meter.
             *
             * @return  the value for the report interval attribute.
             */
            uint16_t report_interval ()
            {
               return _report_interval;
            }

            /*!
             * Set the report interval value for the meter.
             *
             * @param value   the value to set the report interval attribute to.
             */
            void report_interval (uint16_t value)
            {
               _report_interval = value;
            }
#endif

            //! @}

            // =============================================================================
            // Attribute API
            // =============================================================================

            //! \see Interface::attribute
            HF::Attributes::IAttribute *attribute (uint8_t uid);

            friend HF::Attributes::IAttribute *Interfaces::create_attribute (SimplePowerMeter::Server *, uint8_t);

            protected:

            /*!
             * Create a report message.
             *
             * @return  message to send or \c nullptr if the message cannot be created.
             */
            virtual Report *report ();

            //! \see AbstractInterface::attributes
            HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const;
         };

         /*!
          * Simple Power Meter Interface : Client side implementation.
          *
          * This class provides the client side of the Simple Power Meter interface.
          */
         struct Client:public InterfaceRole <SimplePowerMeter::Base, Interface::CLIENT_ROLE>
         {
            // ======================================================================
            // Events
            // ======================================================================
            //! \name Events
            //! @{

            /*!
             * Receive a report message from a server.
             *
             * @param [in] report   the report received from the server.
             */
            virtual void report (Report &report) = 0;

            //! @}
            // =============================================================================

            protected:

            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);
         };

      }  // namespace SimplePowerMeter

   }  // namespace Interfaces

}   // namespace HF

#endif /* HF_ITF_SIMPLE_POWER_METER */
