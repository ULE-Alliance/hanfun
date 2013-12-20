// =============================================================================
/*!
 * \file       inc/hanfun/core/device_information.h
 *
 * This file contains the definition for the core Device Information interface
 * of the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#ifndef HF_DEVICE_INFORMATION_H
#define HF_DEVICE_INFORMATION_H

#include "hanfun/common.h"

#include "hanfun/interface.h"

#include "hanfun/core.h"

using namespace HF;
using namespace HF::Interfaces;

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
      /*!
       * This class implements the Device Information code interface.
       */
      class DeviceInformation:public Service <Interface::DEVICE_INFORMATION>
      {
         protected:

         HF::UID  *_uid;         //!< Device UID.

         string   _sw_version;   //!< Application Version.
         string   _hw_version;   //!< Hardware Version.

         uint16_t _emc;          //!< Electronic Manufacture Code.

         string   _manufacturer; //!< Manufacturer Name.

         string   _location;     //!< Location.

         string   _name;         //!< Device friendly name.

         string   _serial_number; //!< Device Serial Number.

         bool _enabled;          //!< Device state.

         public:

         DeviceInformation(AbstractDevice *_device):Service (_device)
         {
            _sw_version    = HF_DEVICE_SW_VERSION;
            _hw_version    = HF_DEVICE_HW_VERSION;

            _emc           = HF_DEVICE_MANUFACTURER_CODE;

            _manufacturer  = HF_DEVICE_MANUFACTURER_NAME;
            _location      = HF_DEVICE_LOCATION;
            _name          = HF_DEVICE_FRIENDLY_NAME;

            _serial_number = HF_DEVICE_SERIAL_NUMBER;

            _enabled       = true;
         }

         /*!
          * HAN-FUN Core version.
          *
          * @return the version for the HAN-FUN core services and interfaces.
          */
         uint8_t core_version () const
         {
            return HF::CORE_VERSION;
         }

         /*!
          * HAN-FUN Profile version.
          *
          * @return the version for the HAN-FUN profiles.
          */
         uint8_t profiles_version () const
         {
            return HF::PROFILES_VERSION;
         }

         /*!
          * HAN-FUN Interface version.
          *
          * @return the version for the HAN-FUN interfaces.
          */
         uint8_t interfaces_version () const
         {
            return HF::INTERFACES_VERSION;
         }

         /*!
          * Get application version.
          *
          * @return  the version of the application.
          */
         virtual string sw_version () const
         {
            return _sw_version;
         }

         /*!
          * Set application version.
          *
          * @param [in] version the version value to set.
          */
         virtual void sw_version (string version)
         {
            _sw_version = version;
         }

         /*!
          * Get Hardware version.
          *
          * @return  the version of the hardware.
          */
         virtual string hw_version () const
         {
            return _hw_version;
         }

         /*!
          * Set Hardware version.
          *
          * @param [in] version the version value to set.
          */
         virtual void hw_version (string version)
         {
            _hw_version = version;
         }

         /*!
          * Get Electronic Manufacture Code.
          *
          * @return  the EMC of the device.
          */
         virtual uint16_t emc () const
         {
            return _emc;
         }

         /*!
          * Set Electronic Manufacture Code.
          *
          * @param [in] code the EMC value to set.
          */
         virtual void emc (uint16_t code)
         {
            _emc = code;
         }

         /*!
          * Get the device UID.
          *
          * @return  a pointer to the device UID.
          */
         virtual HF::UID *device_uid () const
         {
            return _uid;
         }

         /*!
          * Set the device UID.
          *
          * @param [in] id    pointer to the UID to set as the device UID.
          */
         virtual void device_uid (HF::UID *id)
         {
            _uid = id;
         }

         /*!
          * Get the device serial number.
          *
          * @return  the device serial number.
          */
         virtual string serial_number ()
         {
            return _serial_number;
         }

         /*!
          * Set the device serial number.
          *
          * @param [in] serial the serial number to set for the device.
          */
         virtual void serial_number (string serial)
         {
            _serial_number = serial;
         }

         /*!
          * Get the device manufacturer name.
          *
          * @return  the device manufacturer name.
          */
         virtual string manufacturer ()
         {
            return _manufacturer;
         }

         /*!
          * Set the device manufacturer name.
          *
          * @param [in] name  value to set as the device manufacturer name.
          */
         virtual void manufacturer (string name)
         {
            _manufacturer = name;
         }

         /*!
          * Get the device location.
          *
          * @return  the devices location.
          */
         virtual string location ()
         {
            return _location;
         }

         /*!
          * Set the device location.
          *
          * @param [in] value the location to set for the device.
          */
         virtual void location (string value)
         {
            _location = value;
         }

         /*!
          * Get the device friendly name.
          *
          * @return  the device friendly name.
          */
         virtual string friendly_name ()
         {
            return _name;
         }

         /*!
          * Set the device friendly name.
          *
          * @param [in] value the friendly name value to set for the device.
          */
         virtual void friendly_name (string value)
         {
            _name = value;
         }

         /*!
          * Indicate if the device units are enabled or disabled.
          *
          * @retval  true the device's units are enabled.
          * @retval  false the device's units are disabled.
          */
         virtual bool enabled ()
         {
            return _enabled;
         }

         /*!
          * Enable or disable all units in the device.
          *
          * @param value   true all units in the device are enabled,
          *                false all units in the device are disabled.
          */
         virtual void enabled (bool value)
         {
            _enabled = value;
         }

         //! \see Interface::role
         Interface::Role role () const
         {
            return Interface::SERVER_ROLE;
         }
      };
   }  // namespace Core

}  // namespace HF

#endif /* HF_DEVICE_INFORMATION_H */
