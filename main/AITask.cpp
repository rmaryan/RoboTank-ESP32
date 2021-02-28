/*
 * AITask.cpp
 *
 * Copyright (c) 2021 Maryan Rachynskyy
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "AITask.h"

#include <esp_log.h>
static const char* LOG_TAG = "AI";

#include "MotorL298NDriver.h"
#include "SoundModuleController.h"
#include "ArmController.h"
#include "RCControllerTask.h"
#include "LightsController.h"
#include "RoboTankUtils.h"

// task definitions for FreeRTOS
#define AI_TASK_NAME "AI_TASK"
#define AI_TASK_PRIORITY 5
#define TASK_STACK_SIZE 512

xTaskHandle AITask::handle = NULL;

void AITask::AItaskFunction() {
	while(true)	{
		//FIXME Quick and dirty RC code for the motors and a hand. To be completed.
		const uint16_t MAX_SPEED = 4095;
		uint16_t in_steering = RCControllerTask::getChannelState(0);
		uint16_t in_throttle = RCControllerTask::getChannelState(1);

		if((in_steering == 0) || (in_throttle == 0)) {
			MotorL298NDriver::go(0, 0);
			continue;
		}

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

		delay_ms(200);
	}
}

void AITask::init() {
	// Start the listener task
	xTaskCreate(AItaskfun, AI_TASK_NAME, TASK_STACK_SIZE, NULL, AI_TASK_PRIORITY, &handle);
	if(handle == NULL ) {
		ESP_LOGE(LOG_TAG, "AI task creation failed");
		return;
	}

	ESP_LOGI(LOG_TAG, "AI initiated");

	// Play the hello sound
	SoundModuleController::playSound(3); // The hello sound ID
}
