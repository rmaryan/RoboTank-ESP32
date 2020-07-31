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
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include "sdkconfig.h"
#include "OTAManager.h"
#include "pin_mapping.h"
#include "PWMBoardController.h"
#include "MotorL298NDriver.h"
#include "SoundModuleController.h"
#include "ArmController.h"
#include "RCControllerTask.h"
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
	SoundModuleController::setVolume(7); // Max volume
	SoundModuleController::playSound(3); // The hello sound ID

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

	//RC Controller
	RCControllerTask::init();

	while(true)	{
		// Quick and dirty RC for the motors and a hand
		const uint16_t MAX_SPEED = 4095;
		uint16_t in_steering = RCControllerTask::getChannelState(0);
		uint16_t in_throttle = RCControllerTask::getChannelState(1);

		int16_t leftSpeed = map(in_throttle, 1000, 2000, -MAX_SPEED, MAX_SPEED);
		int16_t rightSpeed = leftSpeed;

		int16_t turnRate = map(in_steering, 1000, 2000, MAX_SPEED, -MAX_SPEED);

		leftSpeed += turnRate;
		rightSpeed -= turnRate;

		if(leftSpeed > MAX_SPEED) {
			uint16_t delta = MAX_SPEED - leftSpeed;
			leftSpeed -= delta;
			rightSpeed += delta;
		} else
			if(rightSpeed > MAX_SPEED) {
				uint16_t delta = MAX_SPEED - rightSpeed;
				leftSpeed += delta;
				rightSpeed -= delta;
			}
		MotorL298NDriver::go(leftSpeed, rightSpeed);

		// hand is safeguarded by the channel 5
		if(RCControllerTask::getChannelState(5) == 2000) {
			// the arm servo is chosen by the channel 4
			uint8_t servoID = 0;
			switch (RCControllerTask::getChannelState(4)) {
			case 1000:
				servoID = 2;
				break;
			case 1500:
				servoID = 1;
				break;
			}
			uint16_t servoPosition = RCControllerTask::getChannelState(3);
			if((servoPosition >1400) && (servoPosition < 1600)) servoPosition = 1500;
			ArmController::turnServo(servoID, map(servoPosition, 1000, 2000, -10, +10));

		} else {
			ArmController::parkArm();
		}

		vTaskDelay(200 / portTICK_RATE_MS);
	}
}
