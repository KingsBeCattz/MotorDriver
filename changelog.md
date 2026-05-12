# Changelog

All notable changes to this project will be documented in this file.

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