// =============================================================================
/*!
 * \file       inc/hanfun/interfaces/simple_power_meter.h
 *
 * This file contains the definitions for the Simple Power Meter interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_ITF_SIMPLE_POWER_METER_H
#define HF_ITF_SIMPLE_POWER_METER_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

// =============================================================================
// API
// =============================================================================

namespace HF
{
   namespace Interfaces
   {
      /*!
       * Simple Power Meter Interface : Parent.
       *
       * This is the parent class for the Simple Power Meter interface implementation.
       */
      struct SimplePowerMeter:public Base <Interface::SIMPLE_POWER_METER>
      {
         //! Commands IDs.
         enum Commands
         {
            REPORT_CMD            = 0x01, //!< Report command ID.
            MEASUREMENT_RESET_CMD = 0x01  //!< Measurement Reset command ID.
         };

         /*!
          * Attributes IDs.
          */
         enum Attribute
         {
            ENERGY_ATTR             = 0x01, //!< Energy Attribute ID.
            ENERGY_AT_RESET_ATTR    = 0x02, //!< Energy at Last Reset Attribute ID.
            TIME_AT_RESET_ATTR      = 0x03, //!< Time at Last Reset Attribute ID.
            POWER_ATTR              = 0x04, //!< Instantaneous Power Attribute ID.
            AVG_POWER_ATTR          = 0x05, //!< Average Power Attribute ID.
            AVG_POWER_INTERVAL_ATTR = 0x06, //!< Average Power Interval Attribute ID.
            VOLTAGE_ATTR            = 0x07, //!< Voltage Attribute ID.
            CURRENT_ATTR            = 0x08, //!< Current Attribute ID.
            FREQUENCY_ATTR          = 0x09, //!< Frequency Attribute ID.
            POWER_FACTOR_ATTR       = 0x0A, //!< Power Factor Attribute ID.
            REPORT_INTERVAL_ATTR    = 0x0B, //!< Report Interval Attribute ID.
         };


         /*!
          * This represents a measurement for a given attribute.
          */
         struct Measurement:public Serializable
         {
            uint8_t  unit;      //!< Measurement precision/type.
            uint32_t value;     //!< Measurement value.

            //! \see HF::Serializable::size.
            size_t size () const
            {
               return sizeof(uint8_t) +   // Precision size.
                      sizeof(uint32_t);   // Value size.
            }

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, static_cast <uint8_t>(unit));
               offset += array.write (offset, value);

               return offset - start;
            }

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0)
            {
               size_t  start = offset;

               uint8_t temp;
               offset += array.read (offset, temp);

               unit    = static_cast <Precision>(temp);

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
         struct Report:public Serializable
         {
            Measurement energy;              //!< Energy measurement.

            Measurement last_energy;         //!< Energy measurement at last reset.
            Measurement last_time;           //!< Device time measurement at last reset.

            Measurement power;               //!< Instantaneous Power measurement.
            Measurement avg_power;           //!< Average Power measurement.

            uint16_t    avg_power_interval;  //!< Average Power Interval.

            uint8_t     power_factor;        //!< Power Factor.

            Measurement voltage;             //!< Voltage measurement.
            Measurement current;             //!< Current measurement.
            Measurement frequency;           //!< Frequency measurement.

            Report();

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         protected:

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
      class SimplePowerMeterServer:public InterfaceRole <SimplePowerMeter, Interface::SERVER_ROLE>
      {
         protected:

         /* Attributes */

         Measurement _energy;              //!< Energy measurement attribute.

         Measurement _last_energy;         //!< Energy measurement at last reset attribute.
         Measurement _last_time;           //!< Device time measurement at last reset attribute.

         Measurement _power;               //!< Instantaneous Power measurement attribute.
         Measurement _avg_power;           //!< Average Power measurement attribute.

         uint16_t    _avg_power_interval;  //!< Average Power Interval attribute.
         uint8_t _power_factor;            //!< Power Factor attribute.

         Measurement _voltage;             //!< Voltage measurement attribute.
         Measurement _current;             //!< Current measurement attribute.
         Measurement _frequency;           //!< Frequency measurement attribute.

         uint16_t    _report_interval;     //!< Report Interval attribute.

         uint32_t    _last_periodic;       //! Time in seconds of the last report message sent.

         public:

         //! Constructor
         SimplePowerMeterServer();

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

         //! @}

         protected:

         /*!
          * Create a report message.
          *
          * @return  message to send or \c nullptr if the message cannot be created.
          */
         virtual Report *report ();
      };

      /*!
       * Simple Power Meter Interface : Client side implementation.
       *
       * This class provides the client side of the Simple Power Meter interface.
       */
      struct SimplePowerMeterClient:public InterfaceRole <SimplePowerMeter, Interface::CLIENT_ROLE>
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

         Result handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset);
      };

   }  // namespace Interfaces

}   // namespace HF

#endif /* HF_ITF_SIMPLE_POWER_METER */
