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

#include "esp_log.h"
static const char *LOG_TAG = "AI";

#include "MotorL298NDriver.h"
#include "SoundModuleController.h"
#include "ArmController.h"
#include "RCControllerTask.h"
#include "LightsController.h"
#include "SensorsController.h"
#include "RoboTankUtils.h"

// task definitions for FreeRTOS
#define AI_TASK_NAME "AI_TASK"
#define AI_TASK_PRIORITY 7
#define TASK_STACK_SIZE 2048

#define RC_CHANNEL_UP 1000
#define RC_CHANNEL_MID 1500
#define RC_CHANNEL_DOWN 2000

// motors and servo speed limits
#define MOTOR_MAX_SPEED 4095
#define SERVO_FAST_SPEED 5
#define SERVO_SLOW_SPEED 1
#define SERVO_DEAD_ZONE 100
#define SERVO_SLOW_ZONE 300

xTaskHandle AITask::handle = NULL;
AI_STATE AITask::ai_mode = AI_STATE_IDLE;

uint8_t AITask::rgbColor[3] = { 255, 0, 0 };
uint8_t AITask::increasingColor = 1;
uint8_t AITask::decreasingColor = 0;

void AITask::AItaskFunction() {
	// This method implements the RC flow chart as specified in the README.md
	while (true) {
		switch (ai_mode) {

		case AI_STATE_IDLE:
			// In idle mode robot just waits for one of the other modes activation
			// Transition to the other mode is activated by switching SWA down

			if (RCControllerTask::getChannelState(
					RCControllerTask::RC_SWA) == RC_CHANNEL_DOWN) {
				SoundModuleController::playSound(0);
				ai_mode = AI_STATE_RC_PRE;
				ESP_LOGI(LOG_TAG, "Activating the robot AI: preheat");
				delay_ms(1000);
			} else {
				delay_ms(500);
			}
			break;

		case AI_STATE_RC_PRE:
			// This mode is activated when SWA switch was turned down
			// Waiting for the SWA to return back up
			// Then - the robot is switched either to the RC mode

			if (RCControllerTask::getChannelState(
					RCControllerTask::RC_SWA) == RC_CHANNEL_UP) {
				ai_mode = AI_STATE_RC;
				SoundModuleController::playSound(1);
				ESP_LOGI(LOG_TAG, "Activating the robot AI: RC mode");
				delay_ms(1000);
			}

			delay_ms(500);
			break;

		case AI_STATE_RC:
			// In this mode the robot is controlled by the remote control
			// This mode can be deactivated by turning SWA down (or if the RC connection was lost)
			uint16_t swa_state = RCControllerTask::getChannelState(
					RCControllerTask::RC_SWA);
			if ((swa_state == 0) || (swa_state == RC_CHANNEL_DOWN)) {
				ai_mode = AI_STATE_IDLE;
				break;
			}

			// Should we switch to the AI code?
			if (RCControllerTask::getChannelState(
					RCControllerTask::RC_SWB) == RC_CHANNEL_DOWN) {
				processTickAI();
			} else {
				processTickRC();
			}

			delay_ms(200);
			break;
		}
	}
}

void AITask::processTickAI() {
	// TODO The AI mode code will be implemented here
	// At this point - just part the arm
	// Later - a smooth transition between the RC and AI modes should be implemented
	ArmController::parkArm();
}

void AITask::processTickRC() {
	// Process the right gimbal position, sending commands to the chassis motors
	uint16_t in_steering = RCControllerTask::getChannelState(
			RCControllerTask::RC_RG_H);
	uint16_t in_throttle = RCControllerTask::getChannelState(
			RCControllerTask::RC_RG_V);

	if ((in_steering == 0) || (in_throttle == 0)) {
		MotorL298NDriver::go(0, 0);
	} else {
		int16_t leftSpeed = map(in_throttle, RC_CHANNEL_DOWN, RC_CHANNEL_UP,
				-MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
		int16_t rightSpeed = leftSpeed;

		int16_t turnRate = map(in_steering, RC_CHANNEL_UP, RC_CHANNEL_DOWN,
				MOTOR_MAX_SPEED, -MOTOR_MAX_SPEED);

		leftSpeed += turnRate;
		rightSpeed -= turnRate;

		if (leftSpeed > MOTOR_MAX_SPEED) {
			uint16_t delta = MOTOR_MAX_SPEED - leftSpeed;
			leftSpeed -= delta;
			rightSpeed += delta;
		} else if (rightSpeed > MOTOR_MAX_SPEED) {
			uint16_t delta = MOTOR_MAX_SPEED - rightSpeed;
			leftSpeed += delta;
			rightSpeed -= delta;
		}
		MotorL298NDriver::go(leftSpeed, rightSpeed);
	}

	// change the hand position if needed
	// Get the state of the SWC - it selects the level of servos to be changed
	uint8_t servoLowerID = 0;
	uint8_t servoUpperID = 0;
	switch (RCControllerTask::getChannelState(RCControllerTask::RC_SWC)) {
	case RC_CHANNEL_UP:
		servoLowerID = 4;
		servoUpperID = 5;
		break;
	case RC_CHANNEL_MID:
		servoLowerID = 2;
		servoUpperID = 3;
		break;
	default:
		servoLowerID = 0;
		servoUpperID = 1;
	}

	// calculate the speed of servo rotations
	ArmController::turnServo(servoLowerID,
			calcServoSpeed(
					RCControllerTask::getChannelState(
							RCControllerTask::RC_LG_H)));
	ArmController::turnServo(servoUpperID,
			calcServoSpeed(
					RCControllerTask::getChannelState(
							RCControllerTask::RC_LG_V)));

	// update the lights state
	if (RCControllerTask::getChannelState(
			RCControllerTask::RC_SWD) == RC_CHANNEL_UP) {
		LightsController::lightsON(LightsController::LIGHTS_BOTH);
		animateRGB();
	} else {
		LightsController::allOff();
	}
}

void AITask::animateRGB() {
	if (rgbColor[decreasingColor] == 0) {
		// time to switch the direction
		decreasingColor = increasingColor;
		increasingColor = (decreasingColor == 2) ? 0 : decreasingColor + 1;
	} else {
		rgbColor[decreasingColor] -= 1;
		rgbColor[increasingColor] += 1;
	}
	LightsController::rgbSet(LightsController::RGB_BOTH, rgbColor[0],
			rgbColor[1], rgbColor[2]);
}

int16_t AITask::calcServoSpeed(uint16_t channelState) {
	// are we in dead zone?
	if ((channelState > (RC_CHANNEL_MID - SERVO_DEAD_ZONE))
			&& (channelState < (RC_CHANNEL_MID + SERVO_DEAD_ZONE))) {
		return 0;
	} else
	// are we in slow zone?
	if ((channelState > (RC_CHANNEL_MID - SERVO_SLOW_ZONE))
			&& (channelState < (RC_CHANNEL_MID + SERVO_SLOW_ZONE))) {
		return (channelState < RC_CHANNEL_MID)?-SERVO_SLOW_SPEED:SERVO_SLOW_SPEED;
	} else
		return (channelState < RC_CHANNEL_MID)?-SERVO_FAST_SPEED:SERVO_FAST_SPEED;
}

void AITask::init() {
// Start the listener task
	xTaskCreate(AItaskfun, AI_TASK_NAME, TASK_STACK_SIZE, NULL,
	AI_TASK_PRIORITY, &handle);
	if (handle == NULL) {
		ESP_LOGE(LOG_TAG, "AI task creation failed");
		return;
	}

	ESP_LOGI(LOG_TAG, "AI initiated");

// Play the hello sound
	SoundModuleController::playSound(3);	// The hello sound ID
}
