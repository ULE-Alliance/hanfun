// =============================================================================
/*!
 * \file       inc/hanfun/core/device_information.h
 *
 * This file contains the definition for the core Device Information interface
 * of the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 *
 * TODO Add support for attributes SET/GET.
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

#ifndef HF_DEVICE_SW_VERSION
//! Device Application Version
   #define HF_DEVICE_SW_VERSION          "0.0.0"
#endif

#ifndef HF_DEVICE_HW_VERSION
//! Device Hardware Version
   #define HF_DEVICE_HW_VERSION          "revA"
#endif

#ifndef HF_DEVICE_MANUFACTURER_CODE
//! Device Electronic Manufacturer Code
   #define HF_DEVICE_MANUFACTURER_CODE   0x0000
#endif

#ifndef HF_DEVICE_MANUFACTURER_NAME
//! Device Manufacturer Name
   #define HF_DEVICE_MANUFACTURER_NAME   "None"
#endif

#ifndef HF_DEVICE_LOCATION
//! Device Location
   #define HF_DEVICE_LOCATION            "Unknown"
#endif

#ifndef HF_DEVICE_FRIENDLY_NAME
//! Device Friendly Name
   #define HF_DEVICE_FRIENDLY_NAME       ""
#endif

#ifndef HF_DEVICE_SERIAL_NUMBER
//! Device Serial Number.
   #define HF_DEVICE_SERIAL_NUMBER       ""
#endif

// =============================================================================
// API
// =============================================================================
namespace HF
{
   namespace Core
   {
      namespace DeviceInformation
      {
         constexpr static uint8_t  CORE_VERSION      = HF::CORE_VERSION;       //!< HAN-FUN Core version.
         constexpr static uint8_t  PROFILE_VERSION   = HF::PROFILES_VERSION;   //!< HAN-FUN Profile version.
         constexpr static uint8_t  INTERFACE_VERSION = HF::INTERFACES_VERSION; //!< HAN-FUN Interface version.

         constexpr static uint16_t EMC               = HF_DEVICE_MANUFACTURER_CODE; //!< Electronic Manufacture Code.

         static const string SW_VERSION;          //!< Application Version.
         static const string HW_VERSION;          //!< Hardware Version.
         static const string MANUFACTURER;        //!< Manufacturer Name.

         /*!
          * This class defines the interface Device Information API.
          */
         struct Interface:public Service <HF::Interface::DEVICE_INFORMATION>
         {
            Interface(IDevice &_device):
               Service (_device)
            {}

            /*!
             * Get the device serial number.
             *
             * @return  the device serial number.
             */
            virtual string serial_number () = 0;

            /*!
             * Set the device serial number.
             *
             * @param [in] serial the serial number to set for the device.
             */
            virtual void serial_number (string serial) = 0;

            /*!
             * Get the device location.
             *
             * @return  the devices location.
             */
            virtual string location () = 0;

            /*!
             * Set the device location.
             *
             * @param [in] value the location to set for the device.
             */
            virtual void location (string value) = 0;

            /*!
             * Get the device friendly name.
             *
             * @return  the device friendly name.
             */
            virtual string friendly_name () = 0;

            /*!
             * Set the device friendly name.
             *
             * @param [in] value the friendly name value to set for the device.
             */
            virtual void friendly_name (string value) = 0;

            /*!
             * Indicate if the device units are enabled or disabled.
             *
             * @retval  true the device's units are enabled.
             * @retval  false the device's units are disabled.
             */
            virtual bool enabled () = 0;

            /*!
             * Enable or disable all units in the device.
             *
             * @param value   true all units in the device are enabled,
             *                false all units in the device are disabled.
             */
            virtual void enable (bool value) = 0;

            //! \see Interface::role
            Interface::Role role () const
            {
               return Interface::SERVER_ROLE;
            }
         };

         /*!
          * This class provides a simple implementation of the DeviceInformation
          * interface.
          */
         class Default:public DeviceInformation::Interface
         {
            protected:

            bool _enabled;

            string _friendly_name;

            string _location;

            string _serial;

            public:

            Default(IDevice &device):
               DeviceInformation::Interface (device)
            {}

            string serial_number ()
            {
               return _serial;
            }

            void serial_number (string serial)
            {
               _serial = serial;
            }

            string location ()
            {
               return _location;
            }

            void location (string value)
            {
               _location = value;
            }

            string friendly_name ()
            {
               return _friendly_name;
            }

            void friendly_name (string value)
            {
               _friendly_name = value;
            }

            bool enabled ()
            {
               return _enabled;
            }

            void enable (bool value)
            {
               _enabled = value;
            }
         };

      }  // namespace DeviceInformation

   }  // namespace Core

}  // namespace HF

#endif /* HF_DEVICE_INFORMATION_H */
