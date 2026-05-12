#include <Arduino.h>
#include "DiffDrive.hpp"

namespace MotorDriver
{
  void DiffDrive::begin()
  {
    if (!_leftMotor.isInputAttached() || !_rightMotor.isInputAttached())
      return; // Cannot initialize without both motors having their input pins attached

    _leftMotor.begin();
    _rightMotor.begin();

    if (!_leftMotor.isInitialized() || !_rightMotor.isInitialized())
      return; // Failed to initialize one or both motors

    if (isEnablePinAttached())
      pinMode(_EN, OUTPUT);

    _initialized = true;
  }

  void DiffDrive::attachEnablePin(Pin en_pin)
  {
    if (_EN != PIN_UNUSED)
      return; // Already attached, ignore subsequent calls
    if (en_pin == PIN_UNUSED)
      return; // Invalid pin assignment

    _EN = en_pin;
  }

  void DiffDrive::_centerPivotDrive(SignedPWM power, float steering)
  {
    if (steering == 0.0f)
    {
      _leftMotor.drive(power);
      _rightMotor.drive(power);
      return;
    }

    float steerFactor = abs(steering);
    _leftMotor.drive((SignedPWM)((power * steerFactor) * (steering < 0.0f ? -1 : 1)));
    _rightMotor.drive((SignedPWM)((power * steerFactor) * (steering > 0.0f ? -1 : 1)));
  }

  void DiffDrive::_wheelPivotDrive(SignedPWM power, float steering)
  {
    if (steering == 0.0f)
    {
      _leftMotor.drive(power);
      _rightMotor.drive(power);
      return;
    }

    float steerFactor = 1.0f - abs(steering);                                      // Factor to reduce power on the pivoting wheel
    _leftMotor.drive(steering < 0.0f ? power : (SignedPWM)(power * steerFactor));  // Reduce power on left motor for right turns
    _rightMotor.drive(steering > 0.0f ? power : (SignedPWM)(power * steerFactor)); // Reduce power on right motor for left turns
  }

  void DiffDrive::update(bool quick_spin)
  {
    if (!_initialized || _get_power == nullptr || _get_steering == nullptr)
      return; // Cannot update if not initialized or if power/steering functions are not attached

    SignedPWM power = _get_power();
    if (power == 0)
    {
      stop();
      return;
    }

    if (isEnablePinAttached())
      digitalWrite(_EN, HIGH); // Ensure motors are enabled if an enable pin is attached

    _enabled = true;

    if (quick_spin)
    {
      // In quick spin mode, we ignore the steering input and drive the motors in opposite directions for maximum turning speed
      _leftMotor.drive(power);
      _rightMotor.drive(-power);
      return;
    }

    float steering = _get_steering();

    switch (_pivot_mode)
    {
    case PivotMode::Center:
      _centerPivotDrive(power, steering);
      break;
    case PivotMode::Wheel:
      _wheelPivotDrive(power, steering);
      break;
    }
  }
}