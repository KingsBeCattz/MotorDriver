#pragma once

namespace MotorDriver
{
  enum class Feedback : unsigned char
  {
    OK = 0,
    INPUT_NOT_ATTACHED = 1,
    ENABLE_NOT_ATTACHED = 2,
    NOT_INITIALIZED = 3,
    INPUT_ALREADY_ATTACHED = 4,
    ENABLE_ALREADY_ATTACHED = 5,
    INVALID_PIN_ASSIGNMENT = 6,
    MOTOR_INITIALIZATION_FAILED = 7,
    INVALID_FUNCTION = 8,
  };

  /**
   * @brief Converts a Feedback code to a human-readable string.
   * @param code The feedback code to convert.
   * @return A pointer to the human-readable string.
   */
  inline const char *feedbackToString(Feedback code)
  {
    switch (code)
    {
    case Feedback::OK:
      return "Operation completed successfully";
    case Feedback::INPUT_NOT_ATTACHED:
      return "No input pin has been attached";
    case Feedback::ENABLE_NOT_ATTACHED:
      return "No enable pin has been attached";
    case Feedback::NOT_INITIALIZED:
      return "Motor not initialized, or required callbacks not attached (power function and steering function)";
    case Feedback::INPUT_ALREADY_ATTACHED:
      return "Input pin already attached, cannot reassign";
    case Feedback::ENABLE_ALREADY_ATTACHED:
      return "Enable pin already attached, cannot reassign";
    case Feedback::INVALID_PIN_ASSIGNMENT:
      return "Specified pin number is not valid for this operation (used MotorDriver::PIN_UNUSED)";
    case Feedback::MOTOR_INITIALIZATION_FAILED:
      return "Motor failed to initialize correctly on DiffDrive.begin()";
    case Feedback::INVALID_FUNCTION:
      return "Null pointer provided when attaching a power or steering callback";
    default:
      return "Unknown feedback code";
    }
  }
}