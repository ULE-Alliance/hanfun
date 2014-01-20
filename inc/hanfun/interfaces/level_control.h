// =============================================================================
/*!
 * \file          inc/hanfun/interfaces/level_control.h
 *
 * This file contains the definitions for the Level Control interface.
 *
 * \author        Filipe Alves <filipe.alves@bithium.com>
 *
 * \version	      x.x.x
 *
 * \copyright     Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#ifndef HF_ITF_LEVEL_CONTROL_H
#define HF_ITF_LEVEL_CONTROL_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      /*!
       * Level Control Interface : Parent.
       *
       * This is the parent class for the Level Control interface implementation.
       */
      struct LevelControl:public Base <Interface::LEVEL_CONTROL>
      {
         //! Command IDs.
         enum
         {
            SET_LEVEL_CMD = 0x01       //!< Set Level Command ID.
         };

         struct Message:public Serializable
         {
            uint8_t level;

            Message(uint8_t level = 0):level (level) {}

            //! \see HF::Serializable::size.
            size_t size () const
            {
               return sizeof(level);
            }

            //! \see HF::Serializable::pack.
            size_t pack (ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, level);

               return offset - start;
            }

            //! \see HF::Serializable::unpack.
            size_t unpack (const ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += array.read (offset, level);

               return offset - start;
            }
         };
      };

      /*!
       * Level Control Interface : Server side implementation.
       *
       * This class provides the server side of the Level Control interface.
       */
      class LevelControlServer:public InterfaceRole <LevelControl, Interface::SERVER_ROLE>
      {
         protected:

         //! Current level value.
         uint8_t _level;

         public:

         //! Constructor
         LevelControlServer():_level (0) {}

         // ======================================================================
         // API
         // ======================================================================

         /*!
          * Getter for the current level.
          *
          * @return  the current level.
          */
         uint8_t level ();

         /*!
          * Setter for the server level.
          *
          * @param new_level  the new level value to use.
          */
         void level (uint8_t new_level);

         //! \see Interface::handle.
         bool handle (Protocol::Message &message, ByteArray &payload, size_t offset);

         // =============================================================================
         // Events
         // =============================================================================
         //! \name Events
         //! @{

         /*!
          * Callback for a \c SET_LEVEL_CMD message.
          *
          * @param [in] new_level    the new level value to use.
          */
         virtual void level_change (uint8_t new_level);

         //! @}
         // =============================================================================
      };

      /*!
       * Level Control Interface : Client side implementation.
       *
       * This class provides the client side of the Level Control interface.
       */
      class LevelControlClient:public InterfaceRole <LevelControl, Interface::CLIENT_ROLE>
      {
         public:

         // ======================================================================
         // Commands
         // ======================================================================
         // \name Commands
         //@{

         /*!
          * Send \c SET_LEVEL_CMD message to the given network address and
          * the given level.
          *
          * @param [in] addr       network address to send the message to.
          * @param [in] new_level    level value to send in the message.
          */
         void level (Protocol::Address &addr, uint8_t new_level);

         //@}
      };

   }  // namespace Interfaces

}   // namespace HF

#endif /* LEVEL_CONTROL_H_ */
