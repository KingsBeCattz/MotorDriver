#pragma once

#include <utility>
#include "Types.hpp"
#include "Feedback.hpp"

namespace MotorDriver
{
  class Motor
  {
  private:
    Pin _IN1 = PIN_UNUSED;
    Pin _IN2 = PIN_UNUSED;
    Pin _EN = PIN_UNUSED;

    PinMode _IN_mode = PinMode::Digital;
    PinMode _EN_mode = PinMode::PWM;

    bool _IN_attached = false;
    bool _EN_attached = false;

    bool _initialized = false;

    UnsignedPWM _digitalActivationThreshold = 0; // Threshold for interpreting PWM as digital HIGH
    SignedPWM _currentPWM = 0;                   // Current PWM value for the motor. Positive = IN1 active; Negative = IN2 active
    UnsignedPWM _deadzoneThreshold = 0;          // Threshold below which PWM values are treated as zero to prevent motor stalling

    SignedPWM _clampToDigital(SignedPWM pwm);
    void _writeInput(SignedPWM pwm);
    void _writeEnable(UnsignedPWM pwm);

  public:
    ~Motor() { stop(); }

    /**
     * @brief Initializes the motor by configuring the attached pins. This should be called after attaching the input and enable pins.
     */
    Feedback begin();

    // Setters

    /**
     * @brief Attaches the motor input pins (IN1, IN2) with the specified mode.
     * If the pins are already attached, this function will ignore subsequent calls.
     * @param in1 The pin number for IN1. If PIN_UNUSED, the existing
     * pin will be retained.
     * @param in2 The pin number for IN2. If PIN_UNUSED, the existing
     * pin will be retained.
     * @param mode The mode for the input pins (PinMode::Digital or PinMode::PWM).
     */
    Feedback attachInput(Pin in1, Pin in2, PinMode mode);

    /**
     * @brief Attaches the motor enable pin (EN) with the specified mode.
     * If the pin is already attached, this function will ignore subsequent calls.
     * @param en The pin number for EN. If PIN_UNUSED, the existing pin will be retained.
     * @param mode The mode for the enable pin (PinMode::Digital or PinMode::PWM).
     * Note: The enable pin is optional; the motor can operate without it if only direction control is needed.
     */
    Feedback attachEnable(Pin en, PinMode mode);

    /**
     * @brief Sets the digital activation threshold for interpreting PWM values as digital HIGH in Digital mode. Default is 0.
     */
    void setDigitalActivationThreshold(UnsignedPWM threshold) { _digitalActivationThreshold = threshold; }

    /**
     * @brief Sets the deadzone threshold for treating PWM values as zero to prevent motor stalling. Default is 0.
     */
    void setDeadzoneThreshold(UnsignedPWM threshold) { _deadzoneThreshold = threshold; }

    // Control methods

    Feedback drive(SignedPWM pwm);
    Feedback stop() { return drive(0); }
    Feedback forward(UnsignedPWM speed) { return drive(speed); }
    Feedback backward(UnsignedPWM speed) { return drive(-speed); }

    // Getters

    /**
     * @brief Checks if the motor has been initialized by calling begin(). Note that the motor cannot operate until it has been initialized, so this indicates whether the motor is ready to receive control commands.
     */
    bool isInitialized() const { return _initialized; }
    /**
     * @brief Checks if the input pins (IN1 and IN2) are attached. Note that the motor cannot operate without both input pins attached, so this indicates whether the motor is ready for control commands.
     */
    bool isInputAttached() const { return _IN_attached; }
    /**
     * @brief Checks if the enable pin is attached. Note that the motor can still operate without an enable pin, but this indicates whether one is currently configured.
     */
    bool isEnableAttached() const { return _EN_attached; }

    /**
     * @brief Retrieves the currently attached input pins (IN1, IN2) as a pair.
     */
    std::pair<Pin, Pin> getInputPins() const { return {_IN1, _IN2}; }
    /**
     * @brief Retrieves the currently attached input mode.
     */
    PinMode getInputMode() const { return _IN_mode; }
    /**
     * @brief Retrieves the currently attached enable pin (EN).
     */
    Pin getEnablePin() const { return {_EN}; }
    /**
     * @brief Retrieves the currently attached enable mode.
     */
    PinMode getEnableMode() const { return _EN_mode; }

    /**
     * @brief Retrieves the current digital activation threshold for interpreting PWM values as digital HIGH in Digital mode. Default is 0.
     */
    UnsignedPWM getDigitalActivationThreshold() const { return _digitalActivationThreshold; }

    /**
     * @brief Retrieves the current deadzone threshold for treating PWM values as zero to prevent motor stalling. Default is 0.
     */
    UnsignedPWM getDeadzoneThreshold() const { return _deadzoneThreshold; }

    /**
     * @brief Retrieves the current PWM value being applied to the motor. Positive values indicate forward direction (IN1 active), while negative values indicate reverse direction (IN2 active). In Digital mode, this will reflect the last set speed clamped to either 0 or the digital activation threshold.
     */
    SignedPWM getCurrentPWM() const { return _currentPWM; }
  };
}