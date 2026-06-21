# Changelog

All notable changes to this project will be documented in this file.

## [0.3.2] - 2026-06-20
### Changed
- `Motor::getInputPins()` now returns a named `PinPair` struct (`in1`, `in2`) instead of `std::pair<Pin, Pin>` for compatibility with Arduino toolchains that may not provide `<utility>`.

---

## [0.3.1] - 2026-05-15
### Fixed
- `Motor::setDigitalActivationThreshold` and `Motor::setDeadzoneThreshold` now correctly return `void` instead of `Feedback` (previously caused undefined behavior due to missing return statement).

---

## [0.3.0] - 2026-05-15
### Added
- `MotorDriver::feedbackToString(MotorDriver::Feedback)` — returns a human-readable description string for any `MotorDriver::Feedback` code.

---

## [0.2.0] - 2026-05-14
### Added
- `MotorDriver::Feedback` enum — explicit typed feedback codes returned by driver operations:
  - `OK` — operation completed successfully.
  - `INPUT_NOT_ATTACHED` — no input pin has been attached.
  - `ENABLE_NOT_ATTACHED` — no enable pin has been attached.
  - `NOT_INITIALIZED` — the motor or driver has not been initialized, or required callbacks are not attached.
  - `INPUT_ALREADY_ATTACHED` — an input pin is already attached and cannot be reassigned.
  - `ENABLE_ALREADY_ATTACHED` — an enable pin is already attached and cannot be reassigned.
  - `INVALID_PIN_ASSIGNMENT` — the specified pin number is not valid for this operation.
  - `MOTOR_INITIALIZATION_FAILED` — the motor failed to initialize correctly.
  - `INVALID_FUNCTION` — a null pointer was provided when attaching a power or steering callback.

---

## [0.1.2] - 2026-05-11
### Fixed
- Fix drive method to correctly apply steerFactor for motor power adjustments

---

## [0.1.1] - 2026-05-11
### Fixed
- Fix deadzone threshold check in drive method to use correct variable name

---

## [0.1.0] - 2026-05-11
### Added
- `Motor::setDeadzone(uint8_t)` — sets the minimum PWM threshold below which the motor is not driven.
- `Motor::getDeadzone() const` — retrieves the current deadzone value.
### Changed
- Clarified that `_digitalActivationThreshold` defaults to 0.

---

## [0.0.4] - 2026-05-11
### Fixed
- Added missing `#include <Arduino.h>` include in `DiffDrive.hpp`.

---

## [0.0.3] - 2026-05-11
### Fixed
- Removed default argument from `MotorDriver::DiffDrive::update(bool)` definition in `.cpp` to comply with C++ standard (default arguments only allowed in declarations)

---

## [0.0.2] - 2026-05-11

### Added
- `MotorDriver::DiffDrive::getEnablePin() const` — retrieves the currently attached enable pin for the differential drive.

---

## [0.0.1] - Initial release