# Progress: RoboTank-ESP32

## What Works
Based on existing codebase analysis, the following subsystems appear implemented:
- RC Controller PWM signal decoding (FS-i6 protocol) for channel reading and command mapping
- Motor control via L298N driver interface with directional/speed commands from RC input or AI task logic
- Servo-based robotic arm position management through ArmController
- RMT-driven RGB LED strip effects managed by LEDsMT controller
- Sensor data aggregation pipeline in SensorsController for real-time state monitoring and feedback to tasks
- Audio playback system integration via SoundModuleController (likely WAV/MP3 module communication)
- OTA firmware update capability with OTAManager handling WiFi-based upload workflow without serial reconnection
- Pin mapping validation against physical build wiring (`include/pin_mapping.h`) verified with actual hardware connections shown in schematics
- RC controller signal processing accuracy for FS-i6 receiver (PPM protocol) tested on real hardware successfully
- Sensor calibration values and threshold configurations tuned per installed sensors, all thresholds working correctly

## What's Left to Build / Improve
### General
- Power management integration between LightsController sleep/wake cycle, OTAManager power considerations during OTA window, and overall current draw optimization
- Web interface content in `src/webcontent/` directory - endpoint handlers for WiFi AP/client mode operation not yet visible from file structure alone

### AI Mode Implementation (TODO)
- `AITask::processTickAI()` contains only a `ArmController::parkArm()` call — the method body is marked as TODO
  - **Option A**: Implement basic autonomous mode (obstacle avoidance + patrolling behavior)
  - **Option B**: Rename to `manualMode()` or `rcControlTick()` if AI autonomy is out of scope
- Current state at `src/AITask.cpp`: empty stub with single park call as placeholder

### Hard-coded Timeouts and Magic Numbers
- `OTAManager.cpp` line ~290: index calculation `(body_start_p - ota_buff)` without explicit intermediate variables — hard to read and debug
- `SensorsController.cpp` line ~145: `rmt_receive()` + `xQueueReceive()` in `while(1)` loop with 1000ms timeout
  - **Risk**: If US sensor is not connected or unstable, this blocks the task for 1+ second
  - **Recommendation**: Use non-blocking RMT callbacks or shorter timeouts with periodic polling

### Insufficient Error Handling on init() Calls
- Multiple modules call `init()` without checking ESP-IDF return codes (`esp_err_t`):
  - `MotorL298NDriver::init()` — what if I2C bus is unavailable?
  - `ArmController::init()` — was PCA9685 initialized successfully?
  - **Recommendation**: Add error checks on all ESP-IDF driver initialization calls; log failures and return status codes

### Pin Configuration Refactoring
- `include/pin_mapping.h` contains all hardware connections as hard-coded macros
- **Recommendation**: Move configurable pins to:
  - `sdkconfig` or NVS for pins that may vary between project builds
  - A `pin_config_t` struct with runtime validation

### OTA Safety Improvements
- Current code lacks firmware integrity verification:
  - No CRC/hash check of uploaded firmware — corrupted file can brick the device
  - No `content_length` validation before writing
- **Recommendation**: Add HMAC or XOR checksum verification before `esp_ota_end()`

### Persistent Storage for Configuration Data (NVS)
- WiFi credentials are stored in ESP-IDF WIFI_STORAGE (Flash), but the following are NOT persisted:
  - Servo calibration data (`SERVO_CALIBRATION_MODE`)
  - Last device state / events for debugging purposes
- **Recommendation**: Use NVS to store calibration tables, last known state, and error logs

## Current Status
**Version**: Early-stage firmware (commit `cdf3718`)
**State**: Core modules implemented, integration testing needed per subsystem combination
**Branch**: main (default)

## Known Issues
Not documented in codebase history yet. To be updated as bugs are discovered during hardware testing and field trials.

## Evolution of Project Decisions
Initial memory bank created at commit `cdf3718`. Key architectural decisions to revisit as hardware iterations occur:
- L298N vs MOSFET motor drivers (power efficiency tradeoffs)
- RMT peripheral for LEDs vs DMA SPI (pixel timing precision requirements depend on LED chip type WS2812B etc.)
- FS-i6 RC receiver protocol selection determining `RCControllerTask` parsing approach