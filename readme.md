# MotorDriver

An Arduino C++ library for controlling DC motors and differential drive robots. Provides a clean, layered API for single motor control (`Motor`) and two-motor differential drive (`DiffDrive`), with support for both digital and PWM pin modes, optional enable pins, and callback-based power/steering inputs.

---

## Features

- **Single motor control** via the `Motor` class (forward, backward, stop, PWM drive)
- **Differential drive control** via the `DiffDrive` class
- **Two steering/pivot modes**: Center pivot and Wheel pivot
- **Digital and PWM pin modes** for both input and enable pins
- **Configurable digital activation threshold** for PWM→digital clamping
- **Callback-based architecture**: attach functions that return power and steering values
- **Optional enable pin** support at both the motor and drive level
- **Safe destruction**: motors stop and disable automatically on object destruction

---

## Types (`Types.hpp`)

| Type | Description |
|------|-------------|
| `Pin` | `unsigned char` — pin number |
| `SignedPWM` | `short` — motor power in `[-255, 255]`; sign encodes direction |
| `UnsignedPWM` | `unsigned char` — non-directional power in `[0, 255]` |
| `PowerFn` | `SignedPWM (*)()` — callback returning current power level |
| `SteerFn` | `float (*)()` — callback returning steering value in `[-1.0, 1.0]` |
| `PIN_UNUSED` | `255` — sentinel value indicating an unassigned pin |

### Enumerations

```cpp
enum class PinMode : bool {
    Digital, // GPIO digital output
    PWM,     // PWM-capable output
};

enum class PivotMode : bool {
    Center, // Pivot around midpoint between wheels (one wheel reverses)
    Wheel,  // Pivot around the inner wheel (inner wheel stops)
};
```

---

## Motor Class

Controls a single DC motor via two input pins (IN1, IN2) and an optional enable pin (EN).

### Setup

```cpp
Motor motor;

// Attach input pins (required)
motor.attachInput(IN1_PIN, IN2_PIN, PinMode::PWM);

// Attach enable pin (optional)
motor.attachEnable(EN_PIN, PinMode::PWM);

// Initialize (call after attaching pins, before driving)
motor.begin();
```

### Control

```cpp
motor.drive(200);       // Forward at PWM 200
motor.drive(-150);      // Reverse at PWM 150
motor.forward(255);     // Full speed forward
motor.backward(100);    // Reverse at PWM 100
motor.stop();           // Stop the motor
```

### Digital Mode & Threshold

When using `PinMode::Digital`, PWM values are clamped to `HIGH`/`LOW` based on a configurable threshold:

```cpp
motor.attachInput(IN1_PIN, IN2_PIN, PinMode::Digital);
motor.setDigitalActivationThreshold(50); // Values > 50 → HIGH; < -50 → LOW
```

### Getters

```cpp
motor.isInitialized();             // bool
motor.isInputAttached();           // bool
motor.isEnableAttached();          // bool
motor.getInputPins();              // std::pair<Pin, Pin>
motor.getInputMode();              // PinMode
motor.getEnablePin();              // Pin
motor.getEnableMode();             // PinMode
motor.getCurrentPWM();             // SignedPWM
motor.getDigitalActivationThreshold(); // UnsignedPWM
```

---

## DiffDrive Class

Controls a two-motor differential drive robot. Internally manages two `Motor` instances (`_leftMotor` and `_rightMotor`) and maps power/steering callbacks to motor outputs.

### Setup

```cpp
DiffDrive drive;

// Configure motors
drive.getLeftMotor().attachInput(L_IN1, L_IN2, PinMode::PWM);
drive.getRightMotor().attachInput(R_IN1, R_IN2, PinMode::PWM);

// Attach callbacks
drive.attachPowerFunction([]() -> SignedPWM { return readJoystickY(); });
drive.attachSteeringFunction([]() -> float { return readJoystickX(); });

// Optional: shared enable pin
drive.attachEnablePin(EN_PIN);

// Optional: pivot mode (default is Wheel)
drive.setPivotMode(PivotMode::Center);

// Initialize
drive.begin();
```

### Main Loop

Call `update()` repeatedly to drive the motors according to the attached callbacks:

```cpp
void loop() {
    drive.update();        // Normal update
    drive.update(true);    // Quick-spin: ignores steering, spins motors in opposite directions
}
```

### Stopping

```cpp
drive.stop(); // Stops both motors; pulls EN LOW if enable pin is attached
```

### Pivot Modes

| Mode | Behavior |
|------|----------|
| `PivotMode::Wheel` *(default)* | Inner wheel slows to stop; outer wheel maintains power. Gradual, natural turns. |
| `PivotMode::Center` | One wheel reverses while the other drives forward. Sharp, in-place turns. |

### Quick Spin

When `update(true)` is called, steering is ignored and both motors are driven in opposite directions at the full power returned by `PowerFn`. This produces the fastest possible rotation.

### Getters

```cpp
drive.isInitialized();              // bool
drive.isEnabled();                  // bool
drive.isEnablePinAttached();        // bool
drive.getPivotMode();               // PivotMode
drive.getAttachedPowerFunction();   // PowerFn
drive.getAttachedSteeringFunction(); // SteerFn
drive.getLeftMotor();               // Motor&
drive.getRightMotor();              // Motor&
```

---

## Notes

- **Pin attachment is one-time**: calling `attachInput`, `attachEnable`, or `attachEnablePin` more than once is silently ignored. Assign pins before calling `begin()`.
- **Initialization guard**: `begin()` will silently return if the required pins are not attached. Always verify with `isInitialized()` before driving.
- **Redundant write suppression**: `Motor` tracks `_currentPWM` and skips `digitalWrite`/`analogWrite` calls when the value hasn't changed, reducing bus traffic on fast update loops.
- **Safe destruction**: both `Motor` and `DiffDrive` call `stop()` (and pull EN LOW if applicable) in their destructors.

---

## File Structure

```
MotorDriver/
├── Types.hpp        # Typedefs and enumerations
├── Motor.hpp        # Motor class declaration
├── Motor.cpp        # Motor class implementation
├── DiffDrive.hpp    # DiffDrive class declaration
├── DiffDrive.cpp    # DiffDrive class implementation
└── MotorDriver.hpp  # Convenience header (includes all of the above)
```

Include `MotorDriver.hpp` in your sketch to access the full library.

---

# 📄 License

MIT License — free for personal and commercial use.