// =============================================================================
/*!
 * \file       inc/hanfun/devices.h
 *
 * This file contains the definitions for the devices in a HAN-FUN network.
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
#include "hanfun/core/attribute_reporting.h"

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

         Units::IUnit *unit (uint8_t id) const;

         void send (Protocol::Packet &packet);

         void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

         protected:

         //! Last reference number used to send a packet.
         uint8_t next_reference;

         //! List containing pointers to the units present in the device.
         units_t _units;

         //! Support for filtering duplicated message from the network.
         Protocol::Filters::Repeated repeated_filter;

         //! Support for generating missing responses.
         Protocol::Filters::ResponseRequired response_filter;

         AbstractDevice():
            next_reference (0)
         {}

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
         virtual Transport::Link *link (uint16_t addr) const = 0;

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
         struct IUnit0:public HF::Core::Unit0, public HF::IDevice::IUnit0
         {
            IUnit0(HF::IDevice &device):
               HF::Core::Unit0 (device)
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
            static_assert (std::is_base_of <HF::Core::DeviceManagement::Client,
                                            typename HF::Unit0 <IUnit0, ITF...>::DeviceMgt>::value,
                           "DeviceMgt must be of type HF::Core::DeviceManagement::Client");

            Unit0(IDevice &device):
               HF::Unit0 <IUnit0, ITF...>(device)
            {}

            Core::DeviceInformation::Server *device_info () const
            {
               return HF::Unit0 <IUnit0, ITF...>::device_info ();
            }

            Core::DeviceInformation::Server *device_info ()
            {
               return HF::Unit0 <IUnit0, ITF...>::device_info ();
            }

            Core::AttributeReporting::Server *attribute_reporting () const
            {
               return HF::Unit0 <IUnit0, ITF...>::attribute_reporting ();
            }

            Core::AttributeReporting::Server *attribute_reporting ()
            {
               return HF::Unit0 <IUnit0, ITF...>::attribute_reporting ();
            }
         };

         /*!
          * Unit0 using default classes to provide the core services.
          */
         struct DefaultUnit0:public Unit0 <HF::Core::DeviceInformation::Server,
                                           HF::Core::DeviceManagement::Client, HF::Core::AttributeReporting::Server>
         {
            DefaultUnit0(IDevice &device):
               Unit0 <Core::DeviceInformation::Server, Core::DeviceManagement::Client,
                      Core::AttributeReporting::Server>(device)
            {}
         };

         /*!
          * Template for HAN-FUN devices.
          */
         template<typename CoreServices>
         class Abstract:public AbstractDevice
         {
            public:

            // =============================================================================
            // IDevice API.
            // =============================================================================

            uint16_t address () const
            {
               return unit0 ()->device_management ()->address ();
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

            CoreServices *unit0 () const
            {
               return const_cast <CoreServices *>(&_unit0);
            }

            protected:

            Transport::Link *_link;

            CoreServices    _unit0;

            Abstract():_unit0 (*this)
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

         /*!
          * Top-level parent for transport layer implementations on a HAN-FUN Concentrator.
          */
         class Transport:public HF::Transport::AbstractLayer
         {
            protected:

            HF::Transport::Link *link;

            public:

            Transport():
               link (nullptr)
            {}

            //! \see HF::Transport::AbstractLayer::add
            void add (HF::Transport::Endpoint *ep)
            {
               HF::Transport::AbstractLayer::add (ep);

               if (link != nullptr)
               {
                  ep->connected (link);
               }
            }

            /*!
             * Add the given link to the list of known links and call the connected method
             * for all registered end-points.
             *
             * @param [in] _link  pointer to the link to add.
             */
            void add (HF::Transport::Link *_link)
            {
               assert (_link != nullptr);

               this->link = _link;
               HF::Transport::AbstractLayer::connected (_link);
            }

            /*!
             * Call the \c disconnected method for all registered end-points with the
             * given \c _link and remove it from list of known links.
             *
             * If \c _link is \c nullptr, then remove all known links.
             *
             * @param [in] _link  pointer to the link to remove.
             */
            void remove (HF::Transport::Link *_link = nullptr)
            {
               if ((_link == nullptr && this->link != nullptr) || (_link == this->link))
               {
                  HF::Transport::AbstractLayer::disconnected (this->link);
                  this->link = nullptr;
               }
            }

            using HF::Transport::AbstractLayer::remove;

            /*!
             * Find the link used to send messages to the HAN-FUN device with
             * the given \c address.
             *
             * @param [in] address  the HAN-FUN address to find the link for.
             *
             * @return  a pointer to the link for the given address or
             *          \c nullptr if no link exists for the given address.
             */
            HF::Transport::Link *find (uint16_t address)
            {
               UNUSED (address);
               return link;
            }
         };

      } // namespace Node

      /*!
       * This is the namespace for the implementation of HAN-FUN concentrators.
       */
      namespace Concentrator
      {
         struct IUnit0:public HF::Core::Unit0, public HF::IDevice::IUnit0
         {
            IUnit0(HF::IDevice &device):
               HF::Core::Unit0 (device)
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
            static_assert (std::is_base_of <HF::Core::DeviceManagement::Server,
                                            typename HF::Unit0 <IUnit0, ITF...>::DeviceMgt>::value,
                           "DeviceMgt must be of type HF::Core::DeviceManagement::Server");

            typedef typename std::tuple_element <3, decltype (HF::Unit0 <IUnit0, ITF...>::interfaces)>::type BindMgt;

            static_assert (std::is_base_of <HF::Core::BindManagement::Server, BindMgt>::value,
                           "BindMgt must be of type HF::Core::BindManagement::Server");

            Unit0(HF::IDevice &device):
               HF::Unit0 <IUnit0, ITF...>(device)
            {}

            BindMgt *bind_management () const
            {
               return const_cast <BindMgt *>(&std::get <3>(HF::Unit0 <IUnit0, ITF...>::interfaces));
            }

            BindMgt *bind_management ()
            {
               return &std::get <3>(HF::Unit0 <IUnit0, ITF...>::interfaces);
            }

            Core::DeviceInformation::Server *device_info () const
            {
               return HF::Unit0 <IUnit0, ITF...>::device_info ();
            }

            Core::DeviceInformation::Server *device_info ()
            {
               return HF::Unit0 <IUnit0, ITF...>::device_info ();
            }

            Core::AttributeReporting::Server *attribute_reporting () const
            {
               return HF::Unit0 <IUnit0, ITF...>::attribute_reporting ();
            }

            Core::AttributeReporting::Server *attribute_reporting ()
            {
               return HF::Unit0 <IUnit0, ITF...>::attribute_reporting ();
            }
         };

         /*!
          * Unit0 using default classes to provide the core services.
          */
         struct DefaultUnit0:public Unit0 <Core::DeviceInformation::Server,
                                           Core::DeviceManagement::DefaultServer,
                                           Core::AttributeReporting::Server,
                                           Core::BindManagement::Server>
         {
            DefaultUnit0(IDevice &device):
               Unit0 (device)
            {}
         };

         /*!
          *
          */
         class AbstractBase:public AbstractDevice
         {
            public:

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

            void connected (HF::Transport::Link *link);

            void disconnected (HF::Transport::Link *link);

            void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

            virtual ~AbstractBase() {}

            protected:

            //! List of link present in this Concentrator.
            std::forward_list <Transport::Link *> _links;

            // =============================================================================

            //! \see AbstractDevice::link
            HF::Transport::Link *link (uint16_t addr) const;

            /*!
             * Route the given packet to the corresponding device.
             *
             * @param [in] packet  reference for the packet to route.
             * @param [in] payload reference to the ByteArray containing the packet payload.
             * @param [in] offset  offset from where the packet data starts.
             */
            virtual void route_packet (Protocol::Packet &packet, Common::ByteArray &payload,
                                         size_t offset);

            virtual Concentrator::IUnit0 *unit0 () const = 0;
         };

         /*!
          * Template for HAN-FUN concentrator devices.
          */
         template<typename CoreServices>
         class Abstract:public AbstractBase
         {
            public:

            CoreServices *unit0 () const
            {
               return const_cast <CoreServices *>(&_unit0);
            }

            protected:

            CoreServices _unit0;

            Abstract() : AbstractBase(), _unit0 (*this)
            {}
         };

         /*!
          * Top-level parent for transport layer implementations on a HAN-FUN Concentrator.
          */
         class Transport:public HF::Transport::AbstractLayer
         {
            protected:

            std::forward_list <HF::Transport::Link *> links;

            public:

            //! \see HF::Transport:Layer::destroy
            void destroy ();

            //! \see HF::Transport::AbstractLayer::add
            void add (HF::Transport::Endpoint *ep)
            {
               HF::Transport::AbstractLayer::add (ep);
               /* *INDENT-OFF* */
               std::for_each(links.begin(), links.end(), [ep](HF::Transport::Link *link)
               {
                  ep->connected (link);
               });
               /* *INDENT-ON* */
            }

            /*!
             * Add the given link to the list of known links and call the connected method
             * for all registered end-points.
             *
             * @param [in] link  pointer to the link to add.
             */
            void add (HF::Transport::Link *link)
            {
               assert (link != nullptr);

               links.push_front (link);
               HF::Transport::AbstractLayer::connected (link);
            }

            /*!
             * Call the \c disconnected method for all registered end-points with the
             * given \c link and remove it from list of known links.
             *
             * If \c link is \c nullptr, then remove all known links.
             *
             * @param [in] link  pointer to the link to remove.
             */
            void remove (HF::Transport::Link *link = nullptr);

            using HF::Transport::AbstractLayer::remove;

            /*!
             * Find the link used to send messages to the HAN-FUN device with
             * the given \c address.
             *
             * @param [in] address  the HAN-FUN address to find the link for.
             *
             * @return  a pointer to the link for the given address or
             *          \c nullptr if no link exists for the given address.
             */
            HF::Transport::Link *find (uint16_t address);
         };

      } // namespace Concentrator

   }  // namespace Devices

}  // namespace HF

#endif /* HF_DEVICES_H */
