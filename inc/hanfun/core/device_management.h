// =============================================================================
/*!
 * \file       inc/hanfun/core/device_management.h
 *
 * This file contains the definitions for the core Device Management Interface
 * of the HAN-FUN protocol.
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
#ifndef HF_DEVICE_MANGEMENT_H
#define HF_DEVICE_MANGEMENT_H

#include <algorithm>
#include <map>
#include <vector>

#include "hanfun/common.h"
#include "hanfun/uids.h"

#include "hanfun/protocol.h"

#include "hanfun/core.h"
#include "hanfun/device.h"

namespace HF
{
   // =============================================================================
   // Forward declarations
   // =============================================================================

   namespace Devices
   {
      namespace Concentrator
      {
         struct IUnit0;

      }  // namespace Concentrator

   }  // namespace Devices

   // =============================================================================
   // API
   // =============================================================================
   namespace Core
   {
      // Forward declaration.
      namespace DeviceManagement
      {
         struct Server;
      }  // namespace DeviceManagement

      HF::Attributes::IAttribute *create_attribute (DeviceManagement::Server *server, uint8_t uid);

      /*!
       * This namespace contains the classes that implement the Device Management service.
       */
      namespace DeviceManagement
      {
         //! Commands.
         typedef enum
         {
            REGISTER_CMD      = 0x01, //!< Register device command.
            DEREGISTER_CMD    = 0x02, //!< De-register device command.
            START_SESSION_CMD = 0x03, //!< TODO Start Session Read Registration Info.
            END_SESSION_CMD   = 0x04, //!< TODO End Session Read Registration Info.
            GET_ENTRIES_CMD   = 0x05, //!< TODO Get Entries Command.
         } CMD;

         //! Attributes.
         typedef enum
         {
            NUMBER_OF_ENTRIES_ATTR = 0x01, //!< Number of entries attribute.
         } Attributes;

         // =============================================================================

         /*!
          * Unit Entry
          */
         struct Unit
         {
            uint8_t                         id;      //!< Unit Id.
            uint16_t                        profile; //!< Unit UID. \see IProfile::UID.

            //! Optional interfaces.
            std::vector <Common::Interface> interfaces;

            Unit(uint8_t id = 0, uint16_t profile = 0):
               id (id), profile (profile)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);

            /*!
             * This method checks if the remote device unit implements the given interface.
             *
             * The known interfaces for the profile associated with this unit and the
             * declared optional interfaces are searched, to check if the requested interface
             * is available.
             *
             * @param [in] itf_uid    the interface UID to check if the profile implements.
             * @param [in] role       the interface role to match.
             *
             * @retval  true     if this unit implements the requested interface,
             * @retval  false    otherwise.
             */
            bool has_interface (uint16_t itf_uid, HF::Interface::Role role);
         };

         /*!
          * Device Entry.
          */
         struct Device
         {
            uint16_t           address;   //!< Device Address.
            std::vector <Unit> units;     //!< Unit list of the interface.

            uint16_t           emc;       //!< Device EMC if applicable, 0 otherwise.

            HF::UID::UID       uid;       //!< Device UID.

            Device():address (Protocol::BROADCAST_ADDR), emc (0)
            {}

            virtual ~Device()
            {}

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);

            // =============================================================================
            // Operators
            // =============================================================================

            bool operator ==(Device &other)
            {
               if (this->address != other.address || this->uid != other.uid)
               {
                  return false;
               }
               else
               {
                  return true;
               }
            }

            bool operator !=(Device &other)
            {
               return !(*this == other);
            }
         };

         typedef Common::Pointer<Device>  DevicePtr;

         // =============================================================================
         // Register Command Messages
         // =============================================================================

         /*!
          * Register command message.
          */
         struct RegisterMessage
         {
            uint16_t           emc;   //! Device EMC if applicable, 0 otherwise.
            std::vector <Unit> units; //! Device units listing.

            HF::UID::UID       uid;   //! Device UID.

            RegisterMessage(uint16_t emc = 0x0000, HF::UID::UID _uid = HF::UID::UID ()):
               emc (emc), uid (_uid)
            {}

            virtual ~RegisterMessage();

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         /*!
          * Register command response.
          */
         struct RegisterResponse:public Protocol::Response
         {
            uint16_t address; //!< Address for the device to use.
            uint16_t emc;     //!< HAN-FUN Concentrator EMC if applicable.

            RegisterResponse(uint16_t address = 0, uint16_t emc = 0):
               address (address & Protocol::BROADCAST_ADDR), emc (emc)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // De-register Command Messages.
         // =============================================================================

         /*!
          * De-register command message.
          */
         struct DeregisterMessage
         {
            uint16_t address; //!< Address of the device to de-register.

            DeregisterMessage(uint16_t address = 0):
               address (address)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         /*!
          * Register command response.
          */
         struct DeregisterResponse:public Protocol::Response
         {
            uint16_t address; //!< Address for the device.

            DeregisterResponse(Common::Result code = Common::FAIL_UNKNOWN,
                               uint16_t address = Protocol::BROADCAST_ADDR):
               Protocol::Response (code), address (address & Protocol::BROADCAST_ADDR)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // Read Session Messages
         // =============================================================================

         /*!
          * Start Read Session Command Message.
          */
         struct StartSessionResponse:public Protocol::Response
         {
            uint16_t count; //!< Number of device entries.

            StartSessionResponse(uint16_t count = 0):
               count (count)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         /*!
          * Get Entries Command Message.
          */
         struct GetEntriesMessage
         {
            uint16_t offset; //! Start index for the first entry to be provided.
            uint8_t  count;  //! Number of entries to be sent in the response.

            GetEntriesMessage(uint16_t offset = 0, uint8_t count = 0):
               offset (offset), count (count)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         struct GetEntriesResponse:public Protocol::Response
         {
            std::vector <Device> entries;

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * Parent class for the Device Management interface implementation.
          */
         struct Abstract:public Service <HF::Interface::DEVICE_MANAGEMENT>
         {
            static constexpr uint16_t START_ADDR = 0x0001;

            protected:

            Abstract(Unit0 &unit):
               Service (unit)
            {}
         };

         /*!
          * Device Management interface : Client side.
          */
         class Client:public ServiceRole <Abstract, HF::Interface::CLIENT_ROLE>
         {
            protected:

            uint16_t _address; //! Device HAN-FUN Address.

            public:

            Client(Unit0 &unit):
               ServiceRole (unit), _address (Protocol::BROADCAST_ADDR)
            {}

            virtual ~Client() {}

            /*!
             * Return the address given by the HF Concentrator to the Device.
             *
             * @return  the device HAN-FUN address.
             */
            virtual uint16_t address () const
            {
               return _address;
            }

            // ======================================================================
            // Commands
            // ======================================================================
            //! \name Commands
            //! @{

            /*!
             * Send a register message.
             */
            void register_device ();

            /*!
             * Send a de-register message for the given \c address.
             *
             * @param [in] address    the address of the device to de-register.
             */
            void deregister (uint16_t address);

            void deregister ()
            {
               deregister (_address);
            }

            //! @}
            // ======================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! \name Events
            //! @{

            /*!
             * This method is called when a response to a registration message
             * is received.
             *
             * @param [in] response  the register response that was received.
             */
            virtual void registered (RegisterResponse &response);

            /*!
             * This method is called when a response to a de-registration message
             * is received.
             *
             * @param [in] response    the response received.
             */
            virtual void deregistered (DeregisterResponse &response);

            //! @}
            // ======================================================================

            protected:

            using ServiceRole::payload_size;

            //! \see AbstractInterface::payload_size
            size_t payload_size (Protocol::Message::Interface &itf) const;

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);
         };

         /*!
          * Device Management interface : Server side.
          */
         struct Server:public ServiceRole <Abstract, HF::Interface::SERVER_ROLE>
         {
            Server(HF::Devices::Concentrator::IUnit0 &unit);

            virtual ~Server() {}

            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Return a reference to the unit that this service belongs to.
             *
             * This is the same reference as AbstractService::unit, but static casted
             * to allow access to the the other interfaces.
             *
             * @return  a reference to the unit that holds this interface.
             */
            HF::Devices::Concentrator::IUnit0 &unit0 ();

            /*!
             * Return the Device entry for the given address.
             *
             * @param [in] address    the device address.
             *
             * @retval  a pointer the Device entry associated with the given address,
             * @retval  nullptr if the entry does not exist.
             */
            virtual DevicePtr entry (uint16_t address) const = 0;

            /*!
             * Return the Device entry for the given UID.
             *
             * @param [in] uid   the device UID.
             *
             * @retval  a pointer the Device entry associated with the given UID,
             * @retval  nullptr if the entry does not exist.
             */
            virtual DevicePtr entry (const HF::UID::UID &uid) const = 0;

            /*!
             * Store the given \c device entry to persistent storage.
             *
             * @param [in] device   the device entry to store.
             *
             * @return     if the device entry was saved.
             */
            virtual Common::Result save (Device *device) = 0;

            /*!
             * Remove the given \c device entry from persistent storage.
             *
             * @param [in] device   the device entry to remove.
             *
             * @return     if the device entry was removed.
             */
            virtual Common::Result destroy (Device *device) = 0;

            /*!
             * Return the number of Device entries available.
             *
             * @return  number of Device entries present.
             */
            virtual uint16_t entries_count () const = 0;

            /*!
             * Return the a vector with \c count Device entries starting at \c offset.
             *
             * @param [in] offset   the offset to start at.
             * @param [in] count    the
             * @return
             */
            virtual std::vector <const Device *> entries (uint16_t offset, uint16_t count) const = 0;

            /*!
             * Return all device entries starting at \c offset.
             *
             * @param [in] offset   the offset to start at.
             *
             * @return a vector containing the requested entries.
             */
            std::vector <const Device *> entries (uint16_t offset = 0)
            {
               if (offset < entries_count ())
               {
                  return entries (offset, static_cast <uint16_t>(entries_count () - offset));
               }
               else
               {
                  return std::vector <const Device *>(0);
               }
            }

            // =============================================================================
            // Interface Attribute API.
            // =============================================================================

            //! \see Interface::attribute
            HF::Attributes::IAttribute *attribute (uint8_t uid)
            {
               return Core::create_attribute (this, uid);
            }

            protected:

            Server(Unit0 &unit):ServiceRole (unit)
            {}

            //! \see AbstractInterface::attributes
            HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED (pack_id);
               return HF::Attributes::UIDS {NUMBER_OF_ENTRIES_ATTR};
            }

            /*!
             * Return next available address for registering a device.
             *
             * @return  the address to use in the next registration.
             */
            virtual uint16_t next_address () = 0;

            // ======================================================================
            // Events
            // ======================================================================
            //! \name Events
            //! @{

            /*!
             * This method is called when a registration message is received.
             *
             * \see DeviceManagementServer::handle
             */
            virtual Common::Result register_device (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

            /*!
             * This method is called when a deregistration message is received.
             *
             * \see DeviceManagementServer::handle
             */
            virtual Common::Result deregister_device (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);

            //! @}
            // ======================================================================

            // ======================================================================
            // Commands
            // ======================================================================
            //! \name Commands
            //! @{

            /*!
             * This method serves to indicate if a given \c member of the interface
             * can be used by the \c source device affecting the \c destination
             * device configuration on the system.
             *
             * @param [in] member       interface member UID.
             * @param [in] source       device entry for the requesting device.
             * @param [in] destination  device entry for the affected device.
             *
             * @retval  true     the operation is allowed,
             * @retval  false    otherwise.
             */
            virtual bool authorized (uint8_t member, DevicePtr &source, DevicePtr &destination) = 0;

            /*!
             * De-register the device that corresponds to the given Device entry.
             *
             * \warning this method by-passes the authorization scheme.
             *
             * @param [in] device   reference to the device entry to de-register.
             *
             * @return the result of the destroy method.
             */
            virtual Common::Result deregister (Device &device);

            //! @}
            // ======================================================================

            using ServiceRole <Abstract, HF::Interface::SERVER_ROLE>::payload_size;

            //! \see AbstractInterface::payload_size
            size_t payload_size (Protocol::Message::Interface &itf) const;

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset);
         };

         // =========================================================================
         // Default API Implementations
         // =========================================================================

         /*!
          * This class provide a simple RAM based implementation of the DeviceManagement::Server
          * interface.
          */
         struct DefaultServer:public Server
         {
            DefaultServer(Unit0 &unit):Server (unit)
            {}

            virtual ~DefaultServer();

            // =============================================================================
            // API
            // =============================================================================

            DevicePtr entry (uint16_t address) const;

            DevicePtr entry (const HF::UID::UID &uid) const;

            Common::Result save (Device *device);

            Common::Result destroy (Device *device);

            uint16_t entries_count () const
            {
               return _entries.size ();
            }

            std::vector <const DeviceManagement::Device *> entries (uint16_t offset, uint16_t count) const;

            using Server::entries;

            uint16_t next_address ();

            protected:

            std::vector <Device *> _entries;

            virtual bool authorized (uint8_t member, DevicePtr &source, DevicePtr &destination);
         };

         // =============================================================================
         // Operators
         // =============================================================================

         inline bool operator ==(const Unit &lhs, const Unit &rhs)
         {
            if (lhs.id != rhs.id)
            {
               return false;
            }

            if (lhs.profile != rhs.profile)
            {
               return false;
            }

            if (lhs.interfaces.size () != rhs.interfaces.size ())
            {
               return false;
            }

            for (uint8_t i = 0; i < lhs.interfaces.size (); i++)
            {
               if (lhs.interfaces[i] != rhs.interfaces[i])
               {
                  return false;
               }
            }

            return true;
         }

         inline bool operator !=(const Unit &lhs, const Unit &rhs)
         {
            return !(lhs == rhs);
         }

         inline bool operator ==(const Device &lhs, const Device &rhs)
         {
            if (lhs.address != rhs.address)
            {
               return false;
            }

            if (lhs.units.size () != rhs.units.size ())
            {
               return false;
            }

            for (uint8_t i = 0; i < lhs.units.size (); i++)
            {
               if (lhs.units[i] != rhs.units[i])
               {
                  return false;
               }
            }

            return true;
         }

         inline bool operator !=(const Device &lhs, const Device &rhs)
         {
            return !(lhs == rhs);
         }

      } // namespace DeviceManagement

   } // namespace Core

} // namespace HF

#endif /* HF_DEVICE_MANGEMENT_H */
