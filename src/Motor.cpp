#include <Arduino.h>
#include "Motor.hpp"

namespace MotorDriver
{
  SignedPWM Motor::_clampToDigital(SignedPWM pwm)
  {
    if (pwm > _digitalActivationThreshold)
      return 255;
    if (pwm < -_digitalActivationThreshold)
      return -255;
    return 0;
  }

  void Motor::_writeInput(SignedPWM pwm)
  {
    if (!_initialized)
      return; // Cannot write input if not initialized

    switch (_IN_mode)
    {
    case PinMode::Digital:
    {
      SignedPWM clampedPWM = _clampToDigital(pwm);

      if (clampedPWM == _currentPWM)
        return; // No change, skip redundant writes

      digitalWrite(_IN1, clampedPWM > 0 ? HIGH : LOW);
      digitalWrite(_IN2, clampedPWM < 0 ? HIGH : LOW);
      _currentPWM = clampedPWM;
      break;
    }
    case PinMode::PWM:
    {
      if (pwm == _currentPWM)
        return; // No change, skip redundant writes

      UnsignedPWM absPWM = abs(pwm);
      analogWrite(_IN1, pwm > 0 ? absPWM : 0);
      analogWrite(_IN2, pwm < 0 ? absPWM : 0);
      _currentPWM = pwm;
      break;
    }
    }
  }

  void Motor::_writeEnable(UnsignedPWM pwm)
  {
    if (!_initialized || !_EN_attached)
      return; // Cannot write enable if not initialized or enable pin not attached

    switch (_EN_mode)
    {
    case PinMode::Digital:
      digitalWrite(_EN, pwm > _digitalActivationThreshold ? HIGH : LOW);
      break;
    case PinMode::PWM:
      analogWrite(_EN, pwm);
      break;
    }
  }

  void Motor::drive(SignedPWM pwm)
  {
    if (!_initialized)
      return;

    if (abs(pwm) < _deadzoneThreshold)
      pwm = 0;

    _writeEnable(abs(pwm));
    _writeInput(pwm);
  }

  void Motor::attachInput(Pin in1, Pin in2, PinMode mode)
  {
    if (Motor::_IN_attached)
      return; // Already attached, ignore subsequent calls
    if (in1 == PIN_UNUSED || in2 == PIN_UNUSED)
      return; // Invalid pin assignments

    Motor::_IN1 = in1;
    Motor::_IN2 = in2;
    Motor::_IN_mode = mode;
    Motor::_IN_attached = true;
  }

  void Motor::attachEnable(Pin en, PinMode mode)
  {
    if (Motor::_EN_attached)
      return; // Already attached, ignore subsequent calls
    if (en == PIN_UNUSED)
      return; // Invalid pin assignment
    Motor::_EN = en;

    Motor::_EN_mode = mode;
    Motor::_EN_attached = true;
  }

  void Motor::begin()
  {
    if (!Motor::_IN_attached)
      return; // Cannot initialize without both input pins attached

    pinMode(Motor::_IN1, OUTPUT);
    pinMode(Motor::_IN2, OUTPUT);
    if (Motor::_EN_attached)
      pinMode(Motor::_EN, OUTPUT);

    Motor::_initialized = true;
  }
}