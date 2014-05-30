// =============================================================================
/*!
 * \file       apps/base.cpp
 *
 * This file contains the implementation of the Base class that represents the
 * HAN-FUN Concentrator on the application.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "base.h"

// =============================================================================
// Helper Functions
// =============================================================================

static void print (HF::Common::ByteArray &array)
{
   std::cout << "Payload : ";

   for (size_t i = 0; i < array.size (); ++i)
   {
      cout << hex << setw (2) << setfill ('0') << (int) array[i] << " ";
   }

   std::cout << std::endl;
}

static void print (HF::Protocol::Packet &packet)
{
   std::cout << "Packet : " << std::endl;

   std::cout << "\tSource  : " << hex << setw (2) << setfill ('0')
             << (int) packet.source.mod << " "
             << (int) packet.source.device << " "
             << (int) packet.source.unit << std::endl;

   std::cout << "\tDest    : " << hex << setw (2) << setfill ('0')
             << (int) packet.destination.mod << " "
             << (int) packet.destination.device << " "
             << (int) packet.destination.unit << std::endl;

   std::cout << "\tMessage :" << std::endl;

   std::cout << "\t\tReference : " << hex << setw (2) << setfill ('0')
             << (int) packet.message.reference << std::endl;

   std::cout << "\t\tType:" << hex << setw (2) << setfill ('0')
             << (int) packet.message.type << std::endl;

   std::cout << "\t\tInterface: " << hex << setw (2) << setfill ('0')
             << (int) packet.message.itf.role << " "
             << (int) packet.message.itf.uid << " "
             << (int) packet.message.itf.member << std::endl;

   std::cout << "\t\tLength: " << (int) packet.message.length << std::endl;
}

// =============================================================================
// DeviceManagement
// =============================================================================

const std::string DeviceManagement::FILENAME = "devices.hf";

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
                           }
                          );

   return it == entries ().end ();
}

bool DeviceManagement::deregister (uint16_t address)
{
   if (address == 0 || address == HF::Protocol::BROADCAST_ADDR)
   {
      return false;
   }

   auto it = std::find_if (entries ().begin (), entries ().end (),
                           [address](HF::Core::DeviceManagement::Device *dev)
                           {
                              return address == dev->address;
                           }
                          );

   if (it != entries ().end ())
   {
      destroy (*it);
      return true;
   }

   return false;
}

std::ostream &operator <<(std::ostream &stm, const HF::Common::ByteArray &array)
{
   for_each (array.begin (), array.end (), [&stm](uint8_t byte) {
                stm << byte;
             }
            );

   return stm;
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
   cout << "[HANFUN] Saving Entries." << endl;

   std::ofstream ofs;

   std::string   filename = prefix + "/" + std::string (FILENAME);

   ofs.open (filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

   std::for_each (entries ().begin (), entries ().end (),
                  [&ofs](HF::Core::DeviceManagement::Device *device)
                  {
                     /* Save Devices */

                     uint16_t size_d = device->size ();

                     HF::Common::ByteArray array (size_d);
                     device->pack (array);

                     ofs.write ((char *) &size_d, sizeof(uint16_t));

                     ofs << array;

                     /* Save UID */
                     uint16_t size_d_uid = device->uid->size ();

                     HF::Common::ByteArray array_uid (size_d_uid);

                     device->uid->pack (array_uid);

                     ofs.write ((char *) &size_d_uid, sizeof(uint16_t));

                     uint8_t type = device->uid->type ();
                     ofs.write ((char *) &type, sizeof(uint8_t));

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
   std::cout << "[HANFUN] Restoring Entries." << std::endl;

   std::ifstream ifs;

   std::string   filename = prefix + "/" + FILENAME;

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
      HF::Common::ByteArray buffer (size);

      for (size_t j = 0; j < size; ++j)
      {
         uint8_t data;

         ifs.read ((char *) &data, sizeof(uint8_t));
         buffer.push_back (data);
      }

      device_tmp = new HF::Core::DeviceManagement::Device ();
      device_tmp->unpack (buffer);

      /* Get UID Size */
      ifs.read ((char *) &size, sizeof(uint16_t));

      if (!ifs.good ())
      {
         break;
      }

      /* Get UID Size */
      uint8_t type;
      ifs.read ((char *) &type, sizeof(uint8_t));

      if (!ifs.good ())
      {
         break;
      }

      /* Restore UID */
      HF::Common::ByteArray buffer_uid (size);

      for (size_t j = 0; j < size; ++j)
      {
         uint8_t data;

         ifs.read ((char *) &data, sizeof(uint8_t));
         buffer_uid.push_back (data);
      }

      switch (type)
      {
         case HF::UID::NONE_UID:
            device_tmp->uid = new HF::UID::NONE ();
            break;

         case HF::UID::IPUI_UID:
            device_tmp->uid = new HF::UID::IPUI ();
            break;

         case HF::UID::RFPI_UID:
            device_tmp->uid = new HF::UID::RFPI ();
            break;

         case HF::UID::URI_UID:
            device_tmp->uid = new HF::UID::URI ();
            break;
      }

      device_tmp->uid->unpack (buffer_uid);

      HF::Core::DeviceManagement::DefaultServer::save (device_tmp);
   }

   ifs.close ();
}

// =============================================================================
// BindEntry
// =============================================================================

const std::string BindEntry::FILENAME = "binds.hf";

// =============================================================================
// BindEntry::save
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindEntry::save (std::forward_list <BindEntry> &entries, std::string prefix)
{
   cout << "[HANFUN] Saving Binds." << endl;

   std::ofstream ofs;

   std::string   filename = prefix + "/" + FILENAME;

   ofs.open (filename, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

   std::for_each (entries.begin (), entries.end (), [&ofs](BindEntry &entry) {
                     ofs.write ((char *) &entry.source, sizeof(uint16_t));
                     ofs.write ((char *) &entry.destination, sizeof(uint16_t));
                  }
                 );

   ofs.flush ();
   ofs.close ();
}

// =============================================================================
// BindEntry::restore
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindEntry::restore (std::forward_list <BindEntry> &entries, std::string prefix)
{
   cout << "[HANFUN] Restoring Binds." << endl;

   std::ifstream ifs;

   std::string   filename = prefix + "/" + FILENAME;

   ifs.open (filename, ios::in | ios::binary);

   while (ifs.good ())
   {
      uint16_t source;
      uint16_t destination;

      ifs.read ((char *) &source, sizeof(uint16_t));
      ifs.read ((char *) &destination, sizeof(uint16_t));

      entries.emplace_front (source, destination);

      if (!ifs.good ())
      {
         break;
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
   cout << "[HANFUN] >>>>>>>>>>>>> Message Received <<<<<<<<<<<<<" << endl;

   print (payload);

   print (packet);

   HF::Devices::Concentrator::Base <Unit0>::receive (packet, payload, offset);
}

// =============================================================================
// Base::route_packet
// =============================================================================
/*!
 *
 */
// =============================================================================
void Base::route_packet (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload,
                         size_t offset)
{
   UNUSED (payload);
   UNUSED (offset);

   cout << "[HANFUN] >>>>>>>>>>>>> Route Packet <<<<<<<<<<<<<" << endl;

   for (forward_list <BindEntry>::iterator it = bind_entries.begin (); it != bind_entries.end (); ++it)
   {
      if (packet.source.device == it->source)
      {
         packet.destination.device = it->destination;
         break;
      }
   }

   packet.destination.unit = 1;

   packet.link             = nullptr;

   send (packet);

   cout << "[HANFUN] >>>>>>>>>>>>> Route Packet <<<<<<<<<<<<<" << endl;
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
   auto it = std::find_if (bind_entries.begin (), bind_entries.end (),
                           [dev_addr_1, dev_addr_2](BindEntry &entry) {
                              return ((entry.source == dev_addr_1) && (entry.destination == dev_addr_2));
                           }
                          );

   return it != bind_entries.end ();
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
   // Check if source and destination address exist
   auto devices = unit0.management ()->entries ();

   bool dev1    = false;
   bool dev2    = false;

   for_each (devices.begin (), devices.end (),
             [dev_addr_1, dev_addr_2, &dev1, &dev2](HF::Core::DeviceManagement::Device *dev)
             {
                if (dev_addr_1 == dev->address)
                {
                   dev1 = true;
                }

                if (dev_addr_2 == dev->address)
                {
                   dev2 = true;
                }
             }
            );

   // Check if the bind combination isn't already created
   bool has_bind = this->has_bind (dev_addr_1, dev_addr_2);

   if (has_bind && dev1 && dev2)
   {
      bind_entries.emplace_front (dev_addr_1, dev_addr_2);
      return 0;
   }
   else
   {
      if (!has_bind)
      {
         return 1;
      }
      else if (!dev1)
      {
         return 2;
      }
      else if (!dev1)
      {
         return 3;
      }
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
void Base::unbind (uint16_t dev_addr_1, uint16_t dev_addr_2)
{
   bind_entries.remove_if ([dev_addr_1, dev_addr_2](BindEntry &entry) {
                              return ((entry.source == dev_addr_1) && (entry.destination == dev_addr_2));
                           }
                          );
}


// =============================================================================
// Base::unbind
// =============================================================================
/*!
 *
 */
// =============================================================================
void Base::unbind (uint16_t address)
{
   bind_entries.remove_if ([address](BindEntry &entry)
                           {
                              bool res = ((entry.source == address) || (entry.destination == address));

                              if (res)
                              {
                                 cout << "Bind: " << entry.source << " - " << entry.destination << " removed!" << endl;
                              }

                              return res;
                           }
                          );
}
