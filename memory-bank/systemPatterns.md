# System Patterns: RoboTank-ESP32

## Architecture Overview
The system follows a **Modular Task-Based** architecture where each major subsystem operates as an independent module with clear separation of concerns. The main entry point is `src/Robo-tank-ESP32.cpp` which initializes all components and starts FreeRTOS tasks.

## Key Technical Decisions

### FreeRTOS Task Model
- Multiple concurrent tasks communicate via queues, semaphores, or shared state
- **AITask.h/cpp** - Central AI/logic coordination task handling high-level decisions
- **RCControllerTask.h/cpp** - Dedicated RC signal processing task running independently

### Hardware Abstraction Pattern
Each peripheral has its own controller class:
- `ArmController` - Servo-based robotic arm manipulation
- `MotorL298NDriver` - DC motor control via L298N H-bridge module
- `LEDRMT` - RGB LED strip driver using RMT peripheral
- `LightsController` - Tank lighting system management
- `SensorsController` - Multi-sensor data aggregation
- `SoundModuleController` - WTV020SD16P sound board controller for .AD4 track playback (files stored on SD card)
- `PWMBoardController` - PWM signal generation/board-level output

### Logging Architecture
- **BufLogger** - Intercepts esp-log framework via ring buffer, collects messages and redirects them to alternative receivers (serial port + web page). On overflow, oldest messages are dropped. Does NOT write to SD card.
- **rtank_esp_log.h/cpp** - Module-specific log wrapper using ESP-IDF log facility

## Component Relationships
```
                     +------------------+
                     |   main (Entry)   |
                     +--------+---------+
                              |
             +---------------+---------------+
             |               |               |
      +------v-----+  +------v------+ +----v-------+
      | AITask     |  | RCController| | OTAManager |
      +------------+  +-------------+ +------------+
             |               
     +-------v---------------+--------+       
     |         Shared State / API      |     
     +-------------------------------+        
                             |                
            +--------------+--+--+-----------+ 
            |              |       |          |      
   +------v------+ +-----v----+ +-v---------+-+----------+
   |ArmController|  | Motors   ||Sensors    ||SoundModule|
   +-------------+  +----------+ +----------++-+---------+
            |                             |           |     
   +-------v--------+             +------v------+     v      
   |PWMBoardCtrl/Li|          +-WebServer----->|  .AD4 Files
   +----------------+          | (BufLogger)    |     on SD    
   +------------------+        +----------------+ 
```

## Sound System Architecture
- **SoundModuleController** manages WTV020SD16P sound module via parallel port interface
- Sounds stored as `.AD4` files on SD card (track IDs 1-512)
- Triggered by other modules during specific events (e.g., AI decisions, sensor alerts)

## Design Patterns in Use
- **Controller Pattern**: Each hardware subsystem has a dedicated controller class
- **Observer-like communication**: Shared state updates trigger downstream reactions
- **Singleton-style modules**: Controller instances typically created once at initialization with global/static access patterns