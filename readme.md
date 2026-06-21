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
- **Explicit typed feedback**: all operations return a `Feedback` code for error handling
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

enum class Feedback : unsigned char {
    OK = 0,                    // Operation completed successfully
    INPUT_NOT_ATTACHED = 1,    // No input pin has been attached
    ENABLE_NOT_ATTACHED = 2,   // No enable pin has been attached
    NOT_INITIALIZED = 3,       // Motor/driver not initialized, or required callbacks not attached
    INPUT_ALREADY_ATTACHED = 4,  // Input pin already attached, cannot reassign
    ENABLE_ALREADY_ATTACHED = 5, // Enable pin already attached, cannot reassign
    INVALID_PIN_ASSIGNMENT = 6,  // Specified pin number is not valid for this operation
    MOTOR_INITIALIZATION_FAILED = 7, // Motor failed to initialize correctly
    INVALID_FUNCTION = 9,      // Null pointer provided when attaching a power or steering callback
};

// Convert a Feedback code to a human-readable string
const char* feedbackToString(Feedback);
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

### Deadzone

The motor ignores PWM values below the deadzone threshold, preventing audible whining at low power levels:

```cpp
motor.setDeadzone(20); // Values in [-20, 20] → motor does not drive (default: 20)
```

### Getters

```cpp
motor.isInitialized();             // bool
motor.isInputAttached();           // bool
motor.isEnableAttached();          // bool
motor.getInputPins();              // PinPair
motor.getInputMode();              // PinMode
motor.getEnablePin();              // Pin
motor.getEnableMode();             // PinMode
motor.getCurrentPWM();             // SignedPWM
motor.getDigitalActivationThreshold(); // UnsignedPWM
motor.getDeadzone();               // UnsignedPWM
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

// Create callbacks
SignedPWM powerFunction() {
    return readJoystickY();
}

float steeringFunction() {
    return readJoystickX();
}

// Attach callbacks
drive.attachPowerFunction(powerFunction);
drive.attachSteeringFunction(steeringFunction);

// Optional: shared enable pin
drive.attachEnablePin(EN_PIN);

// Optional: pivot mode (default is Wheel)
drive.setPivotMode(PivotMode::Center);

// Initialize
drive.begin();
```

### Error Handling

All major operations return a `Feedback` code. It is recommended to check the result of `begin()` and `update()`:

```cpp
if (drive.begin() != MotorDriver::Feedback::OK) {
    // Handle initialization error
}

void loop() {
    Feedback result = drive.update();
    if (result != MotorDriver::Feedback::OK) {
        // Handle runtime error
    }
}
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
drive.isInitialized();               // bool
drive.isEnabled();                   // bool
drive.isEnablePinAttached();         // bool
drive.getEnablePin();                // Pin
drive.getPivotMode();                // PivotMode
drive.getAttachedPowerFunction();    // PowerFn
drive.getAttachedSteeringFunction(); // SteerFn
drive.getLeftMotor();                // Motor&
drive.getRightMotor();               // Motor&
```

---

## Notes

- **Pin attachment is one-time**: calling `attachInput`, `attachEnable`, or `attachEnablePin` more than once returns `Feedback::INPUT_ALREADY_ATTACHED` or `Feedback::ENABLE_ALREADY_ATTACHED` respectively.
- **Initialization guard**: `begin()` returns `Feedback::INPUT_NOT_ATTACHED` if required pins are not attached, and `Feedback::MOTOR_INITIALIZATION_FAILED` if initialization fails. Always verify with `isInitialized()` before driving.
- **Callback validation**: passing `nullptr` to `attachPowerFunction()` or `attachSteeringFunction()` returns `Feedback::INVALID_FUNCTION`.
- **Redundant write suppression**: `Motor` tracks `_currentPWM` and skips `digitalWrite`/`analogWrite` calls when the value hasn't changed, reducing bus traffic on fast update loops.
- **Safe destruction**: both `Motor` and `DiffDrive` call `stop()` (and pull EN LOW if applicable) in their destructors.
- **Digital activation threshold**: defaults to `0`, meaning any non-zero PWM value triggers `HIGH`. Adjust with `setDigitalActivationThreshold()` if finer control is needed.
- **Feedback messages**: use `MotorDriver::feedbackToString(code)` to get a human-readable description of any `Feedback` value. Useful for serial debugging: `Serial.println(MotorDriver::feedbackToString(result))`.

---

## File Structure

```
MotorDriver/
├── Types.hpp        # Typedefs and enumerations (including Feedback)
├── Motor.hpp        # Motor class declaration
├── Motor.cpp        # Motor class implementation
├── DiffDrive.hpp    # DiffDrive class declaration
├── DiffDrive.cpp    # DiffDrive class implementation
└── MotorDriver.hpp  # Convenience header (includes all of the above)
```

Include `MotorDriver.hpp` in your sketch to access the full library.