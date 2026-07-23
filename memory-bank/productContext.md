# Product Context: RoboTank-ESP32

## Purpose
RoboTank-ESP32 serves as a fully programmable, WiFi-enabled robotic tank platform designed for educational purposes and hobbyist experimentation with robotics, embedded systems, and IoT concepts.

## Problems Solved
- **Complex hardware integration**: Provides pre-built, tested drivers for common robot components (motors, servos, sensors) so users can focus on application logic
- **Wireless control convenience**: Enables RC-style control combined with WiFi-based OTA updates and web interface access
- **Development workflow simplification**: PlatformIO + ESP-IDF setup eliminates manual SDK configuration

## How It Works
The system operates through multiple FreeRTOS tasks running on the ESP32:
1. **RCControllerTask** reads PWM signals from an FS-i6 transmitter receiver and maps channels to robot commands
2. **AITask** handles autonomous/remote logic processing based on RC input and sensor data
3. **Component controllers** (ArmController, MotorsL298NDriver, LEDsMT, SensorsController, etc.) manage individual hardware subsystems concurrently
4. **BufLogger** handles logging: intercepts ESP-IDF log output into ring buffer → serves via HTTP to web page + serial port
5. **OTAManager** handles firmware updates without physical connection

## User Experience Goals
- Plug-and-play setup with PlatformIO upload via USB or OTA
- Immediate feedback through LEDs, sounds, and motor response
- Intuitive RC control mapping for both manual operation and programmed sequences
- Reliable sensor readings and actuator responses in real-time