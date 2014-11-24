// =============================================================================
/*!
 * @file       inc/hanfun/core/device_information.h
 *
 * This file contains the definition for the core Device Information interface
 * of the HAN-FUN protocol.
 *
 * @version    1.1.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_DEVICE_INFORMATION_H
#define HF_DEVICE_INFORMATION_H

#include "hanfun/common.h"

#include "hanfun/interface.h"

#include "hanfun/core.h"

// =============================================================================
// Defines
// =============================================================================

#ifndef HF_DEVICE_MANUFACTURER_CODE
//! Device Electronic Manufacturer Code
   #define HF_DEVICE_MANUFACTURER_CODE   0x0000
#endif

// =============================================================================
// API
// =============================================================================
namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace DeviceInformation
      {
         struct Server;
      }  // namespace DeviceInformation

      /*!
       * @ingroup dev_info
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
      HF::Attributes::IAttribute *create_attribute (HF::Core::DeviceInformation::Server *server,
                                                    uint8_t uid);

      /*!
       * This namespace contains the classes that implement the Device Information service.
       */
      namespace DeviceInformation
      {
         /*!
          * @addtogroup dev_info Device Information
          * @ingroup core
          *
          * This module contains the classes that implement the %Device Information service.
          * @{
          */
         constexpr static uint8_t  CORE_VERSION      = HF::CORE_VERSION;       //!< HAN-FUN Core version.
         constexpr static uint8_t  PROFILE_VERSION   = HF::PROFILES_VERSION;   //!< HAN-FUN Profile version.
         constexpr static uint8_t  INTERFACE_VERSION = HF::INTERFACES_VERSION; //!< HAN-FUN Interface version.

         constexpr static uint16_t EMC               = HF_DEVICE_MANUFACTURER_CODE; //!< Electronic Manufacture Code.

         //! Attributes.
         typedef enum _Attributes
         {
            CORE_VERSION_ATTR      = 0x01,   //!< HF Core version attribute.              (M)
            PROFILE_VERSION_ATTR   = 0x02,   //!< HF Profile version attribute.           (M)
            INTERFACE_VERSION_ATTR = 0x03,   //!< HF Interface version attribute.         (M)
            EXTRA_CAP_ATTR         = 0x04,   //!< Extra capabilities attribute.           (M)
            MIN_SLEEP_TIME_ATTR    = 0x05,   //!< Minimum sleep time attribute.           (O)
            ACTUAL_RESP_TIME_ATTR  = 0x06,   //!< Actual response time attribute.         (O)
            APP_VERSION_ATTR       = 0x07,   //!< Hardware version attribute.             (O)
            HW_VERSION_ATTR        = 0x08,   //!< Hardware version attribute.             (O)
            EMC_ATTR               = 0x09,   //!< Electronic Manufacture Code attribute.  (O)
            DECT_ID_ATTR           = 0x0A,   //!< RFPI / IPUI                             (0)
            MANUFACTURE_NAME_ATTR  = 0x0B,   //!< Manufacture's name attribute.           (O)
            LOCATION_ATTR          = 0x0C,   //!< Location attribute.                     (O)
            ENABLED_ATTR           = 0x0D,   //!< Device enabled attribute.               (O)
            FRIENDLY_NAME_ATTR     = 0x0E,   //!< Device friendly name attribute.         (O)
            UID_ATTR               = 0x0F,   //!< Device UID attribute.                   (O)
            SERIAL_NUMBER_ATTR     = 0x10,   //!< Serial number attribute.                (O)
            __LAST_ATTR__          = SERIAL_NUMBER_ATTR,
         } Attributes;

         /*!
          * @copybrief HF::Core::create_attribute (HF::Core::DeviceInformation::Server *,uint8_t)
          *
          * @see HF::Core::create_attribute (HF::Core::DeviceInformation::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * Parent class for the Device Information interface implementation.
          */
         class Abstract:public Service <HF::Interface::DEVICE_INFORMATION>
         {
            protected:

            /*!
             * Constructor.
             *
             * @param [in] unit  reference to the unit containing this service.
             */
            Abstract(Unit0 &unit):
               Service (unit)
            {}
         };

         /*!
          * Device Information interface : Server side.
          */
         struct Server:public ServiceRole <Abstract, HF::Interface::SERVER_ROLE>
         {
            HF::UID::UID device_uid;   //! Device UID.

            /*!
             * Constructor.
             *
             * @param [in] unit  reference to the unit containing this service.
             */
            Server(HF::Core::Unit0 &unit):
               ServiceRole <Abstract, HF::Interface::SERVER_ROLE>(unit), _capabilities (0)
            {}

            virtual ~Server() {}

            // =============================================================================
            // Interface Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute (uint8_t uid)
            {
               return Core::create_attribute (this, uid);
            }

            HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const;

            /*!
             * Set extra capabilities attribute paging bit to given @c value.
             *
             * @param [in] value    paging bit value to place in extra capabilities attribute.
             */
            void paging (bool value);

            /*!
             * Get extra capabilities attribute paging bit.
             *
             * @retval  true the broadcast bit is 1.
             * @retval  false the broadcast bit is 0.
             */
            bool has_paging ()  const;

            /*!
             * Set extra capabilities attribute broadcast bit to given @c value.
             *
             * @param [in] value    broadcast bit value to place in extra capabilities attribute.
             */
            void broadcast (bool value);

            /*!
             * Get extra capabilities attribute broadcast bit.
             *
             * @retval  true the broadcast bit is 1.
             * @retval  false the broadcast bit is 0.
             */
            bool has_broadcast () const;

            /*!
             * Setter for the extra capabilities attribute.
             *
             * @param [in] value    bitmask value for the extra capabilities attribute.
             */
            void capabilities (uint8_t value);

            /*!
             * Getter for the extra capabilities attribute bitmask.
             *
             * @return  the bitmask value of the extra capabilities attribute.
             */
            uint8_t capabilities ();

            protected:

            uint8_t _capabilities;
         };

         /*!
          * Create a message that can be used to retrieve the mandatory attributes
          * on a remote device.
          *
          * @return    pointer to a message to retrieve the mandatory attributes.
          */
         Protocol::Message *mandatory ();

         /*!
          * Create a message that can be used to retrieve all the attributes
          * on a remote device.
          *
          * @return    pointer to a message to retrieve all the attributes.
          */
         Protocol::Message *all ();

         /*!
          * Create a message that can be used to retrieve the attributes with the given @c uids of the
          * device information interface on a remote device.
          *
          * @param [in] uids array containing the attribute uids to retrive from the remote device.
          *
          * @return    pointer to a message to retrieve the attributes with the given uid's.
          */
         Protocol::Message *get (HF::Attributes::UIDS &uids);

         /*!
          * Create a message that can be used to retrieve the attribute with the given @c uid.
          *
          * @param [in] uid  attribute's uid to retrieve.
          *
          * @return    pointer to a message to retrieve the attribute with the given uid.
          */
         Protocol::Message *get (uint8_t uid);

         /*! @} */

      }  // namespace DeviceInformation

   }  // namespace Core

}  // namespace HF

#endif /* HF_DEVICE_INFORMATION_H */
