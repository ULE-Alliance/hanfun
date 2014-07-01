// =============================================================================
/*!
 * \file       apps/base.cpp
 *
 * This file contains the implementation of the Base class that represents the
 * HAN-FUN Concentrator on the base example application.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "base.h"

#include "common.h"
#include "application.h"

// =============================================================================
// DeviceManagement
// =============================================================================

// =============================================================================
// DeviceManagement::available
// =============================================================================
/*!
 *
 */
// =============================================================================
bool DeviceManagement::available (uint16_t address)
{
   if (address == 0 || address == HF::Protocol::BROADCAST_ADDR)
   {
      return false;
   }

   auto it = std::find_if (entries ().begin (), entries ().end (),
                           [address](HF::Core::DeviceManagement::Device *dev)
                           {
                              return address == dev->address;
                           });

   return it == entries ().end ();
}

// =============================================================================
// DeviceManagement::next_address
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t DeviceManagement::next_address ()
{
   uint16_t result = _next_address;

   _next_address = HF::Protocol::BROADCAST_ADDR;

   if (result == HF::Protocol::BROADCAST_ADDR)
   {
      for (result = 1; result < HF::Protocol::BROADCAST_ADDR && !available(result); result++);
   }

   return result;
}

// =============================================================================
// DeviceManagement::deregister
// =============================================================================
/*!
 *
 */
// =============================================================================
bool DeviceManagement::deregister (uint16_t address)
{
   if (address == 0 || address == HF::Protocol::BROADCAST_ADDR)
   {
      return false;
   }

   auto _entry = entry(address);

//   auto it = std::find_if (_entries.begin (), _entries.end (),
//                           [address](HF::Core::DeviceManagement::Device *dev)
//                           {
//                              return address == dev->address;
//                           }
//                          );
//   if (it != _entries.end ())
//   {
//      HF::Core::DeviceManagement::Server::deregister (*(*it));
//      return true;
//   }
   if (_entry != nullptr)
   {
      HF::Core::DeviceManagement::Server::deregister (*_entry);
      return true;
   }

   return false;
}

// =============================================================================
// DeviceManagement::save
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagement::save (std::string prefix)
{
   LOG (INFO) << "Saving registration entries ..." << NL;

   if (!loaded)
   {
      return;
   }

   std::ofstream ofs;

   std::string   filename = prefix + "/" + HF_APP_DEV_MGT_FILENAME;

   ofs.open (filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

   std::for_each (_entries.begin (), _entries.end (),
                  [&ofs](HF::Core::DeviceManagement::Device *device)
                  {
                     /* Save Devices */

                     uint16_t size_d = device->size ();

                     HF::Common::ByteArray array (size_d);
                     device->pack (array);

                     ofs.write ((char *) &size_d, sizeof(uint16_t));

                     ofs << array;

                     /* Save UID */
                     HF::Common::ByteArray array_uid (device->uid->size ());

                     HF::UID::pack(*device->uid, array_uid, 0);

                     uint16_t size_uid = array_uid.size();
                     ofs.write ((char *) &size_uid, sizeof(uint16_t));

                     ofs << array_uid;
                  }
                 );

   ofs.flush ();
   ofs.close ();
}

// =============================================================================
// DeviceManagement::restore
// =============================================================================
/*!
 *
 */
// =============================================================================
void DeviceManagement::restore (std::string prefix)
{
   LOG (INFO) << "Restoring registration entries ..." << NL;

   std::ifstream ifs;

   std::string   filename = prefix + "/" + HF_APP_DEV_MGT_FILENAME;

   ifs.open (filename, ios::in | ios::binary);

   while (ifs.good ())
   {
      HF::Core::DeviceManagement::Device *device_tmp;

      uint16_t size;

      /* Get Device Size */
      ifs.read ((char *) &size, sizeof(uint16_t));

      if (!ifs.good ())
      {
         break;
      }

      /* Restore Device */
      HF::Common::ByteArray buffer;
      buffer.reserve(size);

      for (size_t j = 0; j < size; ++j)
      {
         uint8_t data;

         ifs.read ((char *) &data, sizeof(uint8_t));
         buffer.push_back (data);
      }

      device_tmp = new HF::Core::DeviceManagement::Device ();
      device_tmp->unpack (buffer);

      LOG (TRACE) << "Device : " << device_tmp->address << NL;

      /* Get UID Size */
      ifs.read ((char *) &size, sizeof(uint16_t));

      if (!ifs.good ())
      {
         break;
      }

      /* Restore UID */
      HF::Common::ByteArray buffer_uid;
      buffer.reserve(size);

      for (size_t j = 0; j < size; ++j)
      {
         uint8_t data;

         ifs.read ((char *) &data, sizeof(uint8_t));
         buffer_uid.push_back (data);
      }

      HF::UID::unpack (device_tmp->uid, buffer_uid, 0);

      HF::Core::DeviceManagement::DefaultServer::save (device_tmp);

      LOG (TRACE) << "   UID : " << device_tmp->uid << NL;
   }

   ifs.close ();

   loaded = true;
}

// =============================================================================
// BindManagement
// =============================================================================


// =============================================================================
// BindManagement::save
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindManagement::save (std::string prefix)
{
   LOG (INFO) << "Saving binding entries ..." << NL;

   std::ofstream ofs;

   std::string   filename = prefix + "/" + HF_APP_BIND_MGT_FILENAME;

   ofs.open (filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

   std::for_each (entries.begin (), entries.end (), [&ofs](const HF::Core::BindManagement::Entry &entry)
                  {
                     HF::Common::ByteArray temp(entry.size());
                     entry.pack(temp);
                     ofs << temp;
                  });

   ofs.flush ();
   ofs.close ();
}

// =============================================================================
// BindManagement::restore
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindManagement::restore (std::string prefix)
{
   LOG (INFO) << "Restoring binding entries ..." << NL;

   std::ifstream ifs;

   std::string   filename = prefix + "/" + HF_APP_BIND_MGT_FILENAME;

   ifs.open (filename, ios::in | ios::binary);

   HF::Core::BindManagement::Entry entry;

   size_t size = entry.size();

   HF::Common::ByteArray buffer;
   buffer.reserve(size);

   while (ifs.good ())
   {
      buffer.clear();

      size_t j;

      for (j = 0; j < size && ifs.good (); ++j)
      {
         uint8_t data;

         ifs.read ((char *) &data, sizeof(uint8_t));
         buffer.push_back (data);
      }

      // Read an entire entry.
      if (j  == size)
      {
         entry.unpack(buffer);
         auto res = this->add(entry.source, entry.destination, entry.itf);
         LOG (TRACE) << "Bind Add : " << res.first << NL;
      }
   }

   ifs.close ();
}

// =============================================================================
// Base
// =============================================================================

// =============================================================================
// Base::receive
// =============================================================================
/*!
 *
 */
// =============================================================================
void Base::receive (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload, size_t offset)
{
   LOG (DEBUG) << ">>>>>>>>>>>>> Message Received <<<<<<<<<<<<<" << NL;

   LOG (TRACE) << "Payload : " << payload << NL;

   LOG (DEBUG) << packet << std::endl;

   HF::Devices::Concentrator::Abstract <Unit0>::receive (packet, payload, offset);
}

// =============================================================================
// Base::has_bind
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Base::has_bind (uint16_t dev_addr_1, uint16_t dev_addr_2)
{
   HF::Protocol::Address source(dev_addr_1, 1);
   HF::Common::Interface itf(HF::Interface::ON_OFF, HF::Interface::SERVER_ROLE);
   HF::Protocol::Address destination(dev_addr_2, 1);

   return unit0.bind_management()->entries.find(source, itf, destination) != nullptr;
}

// =============================================================================
// Base::bind
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Base::bind (uint16_t dev_addr_1, uint16_t dev_addr_2)
{
   UNUSED(dev_addr_1);
   UNUSED(dev_addr_2);

   if (this->has_bind (dev_addr_1, dev_addr_2))
   {
      return 1;
   }

   HF::Protocol::Address source(dev_addr_1, 1);
   HF::Common::Interface itf(HF::Interface::ON_OFF, HF::Interface::SERVER_ROLE);
   HF::Protocol::Address destination(dev_addr_2, 1);

   auto res = unit0.bind_management()->add(source, destination, itf);

   if (res.first == HF::Common::OK)
   {
      return 0;
   }

   if (unit0.device_management()->entry(dev_addr_1) == nullptr)
   {
      return 2;
   }

   if (unit0.device_management()->entry(dev_addr_2) == nullptr)
   {
      return 3;
   }

   return 4;
}

// =============================================================================
// Base::unbind
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Base::unbind (uint16_t dev_addr_1, uint16_t dev_addr_2)
{
   UNUSED(dev_addr_1);
   UNUSED(dev_addr_2);

   HF::Protocol::Address source(dev_addr_1, 1);
   HF::Common::Interface itf(HF::Interface::ON_OFF, HF::Interface::SERVER_ROLE);
   HF::Protocol::Address destination(dev_addr_2, 1);

   return unit0.bind_management()->remove(source,destination, itf) == HF::Common::Result::OK;
}
