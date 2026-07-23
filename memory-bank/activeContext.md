# Active Context: RoboTank-ESP32

## Current Work Focus
Initial memory bank creation for the project. The codebase is a well-structured ESP32 firmware application built with PlatformIO and ESP-IDF framework, managing multiple robotic subsystems concurrently via FreeRTOS tasks.

## Recent Changes
No recent changes documented yet - this is the first session documenting this repository (commit: `cdf3718`).

## Next Steps
- Review all source files in detail to understand current implementation state
- Validate RC controller PWM signal processing logic

## Active Decisions and Considerations
- Project uses `include/` for headers and `src/` for implementations consistently
- Pin configuration is centralized in `pin_mapping.h` header - reference this file before any hardware-related changes
- FreeRTOS tasks are used extensively (AITask, RCControllerTask) with separate task files
- PlatformIO platform (`platformio.ini`) drives the build system rather than raw ESP-IDF Makefiles (beware - project uses pioarduino fork and VS Code plugin)

## Important Patterns and Preferences
- Each subsystem has both a header (.h) implementation (.cpp) pair in `include/` and `src/` respectively
- Utility functions are centralized in `RoboTankUtils.cpp`
- Per-module logging via `rtank_esp_log.h` for consistent log output

## Learnings and Project Insights
- The project includes schematics for mechanical design (`3D Models/`) and circuit diagrams (`flow chart.drawio`, `.png`) - useful reference before hardware-related modifications
- SD card directory contains `.AD4` sound files (WTV020SD16P format) for playback via SoundModuleController at track IDs 1-512

<!-- MEMORY BANK INITIALIZED -->