// =============================================================================
/*!
 * \file       inc/hanfun/core/device_mangement.h
 *
 * This file contains the definitions for the core Device Management Interface
 * of the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_DEVICE_MANGEMENT_H
#define HF_DEVICE_MANGEMENT_H

#include <vector>
#include <map>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

#include "hanfun/core.h"

using namespace HF;
using namespace HF::Interfaces;

namespace HF
{
   namespace Core
   {
      /*!
       * Parent class for the Device Management interface implementation.
       */
      struct DeviceManagement:public Service <Interface::DEVICE_MANAGEMENT>
      {
         static constexpr uint16_t START_ADDR = 0x0001;

         //! Commands.
         enum CMD
         {
            REGISTER_CMD      = 0x01, //!< Register device command.
            DEREGISTER_CMD    = 0x02, //!< De-register device command.
            START_SESSION_CMD = 0x03, //!< Start Session Read Registration Info.
            END_SESSION_CMD   = 0x04, //!< End Session Read Registration Info.
            GET_ENTRIES_CMD   = 0x05, //!< Get Entries Command.
         };

         // =============================================================================

         /*!
          * Optional interface entry.
          */
         struct Interface:public Serializable
         {
            uint16_t role : 1;    //!< Interface role. \see Interface::Role.
            uint16_t uid  : 15;   //!< Interface UID. \see Interface::UID.

            Interface(uint16_t role = 0, uint16_t uid = 0):
               role (role), uid (uid)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         /*!
          * Unit Entry
          */
         struct Unit:public Serializable
         {
            uint8_t            id;      //!< Unit Id.
            uint16_t           profile; //!< Unit UID. \see IProfile::UID.

            vector <Interface> opt_ift;    //!< Optional interfaces.

            Unit(uint8_t id = 0, uint16_t profile = 0):
               id (id), profile (profile)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         /*!
          * Device Entry.
          */
         struct Device:public Serializable
         {
            uint16_t      address; //!< Device Address.
            vector <Unit> units;   //!< Unit list of the interface.

            uint16_t      emc; //! Device EMC if applicable, 0 otherwise.

            HF::UID       *uid; //! Device UID.

            Device():address (Protocol::BROADCAST_ADDR), emc (0), uid (nullptr) {}

            virtual ~Device()
            {
               delete uid;
            }

            // =============================================================================
            // Serializable API
            // =============================================================================

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // Register Command Messages
         // =============================================================================

         /*!
          * Register command message.
          */
         struct RegisterMessage:public Serializable
         {
            uint16_t      emc;   //! Device EMC if applicable, 0 otherwise.
            vector <Unit> units; //! Device units listing.

            RegisterMessage(uint16_t emc = 0x0000):
               emc (emc), _uid (nullptr)
            {}

            virtual ~RegisterMessage();

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);

            HF::UID *uid () const
            {
               return _uid;
            }

            void uid (HF::UID *uid)
            {
               if (_uid != nullptr)
               {
                  delete _uid;
               }

               _uid = nullptr;

               if (uid != nullptr)
               {
                  _uid = uid->clone ();
               }
            }

            protected:

            HF::UID *_uid;     //! Device UID.
         };

         /*!
          * Register command response.
          */
         struct RegisterResponse:public Protocol::Response
         {
            uint16_t address;    //!< Address for the device to use.
            uint16_t emc;        //!< HAN-FUN Concentrator EMC if applicable.

            RegisterResponse(uint16_t address = 0, uint16_t emc = 0):
               address (address & Protocol::BROADCAST_ADDR), emc (emc)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // De-register Command Messages.
         // =============================================================================

         /*!
          * De-register command message.
          */
         struct DeregisterMessage:public Serializable
         {
            uint16_t address;    //!< Address of the device to de-register.

            DeregisterMessage(uint16_t address = 0):
               address (address)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // Read Session Messages
         // =============================================================================

         /*!
          * Start Read Session Command Message.
          */
         struct StartSessionResponse:public Protocol::Response
         {
            uint16_t count;   //!< Number of device entries.

            StartSessionResponse(uint16_t count = 0):
               count (count)
            {}

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         /*!
          * Get Entries Command Message.
          */
         struct GetEntriesMessage:public Serializable
         {
            uint16_t offset; //! Start index for the first entry to be provided.
            uint8_t  count;  //! Number of entries to be sent in the response.

            GetEntriesMessage(uint16_t offset = 0, uint8_t count = 0):
               offset (offset), count (count)
            {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         struct GetEntriesResponse:public Protocol::Response
         {
            vector <Device> entries;

            //! \see HF::Protocol::Response::size.
            size_t size () const;

            //! \see HF::Protocol::Response::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const;

            //! \see HF::Protocol::Response::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // API
         // =============================================================================

         protected:

         DeviceManagement(IDevice *_device):
            Service (_device)
         {}
      };

      /*!
       * Device Management interface : Server side.
       */
      class DeviceManagementServer:public ServiceRole <DeviceManagement, Interface::SERVER_ROLE>
      {
         public:

         //! \see Interface::handle
         bool handle (Protocol::Message &message, ByteArray &payload, size_t offset);

         protected:

         DeviceManagementServer(IDevice *_device):
            ServiceRole (_device)
         {}

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
         virtual bool register_device (Protocol::Message &message, ByteArray &payload,
                                       size_t offset);

         //! @}
         // ======================================================================
      };

      /*!
       * Device Management interface : Client side.
       */
      class DeviceManagementClient:public ServiceRole <DeviceManagement, Interface::CLIENT_ROLE>
      {
         protected:

         uint16_t _address;   //! Device HAN-FUN Address.

         public:

         DeviceManagementClient(IDevice *_device):
            ServiceRole (_device), _address (Protocol::BROADCAST_ADDR)
         {}

         //! \see Interface::handle
         virtual Result handle (Protocol::Message &message, ByteArray &payload, size_t offset);

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
          *
          * @param device  pointer to the device to register.
          */
         void register_device ();

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

         //! @}
         // ======================================================================
      };

   }  // namespace Core

}  // namespace HF

// =============================================================================
// Helper Functions
// =============================================================================

inline bool operator ==(const HF::Core::DeviceManagement::Interface &lhs,
                        const HF::Core::DeviceManagement::Interface &rhs)
{
   return (lhs.role == rhs.role) && (lhs.uid == lhs.uid);
}

inline bool operator !=(const HF::Core::DeviceManagement::Interface &lhs,
                        const HF::Core::DeviceManagement::Interface &rhs)
{
   return !(lhs == rhs);
}

inline bool operator ==(const HF::Core::DeviceManagement::Unit &lhs,
                        const HF::Core::DeviceManagement::Unit &rhs)
{
   if (lhs.id != rhs.id)
   {
      return false;
   }

   if (lhs.profile != rhs.profile)
   {
      return false;
   }

   if (lhs.opt_ift.size () != rhs.opt_ift.size ())
   {
      return false;
   }

   for (uint8_t i = 0; i < lhs.opt_ift.size (); i++)
   {
      if (lhs.opt_ift[i] != rhs.opt_ift[i])
      {
         return false;
      }
   }

   return true;
}

inline bool operator !=(const HF::Core::DeviceManagement::Unit &lhs,
                        const HF::Core::DeviceManagement::Unit &rhs)
{
   return !(lhs == rhs);
}

inline bool operator ==(const HF::Core::DeviceManagement::Device &lhs,
                        const HF::Core::DeviceManagement::Device &rhs)
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

inline bool operator !=(const HF::Core::DeviceManagement::Device &lhs,
                        const HF::Core::DeviceManagement::Device &rhs)
{
   return !(lhs == rhs);
}

#endif /* HF_DEVICE_MANGEMENT_H */
