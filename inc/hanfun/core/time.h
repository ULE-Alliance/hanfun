// =============================================================================
/*!
 * @file       inc/hanfun/core/time.h
 *
 * This file contains the definitions for the Time service.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_CORE_TIME_H
#define HF_CORE_TIME_H

#include "hanfun/protocol.h"
#include "hanfun/core.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace Time
      {
         class Server;
      }

      /*!
       * @ingroup time_itf
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
      HF::Attributes::IAttribute *create_attribute(Time::Server *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Time service.
       */
      namespace Time
      {
         /*!
          * @addtogroup time_itf  Time service
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Time service API.
          * @{
          */

         //! Attributes
         typedef enum _Attributes
         {
            TIME_ATTR     = 1, //!< Time attribute UID.
            __LAST_ATTR__ = TIME_ATTR
         } Attributes;

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Time attribute for the Time service.
          */
         struct Value: public HF::Attributes::Attribute<uint32_t>
         {
            static constexpr uint32_t INVALID  = 0xFFFFFFFF; //!< Invalid value for time.

            static constexpr uint8_t  ID       = TIME_ATTR; //!< Attribute UID.
            static constexpr bool     WRITABLE = true;      //!< Attribute Read/Write

            Value(uint32_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint32_t>(HF::Interface::TIME, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Core::create_attribute (HF::Interfaces::Time::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::Time::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         /*!
          * Time %Service : Parent.
          *
          * This is the parent class for the Time service implementation.
          */
         struct Base: public Service<HF::Interface::TIME>
         {
            protected:

            //! Constructor
            Base(Unit0 &unit): Service<HF::Interface::TIME>(unit) {}
         };

         /*!
          * Time %Service : %Server side implementation.
          *
          * This class provides the server side of the Time interface.
          */
         class Server: public ServiceRole<Time::Base, HF::Interface::SERVER_ROLE>
         {
            protected:

            uint32_t _time = Value::INVALID;   //!< Time value

            public:

            //! Constructor
            Server(Unit0 &unit): ServiceRole<Time::Base, HF::Interface::SERVER_ROLE>(unit) {}

            //! Destructor
            virtual ~Server() {}

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Time for the Time server.
             *
             * @return  the current Time.
             */
            uint32_t time() const;

            /*!
             * Set the Time for the Time server.
             *
             * @param [in] __value the  Time value to set the server to.
             */
            void time(uint32_t __value);

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

         };

         /*!
          * Time %Service : %Client side implementation.
          *
          * This class provides the client side of the Time interface.
          */
         struct Client: public ServiceRole<Time::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(Unit0 &unit): ServiceRole<Time::Base, HF::Interface::CLIENT_ROLE>(unit) {}

            virtual ~Client() {}
         };

         /*! @} */

      }  // namespace Time

   }  // namespace Core

}   // namespace HF

/*!
 * @addtogroup time_itf
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
                         const HF::Core::Time::Attributes attribute);
/*! @} */

#endif /* HF_CORE_TIME_H */
