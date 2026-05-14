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
    INVALID_FUNCTION = 9,
  };
}