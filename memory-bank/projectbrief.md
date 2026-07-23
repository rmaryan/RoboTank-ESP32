# Project Brief: RoboTank-ESP32

## Overview
RoboTank-ESP32 is an ESP32-based robotic tank project featuring advanced control systems including arm manipulation, sensor integration, wireless communication, and SD card logging.

## Core Requirements
- Remote control via RC transmitter (FS-i6) with PWM signal decoding
- Robotic arm control with multiple degrees of freedom
- Real-time sensor monitoring and feedback
- LED strip effects for visual indication
- Audio playback module for alerts/announcements
- Over-the-Air (OTA) firmware updates
- Web-based interface capabilities for triggering OTA and reviewing logs

## Key Features
- Multi-module architecture with separated concerns
- Non-blocking task management via FreeRTOS tasks
- Power-efficient design with sleep/wake support
- Modular component drivers for reusable hardware abstraction