// =============================================================================
/*!
 * @file       apps/base.h
 *
 * This file contains the definition of the Base class that represents the
 * HAN-FUN Concentrator on the application.
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
#ifndef HF_APP_BASE_H
#define HF_APP_BASE_H

#include "hanfun.h"

#include "common.h"

#include "json/json.h"

// =============================================================================
// Base
// =============================================================================

/*!
 * @addtogroup examples
 * @{
 */

namespace DeviceManagement
{
   /*!
    * Custom Device Management Entries class.
    *
    * This class saves the application configuration every time an entry
    * is save or destroyed.
    */
   struct Entries:public HF::Core::DeviceManagement::Entries
   {
      typedef HF::Core::DeviceManagement::Device Device;
      typedef HF::Core::DeviceManagement::DevicePtr DevicePtr;

      HF::Common::Result save (const Device &device);

      HF::Common::Result destroy (const Device &device);

      /*!
       * Insert a device management entry into the database.
       *
       * This is used to add the entries when reading the configuration from the JSON database.
       *
       * @param [in] device   device entry to add.
       */
      void insert (const Device &device);
   };

   /*!
    * Custom Device Management class.
    *
    * This class allows for the application to select the address to
    * be given to next device that registers.
    */
   struct Server:public HF::Core::DeviceManagement::Server <Entries>
   {
      Server(HF::Core::Unit0 &unit):
         HF::Core::DeviceManagement::Server <Entries>(unit),
         _next_address (HF::Protocol::BROADCAST_ADDR)
      {}

      virtual ~Server() {}

      /*!
       * Set the device address for the next registration message.
       *
       * @param [in] addr  address to use in the next registration.
       *
       * @return  the address that was set.
       */
      uint16_t next_address (uint16_t addr)
      {
         _next_address = addr;
         return _next_address;
      }

      /*!
       * Check if the given @c address is available for registration.
       *
       * @param [in] address  device address to check.
       *
       * @retval  <tt>true</tt> if no device is registered with the given @c address;
       * @retval  <tt>false</tt> otherwise.
       */
      bool available (uint16_t address);

      /*!
       * De-register the device with the given @c address.
       *
       * @param [in] address  address for the device to de-register.
       *
       * @retval  <tt>true</tt> if the device was de-registered;
       * @retval  <tt>false</tt> otherwise.
       */
      bool deregister (uint16_t address);

      /*!
       * Save the device entries into the JSON database.
       *
       * @param [in] root  database root to start saving the entries.
       */
      void save (Json::Value &root);

      /*!
       * Restore the device entries from the JSON database.
       *
       * @param [in] root  database root to start reading the entries.
       */
      void restore (Json::Value root);

      protected:

      using HF::Core::DeviceManagement::Server <Entries>::deregister;

      //! The address the next registering device will be assigned.
      uint16_t _next_address;

      uint16_t next_address ();
   };

}     // namespace DeviceManagement

namespace BindManagement
{
   /*!
    * Custom Bind Management Entries class.
    *
    * This class saves the application configuration every time an entry
    * is save or destroyed.
    */
   struct Entries:public HF::Core::BindManagement::Entries
   {
      typedef HF::Core::BindManagement::Entry Entry;
      typedef HF::Core::BindManagement::EntryPtr EntryPtr;

      HF::Common::Result save (const Entry &entry);

      HF::Common::Result destroy (const Entry &entry);

      /*!
       * Insert a bind management entry into the database.
       *
       * This is used to add the entries when reading the configuration from the JSON database.
       *
       * @param [in] entry   device entry to add.
       */
      void insert (Entry &entry);
   };

   /*!
    * Custom Bind Management class.
    *
    * This class allows for the application to save and restore bindings from a file.
    */
   struct Server:public HF::Core::BindManagement::Server <Entries>
   {
      Server(HF::Devices::Concentrator::IUnit0 &unit):
         HF::Core::BindManagement::Server <Entries>(unit)
      {}

      /*!
       * Save the bind entries into the JSON database.
       *
       * @param [in] root  database root to start saving the entries.
       */
      void save (Json::Value &root);

      /*!
       * Restore the bind entries from the JSON database.
       *
       * @param [in] root  database root to start reading the entries.
       */
      void restore (Json::Value root);
   };

}  // namespace BindManagement

//! Custom Unit0 declaration
typedef HF::Devices::Concentrator::Unit0 <HF::Core::DeviceInformation::Server,
                                             ::DeviceManagement::Server,
                                          HF::Core::AttributeReporting::Server,
                                             ::BindManagement::Server> Unit0;

/*!
 * This class represents a HAN-FUN Concentrator.
 */
struct Base:public HF::Devices::Concentrator::Abstract <Unit0>
{
   Base():HF::Devices::Concentrator::Abstract <Unit0>()
   {}

   virtual ~Base()
   {}

   void receive (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload, uint16_t offset);

   using HF::Devices::Concentrator::Abstract <Unit0>::link;

   /*!
    * Check if bind exists.
    *
    * @param [in] dev_addr_1    HAN-FUN address of the first device.
    * @param [in] dev_addr_2    HAN-FUN address of the second device.
    *
    * @retval  true  if the binding exist;
    * @retval  false otherwise.
    */
   bool has_bind (uint16_t dev_addr_1, uint16_t dev_addr_2);

   /*!
    * Create a new bind entry.
    *
    * @param [in] dev_addr_1    HAN-FUN address of the first device.
    * @param [in] dev_addr_2    HAN-FUN address of the second device.
    *
    * @retval  0  Bind created.
    * @retval  1  Bind already created.
    * @retval  2  First device does not exist.
    * @retval  3  Second device does not exist.
    */
   uint8_t bind (uint16_t dev_addr_1, uint16_t dev_addr_2);

   /*!
    * Remove the binding entry for the given devices.
    *
    * If the bind does not exist, nothing changes.
    *
    * @param [in] dev_addr_1    HAN-FUN address of the first device.
    * @param [in] dev_addr_2    HAN-FUN address of the second device.
    *
    * @retval  true  if the bind entry was found and removed,
    * @retval  false otherwise.
    */
   bool unbind (uint16_t dev_addr_1, uint16_t dev_addr_2);
};

// =============================================================================
// Helper Functions
// =============================================================================

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::Common::Interface to the given Json::Value.
 *
 * @param [in]  interface  reference to the HF::Common::Interface to serialize to JSON.
 *
 * @param [out] node       reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (const HF::Common::Interface &interface, Json::Value &node);

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::UID::UID pointer to the given Json::Value.
 *
 * @param [in]  uid     pointer to the HF::UID::UID to serialize to JSON.
 *
 * @param [out] node    reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (HF::UID::UID *uid, Json::Value &node);

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::Core::DeviceManagement::Unit to the given Json::Value.
 *
 * @param [in]  unit    reference to the HF::Core::DeviceManagement::Unit to
 *                      serialize to JSON.
 *
 * @param [out] node    reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (const HF::Core::DeviceManagement::Unit &unit, Json::Value &node);

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::Core::DeviceManagement::Device to the given Json::Value.
 *
 * @param [in]  device  reference to the HF::Core::DeviceManagement::Device to
 *                      serialize to JSON.
 *
 * @param [out] node    reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (const HF::Core::DeviceManagement::Device &device, Json::Value &node);

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::Protocol::Address to the given Json::Value.
 *
 * @param [in]  address reference to the HF::Protocol::Address to serialize to JSON.
 *
 * @param [out] node    reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (const HF::Protocol::Address &address, Json::Value &node);

// =============================================================================
// to_json
// =============================================================================
/*!
 * Serialize a HF::Core::BindManagement::Entry to the given Json::Value.
 *
 * @param [in]  entry   reference to the HF::Core::BindManagement::Entry to
 *                      serialize to JSON.
 *
 * @param [out] node    reference to the Json::Value to place the serialization result in.
 */
// =============================================================================
void to_json (const HF::Core::BindManagement::Entry &entry, Json::Value &node);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Fill a HF::Common::Interface from the given @c Json::Value object.
 *
 * @param [in] node        reference to the Json::Value containing the fields for the
 *                         HF::Common::Interface.
 *
 * @param [out] interface  reference to the HF::Common::Interface to update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Common::Interface &interface);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Create a new HF::UID::UID based on the fields present in the given @c Json::Value
 * object.
 *
 * @param [in] node   reference to the Json::Value containing the fields for the
 *                      HF::UID::UID.
 *
 * @param [out] uid     reference to the pointer that will hold the created HF::UID::UID
 *                      object of the correct type (NONE/RFPI/IPUI/MAC/URI).
 */
// =============================================================================
void from_json (Json::Value &node, HF::UID::UID * &uid);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Fill a HF::Core::DeviceManagement::Unit from the given @c Json::Value object.
 *
 * @param [in] node     reference to the Json::Value containing the fields for the
 *                      HF::Core::DeviceManagement::Unit.
 *
 * @param [out] unit    reference to the HF::Core::DeviceManagement::Unit to update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Core::DeviceManagement::Unit &unit);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Fill a HF::Core::DeviceManagement::Device from the given @c Json::Value object.
 *
 * @param [in] node     reference to the Json::Value containing the fields for the
 *                      HF::Core::DeviceManagement::Device.
 *
 * @param [out] device  reference to the HF::Core::DeviceManagement::Device to
 *                      update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Core::DeviceManagement::Device &device);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Fill a HF::Protocol::Address from the given @c Json::Value object.
 *
 * @param [in] node      reference to the Json::Value containing the fields for the
 *                         HF::Protocol::Address.
 *
 * @param [out] address    reference to the HF::Protocol::Address to update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Protocol::Address &address);

// =============================================================================
// from_json
// =============================================================================
/*!
 * Fill a HF::Core::BindManagement::Entry from the given @c Json::Value object.
 *
 * @param [in] node     reference to the Json::Value containing the fields for the
 *                      HF::Core::BindManagement::Entry.
 *
 * @param [out] entry   reference to the HF::Core::BindManagement::Entry to
 *                      update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Core::BindManagement::Entry &entry);

/*! @} */

#endif /* HF_APP_BASE_H */
