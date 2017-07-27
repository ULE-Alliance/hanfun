// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/colour_control.h
 *
 * This file contains the definitions for the Colour Control interface.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HF_ITF_COLOUR_CONTROL_H
#define HF_ITF_COLOUR_CONTROL_H

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

namespace HF
{
   namespace Interfaces
   {
      // Forward declaration.
      namespace ColourControl
      {
         class IServer;
      }

      /*!
       * @ingroup colour_control_itf
       *
       * Create an attribute object that can hold the attribute with the given @c uid.
       *
       * If @c server is not equal to @c nullptr then initialize it with the current
       * value.
       *
       * @param [in] server   pointer to the object to read the current value from.
       * @param [in] uid      attribute's UID to create the attribute object for.
       *
       * @return  pointer to an attribute object or @c nullptr if the attribute UID does not
       *          exist.
       */
      HF::Attributes::IAttribute *create_attribute(ColourControl::IServer *server, uint8_t uid);

      /*!
       * This namespace contains the implementation of the Colour Control interface.
       */
      namespace ColourControl
      {
         /*!
          * @addtogroup colour_control_itf  Colour Control interface
          * @ingroup interfaces
          *
          * This module contains the classes that define and implement the Colour Control interface API.
          * @{
          */
         //! Command IDs.
         typedef enum _CMD
         {
            MOVE_TO_HUE_CMD                = 0x01, //!< Move To Hue command UID.
            MOVE_HUE_CMD                   = 0x02, //!< Move Hue command UID.
            STEP_HUE_CMD                   = 0x03, //!< Step Hue command UID.
            MOVE_TO_SATURATION_CMD         = 0x04, //!< Move To Saturation command UID.
            MOVE_SATURATION_CMD            = 0x05, //!< Move Saturation command UID.
            STEP_SATURATION_CMD            = 0x06, //!< Step Saturation command UID.
            MOVE_TO_HUE_AND_SATURATION_CMD = 0x07, //!< Move To Hue And Saturation command UID.
            MOVE_TO_XY_CMD                 = 0x08, //!< Move To Xy command UID.
            MOVE_XY_CMD                    = 0x09, //!< Move Xy command UID.
            STEP_XY_CMD                    = 0x0a, //!< Step Xy command UID.
            MOVE_TO_COLOUR_TEMPERATURE_CMD  = 0x0b, //!< Move To Colour Temperature command UID.
            STOP_CMD                       = 0x0c, //!< Stop command UID.
            __LAST_CMD__                   = STOP_CMD
         } CMD;

         //! Attributes
         typedef enum _Attributes
         {
            SUPPORTED_ATTR          = 0x01,  //!< Supported attribute UID.
            MODE_ATTR               = 0x02,  //!< Mode attribute UID.
            HUE_AND_SATURATION_ATTR = 0x03,  //!< Hue And Saturation attribute UID.
            XY_ATTR                 = 0x04,  //!< XY attribute UID.
            COLOUR_TEMPERATURE_ATTR  = 0x05, //!< Colour Temperature attribute UID.
            __LAST_ATTR__           = COLOUR_TEMPERATURE_ATTR
         } Attributes;

         //! Mask elements for Colour Modes.
         typedef enum _Mask : uint8_t
         {
            HS_MODE           = 0x01,              //!< Hue and Saturation mode.
            XY_MODE           = 0x02,              //!< XY mode. CIE 1931 standard.
            TEMPERATURE_MODE  = 0x04,              //!< Colour temperature mode.
            __LAST_MODE__     = TEMPERATURE_MODE
         }Mask;

         //! Direction of movement
         typedef enum _Direction : uint8_t
         {
            UP                = 0x01,
            DOWN              = 0x02,
            SHORTEST          = 0x03,
            LONGEST           = 0x04,
            __LAST_DIRECTION_ = LONGEST
         }Direction;

         /*!
          * Helper class that supports the Hue and Saturation colour mode.
          *
          * This class implements the necessary functions
          * for message serialization.
          */
         struct HS_Colour
         {
            uint16_t hue;
            uint8_t saturation;

            HS_Colour () = default;

            /*!
             * Constructor
             *
             * @param [in] hue            hue colour value.
             * @param [in] saturation     saturation colour value.
             */
            HS_Colour (uint16_t hue, uint8_t saturation) :
                  saturation(saturation)
            {
               this->hue = hue<=HUE_MAX ? hue : HUE_MAX;
            }


            /*!
             * Helper method to invert a traveling angle.
             *
             * This method returns the exterior angle from an internal one,
             * or an interior from an exterior angle.
             *
             * @note This also inverts the angle signal.
             *       If the input angle is positive (CW rotation),
             *       the output is negative (CCW rotation).
             *
             * @param [in] angle    the angle to invert.
             * @return              the inverted angle.
             */
            static int32_t invert_angle (const int32_t angle);

            /*!
             * Helper method to get the angle between two hue values.
             *
             * This method takes in consideration the direction of travel from the initial
             * to the final hue value.
             *
             * @param [in] dir            the travel direction.
             * @param [in] initial_hue    the initial hue value.
             * @param [in] final_hue      the final hue value.
             *
             * @return                    the angle between the final and initial hue value.
             */
            static int32_t get_hue_travel_distance(const Direction dir,
                                                   const uint16_t initial_hue,
                                                   uint16_t final_hue);

            static constexpr uint16_t HUE_MAX = 359;                    // Max Hue Value

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(hue)            // hue
                                                 + sizeof(saturation);  // saturation

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);

            //! @copydoc HF::Attributes::IAttribute::compare
            int compare (const HS_Colour &other) const
            {
               if (this->hue != other.hue)
               {
                  return (this->hue - other.hue);
               }
               else
               {
                  if (this->hue != other.hue)
                  {
                     return (this->hue - other.hue);
                  }
                  else
                  {
                     return 0;
                  }
               }
            }

            //! @copydoc HF::Attributes::IAttribute::changed
            float changed (const HS_Colour &other) const
            {
               if (this->hue != other.hue)
               {
                  return (100 * (this->hue - other.hue) / static_cast<float>(this->hue));
               }
               else
               {
                  if (this->hue != other.hue)
                  {
                     return (100 * (this->hue - other.hue) / static_cast<float>(this->hue));
                  }
                  else
                  {
                     return 0.0f;
                  }
               }
            }
         };

         /*!
          * Helper class that supports the XY colour mode.
          *
          * This class implements the necessary functions
          * for message serialization.
          */
         struct XY_Colour
         {
            uint16_t X;
            uint16_t Y;

            XY_Colour() = default;

            /*!
             * Constructor
             *
             * @param [in] X     X colour value.
             * @param [in] Y     Y colour value.
             */
            XY_Colour(uint16_t X, uint16_t Y): X(X),Y(Y)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X)  // X
                                               + sizeof(Y); // Y

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);

            //! @copydoc HF::Attributes::IAttribute::compare
            int compare (const XY_Colour &other) const
            {
               if( this->X != other.X)
               {
                  return (this->X-other.X);
               }
               else
               {
                  if (this->Y != other.Y)
                  {
                     return (this->Y - other.Y);
                  }
                  else
                  {
                     return 0;
                  }
               }
            }

            //! @copydoc HF::Attributes::IAttribute::changed
            float changed (const XY_Colour &other) const
            {
               if (this->X != other.X)
               {
                  return ( 100*(this->X - other.X)/static_cast<float>(this->X));
               }
               else
               {
                  if (this->Y != other.Y)
                  {
                     return ( 100*(this->Y - other.Y)/static_cast<float>(this->Y));
                  }
                  else
                  {
                     return 0.0f;
                  }
               }
            }
         };

         // =============================================================================
         // Attribute Helper classes
         // =============================================================================

         /*!
          * Helper class to handle the Supported attribute for the Colour Control interface.
          */
         struct Supported: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = SUPPORTED_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;          //!< Attribute Read/Write

            Supported(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Mode attribute for the Colour Control interface.
          */
         struct Mode: public HF::Attributes::Attribute<uint8_t>
         {
            static constexpr uint8_t ID       = MODE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;     //!< Attribute Read/Write

            Mode(uint8_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint8_t>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Hue And Saturation attribute for the Colour Control interface.
          */
         struct HueAndSaturation: public HF::Attributes::Attribute<HS_Colour>
         {
            static constexpr uint8_t ID       = HUE_AND_SATURATION_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                   //!< Attribute Read/Write

            HueAndSaturation(HS_Colour value = HS_Colour(0,0), HF::Interface *owner = nullptr):
               Attribute<HS_Colour>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the XY attribute for the Colour Control interface.
          */
         struct Xy: public HF::Attributes::Attribute<XY_Colour>
         {
            static constexpr uint8_t ID       = XY_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;   //!< Attribute Read/Write

            Xy(XY_Colour value = XY_Colour(0,0), HF::Interface *owner = nullptr):
               Attribute<XY_Colour>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Colour Temperature attribute for the Colour Control interface.
          */
         struct ColourTemperature: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = COLOUR_TEMPERATURE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                  //!< Attribute Read/Write

            ColourTemperature(uint16_t value = 0, HF::Interface *owner = nullptr):
               Attribute<uint16_t>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * @copybrief HF::Interfaces::create_attribute (HF::Interfaces::ColourControl::Server *,uint8_t)
          *
          * @see HF::Interfaces::create_attribute (HF::Interfaces::ColourControl::Server *,uint8_t)
          *
          * @param [in] uid   attribute %UID to create the attribute object for.
          *
          * @retval  pointer to an attribute object
          * @retval  <tt>nullptr</tt> if the attribute UID does not exist.
          */
         HF::Attributes::IAttribute *create_attribute(uint8_t uid);

         // =============================================================================
         // Commands Messages classes
         // =============================================================================

         /*!
          * Move to Hue message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToHueMessage
         {
            uint16_t hue;           //!< The value of the new Hue.
            Direction direction;    //!< Direction of movement.
            uint16_t time;          //!< Time of transition in units of 100msec.

            static constexpr uint16_t HUE_MAX = 359;                    // Max Hue Value

            /*!
             * Constructor
             *
             * @param [in] hue      New hue value.
             * @param [in] dir      Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveToHueMessage (uint16_t hue = 0,
                              Direction direction = Direction::SHORTEST,
                              uint16_t time = 0):
                     direction(direction), time(time)
            {
               this->hue = hue<=HUE_MAX ? hue : HUE_MAX;
            }


            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(hue)            // Hue
                                               + sizeof(uint8_t)        // Direction
                                               + sizeof(time);          // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move Hue Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveHueMessage
         {
            Direction direction;    //!< @c Direction of movement.
            uint16_t rate;          //!< Time of transition in units of 100msec.

            static constexpr uint16_t RATE_MAX = 359;
            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] dir      @c Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveHueMessage (Direction dir = Direction::UP, uint16_t rate = 0) :
                  direction(dir), rate(rate)
            {
               this->rate = rate<=RATE_MAX ? rate : RATE_MAX;
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Direction
                                               + sizeof(rate);       // Rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Step Hue Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct StepHueMessage
         {
            uint8_t step_size;      //!< Step size in degrees.
            Direction direction;    //!< @c Direction of movement.
            uint8_t time;          //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] step_size   Step size in degrees.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            StepHueMessage (uint8_t step_size = 0, Direction dir = Direction::UP, uint8_t time = 0) :
                  step_size(step_size), direction(dir), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(step_size)     // Step size
                                               + sizeof(uint8_t)     // Direction
                                               + sizeof(time);   // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToSaturationMessage
         {
            uint8_t saturation;      //!< The value of new saturation.
            Direction direction;    //!< @c Direction of movement.
            uint16_t time;          //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] saturation  The value of new saturation.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToSaturationMessage (uint8_t saturation = 0,
                                     Direction dir = Direction::UP,
                                     uint16_t time = 0) :
                  saturation(saturation), direction(dir), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(saturation)      // Step size
                                                 + sizeof(uint8_t)      // Direction
                                                 + sizeof(time);        // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveSaturationMessage
         {
            Direction direction;    //!< @c Direction of movement.
            uint8_t rate;          //!< Time of transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] dir      @c Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveSaturationMessage (Direction dir = Direction::UP, uint8_t rate = 0) :
                  direction(dir), rate(rate)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Direction
                                                 + sizeof(rate);       // Rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Step Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct StepSaturationMessage
         {
            uint8_t step_size;      //!< Step size in degrees.
            Direction direction;    //!< @c Direction of movement.
            uint8_t time;          //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] step_size   Step size in degrees.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            StepSaturationMessage (uint8_t step_size = 0,
                                   Direction dir = Direction::UP,
                                   uint8_t time = 0) :
                  step_size(step_size), direction(dir), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(step_size)     // Step size
                                                 + sizeof(uint8_t)     // Direction
                                                 + sizeof(time);   // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to Hue and Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToHueSaturationMessage
         {
            HS_Colour colour;       //!< New Hue and Saturation Colour.
            Direction direction;    //!< @c Direction of movement.
            uint16_t time;          //!< Time of a single step transition in units of 100msec.

            static constexpr uint16_t HUE_MAX = 359;

            /*!
             * Constructor
             *
             * @param [in] hue         New hue value.
             * @param [in] saturation  The value of new saturation.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToHueSaturationMessage (HS_Colour colour = HS_Colour(0,0),
                                     Direction dir = Direction::UP,
                                     uint16_t time = 0) :
                  colour(colour), direction(dir), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = HS_Colour::min_size    // HS colour
                                               + sizeof(uint8_t)        // Direction
                                               + sizeof(time);          // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to XY Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToXYMessage
         {
            XY_Colour colour;
            uint16_t time;          //!< Time of a single step transition in units of 100msec.

            /*!
             * Constructor
             *
             * @param [in] colour       The colour value.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToXYMessage (XY_Colour colour = XY_Colour(0,0), uint16_t time = 0) :
                  colour(colour), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(XY_Colour::X)    // X
                                               + sizeof(XY_Colour::Y)    // Y
                                               + sizeof(time);          // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move XY Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveXYMessage
         {
            int16_t X_rate;
            int16_t Y_rate;

            /*!
             * Constructor
             *
             * @param [in] X_rate       The rate of change in units per seconds
             * @param [in] Y_step       The rate of change in units per seconds
             */
            MoveXYMessage (int16_t X_rate = 0, int16_t Y_rate = 0) :
               X_rate(X_rate), Y_rate(Y_rate)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X_rate)  // X_rate
                                               + sizeof(Y_rate); // Y_rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Step XY Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct StepXYMessage
         {
            int16_t X_step;
            int16_t Y_step;
            uint8_t time;     //!< Time of a single step transition in units of 100msec.

            /*!
             * Constructor
             *
             * @param [in] X_step       The rate of change in units per seconds
             * @param [in] Y_step       The rate of change in units per seconds
             * @param [in] time         Time of a single step transition in units of 100msec.
             */
            StepXYMessage (int16_t X_step = 0, int16_t Y_step = 0, uint8_t time = 0) :
                  X_step(X_step), Y_step(Y_step), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X_step)  // X_step
                                               + sizeof(Y_step)  // Y_step
                                               + sizeof(time);   // Time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to Colour Temperature Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToTemperatureMessage
         {
            uint16_t colour;
            uint16_t time;          //!< Time of a single step transition in units of 100msec.

            /*!
             * Constructor
             *
             * @param [in] colour       The colour value.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToTemperatureMessage (uint16_t colour = 0, uint16_t time = 0) :
                  colour(colour), time(time)
            {
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(colour)   // temperature
                                               + sizeof(time);   // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size () const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack (Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack (const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Colour Control %Interface : Parent.
          *
          * This is the parent class for the Colour Control interface implementation.
          */
         struct Base: public Interface<HF::Interface::COLOUR_CONTROL>
         {
            protected:

            //! Constructor
            Base(): Interface<HF::Interface::COLOUR_CONTROL>() {}
         };

         /*!
          * Colour Control %Interface : %Server side implementation.
          *
          * This class provides the server side of the Colour Control interface.
          */
         class IServer: public InterfaceRole<ColourControl::Base, HF::Interface::SERVER_ROLE>
         {
            public:

            /*!
             * Union that contains the callback arguments that
             * are sent between the APP and the HANFUN implementation.
             */
            typedef union _callback_args_t
            {
               //! Hue or Saturation arguments.
               struct _hs
               {
                  uint16_t n_steps; //!< Counter for the steps needed.
                  int32_t step;     //!< Hue or Saturation step
                  uint16_t end;     //!< End value to stop the iteration.
               } hs;

               //! Hue + Saturation arguments.
               struct _has
               {
                  typedef struct _colour
                  {
                     int32_t hue;
                     int32_t saturation;
                  } colour;

                  uint16_t n_steps;       //!< Counter for the steps needed.
                  colour step;            //!< Hue and saturation step.
                  HS_Colour end;             //!< End value to stop the iteration.
               } has;

               //! XY arguments.
               struct _xy
               {
                  typedef struct _colour
                  {
                     int32_t X;
                     int32_t Y;
                  } colour;

                  uint16_t n_steps; //!< Counter for the steps needed.
                  colour step;   //!< XY colour step.
                  XY_Colour end;    //!< XY value to stop the iteration.
               } xy;

               //! Color temperature arguments.
               struct _temperature
               {
                  uint16_t n_steps; //!< Counter for the steps needed.
                  int32_t step;    //!< Colour temperature step.
                  uint16_t end;     //!< Colour temperature end value to stop the iteration.
               } temperature;

            } callback_args_t;

            callback_args_t callback_args;

            //! Typedef for the callback function.
            typedef bool (IServer::*fptr) (callback_args_t &arg);

            protected:

            uint8_t _supported; //!< Supported
            uint8_t _mode;      //!< Mode

#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR

            HS_Colour _hue_and_saturation;   //!< Hue And Saturation

#endif

#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR

            XY_Colour _xy;   //!< XY

#endif

#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR

            uint16_t _colour_temperature;   //!< Colour Temperature

#endif

            public:

            //! Constructor
            IServer(): InterfaceRole<ColourControl::Base, HF::Interface::SERVER_ROLE>() {}

            //! Destructor
            virtual ~IServer() {}

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Inform the APP that its necessary to call the @c callback function
             * with @c arg arguments after @ time has passed.
             *
             * @param [in] time        time in units of 100ms to wait before calling the callback.
             * @param [in] callback    the callback
             * @param [in] arg         the arguments to be passed on the callback.
             */
            virtual void add_transition(uint16_t time, fptr callback, callback_args_t *arg) =0;

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_HUE_CMD,
             * is received.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] message     the @c MoveToHueMessage received.
             */
            virtual Common::Result move_to_hue(const Protocol::Address &addr,
                                               const MoveToHueMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_HUE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message    the @c MoveHueMessage received.
             */
            virtual Common::Result move_hue(const Protocol::Address &addr,
                                            const MoveHueMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_HUE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepHueMessage received.
             */
            virtual Common::Result step_hue(const Protocol::Address &addr,
                                            const  StepHueMessage &message);

            /*!
             * Callback that is called when doing an animation on the HUE value.
             *
             * @param [in] arg   the arguments to the colour interaction.
             *
             * @retval 0 stop iteration.
             * @retval 1 continue iteration.
             */
            virtual bool hue_callback(callback_args_t &arg);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToSaturationMessage received.
             */
            virtual Common::Result move_to_saturation(const Protocol::Address &addr,
                                                      const MoveToSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveSaturationMessage received.
             */
            virtual Common::Result move_saturation(const Protocol::Address &addr,
                                                   const MoveSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepSaturationMessage received.
             */
            virtual Common::Result step_saturation(const Protocol::Address &addr,
                                                   const StepSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToHueSaturationMessage received.
             */
            virtual Common::Result move_to_hue_and_saturation(const Protocol::Address &addr,
                                                              const MoveToHueSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToXYMessage received.
             */
            virtual Common::Result move_to_xy (const Protocol::Address &addr,
                                               const MoveToXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveXYMessage received.
             */
            virtual Common::Result move_xy (const Protocol::Address &addr,
                                            const MoveXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepXYMessage received.
             */
            virtual Common::Result step_xy (const Protocol::Address &addr,
                                            const StepXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToTemperatureMessage received.
             */
            virtual Common::Result move_to_colour_temperature(const Protocol::Address &addr,
                                                              const MoveToTemperatureMessage &message);

#ifdef HF_ITF_COLOUR_CONTROL_STOP_CMD

            /*!
             * Callback that is called when a @c ColourControl::STOP_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            virtual Common::Result stop(const Protocol::Address &addr);

#endif

            //! @}
            // ======================================================================

            // =============================================================================
            // Get/Set API.
            // =============================================================================

            /*!
             * Get the Supported for the Colour Control server.
             *
             * @return  the current Supported.
             */
            uint8_t supported() const;

            /*!
             * Set the Supported for the Colour Control server.
             *
             * @param [in] __value the  Supported value to set the server to.
             */
            void supported(uint8_t __value);

            /*!
             * Get the Mode for the Colour Control server.
             *
             * @return  the current Mode.
             */
            uint8_t mode() const;

            /*!
             * Set the Mode for the Colour Control server.
             *
             * @param [in] __value the  Mode value to set the server to.
             */
            void mode(uint8_t __value);

#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR

            /*!
             * Get the Hue And Saturation for the Colour Control server.
             *
             * @return  the current Hue And Saturation.
             */
            HS_Colour hue_and_saturation() const;

            /*!
             * Set the Hue And Saturation for the Colour Control server.
             *
             * @param [in] __value the  Hue And Saturation value to set the server to.
             */
            void hue_and_saturation(HS_Colour __value);

#endif

#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR

            /*!
             * Get the XY for the Colour Control server.
             *
             * @return  the current XY.
             */
            XY_Colour xy() const;

            /*!
             * Set the XY for the Colour Control server.
             *
             * @param [in] __value the  XY value to set the server to.
             */
            void xy(XY_Colour __value);

#endif

#ifdef HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR

            /*!
             * Get the Colour Temperature for the Colour Control server.
             *
             * @return  the current Colour Temperature.
             */
            uint16_t colour_temperature() const;

            /*!
             * Set the Colour Temperature for the Colour Control server.
             *
             * @param [in] __value the  Colour Temperature value to set the server to.
             */
            void colour_temperature(uint16_t __value);

#endif

            // =============================================================================
            // Attribute API.
            // =============================================================================

            HF::Attributes::IAttribute *attribute(uint8_t uid);

            HF::Attributes::UIDS attributes(uint8_t pack_id =
                                               HF::Attributes::Pack::MANDATORY) const;

            protected:

            Common::Result handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                          uint16_t offset);
         };

         struct Server: public IServer
         {

         };

         /*!
          * Colour Control %Interface : %Client side implementation.
          *
          * This class provides the client side of the Colour Control interface.
          */
         struct Client: public InterfaceRole<ColourControl::Base, HF::Interface::CLIENT_ROLE>
         {
            Client(): InterfaceRole<ColourControl::Base, HF::Interface::CLIENT_ROLE>() {}

            virtual ~Client() {}

            // ======================================================================
            // Commands
            // ======================================================================
            //! @name Commands
            //! @{

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_hue(const Protocol::Address &addr, uint16_t hue,
                             Direction direction,
                             uint16_t transition);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_CMD,
             * to the broadcast network address.
             */
            void move_to_hue(uint16_t hue,
                             Direction direction,
                             uint16_t transition)
            {
               Protocol::Address addr;
               move_to_hue(addr, hue, direction, transition);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_HUE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_hue(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_HUE_CMD,
             * to the broadcast network address.
             */
            void move_hue()
            {
               Protocol::Address addr;
               move_hue(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_HUE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void step_hue(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_HUE_CMD,
             * to the broadcast network address.
             */
            void step_hue()
            {
               Protocol::Address addr;
               step_hue(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_saturation(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_to_saturation()
            {
               Protocol::Address addr;
               move_to_saturation(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_saturation(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_saturation()
            {
               Protocol::Address addr;
               move_saturation(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void step_saturation(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_SATURATION_CMD,
             * to the broadcast network address.
             */
            void step_saturation()
            {
               Protocol::Address addr;
               step_saturation(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_hue_and_saturation(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_to_hue_and_saturation()
            {
               Protocol::Address addr;
               move_to_hue_and_saturation(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_xy(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_XY_CMD,
             * to the broadcast network address.
             */
            void move_to_xy()
            {
               Protocol::Address addr;
               move_to_xy(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_xy(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_XY_CMD,
             * to the broadcast network address.
             */
            void move_xy()
            {
               Protocol::Address addr;
               move_xy(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void step_xy(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_XY_CMD,
             * to the broadcast network address.
             */
            void step_xy()
            {
               Protocol::Address addr;
               step_xy(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_colour_temperature(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD,
             * to the broadcast network address.
             */
            void move_to_colour_temperature()
            {
               Protocol::Address addr;
               move_to_colour_temperature(addr);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STOP_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void stop(const Protocol::Address &addr);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STOP_CMD,
             * to the broadcast network address.
             */
            void stop()
            {
               Protocol::Address addr;
               stop(addr);
            }

            //! @}
            // ======================================================================
         };

         /*! @} */

      }  // namespace ColourControl

   }  // namespace Interfaces

}   // namespace HF

/*!
 * @addtogroup colour_control_itf
 * @{
 */

// =============================================================================
// Stream Helpers
// =============================================================================

/*!
 * Convert the given @c command into a string and write it to the given @c stream.
 *
 * @param [in] stream   out stream to write the string to.
 * @param [in] command  role value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream, const HF::Interfaces::ColourControl::CMD command);

/*!
 * Convert the given @c attribute into a string and write it to the given @c stream.
 *
 * @param [in] stream      out stream to write the string to.
 * @param [in] attribute   attribute value to convert to a string.
 *
 * @return   <tt>stream</tt>
 */
std::ostream &operator<<(std::ostream &stream,
                         const HF::Interfaces::ColourControl::Attributes attribute);
/*! @} */

#endif /* HF_ITF_COLOUR_CONTROL_H */
