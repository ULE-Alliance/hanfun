// =============================================================================
/*!
 * @file       inc/hanfun/interfaces/colour_control.h
 *
 * This file contains the definitions for the Colour Control interface.
 *
 * @version    1.5.0
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
            MOVE_TO_COLOUR_TEMPERATURE_CMD = 0x0b, //!< Move To Colour Temperature command UID.
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
            COLOUR_TEMPERATURE_ATTR = 0x05,  //!< Colour Temperature attribute UID.
            __LAST_ATTR__           = COLOUR_TEMPERATURE_ATTR
         } Attributes;

         //! Mask elements for Colour Modes.
         typedef enum _Mask : uint8_t
         {
            HS_MODE          = 0x01,               //!< Hue and Saturation mode.
            XY_MODE          = 0x02,               //!< XY mode. CIE 1931 standard.
            TEMPERATURE_MODE = 0x04,               //!< Colour temperature mode.
            __LAST_MODE__    = TEMPERATURE_MODE
         } Mask;

         //! Direction of movement
         typedef enum _Direction : uint8_t
         {
            UP                = 0x01,
            DOWN              = 0x02,
            SHORTEST          = 0x03,
            LONGEST           = 0x04,
            __LAST_DIRECTION_ = LONGEST
         } Direction;

         /*!
          * Helper class that supports the Hue and Saturation colour mode.
          *
          * This class implements the necessary functions
          * for message serialization.
          */
         struct HS_Colour
         {
            uint16_t hue;
            uint8_t  saturation;

            HS_Colour() = default;

            /*!
             * Constructor
             *
             * @param [in] hue            hue colour value.
             * @param [in] saturation     saturation colour value.
             */
            HS_Colour(uint16_t hue, uint8_t saturation):
               saturation(saturation)
            {
               this->hue = hue <= HUE_MAX ? hue : HUE_MAX;
            }

            static constexpr int HUE        = 360;
            static constexpr int SATURATION = 0x100;


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
            template<int max_value = HUE>
            static int32_t invert_angle(const int32_t angle)
            {
               int32_t temp = (max_value - std::abs(angle)) % max_value;

               if (angle >= 0)
               {
                  temp *= -1;
               }

               return temp;
            }

            /*!
             * Helper method to get the angle between two hue values.
             *
             * This method takes in consideration the direction of travel from the initial
             * to the final value.
             *
             * @param [in] dir            the travel direction.
             * @param [in] initial_hue    the initial hue value.
             * @param [in] final_hue      the final hue value.
             *
             * @return                    the angle between the final and initial value.
             */
            template<int max_value = HUE>
            static int32_t get_travel_distance(const Direction dir,
                                               const uint16_t initial_hue,
                                               uint16_t final_hue)
            {
               int32_t dist, result;

               if (final_hue < initial_hue)
               {
                  final_hue += max_value;
               }

               dist = (final_hue - initial_hue) % max_value;

               switch (dir)
               {
                  case Direction::UP:
                  {
                     result = dist;
                     break;
                  }
                  case Direction::DOWN:
                  {
                     result = invert_angle<max_value>(dist);
                     break;
                  }
                  case Direction::SHORTEST:
                  {
                     result = dist <= max_value / 2 ? dist : invert_angle<max_value>(dist);
                     break;
                  }
                  case Direction::LONGEST:
                  {
                     result = dist > max_value / 2 ? dist : invert_angle<max_value>(dist);
                     break;
                  }
               }

               return result;
            }

            static constexpr uint16_t HUE_MAX = 359;                    // Max Hue Value


            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(hue)            // hue
                                                 + sizeof(saturation);  // saturation

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

            //! @copydoc HF::Attributes::IAttribute::compare
            int compare(const HS_Colour &other) const
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
            float changed(const HS_Colour &other) const
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

            //! Operator equal
            bool operator==(const HS_Colour &other) const
            {
               return (hue == other.hue && saturation == other.saturation);
            }

            //! Operator not equal
            bool operator!=(const HS_Colour &other) const
            {
               return !operator==(other);
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
            XY_Colour(uint16_t X, uint16_t Y): X(X), Y(Y)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X)    // X
                                                 + sizeof(Y); // Y

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);

            //! @copydoc HF::Attributes::IAttribute::compare
            int compare(const XY_Colour &other) const
            {
               if (this->X != other.X)
               {
                  return (this->X - other.X);
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
            float changed(const XY_Colour &other) const
            {
               if (this->X != other.X)
               {
                  return (100 * (this->X - other.X) / static_cast<float>(this->X));
               }
               else
               {
                  if (this->Y != other.Y)
                  {
                     return (100 * (this->Y - other.Y) / static_cast<float>(this->Y));
                  }
                  else
                  {
                     return 0.0f;
                  }
               }
            }

            //! Operator equal.
            bool operator==(const XY_Colour &other) const
            {
               return (X == other.X && Y == other.Y);
            }

            //! Operator not equal.
            bool operator!=(const XY_Colour &other) const
            {
               return !operator==(other);
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

            static constexpr uint8_t get_suport(void)
            {
               return
#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
                  Mask::HS_MODE +
#endif
#ifdef HF_ITF_COLOUR_CONTROL_XY_ATTR
                  Mask::XY_MODE +
#endif
#ifdef HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR
                  Mask::TEMPERATURE_MODE
#endif
               ;
            }

            Supported(uint8_t value = get_suport(), HF::Interface *owner = nullptr):
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

            HueAndSaturation(HS_Colour value = HS_Colour(0, 0), HF::Interface *owner = nullptr):
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

            Xy(XY_Colour value = XY_Colour(0, 0), HF::Interface *owner = nullptr):
               Attribute<XY_Colour>(HF::Interface::COLOUR_CONTROL, ID, owner, value, WRITABLE)
            {}
         };

         /*!
          * Helper class to handle the Colour Temperature attribute for the Colour Control interface.
          */
         struct ColourTemperature: public HF::Attributes::Attribute<uint16_t>
         {
            static constexpr uint8_t ID       = COLOUR_TEMPERATURE_ATTR; //!< Attribute UID.
            static constexpr bool    WRITABLE = false;                   //!< Attribute Read/Write

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
            uint16_t                  hue;       //!< The value of the new Hue.
            Direction                 direction; //!< Direction of movement.
            uint16_t                  time;      //!< Time of transition in units of 100msec.

            static constexpr uint16_t HUE_MAX = 359;                    // Max Hue Value

            /*!
             * Constructor
             *
             * @param [in] hue      New hue value.
             * @param [in] dir      Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveToHueMessage(uint16_t hue = 0,
                             Direction direction = Direction::SHORTEST,
                             uint16_t time = 0):
               direction(direction), time(time)
            {
               this->hue = hue <= HUE_MAX ? hue : HUE_MAX;
            }


            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(hue)            // Hue
                                                 + sizeof(uint8_t)      // Direction
                                                 + sizeof(time);        // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move Hue Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveHueMessage
         {
            Direction                 direction; //!< @c Direction of movement.
            uint16_t                  rate;      //!< Time of transition in units of 100msec.

            static constexpr uint16_t RATE_MAX      = 359;
            static constexpr uint8_t  DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] dir      @c Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveHueMessage(Direction dir = Direction::UP, uint16_t rate = 0):
               direction(dir), rate(rate)
            {
               this->rate = rate <= RATE_MAX ? rate : RATE_MAX;
            }

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Direction
                                                 + sizeof(rate);     // Rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Step Hue Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct StepHueMessage
         {
            uint8_t                  step_size; //!< Step size in degrees.
            Direction                direction; //!< @c Direction of movement.
            uint8_t                  time;      //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] step_size   Step size in degrees.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            StepHueMessage(uint8_t step_size = 0, Direction dir = Direction::UP, uint8_t time = 0):
               step_size(step_size), direction(dir), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(step_size) // Step size
                                                 + sizeof(uint8_t) // Direction
                                                 + sizeof(time);   // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToSaturationMessage
         {
            uint8_t                  saturation; //!< The value of new saturation.
            Direction                direction;  //!< @c Direction of movement.
            uint16_t                 time;       //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] saturation  The value of new saturation.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToSaturationMessage(uint8_t saturation = 0,
                                    Direction dir = Direction::UP,
                                    uint16_t time = 0):
               saturation(saturation), direction(dir), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(saturation)     // Step size
                                                 + sizeof(uint8_t)      // Direction
                                                 + sizeof(time);        // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveSaturationMessage
         {
            Direction                direction; //!< @c Direction of movement.
            uint8_t                  rate;      //!< Time of transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] dir      @c Direction of movement
             * @param [in] time     Time for the movement
             */
            MoveSaturationMessage(Direction dir = Direction::UP, uint8_t rate = 0):
               direction(dir), rate(rate)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(uint8_t)     // Direction
                                                 + sizeof(rate);     // Rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Step Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct StepSaturationMessage
         {
            uint8_t                  step_size; //!< Step size in degrees.
            Direction                direction; //!< @c Direction of movement.
            uint8_t                  time;      //!< Time of a single step transition in units of 100msec.

            static constexpr uint8_t DIRECTION_MAX = Direction::DOWN;

            /*!
             * Constructor
             *
             * @param [in] step_size   Step size in degrees.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            StepSaturationMessage(uint8_t step_size = 0,
                                  Direction dir = Direction::UP,
                                  uint8_t time = 0):
               step_size(step_size), direction(dir), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(step_size) // Step size
                                                 + sizeof(uint8_t) // Direction
                                                 + sizeof(time);   // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to Hue and Saturation Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToHueSaturationMessage
         {
            HS_Colour                 colour;    //!< New Hue and Saturation Colour.
            Direction                 direction; //!< @c Direction of movement.
            uint16_t                  time;      //!< Time of a single step transition in units of 100msec.

            static constexpr uint16_t HUE_MAX = 359;

            /*!
             * Constructor
             *
             * @param [in] hue         New hue value.
             * @param [in] saturation  The value of new saturation.
             * @param [in] dir         @c Direction of movement.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToHueSaturationMessage(HS_Colour colour = HS_Colour(0, 0),
                                       Direction dir = Direction::UP,
                                       uint16_t time = 0):
               colour(colour), direction(dir), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = HS_Colour::min_size    // HS colour
                                                 + sizeof(uint8_t)      // Direction
                                                 + sizeof(time);        // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
         };

         /*!
          * Move to XY Message class.
          *
          * @copydetails HF::Interfaces::ColourControl::XY_Colour
          */
         struct MoveToXYMessage
         {
            XY_Colour colour;
            uint16_t  time;         //!< Time of a single step transition in units of 100msec.

            /*!
             * Constructor
             *
             * @param [in] colour       The colour value.
             * @param [in] time        Time of a single step transition in units of 100msec.
             */
            MoveToXYMessage(XY_Colour colour = XY_Colour(0, 0), uint16_t time = 0):
               colour(colour), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(XY_Colour::X)   // X
                                                 + sizeof(XY_Colour::Y) // Y
                                                 + sizeof(time);        // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
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
            MoveXYMessage(int16_t X_rate = 0, int16_t Y_rate = 0):
               X_rate(X_rate), Y_rate(Y_rate)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X_rate)    // X_rate
                                                 + sizeof(Y_rate); // Y_rate

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
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
            StepXYMessage(int16_t X_step = 0, int16_t Y_step = 0, uint8_t time = 0):
               X_step(X_step), Y_step(Y_step), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(X_step)   // X_step
                                                 + sizeof(Y_step) // Y_step
                                                 + sizeof(time);  // Time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
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
            MoveToTemperatureMessage(uint16_t colour = 0, uint16_t time = 0):
               colour(colour), time(time)
            {}

            //! Minimum pack/unpack required data size.
            static constexpr uint16_t min_size = sizeof(colour)  // temperature
                                                 + sizeof(time); // time

            //! @copydoc HF::Common::Serializable::size
            uint16_t size() const
            {
               return min_size;
            }

            //! @copydoc HF::Common::Serializable::pack
            uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const;

            //! @copydoc HF::Common::Serializable::unpack
            uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0);
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

         //! Interface for the Transitions
         struct ITransition
         {
            IServer  &server;          //!< The server instance

            uint16_t period;           //!< Time period for the transition (in 100 msec units).
            uint16_t remaining_time;   //!< Remaining time until the transition is ran.


            /*!
             * Constructor.
             *
             * @param [in] _server     Server instance
             * @param [in] period      The Transition period
             */
            ITransition(IServer &_server, uint16_t period):
               server(_server), period(period), remaining_time(period)
            {}

            virtual ~ITransition()
            {}


            /*!
             * Common part for the transition run code.
             *
             * This will update the remaining time and check if the transition should run.
             *
             * @param [in] time     time elapsed time since the last call.
             * @retval 0   Don't run the transition specialization.
             * @retval 1   Run the transition specialization.
             */
            virtual bool run(uint16_t time)
            {
               remaining_time -= time;

               if (remaining_time == 0)
               {
                  remaining_time = this->period;
                  return true;
               }

               return false;
            }

            /*!
             * Check if the transition should continue.
             *
             * @retval 0   The transition ended.
             * @retval 1   The transition will continue.
             */
            virtual bool next() = 0; // check if the transition continues...
         };


         //! Hue Transition
         struct Hue_Transition: public ITransition
         {
            int32_t  step;    //!< Hue or Saturation step
            uint16_t n_steps; //!< Counter for the steps needed.
            uint16_t end;     //!< End value to stop the iteration.

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] step        the step size for each transition iteration.
             * @param [in] n_steps     number of steps.
             * @param [in] end         end value for the transition.
             */
            Hue_Transition(IServer &_server, uint16_t period, int32_t step = 0,
                           uint16_t n_steps = 0, uint16_t end = 0):
               ITransition(_server, period), step(step), n_steps(n_steps), end(end)
            {}

            //! Default constructor.
            Hue_Transition() = default;

            //! Empty destructor.
            ~Hue_Transition()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         struct Hue_Transition_Continuous: public ITransition
         {
            int32_t step;     //!< Hue or Saturation step

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] step        the step size for each transition iteration.
             */
            Hue_Transition_Continuous(IServer &_server, uint16_t period, int32_t step = 0):
               ITransition(_server, period), step(step)
            {}

            //! Default constructor.
            Hue_Transition_Continuous() = default;

            //! Empty destructor.
            ~Hue_Transition_Continuous()
            {}

            //! @copydoc ColourControl::Hue_Transition::run()
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         //! Saturation Transition
         struct Saturation_Transition: public ITransition
         {
            int32_t  step;    //!< Hue or Saturation step
            uint16_t n_steps; //!< Counter for the steps needed.
            uint16_t end;     //!< End value to stop the iteration.

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] step        the step size for each transition iteration.
             * @param [in] n_steps     number of steps.
             * @param [in] end         end value for the transition.
             */
            Saturation_Transition(IServer &_server, uint16_t period, int32_t step = 0,
                                  uint16_t n_steps = 0,
                                  uint16_t end = 0):
               ITransition(_server, period), step(step), n_steps(n_steps), end(end)
            {}

            //! Default constructor.
            Saturation_Transition() = default;

            //! Empty destructor.
            ~Saturation_Transition()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         struct Saturation_Transition_Continuous: public ITransition
         {
            int32_t step;     //!< Hue or Saturation step

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] step        the step size for each transition iteration.
             */
            Saturation_Transition_Continuous(IServer &_server, uint16_t period, int32_t step = 0):
               ITransition(_server, period), step(step)
            {}

            //! Default constructor.
            Saturation_Transition_Continuous() = default;

            //! Empty destructor.
            ~Saturation_Transition_Continuous()
            {}

            //! @copydoc ColourControl::Hue_Transition::run()
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         //! Hue and Saturation Transition
         struct HS_Transition: public ITransition
         {
            int32_t   hue_step; //!< Hue step
            int32_t   sat_step; //!< Saturation step
            uint16_t  n_steps;  //!< Counter for the steps needed.
            HS_Colour end;      //!< End value to stop the iteration.

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] hue_step    the hue step size for each transition iteration.
             * @param [in] sat_step    the saturation step size for each transition iteration.
             * @param [in] n_steps     number of steps.
             * @param [in] end         end value for the transition.
             */
            HS_Transition(IServer &_server, uint16_t period,
                          int32_t hue_step = 0, int32_t sat_step = 0,
                          uint16_t n_steps = 0, HS_Colour end = HS_Colour(0, 0)):
               ITransition(_server, period), hue_step(hue_step),
               sat_step(sat_step), n_steps(n_steps), end(end)
            {}

            //! Default constructor.
            HS_Transition() = default;

            //! Empty destructor.
            ~HS_Transition()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         //! XY Transition
         struct XY_Transition: public ITransition
         {
            int32_t   X_step;  //!< X step
            int32_t   Y_step;  //!< Y step
            uint16_t  n_steps; //!< Counter for the steps needed.
            XY_Colour end;     //!< End value to stop the iteration.

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] X_step      the X step size for each transition iteration.
             * @param [in] Y_step      the Y step size for each transition iteration.
             * @param [in] n_steps     number of steps.
             * @param [in] end         end value for the transition.
             */
            XY_Transition(IServer &_server, uint16_t period,
                          int32_t X_step = 0, int32_t Y_step = 0,
                          uint16_t n_steps = 0,
                          XY_Colour end = XY_Colour(0, 0)):
               ITransition(_server, period), X_step(X_step), Y_step(Y_step),
               n_steps(n_steps), end(end)
            {}

            //! Default constructor.
            XY_Transition() = default;

            //! Empty destructor.
            ~XY_Transition()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         //! XY Transition
         struct XY_Transition_Continuous: public ITransition
         {
            int16_t X_step;   //!< X step
            int16_t Y_step;   //!< Y step

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] X_step      the X step size for each transition iteration.
             * @param [in] Y_step      the Y step size for each transition iteration.
             */
            XY_Transition_Continuous(IServer &_server, uint16_t period,
                                     int16_t X_step = 0, int16_t Y_step = 0):
               ITransition(_server, period), X_step(X_step), Y_step(Y_step)
            {}

            //! Default constructor.
            XY_Transition_Continuous() = default;

            //! Empty destructor.
            ~XY_Transition_Continuous()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         //! ColourTemperature Transition
         struct Temperature_Transition: public ITransition
         {
            int32_t  step;    //!< temperature step
            uint16_t n_steps; //!< Counter for the steps needed.
            uint16_t end;     //!< End value to stop the iteration.

            /*!
             * Constructor.
             *
             * @param [in] _server     server instance
             * @param [in] period      the Transition period. In units of 100msec.
             * @param [in] X_step      the X step size for each transition iteration.
             * @param [in] Y_step      the Y step size for each transition iteration.
             * @param [in] n_steps     number of steps.
             * @param [in] end         end value for the transition.
             */
            Temperature_Transition(IServer &_server, uint16_t period,
                                   int32_t step = 0,
                                   uint16_t n_steps = 0,
                                   uint16_t end = 0):
               ITransition(_server, period), step(step),
               n_steps(n_steps), end(end)
            {}

            //! Default constructor.
            Temperature_Transition() = default;

            //! Empty destructor.
            ~Temperature_Transition()
            {}

            /*!
             * Run the transition.
             *
             * @param [in] time elapsed time since the last call.
             * @retval 0   The transition didn't ran. Only the remaining time was updated.
             * @retval 1   The transition ran.
             */
            bool run(uint16_t time);

            //! @copydoc ITransition::next()
            bool next()
            {
               return (period != 0 ? true : false);
            }
         };

         /*!
          * Colour Control %Interface : %Server side implementation.
          *
          * This class provides the server side of the Colour Control interface.
          */
         class IServer: public InterfaceRole<ColourControl::Base, HF::Interface::SERVER_ROLE>
         {
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
                                            const StepHueMessage &message);

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
            virtual Common::Result move_to_xy(const Protocol::Address &addr,
                                              const MoveToXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveXYMessage received.
             */
            virtual Common::Result move_xy(const Protocol::Address &addr,
                                           const MoveXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepXYMessage received.
             */
            virtual Common::Result step_xy(const Protocol::Address &addr,
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

         /*!
          * Colour Control %Interface : %Server side implementation.
          *
          * This class provides the server side of the Colour Control interface.
          */
         struct Server: public IServer
         {
            //! Container for the transitions.
            using Container = typename std::vector<ITransition *>;

            public:

            //! Constructor
            Server(): IServer()
            {}

            //! Destructor
            virtual ~Server()
            {}

            /*!
             * Check if there are transitions to run.
             *
             * @return The time left for the next transition, or 0 if there are no transitions.
             */
            static uint16_t has_transitions()
            {
               return transitions.empty() ? 0 : transitions.front()->remaining_time;
            }

            /*!
             * Method responsible for updating the all the transitions.
             *
             * This method updates the time remaining until a transition needs to be ran.
             * If the remaining time is 0 then it runs the transition.
             *
             * @note This method should only be called when a transition should occur. This
             *       isn't a polling method.
             *
             * @return  period (in units of 100msec) until the next transition.
             */
            static uint16_t transition(void);

            // ======================================================================
            // Events
            // ======================================================================
            //! @name Events
            //! @{

            /*!
             * Inform the APP that a new transition was added.
             */
            virtual void changed(void) {}

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_HUE_CMD,
             * is received.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] message     the @c MoveToHueMessage received.
             */
            Common::Result move_to_hue(const Protocol::Address &addr,
                                       const MoveToHueMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_HUE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message    the @c MoveHueMessage received.
             */
            Common::Result move_hue(const Protocol::Address &addr,
                                    const MoveHueMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_HUE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepHueMessage received.
             */
            Common::Result step_hue(const Protocol::Address &addr,
                                    const StepHueMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToSaturationMessage received.
             */
            Common::Result move_to_saturation(const Protocol::Address &addr,
                                              const MoveToSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveSaturationMessage received.
             */
            Common::Result move_saturation(const Protocol::Address &addr,
                                           const MoveSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepSaturationMessage received.
             */
            Common::Result step_saturation(const Protocol::Address &addr,
                                           const StepSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToHueSaturationMessage received.
             */
            Common::Result move_to_hue_and_saturation(
               const Protocol::Address &addr,
               const MoveToHueSaturationMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToXYMessage received.
             */
            Common::Result move_to_xy(const Protocol::Address &addr,
                                      const MoveToXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveXYMessage received.
             */
            Common::Result move_xy(const Protocol::Address &addr,
                                   const MoveXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::STEP_XY_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c StepXYMessage received.
             */
            Common::Result step_xy(const Protocol::Address &addr,
                                   const StepXYMessage &message);

            /*!
             * Callback that is called when a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] message     the @c MoveToTemperatureMessage received.
             */
            Common::Result move_to_colour_temperature(
               const Protocol::Address &addr,
               const MoveToTemperatureMessage &message);

#ifdef HF_ITF_COLOUR_CONTROL_STOP_CMD

            /*!
             * Callback that is called when a @c ColourControl::STOP_CMD,
             * is received.
             *
             * @param [in] addr       the network address to send the message to.
             */
            Common::Result stop(const Protocol::Address &addr);

#endif

            //! @}

            protected:

            static Container transitions;


            /*!
             * Add a transition to the list.
             * @param [in] t     The transition to be added.
             */
            void add_transition(ITransition *t)
            {
               transitions.push_back(t);
               changed();
            }


            /*!
             * Remove a transition from the list.
             * @param [in] itf      The server reference to search for a transition.
             */
            void remove(IServer const &itf)
            {
               auto compare = [&itf](ITransition *entry)
                              {
                                 if (&(entry->server) == &(itf))
                                 {
                                    delete entry;
                                    return true;
                                 }

                                 return false;
                              };

               transitions.erase(std::remove_if(transitions.begin(), transitions.end(),
                                                compare), transitions.end());
            }
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
             * @param [in] addr        the network address to send the message to.
             * @param [in] direction   the direction of change.
             * @param [in] rate        the rate of change.
             */
            void move_hue(const Protocol::Address &addr, Direction direction, uint16_t rate);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_HUE_CMD,
             * to the broadcast network address.
             */
            void move_hue(Direction direction, uint16_t rate)
            {
               Protocol::Address addr;
               move_hue(addr, direction, rate);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_HUE_CMD, to the given
             * network address.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] step        the step size.
             * @param [in] direction   the direction of change.
             * @param [in] time        time to perform a single step.
             */
            void step_hue(const Protocol::Address &addr, uint8_t step,
                          Direction direction, uint8_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_HUE_CMD,
             * to the broadcast network address.
             */
            void step_hue(uint8_t step, Direction direction, uint8_t time)
            {
               Protocol::Address addr;
               step_hue(addr, step, direction, time);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] saturation  the new saturation value.
             * @param [in] direction   direction of movement.
             * @param [in] transition  transition time (in units of 100ms).
             */
            void move_to_saturation(const Protocol::Address &addr, uint8_t saturation,
                                    Direction direction, uint16_t transition);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_to_saturation(uint8_t saturation, Direction direction, uint16_t transition)
            {
               Protocol::Address addr;
               move_to_saturation(addr, saturation, direction, transition);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] direction   the direction of change.
             * @param [in] rate        the rate of change.
             */
            void move_saturation(const Protocol::Address &addr, Direction direction, uint8_t rate);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_saturation(Direction direction, uint8_t rate)
            {
               Protocol::Address addr;
               move_saturation(addr, direction, rate);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] step        the step size.
             * @param [in] direction   the direction of change.
             * @param [in] time        time to perform a single step.
             */
            void step_saturation(const Protocol::Address &addr, uint8_t step, Direction direction,
                                 uint8_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_SATURATION_CMD,
             * to the broadcast network address.
             */
            void step_saturation(uint8_t step, Direction direction, uint8_t time)
            {
               Protocol::Address addr;
               step_saturation(addr, step, direction, time);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD, to the given
             * network address.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] colour      the new colour value.
             * @param [in] direction   direction of movement.
             * @param [in] transition  transition time (in units of 100ms).
             */
            void move_to_hue_and_saturation(const Protocol::Address &addr, HS_Colour colour,
                                            Direction direction, uint16_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD,
             * to the broadcast network address.
             */
            void move_to_hue_and_saturation(HS_Colour colour, Direction direction, uint16_t time)
            {
               Protocol::Address addr;
               move_to_hue_and_saturation(addr, colour, direction, time);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] colour      the new colour value.
             * @param [in] transition  transition time (in units of 100ms).
             *
             */
            void move_to_xy(const Protocol::Address &addr, XY_Colour colour, uint16_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_XY_CMD,
             * to the broadcast network address.
             */
            void move_to_xy(XY_Colour colour, uint16_t time)
            {
               Protocol::Address addr;
               move_to_xy(addr, colour, time);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             * @param [in] X_rate     the X rate of change. (in units of sec)
             * @param [in] Y_rate     the Y rate of change. (in units of sec)
             */
            void move_xy(const Protocol::Address &addr, int16_t X_rate, int16_t Y_rate);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_XY_CMD,
             * to the broadcast network address.
             */
            void move_xy(int16_t X_rate, int16_t Y_rate)
            {
               Protocol::Address addr;
               move_xy(addr, X_rate, Y_rate);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_XY_CMD, to the given
             * network address.
             *
             * @param [in] addr        the network address to send the message to.
             * @param [in] X_step      the X rate of change. (in units of sec)
             * @param [in] Y_step      the Y rate of change. (in units of sec)
             * @param [in] time        time to perform a single step. (in units of 100msec)
             */
            void step_xy(const Protocol::Address &addr,
                         int16_t X_step, int16_t Y_step, uint8_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::STEP_XY_CMD,
             * to the broadcast network address.
             */
            void step_xy(int16_t X_step, int16_t Y_step, uint8_t time)
            {
               Protocol::Address addr;
               step_xy(addr, X_step, Y_step, time);
            }

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD, to the given
             * network address.
             *
             * @param [in] addr       the network address to send the message to.
             */
            void move_to_colour_temperature(const Protocol::Address &addr,
                                            uint16_t colour, uint16_t time);

            /*!
             * Send a HAN-FUN message containing a @c ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD,
             * to the broadcast network address.
             */
            void move_to_colour_temperature(uint16_t colour, uint16_t time)
            {
               Protocol::Address addr;
               move_to_colour_temperature(addr, colour, time);
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
