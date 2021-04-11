/*
 *  Tank robot with sensors and manipulators built on ESP32 microcontroller.
 *
 *  THE ENTRY POINT
 *
 *  Minimum ESP-IDF version required: 4.0
 *
 *  Copyright (c) 2020 Maryan Rachynskyy
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"
#include "OTAManager.h"
#include "PWMBoardController.h"
#include "MotorL298NDriver.h"
#include "SoundModuleController.h"
#include "ArmController.h"
#include "LightsController.h"
#include "SensorsController.h"
#include "RCControllerTask.h"
#include "AITask.h"
#include "RoboTankUtils.h"

#ifdef LOG_USE_WEB_FRONTEND
#include "BufLogger.h"
#endif

static const char* LOG_TAG = "ROOT";

extern "C" {
void app_main();
}

void app_main()
{
	// Initialize the web front-end logger
#ifdef LOG_USE_WEB_FRONTEND
	BufLogger::init();
#endif

	ESP_LOGI(LOG_TAG, "Starting the launch sequence...");
	ESP_LOGI(LOG_TAG, "Version: %s %s", __TIME__, __DATE__);

	// Initializing the OTA and web server subsystem
	OTAManager::init();

	// Hello Sound!
	SoundModuleController::init();

	// PWM Board
	PWMBoardController::init();

	// Arm Controller
	ArmController::init();
	// This is the arm servos calibration hook.
	// If you uncomment SERVO_CALIBRATION_MODE definition in ArmController.h - the main execution thread is highjacked
	// Connect to the robot in serial console and send servo position commands as requested. This way you can define edge
	// and parking positions for the servos.
#ifdef SERVO_CALIBRATION_MODE
	ArmController::servoCalibration();
	// Nothing is executed beyond this step
#endif

	// Lights controller
	LightsController::init();

	// IR and Ultrasonic Sensors
	SensorsController::init();

	//RC Controller
	RCControllerTask::init();

	//Launch of the AI. Wait 1 second for other modules to settle.
	delay_ms(1000);
	AITask::init();
}
