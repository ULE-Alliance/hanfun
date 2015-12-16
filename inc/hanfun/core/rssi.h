// =============================================================================
/*!
 * @file       inc/hanfun/core/rssi.h
 *
 * This file contains the definitions for the RSSI interface.
 *
 * @version    1.4.0
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_CORE_RSSI_H
#define HF_CORE_RSSI_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace RSSI
      {
         class Server;
      }

      /*!
       * @ingroup rssi_itf
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
      HF::Attributes::IAttribute *create_attribute(RSSI::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the RSSI interface.
       */
      namespace RSSI
      {
         /*!
          * @addtogroup rssi_itf  RSSI interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the RSSI interface API.
          * @{
          */

         //! Attributes
         typedef enum _Attributes
         {
            RSSI_ATTR     = 1, //!< RSSI attribute UID.
            __LAST_ATTR__ = RSSI_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the RSSI attribute for the RSSI interface.
          */
         struct Value: public HF::Attributes::Attribute<int8_t>
         {
            static constexpr uint8_t ID       = RSSI_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;     //!< Attribute Read/Write

            Value(int8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<int8_t>(HF::Interface::RSSI, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::RSSI::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::RSSI::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * RSSI %Interfaces::Interface : Parent.
          *
          * This is the parent class for the RSSI interface implementation.
          */
         struct Base: public Interfaces::Interface<HF::Interface::RSSI>
         {
            protected:

            //! Constructor
            Base(): Interfaces::Interface<HF::Interface::RSSI>() {}
         };

         /*!
          * RSSI %Interfaces::Interface : %Server side implementation.
          *
          * This class provides the server side of the RSSI interface.
          */
         class Server: public Interfaces::InterfaceRole<RSSI::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            int8_t _value;   //!< RSSI

            public:

            //! Constructor
            Server(): Interfaces::InterfaceRole<RSSI::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~Server() {}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the RSSI for the RSSI server.
             *
             * @return  the current RSSI.
             */
            int8_t value() const;

            /*!
             * Set the RSSI for the RSSI server.
             *
             * @param [in] __value the  RSSI value to set the server to.
             */
            void value(int8_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

         };

         /*!
          * RSSI %Interfaces::Interface : %Client side implementation.
          *
          * This class provides the client side of the RSSI interface.
          */
         struct Client: public Interfaces::InterfaceRole<RSSI::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): Interfaces::InterfaceRole<RSSI::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}
         };

         /*! @} */

      }  // namespace RSSI

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup rssi_itf
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
                         const HF::Core::RSSI::Attributes attribute);
/*! @} */

#endif /* HF_CORE_RSSI_H */
