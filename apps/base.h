// =============================================================================
/*!
 * \file       apps/base.h
 *
 * This file contains the definition of the Base class that represents the
 * HAN-FUN Concentrator on the application.
 *
 * \version    1.0.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
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
// Defines
// =============================================================================

// =============================================================================
// Base
// =============================================================================

/*!
 * Custom Device Management class.
 *
 * This class allows for the application to select the address to
 * be given to next device that registers.
 */
struct DeviceManagement:public HF::Core::DeviceManagement::DefaultServer
{
   DeviceManagement(HF::Core::Unit0 &unit):
      HF::Core::DeviceManagement::DefaultServer (unit), loaded (false),
      _next_address (HF::Protocol::BROADCAST_ADDR)
   {}

   virtual ~DeviceManagement() {}

   uint16_t next_address (uint16_t addr)
   {
      _next_address = addr;
      return _next_address;
   }

   bool available (uint16_t address);

   bool deregister (uint16_t address);

   using HF::Core::DeviceManagement::DefaultServer::deregister;

   void clear ();

   HF::Common::Result save (HF::Core::DeviceManagement::Device *device);

   void save (Json::Value &root);

   void restore (Json::Value root);

   protected:

   bool     loaded;

   uint16_t _next_address;

   uint16_t next_address ();
};

/*!
 * Custom Bind Management class.
 *
 * This class allows for the application to save and restore bindings from a file.
 */
struct BindManagement:public HF::Core::BindManagement::Server
{
   BindManagement(HF::Devices::Concentrator::IUnit0 &unit):
      HF::Core::BindManagement::Server (unit), loaded (false)
   {}

   std::pair <HF::Common::Result, const HF::Core::BindManagement::Entry *> add (
      const HF::Protocol::Address &source,
      const HF::Protocol::Address &destination,
      const HF::Common::Interface &itf);

   HF::Common::Result remove (const HF::Protocol::Address &source,
                              const HF::Protocol::Address &destination,
                              const HF::Common::Interface &itf);


   void save (Json::Value &root);

   void restore (Json::Value root);

   protected:

   bool loaded;
};

/*!
 * Custom Unit0 class to make use of the previous DeviceManagment class.
 */
struct Unit0:public HF::Devices::Concentrator::Unit0 <HF::Core::DeviceInformation::Server, DeviceManagement,
                                                      HF::Core::AttributeReporting::Server, BindManagement>
{
   Unit0(HF::IDevice &device):HF::Devices::Concentrator::Unit0 <HF::Core::DeviceInformation::Server,
                                                                DeviceManagement,
                                                                HF::Core::AttributeReporting::Server,
                                                                BindManagement>(device)
   {}
};

struct Base:public HF::Devices::Concentrator::Abstract <Unit0>
{
   Base():HF::Devices::Concentrator::Abstract <Unit0>()
   {}

   virtual ~Base()
   {}

   void receive (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload, size_t offset);

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
 * Fill a HF::Common::Interface from the given \c Json::Value object.
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
 * Create a new HF::UID::UID based on the fields present in the given \c Json::Value
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
 * Fill a HF::Core::DeviceManagement::Unit from the given \c Json::Value object.
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
 * Fill a HF::Core::DeviceManagement::Device from the given \c Json::Value object.
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
 * Fill a HF::Protocol::Address from the given \c Json::Value object.
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
 * Fill a HF::Core::BindManagement::Entry from the given \c Json::Value object.
 *
 * @param [in] node     reference to the Json::Value containing the fields for the
 *                      HF::Core::BindManagement::Entry.
 *
 * @param [out] entry   reference to the HF::Core::BindManagement::Entry to
 *                      update.
 */
// =============================================================================
void from_json (Json::Value &node, HF::Core::BindManagement::Entry &entry);

#endif /* HF_APP_BASE_H */
