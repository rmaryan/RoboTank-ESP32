# Tech Context: RoboTank-ESP32

## Technologies Used
- **Microcontroller**: ESP32 (dual-core Xtensa LX6)
- **Framework**: ESP-IDF (Espressif IoT Development Framework)
- **Build System**: PlatformIO with CMake (pioarduino fork)
- **RTOS**: FreeRTOS (bundled with ESP-IDF)
- **Language**: C/C++

## Development Environment Setup
- IDE: VS Code with pioarduino extension
- Configuration file: `platformio.ini` in project root
- Upload methods: USB serial or OTA (`OTAManager`)

## Dependencies & Libraries
Dependencies are declared in `platformio.ini`. Key ones likely include:
- ESP-IDF core (WiFi, SPIFFS/LittleFS)
- RMT driver for LED control
- Servo/PWM drivers for arm and motors
- SD/MMC library for card storage
- FS-i6 RC receiver signal decoding (PWM/GCS protocol parsing)

## Technical Constraints
- **Memory**: ESP32 SRAM (~520KB internal), PSRAM if equipped
- **Power**: Battery-powered, requires sleep/wake support (`LightsController` and `OTAManager`) for power management
- **Pin mapping**: Centralized in `include/pin_mapping.h` - all hardware references must align here
- **WiFi antenna**: Internal PCB trace on ESP32-WROOM; RF considerations near antenna area

## Tool Usage Patterns
- PlatformIO CLI (`pio run`, `pio upload`, ` pio device monitor`) for build/deploy/debug cycle - beware of the pioarduino specifics
- CMakeLists.txt in both root and `src/` directories (PlatformIO handles this automatically)