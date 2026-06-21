#pragma once

namespace MotorDriver
{
  /**
   * @brief Type for representing a pin number.
   */
  typedef unsigned char Pin;

  /**
   * @brief Signed PWM value in the range [-255, 255].
   *
   * Represents a motor power level where the sign encodes direction
   * (negative = reverse, positive = forward) and the magnitude encodes
   * the duty cycle intensity.
   */
  typedef short SignedPWM;

  /**
   * @brief Unsigned PWM value in the range [0, 255].
   *
   * Represents a non-directional power level, typically used for
   * enable pins or digital logic where only magnitude matters.
   */
  typedef unsigned char UnsignedPWM;

  /**
   * @brief Function pointer type for providing power levels to the motors.
   */
  typedef SignedPWM (*PowerFn)();
  /**
   * @brief Function pointer type for providing steering values for differential drive.
   *
   * The steering value is typically in the range [-1.0, 1.0], where -1.0 represents full left turn,
   * 0.0 represents straight ahead, and 1.0 represents full right turn.
   */
  typedef float (*SteerFn)();

  /** Special marker indicating that a pin is not assigned. */
  static constexpr Pin PIN_UNUSED = 255;

  /**
   * @brief Describes the electrical capability of a pin.
   */
  enum class PinMode : bool
  {
    Digital = false, ///< GPIO digital output
    PWM = true,      ///< PWM-capable output
  };

  enum class PivotMode : bool
  {
    Center = false, ///< Pivot around the center point between the two wheels
    Wheel = true,   ///< Pivot around the wheel on the side of the turn (left wheel for left turns, right wheel for right turns)
  };

  struct PinPair
  {
    Pin in1;
    Pin in2;
  };
}