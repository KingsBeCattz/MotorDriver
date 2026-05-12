#pragma once

#include "Motor.hpp"

namespace MotorDriver
{
  class DiffDrive
  {
  private:
    Motor _leftMotor;
    Motor _rightMotor;

    Pin _EN = PIN_UNUSED;

    bool _enabled = false;

    bool _initialized = false;

    PivotMode _pivot_mode = PivotMode::Wheel;

    /**
     * @brief Function pointer providing current power input as a SignedPWM [-255..255].
     */
    PowerFn _get_power = nullptr;

    /**
     * @brief Function pointer providing steering input as a float [-1.0..1.0].
     */
    SteerFn _get_steering = nullptr;

    void _centerPivotDrive(SignedPWM power, float steering);
    void _wheelPivotDrive(SignedPWM power, float steering);

  public:
    ~DiffDrive()
    {
      if (isEnablePinAttached())
        digitalWrite(_EN, LOW); // Ensure motors are disabled on destruction if enable pin is attached
    }
    /**
     * @brief Initializes the differential drive by configuring the attached motors and standby pin. This should be called after attaching the motors, the power function, steering function, and optionally the standby pin.
     */
    void begin();

    // Setters
    /**
     * @brief Attaches the power function that provides the current power level for the motors. The function should return a SignedPWM value in the range [-255, 255], where negative values indicate reverse direction, positive values indicate forward direction, and the magnitude indicates the intensity of the power.
     */
    void attachPowerFunction(PowerFn power_fn) { _get_power = power_fn; }
    /**
     * @brief Attaches the steering function that provides the current steering input for the differential drive. The function should return a float value in the range [-1.0, 1.0], where -1.0 represents full left turn, 0.0 represents straight ahead, and 1.0 represents full right turn.
     */
    void attachSteeringFunction(SteerFn steer_fn) { _get_steering = steer_fn; }
    /**
     * @brief Attaches the enable pin for the motors. This pin will be set HIGH when the motors are enabled and LOW when they are disabled. If no enable pin is attached, the motors will always be enabled as long as power is applied to them, and the stop() function will only stop the motors by setting their inputs to LOW without cutting power.
     */
    void attachEnablePin(Pin en_pin);

    /**
     * @brief Sets the digital activation threshold for interpreting PWM values as digital HIGH in Digital mode.
     */
    void setDigitalActivationThreshold(UnsignedPWM threshold)
    {
      _leftMotor.setDigitalActivationThreshold(threshold);
      _rightMotor.setDigitalActivationThreshold(threshold);
    }

    /**
     * @brief Sets the pivot mode for the differential drive. In Center pivot mode, the robot will pivot around the center point between the two wheels, meaning that one wheel will reverse while the other moves forward for turns. In Wheel pivot mode, the robot will pivot around the wheel on the side of the turn (left wheel for left turns, right wheel for right turns), meaning that one wheel will stop while the other moves forward for turns. The default mode is Wheel pivot.
     */
    void setPivotMode(PivotMode mode) { _pivot_mode = mode; }

    // Getters
    /**
     * @brief Provides access to the left Motor instance for direct control or configuration. Note that the motors must be initialized by calling begin() before they can be controlled, so it's important to check isInitialized() before attempting to use the motor instances.
     */
    Motor &getLeftMotor() { return _leftMotor; }
    /**
     * @brief Provides access to the right Motor instance for direct control or configuration. Note that the motors must be initialized by calling begin() before they can be controlled, so it's important to check isInitialized() before attempting to use the motor instances.
     */
    Motor &getRightMotor() { return _rightMotor; }
    /**
     * @brief Provides const access to the left Motor instance. This can be used to read the motor's configuration or state without allowing modification. Note that the motors must be initialized by calling begin() before they can be accessed, so it's important to check isInitialized() before attempting to use the motor instances.
     */
    Motor const &getLeftMotor() const { return _leftMotor; }
    /**
     * @brief Provides const access to the right Motor instance. This can be used to read the motor's configuration or state without allowing modification. Note that the motors must be initialized by calling begin() before they can be accessed, so it's important to check isInitialized() before attempting to use the motor instances.
     */
    Motor const &getRightMotor() const { return _rightMotor; }

    /**
     * @brief Checks if the differential drive has been initialized by calling begin(). Note that the drive cannot operate until it has been initialized, so this indicates whether the drive is ready to receive control commands.
     */
    bool isInitialized() const { return _initialized; }
    /**
     * @brief Checks if the enable pin is attached. Note that the drive can still operate without an enable pin, but this indicates whether one is currently configured.
     */
    bool isEnabled() const { return _enabled; }
    /**
     * @brief Checks if the enable pin is attached. Note that the drive can still operate without an enable pin, but this indicates whether one is currently configured.
     */
    bool isEnablePinAttached() const { return _EN != PIN_UNUSED; }
    /**
     * @brief Retrieves the currently attached enable pin.
     */
    PowerFn getAttachedPowerFunction() const { return _get_power; }
    /**
     * @brief Retrieves the currently attached steering function.
     */
    SteerFn getAttachedSteeringFunction() const { return _get_steering; }
    /**
     * @brief Retrieves the current pivot mode for the differential drive.
     */
    PivotMode getPivotMode() const { return _pivot_mode; }

    // Control method
    /**
     * @brief Stops the motors and disables them if an enable pin is attached. This is a safe way to ensure the robot halts immediately, as it will cut power to the motors if possible. If no enable pin is attached, it will simply stop the motors by setting their inputs to LOW, which may not cut power but will at least stop them from driving forward or backward.
     */
    void stop()
    {
      _leftMotor.stop();
      _rightMotor.stop();
      if (isEnablePinAttached())
        digitalWrite(_EN, LOW); // Disable motors if enable pin is attached
      _enabled = false;
    }
    /**
     * @brief Updates the motor outputs based on the current power and steering inputs. This should be called repeatedly in the main loop to continuously control the motors according to the attached power and steering functions. If quick_spin is true, the drive will ignore the steering input and drive the motors in opposite directions for maximum turning speed.
     */
    void update(bool quick_spin = false);
  };
}