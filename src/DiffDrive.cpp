#include <Arduino.h>
#include "DiffDrive.hpp"

namespace MotorDriver
{
  Feedback DiffDrive::begin()
  {
    if (!_leftMotor.isInputAttached() || !_rightMotor.isInputAttached())
      return Feedback::INPUT_NOT_ATTACHED; // Cannot initialize without both motors having their input pins attached

    _leftMotor.begin();
    _rightMotor.begin();

    if (!_leftMotor.isInitialized() || !_rightMotor.isInitialized())
      return Feedback::MOTOR_INITIALIZATION_FAILED; // Failed to initialize one or both motors

    if (isEnablePinAttached())
      pinMode(_EN, OUTPUT);

    _initialized = true;
    return Feedback::OK;
  }

  Feedback DiffDrive::attachEnablePin(Pin en_pin)
  {
    if (_EN != PIN_UNUSED)
      return Feedback::ENABLE_ALREADY_ATTACHED; // Already attached, ignore subsequent calls
    if (en_pin == PIN_UNUSED)
      return Feedback::INVALID_PIN_ASSIGNMENT; // Invalid pin assignment

    _EN = en_pin;
    return Feedback::OK;
  }

  Feedback DiffDrive::_centerPivotDrive(SignedPWM power, float steering)
  {
    if (!_initialized)
      return Feedback::NOT_INITIALIZED;

    if (steering == 0.0f)
    {
      Feedback left_motor_feedback = _leftMotor.drive(power);
      Feedback right_motor_feedback = _rightMotor.drive(power);
      if (left_motor_feedback != Feedback::OK)
        return left_motor_feedback;
      if (right_motor_feedback != Feedback::OK)
        return right_motor_feedback;

      return Feedback::OK;
    }

    float steerFactor = abs(steering);
    Feedback left_motor_feedback = _leftMotor.drive((SignedPWM)((power * steerFactor) * (steering < 0.0f ? -1 : 1)));
    Feedback right_motor_feedback = _rightMotor.drive((SignedPWM)((power * steerFactor) * (steering > 0.0f ? -1 : 1)));
    if (left_motor_feedback != Feedback::OK)
      return left_motor_feedback;
    if (right_motor_feedback != Feedback::OK)
      return right_motor_feedback;

    return Feedback::OK;
  }

  Feedback DiffDrive::_wheelPivotDrive(SignedPWM power, float steering)
  {
    if (steering == 0.0f)
    {
      Feedback left_motor_feedback = _leftMotor.drive(power);
      Feedback right_motor_feedback = _rightMotor.drive(power);
      if (left_motor_feedback != Feedback::OK)
        return left_motor_feedback;
      if (right_motor_feedback != Feedback::OK)
        return right_motor_feedback;
      return Feedback::OK;
    }

    float steerFactor = 1.0f - abs(steering);                                                                      // Factor to reduce power on the pivoting wheel
    Feedback left_motor_feedback = _leftMotor.drive(steering < 0.0f ? (SignedPWM)(power * steerFactor) : power);   // Reduce power on left motor for right turns
    Feedback right_motor_feedback = _rightMotor.drive(steering > 0.0f ? (SignedPWM)(power * steerFactor) : power); // Reduce power on right motor for left turns
    if (left_motor_feedback != Feedback::OK)
      return left_motor_feedback;
    if (right_motor_feedback != Feedback::OK)
      return right_motor_feedback;
    return Feedback::OK;
  }

  Feedback DiffDrive::update(bool quick_spin)
  {
    if (!_initialized || _get_power == nullptr || _get_steering == nullptr)
      return Feedback::NOT_INITIALIZED; // Cannot update if not initialized or if power/steering functions are not attached

    SignedPWM power = _get_power();
    if (power == 0)
    {
      stop();
      return Feedback::OK; // No power, so just stop the motors
    }

    if (isEnablePinAttached())
      digitalWrite(_EN, HIGH); // Ensure motors are enabled if an enable pin is attached

    _enabled = true;

    if (quick_spin)
    {
      // In quick spin mode, we ignore the steering input and drive the motors in opposite directions for maximum turning speed
      Feedback left_motor_feedback = _leftMotor.drive(power);
      Feedback right_motor_feedback = _rightMotor.drive(-power);
      if (left_motor_feedback != Feedback::OK)
        return left_motor_feedback;
      if (right_motor_feedback != Feedback::OK)
        return right_motor_feedback;
      return Feedback::OK;
    }

    float steering = _get_steering();

    switch (_pivot_mode)
    {
    case PivotMode::Center:
      return _centerPivotDrive(power, steering);
      break;
    case PivotMode::Wheel:
      return _wheelPivotDrive(power, steering);
      break;
    }
    return Feedback::OK;
  }
}