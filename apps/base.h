// =============================================================================
/*!
 * \file       apps/base.h
 *
 * This file contains the definition of the Base class that represents the
 * HAN-FUN Concentrator on the application.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_APP_BASE_H
#define HF_APP_BASE_H

#include "hanfun.h"

#include "common.h"

// =============================================================================
// Defines
// =============================================================================

#ifndef HF_APP_PREFIX
#define HF_APP_PREFIX    "."
#endif

#define HF_APP_DEV_MGT_FILENAME  "devices.hf"
#define HF_APP_BIND_MGT_FILENAME "binds.hf"

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
      HF::Core::DeviceManagement::DefaultServer (unit), loaded(false),
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

   void clear ();

   void save (std::string prefix);

   void restore (std::string prefix);

   protected:

   bool loaded;

   uint16_t _next_address;

   uint16_t next_address ();
};

/*!
 * Custom Bind Management class.
 *
 * This class allows for the application to save and restore bindings from a file.
 */
struct BindManagement : public HF::Core::BindManagement::Server
{
   BindManagement (HF::Devices::Concentrator::IUnit0 &unit):
      HF::Core::BindManagement::Server (unit), loaded(false)
   {}

   void save (std::string prefix);

   void restore (std::string prefix);

   protected:

   bool loaded;
};

/*!
 * Custom Unit0 class to make use of the previous DeviceManagment class.
 */
struct Unit0:public HF::Devices::Concentrator::Unit0 <HF::Core::DeviceInformation::Default,
   DeviceManagement, BindManagement>
{
   Unit0(HF::IDevice &device): HF::Devices::Concentrator::Unit0<HF::Core::DeviceInformation::Default,
         DeviceManagement, BindManagement> (device)
   {}
};

struct Base : public HF::Devices::Concentrator::Abstract<Unit0>
{
   Base(): HF::Devices::Concentrator::Abstract <Unit0>()
   {
      unit0.device_management ()->restore (HF_APP_PREFIX);
      // Bind entries add depends on static data initialization.
      // The bind entries restore is performed in HF::Application::Initialize.
   }

   virtual ~Base()
   {
      unit0.bind_management ()->save (HF_APP_PREFIX);
      unit0.device_management ()->save (HF_APP_PREFIX);
   }

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

#endif /* HF_APP_BASE_H */
