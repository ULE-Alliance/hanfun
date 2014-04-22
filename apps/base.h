// =============================================================================
/*!
 * \file       /Lana/apps/base.h
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

// =============================================================================
// Defines
// =============================================================================

// =============================================================================
// Classes
// =============================================================================

/*!
 * Custom Device Management class.
 *
 * This class allows for the application to select the address to
 * be given to next device that registers.
 */
struct DeviceManagement: public HF::Core::DeviceManagement::DefaultServer
{
   const std::string FILENAME = "devices.hf";

   DeviceManagement (HF::IDevice* device) :
         HF::Core::DeviceManagement::DefaultServer(device),
         _next_address(HF::Protocol::BROADCAST_ADDR)
   {
   }

   virtual ~DeviceManagement () {}

   uint16_t next_address (uint16_t addr)
   {
      _next_address = addr;
      return _next_address;
   }

   bool available(uint16_t address);

   bool deregister(uint16_t address);

   void clear();

   void save(std::string prefix);

   void restore(std::string prefix);

   protected:

   uint16_t _next_address;

   uint16_t next_address ()
   {
      uint16_t result = _next_address;

      _next_address = HF::Protocol::BROADCAST_ADDR;

      if (result == HF::Protocol::BROADCAST_ADDR)
      {
         // TODO Find next address.
      }

      return result;
   }

};

/*!
 * Custom Unit0 class to make use of the previous DeviceManagment class.
 */
struct Unit0: public HF::Unit0<HF::Core::DeviceInformation::Default, DeviceManagement>
{
   Unit0 (HF::IDevice* device) :
         HF::Unit0<HF::Core::DeviceInformation::Default, DeviceManagement>(device)
   {
   }
};


/*!
 * BindEntry
 */
struct BindEntry
{
      const static std::string FILENAME;

      std::uint16_t source;
      std::uint16_t destination;

      BindEntry(std::uint16_t source, std::uint16_t destination):
         source(source), destination(destination)
      {}

      static void save(std::forward_list<BindEntry> &entries, std::string prefix);

      static void restore(std::forward_list<BindEntry> &entries, std::string prefix);
};

/*!
 * This class represents the HAN-FUN Concentrator that the application is running.
 */
class Base: public HF::Devices::Concentrator::Base<Unit0>
{
   protected:

   HF::UID::RFPI _uid;

   std::forward_list<BindEntry> bind_entries;

   static const std::string path_prefix;

   public:

   Base() : HF::Devices::Concentrator::Base<Unit0>()
   {
      unit0.management()->restore(path_prefix);
      BindEntry::restore(bind_entries, path_prefix);
   }

   virtual ~Base()
   {
      BindEntry::save(bind_entries, path_prefix);
      unit0.management()->save(path_prefix);
   }

   HF::UID::UID *uid () const
   {
      return (HF::UID::UID *) (&_uid);
   }

   void receive (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset);

   const links_t &links ()
   {
      return _links;
   }

   using HF::Devices::Concentrator::Base<Unit0>::link;

   /*!
    * Return the bind entries present in the system.
    *
    * @return  list of the bind entries.
    */
   const std::forward_list<BindEntry> &binds() const
   {
      return bind_entries;
   }

   /*!
    * Check if bind exists.
    *
    * @param [in] dev_addr_1    HAN-FUN address of the first device.
    * @param [in] dev_addr_2    HAN-FUN address of the second device.
    *
    * @retval  true  if the binding exist;
    * @retval  false otherwise.
    */
   bool has_bind(uint16_t dev_addr_1, uint16_t dev_addr_2);

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
   uint8_t bind(uint16_t dev_addr_1, uint16_t dev_addr_2);

   /*!
    * Remove the binding entry for the given devices.
    *
    * If the bind does not exist, nothing changes.
    *
    * @param [in] dev_addr_1    HAN-FUN address of the first device.
    * @param [in] dev_addr_2    HAN-FUN address of the second device.
    */
   void unbind(uint16_t dev_addr_1, uint16_t dev_addr_2);

   /*!
    * Remove all bindings related with the device with the given address.
    *
    * @param [in] address  device address to remove bindings from.
    */
   void unbind(uint16_t address);

   protected:

   void route_packet (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset);
};

#endif /* HF_APP_BASE_H */
