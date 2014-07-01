// =============================================================================
/*!
 * \file       inc/hanfun/devices.h
 *
 * This file contains the definitions for the devices in a HAN-FUN network.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_DEVICES_H
#define HF_DEVICES_H

#include <cstring>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

#include "hanfun/device.h"

#include "hanfun/core.h"
#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/bind_management.h"

#include "hanfun/transport.h"

namespace HF
{
   /*!
    * This is the top-level namespace for the HAN-FUN devices implementation.
    */
   namespace Devices
   {
      /*!
       * This class provides the basic implementation for the Device API.
       */
      struct AbstractDevice:public IDevice
      {
         // =============================================================================
         // IDevice API
         // =============================================================================

         const units_t &units () const
         {
            return _units;
         }

         /*!
          * Add unit to devices unit lists.
          *
          * @param unit    pointer to the unit to add to the list.
          */
         void add (Units::IUnit *unit)
         {
            _units.push_front (unit);
         }

         /*!
          * Remove unit from device's unit list.
          *
          * @param unit    pointer to the unit to remove from the list.
          */
         void remove (Units::IUnit *unit)
         {
            _units.remove (unit);
         }

         /*!
          * Return a pointer to the unit with the given \c id.
          *
          * @param [in] id the id of the unit to retrieve.
          *
          * @return  a pointer to the unit if it exists, \c nullptr otherwise.
          */
         Units::IUnit *unit (uint8_t id);

         void send (Protocol::Packet &packet);

         void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

         protected:

         //! List containing pointers to the units present in the device.
         units_t _units;

         AbstractDevice() {}

         // =============================================================================

         /*!
          * Return the link that can be used to send a packet to the device with the given
          * address.
          *
          * @param [in] addr    the address of the device to send the packet to.
          *
          * @return             a pointer to the link that can be used to send the packet,
          *                     \c nullptr otherwise;
          */
         virtual Transport::Link *link (uint16_t addr) const
         {
            UNUSED (addr);
            return nullptr;
         }

         /*!
          * Check if the incoming packet is a retransmission.
          *
          * This method uses the source address of the packet and the Protocol::Message::reference
          * field to check if the packet is a retransmission or a new packet.
          *
          * @param [in] packet  reference to the incoming packet.
          *
          * @retval  true if the packet is a retransmission,
          * @retval  false otherwise.
          */
         bool repeated (Protocol::Packet const &packet)
         {
            // FIXME Check if packet is a retransmission.
            UNUSED (packet);
            return false;
         }

         /*!
          * Ensure that the incoming packet sends a response if it is required and
          * no response has been sent.
          *
          * @param [in] result   the result of the IUnit::handle method call for
          *                      the incoming packet.
          *
          * @param [in] packet   reference to the incoming packet.
          */
         virtual void respond (Common::Result result, Protocol::Packet &packet)
         {
            // FIXME Handle packets that require a response and no response has been sent.
            UNUSED (result);
            UNUSED (packet);
         }

         /*!
          * Check if the given packet is for this device.
          *
          * @param [in] packet   reference to the incoming packet.
          *
          * @return
          */
         virtual bool is_local (Protocol::Packet &packet)
         {
            return packet.destination.device == address ();
         }
      };

      /*!
       * This is the namespace for the implementation of the HAN-FUN nodes.
       */
      namespace Node
      {
         struct IUnit0:public HF::Core::Unit0
         {
            IUnit0(HF::IDevice &device):HF::Core::Unit0 (device)
            {}

            virtual HF::Core::DeviceManagement::Client *device_management ()       = 0;

            virtual HF::Core::DeviceManagement::Client *device_management () const = 0;
         };

         /*!
          * Template to create Unit0 for HAN-FUN devices.
          */
         template<typename... ITF>
         struct Unit0:public HF::Unit0 <IUnit0, ITF...>
         {
            static_assert (is_base_of <HF::Core::DeviceManagement::Client, typename HF::Unit0 <IUnit0, ITF...>::DeviceMgt>::value,
                           "DeviceMgt must be of type HF::Core::DeviceInformationClient");

            Unit0(IDevice &device):HF::Unit0 <IUnit0, ITF...>(device)
            {}
         };

         /*!
          * Unit0 using default classes to provide the core services.
          */
         struct DefaultUnit0:public Unit0 <HF::Core::DeviceInformation::Default, HF::Core::DeviceManagement::Client>
         {
            DefaultUnit0(IDevice &device):
               Unit0 <Core::DeviceInformation::Default, Core::DeviceManagement::Client>(device)
            {}
         };

         /*!
          * Template for HAN-FUN devices.
          */
         template<typename CoreServices>
         class Abstract:public AbstractDevice
         {
            protected:

            Transport::Link *_link;

            public:

            CoreServices unit0;

            // =============================================================================
            // IDevice API.
            // =============================================================================

            uint16_t address () const
            {
               return unit0.device_management ()->address ();
            }

            // =============================================================================
            // Transport::Endpoint API
            // =============================================================================

            void connected (Transport::Link *link)
            {
               _link = link;
            }

            void disconnected (Transport::Link *link)
            {
               if (_link == link)
               {
                  _link = nullptr;
               }
            }

            void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
            {
               AbstractDevice::receive (packet, payload, offset);
            }

            protected:

            Abstract():unit0 (*this)
            {}

            // =============================================================================

            //! \see AbstractDevice::link.
            Transport::Link *link (uint16_t addr) const
            {
               UNUSED (addr);
               return _link;
            }

            bool is_local (Protocol::Packet &packet)
            {
               return AbstractDevice::is_local (packet) ||
                      // If we are unregistered only allow packets to unit 0.
                      (address () == Protocol::BROADCAST_ADDR && packet.destination.unit == 0);
            }
         };

      } // namespace Device

      /*!
       * This is the namespace for the implementation of HAN-FUN concentrators.
       */
      namespace Concentrator
      {
         struct IUnit0:public HF::Core::Unit0
         {
            IUnit0(HF::IDevice &device):HF::Core::Unit0 (device)
            {}

            virtual HF::Core::DeviceManagement::Server *device_management ()       = 0;

            virtual HF::Core::DeviceManagement::Server *device_management () const = 0;

            virtual HF::Core::BindManagement::Server *bind_management ()           = 0;

            virtual HF::Core::BindManagement::Server *bind_management () const     = 0;
         };

         /*!
          * Template to create Unit0 for HAN-FUN concentrator devices.
          */
         template<typename... ITF>
         struct Unit0:public HF::Unit0 <IUnit0, ITF...>
         {
            static_assert (is_base_of <HF::Core::DeviceManagement::Server, typename HF::Unit0 <IUnit0, ITF...>::DeviceMgt>::value,
                           "DeviceMgt must be of type HF::Core::DeviceInformation::Server");

            typedef typename tuple_element <2, decltype(HF::Unit0 <IUnit0, ITF...>::interfaces)>::type BindMgt;

            static_assert (is_base_of <HF::Core::BindManagement::Server, BindMgt>::value,
                           "BindMgt must be of type HF::Core::BindManagement::Server");

            Unit0(HF::IDevice &device):HF::Unit0 <IUnit0, ITF...>(device)
            {}

            BindMgt *bind_management () const
            {
               return const_cast <BindMgt *>(&get <2>(HF::Unit0 <IUnit0, ITF...>::interfaces));
            }

            BindMgt *bind_management ()
            {
               return &get <2>(HF::Unit0 <IUnit0, ITF...>::interfaces);
            }
         };

         /*!
          * Unit0 using default classes to provide the core services.
          */
         struct DefaultUnit0:public Unit0 <HF::Core::DeviceInformation::Default,
                                           HF::Core::DeviceManagement::DefaultServer,
                                           HF::Core::BindManagement::Server>
         {
            DefaultUnit0(IDevice &device):
               Unit0 (device)
            {}
         };

         /*!
          * Template for HAN-FUN concentrator devices.
          */
         template<typename CoreServices>
         class Abstract:public AbstractDevice
         {
            public:

            typedef forward_list <Transport::Link *> links_t;

            CoreServices unit0;

            // =============================================================================
            // IDevice API.
            // =============================================================================

            uint16_t address () const
            {
               return 0;
            }

            // =============================================================================
            // Transport::Endpoint API
            // =============================================================================

            void connected (Transport::Link *link)
            {
               _links.push_front (link);

               // Check if a registration exists for this link.
               Core::DeviceManagement::Device * entry = unit0.device_management()->entry(link->uid());
               if (entry != nullptr)
               {
                  link->address(entry->address);
               }
            }

            void disconnected (Transport::Link *link)
            {
               _links.remove (link);
            }

            void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
            {
               if (packet.destination.device == Protocol::BROADCAST_ADDR)
               {
                  route_packet (packet, payload, offset);
               }
               else
               {
                  AbstractDevice::receive (packet, payload, offset);
               }
            }

            protected:

            links_t _links; //!< List of link present in this Concentrator.

            Abstract():unit0 (*this)
            {}

            // =============================================================================

            //! \see AbstractDevice::link
            Transport::Link *link (uint16_t addr) const
            {
               if (_links.empty ())
               {
                  return nullptr;
               }

               /* *INDENT-OFF* */
               auto it = find_if(_links.begin(), _links.end(), [addr](HF::Transport::Link *link)
               {
                  return link->address () == addr;
               });
               /* *INDENT-ON* */

               if (it == _links.end())
               {
                  return nullptr;
               }

               return *it;
            }

            /*!
             * Route the given packet to the corresponding device.
             *
             * @param packet  reference for the packet to route.
             * @param payload reference to the ByteArray containing the packet payload.
             * @param offset  offset from where the packet data starts.
             */
            virtual void route_packet (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
            {
               // FIXME Route the incoming packet.
               UNUSED (packet);
               UNUSED (payload);
               UNUSED (offset);
            }
         };

      } // namespace Concentrator

   }  // namespace Devices

}  // namespace HF

#endif /* HF_DEVICES_H */
