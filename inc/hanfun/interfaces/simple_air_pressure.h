// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/simple_air_pressure.h
 *
 * This file contains the definitions for the Simple Air Pressure interface.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_ITF_SIMPLE_AIR_PRESSURE_H
#define HF_ITF_SIMPLE_AIR_PRESSURE_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace SimpleAirPressure
      {
         class Server;
      }

      /*!
       * @ingroup simple_air_pressure_itf
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
      HF::Attributes::IAttribute *create_attribute(SimpleAirPressure::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Simple Air Pressure interface.
       */
      namespace SimpleAirPressure
      {
         /*!
          * @addtogroup simple_air_pressure_itf  Simple Air Pressure
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Simple Air Pressure interface API.
          * @{
          */

         //! Attributes
         typedef enum _Attributes
         {
            VALUE_ATTR     = 1, //!< Value attribute UID.
            MAXIMUM_ATTR   = 2, //!< Maximum attribute UID.
            MINIMUM_ATTR   = 3, //!< Minimum attribute UID.
            TOLERANCE_ATTR = 4, //!< Tolerance attribute UID.
            __LAST_ATTR__  = TOLERANCE_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Value attribute for the Simple Air Pressure interface.
          */
         struct Value: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = VALUE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;      //!< Attribute Read/Write

            Value(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Maximum attribute for the Simple Air Pressure interface.
          */
         struct Maximum: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = MAXIMUM_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;        //!< Attribute Read/Write

            Maximum(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Minimum attribute for the Simple Air Pressure interface.
          */
         struct Minimum: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = MINIMUM_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;        //!< Attribute Read/Write

            Minimum(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Tolerance attribute for the Simple Air Pressure interface.
          */
         struct Tolerance: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = TOLERANCE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;          //!< Attribute Read/Write

            Tolerance(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::SIMPLE_AIR_PRESSURE, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::SimpleAirPressure::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::SimpleAirPressure::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Simple Air Pressure %Interface : Parent.
          *
          * This is the parent class for the Simple Air Pressure interface implementation.
          */
         struct Base: public Interface<HF::Interface::SIMPLE_AIR_PRESSURE>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::SIMPLE_AIR_PRESSURE>() {}
         };

         /*!
          * Simple Air Pressure %Interface : %Server side implementation.
          *
          * This class provides the server side of the Simple Air Pressure interface.
          */
         class Server: public InterfaceRole<SimpleAirPressure::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint16_t _value;     //!< Value
            uint16_t _maximum;   //!< Maximum
            uint16_t _minimum;   //!< Minimum
            uint16_t _tolerance; //!< Tolerance

            public:

            //! Constructor
            Server(): InterfaceRole<SimpleAirPressure::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Value for the Simple Air Pressure server.
             *
             * @return  the current Value.
             */
            uint16_t value() const;

            /*!
             * Set the Value for the Simple Air Pressure server.
             *
             * @param [in] __value the  Value value to set the server to.
             */
            void value(uint16_t __value);

            /*!
             * Get the Maximum for the Simple Air Pressure server.
             *
             * @return  the current Maximum.
             */
            uint16_t maximum() const;

            /*!
             * Set the Maximum for the Simple Air Pressure server.
             *
             * @param [in] __value the  Maximum value to set the server to.
             */
            void maximum(uint16_t __value);

            /*!
             * Get the Minimum for the Simple Air Pressure server.
             *
             * @return  the current Minimum.
             */
            uint16_t minimum() const;

            /*!
             * Set the Minimum for the Simple Air Pressure server.
             *
             * @param [in] __value the  Minimum value to set the server to.
             */
            void minimum(uint16_t __value);

            /*!
             * Get the Tolerance for the Simple Air Pressure server.
             *
             * @return  the current Tolerance.
             */
            uint16_t tolerance() const;

            /*!
             * Set the Tolerance for the Simple Air Pressure server.
             *
             * @param [in] __value the  Tolerance value to set the server to.
             */
            void tolerance(uint16_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

         };

         /*!
          * Simple Air Pressure %Interface : %Client side implementation.
          *
          * This class provides the client side of the Simple Air Pressure interface.
          */
         struct Client: public InterfaceRole<SimpleAirPressure::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<SimpleAirPressure::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}
         };

         /*! @} */

      }  // namespace SimpleAirPressure

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup simple_air_pressure_itf
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
                         const HF::Interfaces::SimpleAirPressure::Attributes attribute);
/*! @} */

#endif /* HF_ITF_SIMPLE_AIR_PRESSURE_H */
