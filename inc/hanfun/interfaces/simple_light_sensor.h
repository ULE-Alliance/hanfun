// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_light_sensor.h
 *
 * This file contains the definitions for the Simple Light Sensor interface.
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

#ifndef HF_ITF_SIMPLE_LIGHT_SENSOR_H
#define HF_ITF_SIMPLE_LIGHT_SENSOR_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleLightSensor
      {
         class Server;
      }

      /*!
       * @ingroup simple_light_sensor_itf
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
      HF::Attributes::IAttribute *create_attribute(SimpleLightSensor::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Light Sensor interface.
       */
      namespace SimpleLightSensor
      {
         /*!
          * @addtogroup simple_light_sensor_itf  Simple Light Sensor interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Light Sensor interface API.
          * @{
          */

         //! Attributes
         typedef enum _Attributes
         {
            VALUE_ATTR     = 0x01, //!< Value attribute UID.
            MINIMUM_ATTR   = 0x02, //!< Minimum attribute UID.
            MAXIMUM_ATTR   = 0x03, //!< Maximum attribute UID.
            TOLERANCE_ATTR = 0x04, //!< Tolerance attribute UID.
            __LAST_ATTR__  = TOLERANCE_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Value attribute for the Simple Light Sensor interface.
          */
         struct Value: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint8_t ID       = VALUE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;      //!< Attribute Read/Write

            Value(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Minimum attribute for the Simple Light Sensor interface.
          */
         struct Minimum: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint8_t ID       = MINIMUM_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;        //!< Attribute Read/Write

            Minimum(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Maximum attribute for the Simple Light Sensor interface.
          */
         struct Maximum: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;        //!< Attribute Read/Write

            Maximum(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Tolerance attribute for the Simple Light Sensor interface.
          */
         struct Tolerance: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint8_t ID       = TOLERANCE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;          //!< Attribute Read/Write

            Tolerance(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::SIMPLE_LIGHT_SENSOR, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleLightSensor::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleLightSensor::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Light Sensor %Interface : Parent.
          *
          * This is the parent class for the Simple Light Sensor interface implementation.
          */
         struct Base: public Interface<HF::Interface::SIMPLE_LIGHT_SENSOR>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::SIMPLE_LIGHT_SENSOR>() {}
         };

         /*!
          * Simple Light Sensor %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Light Sensor interface.
          */
         class Server: public InterfaceRole<SimpleLightSensor::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint32_t _value;     //!< Value
            uint32_t _minimum;   //!< Minimum
            uint32_t _maximum;   //!< Maximum
            uint32_t _tolerance; //!< Tolerance

            public:

            //! Constructor
            Server(): InterfaceRole<SimpleLightSensor::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Value for the Simple Light Sensor server.
             *
             * @return  the current Value.
             */
            uint32_t value() const;

            /*!
             * Set the Value for the Simple Light Sensor server.
             *
             * @param [in] __value the  Value value to set the server to.
             */
            void value(uint32_t __value);

            /*!
             * Get the Minimum for the Simple Light Sensor server.
             *
             * @return  the current Minimum.
             */
            uint32_t minimum() const;

            /*!
             * Set the Minimum for the Simple Light Sensor server.
             *
             * @param [in] __value the  Minimum value to set the server to.
             */
            void minimum(uint32_t __value);

            /*!
             * Get the Maximum for the Simple Light Sensor server.
             *
             * @return  the current Maximum.
             */
            uint32_t maximum() const;

            /*!
             * Set the Maximum for the Simple Light Sensor server.
             *
             * @param [in] __value the  Maximum value to set the server to.
             */
            void maximum(uint32_t __value);

            /*!
             * Get the Tolerance for the Simple Light Sensor server.
             *
             * @return  the current Tolerance.
             */
            uint32_t tolerance() const;

            /*!
             * Set the Tolerance for the Simple Light Sensor server.
             *
             * @param [in] __value the  Tolerance value to set the server to.
             */
            void tolerance(uint32_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

         };

         /*!
          * Simple Light Sensor %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Light Sensor interface.
          */
         struct Client: public InterfaceRole<SimpleLightSensor::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<SimpleLightSensor::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}
         };

         /*! @} */

      }  // namespace SimpleLightSensor

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_light_sensor_itf
 * @{
 */

// =============================================================================
// Stream Helpers
// =============================================================================


/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::SimpleLightSensor::Attributes attribute);
/*! @} */

#endif /* HF_ITF_SIMPLE_LIGHT_SENSOR_H */
