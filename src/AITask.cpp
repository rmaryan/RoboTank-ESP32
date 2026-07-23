/*
 * AITask.cpp
 *
 * Copyright (c) 2025 Maryan Rachynskyy
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

#include "rtank_esp_log.h"
static const char *LOG_TAG = LOG_TAG_AI;

#include "MotorL298NDriver.h"
#include "SoundModuleController.h"
#include "ArmController.h"
#include "RCControllerTask.h"
#include "LightsController.h"
#include "SensorsController.h"
#include "RoboTankUtils.h"
#include <esp_timer.h>

// task definitions for FreeRTOS
#define AI_TASK_NAME "AI_TASK"
#define AI_TASK_PRIORITY 7
#define TASK_STACK_SIZE 2048

// motors and servo speed limits
#define MOTOR_MAX_SPEED 4095
#define SERVO_FAST_SPEED 5
#define SERVO_SLOW_SPEED 1
#define RC_SERVO_MID 1500
#define SERVO_DEAD_ZONE 100
#define SERVO_SLOW_ZONE 300

TaskHandle_t AITask::handle = NULL;
AI_STATE AITask::ai_mode = AI_STATE_IDLE;

uint8_t AITask::rgbColor[3] = {0, 255, 0};
uint8_t AITask::increasingColor = 1;
uint8_t AITask::decreasingColor = 0;
uint8_t AITask::currentLightsMode = 0; // 0 - off, 1 - on, 2 -ALL + RGB

void AITask::AItaskFunction()
{
	// This method implements the RC flow chart as specified in the README.md
	while (true)
	{
#ifdef RC_DEBUG_ENABLED
		static uint32_t lastDebugTime = 0;
		uint32_t currentTime = esp_timer_get_time() / 1000;
		if (currentTime - lastDebugTime >= 1000)
		{
			lastDebugTime = currentTime;
			ESP_LOGI(LOG_TAG, "RC Channels: 0=%d 1=%d 2=%d 3=%d 4=%d 5=%d 6=%d 7=%d 8=%d 9=%d",
					 RCControllerTask::getChannelAnalogState(0),
					 RCControllerTask::getChannelAnalogState(1),
					 RCControllerTask::getChannelAnalogState(2),
					 RCControllerTask::getChannelAnalogState(3),
					 RCControllerTask::getChannelAnalogState(4),
					 RCControllerTask::getChannelAnalogState(5),
					 RCControllerTask::getChannelAnalogState(6),
					 RCControllerTask::getChannelAnalogState(7),
					 RCControllerTask::getChannelAnalogState(8),
					 RCControllerTask::getChannelAnalogState(9));
		}
#endif

		switch (ai_mode)
		{

		case AI_STATE_IDLE:
			// In idle mode robot just waits for the preheat mode activation by switching SWA down

			if (RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWA) == RCControllerTask::SW_DOWN)
			{
				SoundModuleController::playSound(0);
				ai_mode = AI_STATE_PREHEAT;
				ESP_LOGI(LOG_TAG, "preheat mode activated");
				delay_ms(1000);
			}
			else
			{
				delay_ms(500);
			}
			break;

		case AI_STATE_PREHEAT:
			// This mode is activated when SWA switch was turned down
			// Waiting for the SWA to return back up to arm the robot
			// We use a two-staged approach to avoid accidental arming

			if (RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWA) == RCControllerTask::SW_UP)
			{
				ai_mode = AI_STATE_ARMED;
				SoundModuleController::playSound(1);
				ESP_LOGI(LOG_TAG, "ARMED");
				delay_ms(1000);
			}

			delay_ms(500);
			break;

		case AI_STATE_ARMED:
			// In this mode the robot will be responding to the RC commands
			// This mode can be deactivated by turning SWA down (or if the RC connection was lost)
			if (RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWA) != RCControllerTask::SW_UP)
			{
				ai_mode = AI_STATE_PREHEAT;
				// emergency stop the robot
				MotorL298NDriver::go(0, 0);			 // stop the motors
				SoundModuleController::playSound(2); // disarm sound
				ESP_LOGI(LOG_TAG, "disarmed");
				break;
			}

			// If robot is armed, it will act according to the SWE switch position
			// If SWE pushed - AI mode is active
			// If SWE is not pushed - RC mode is active
			if (RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWE) == RCControllerTask::SW_DOWN)
			{
				processTickAI();
			}
			else
			{
				processTickRC();
			}

			vTaskDelay(pdMS_TO_TICKS(200));
			break;
		}
	}
}

void AITask::processTickAI()
{
	// TODO The AI mode code will be implemented here
	// At this point - just park the arm
	// Later - a smooth transition between the RC and AI modes should be implemented
	ArmController::parkArm();
}

void AITask::processTickRC()
{
	// Process the right gimbal position, sending commands to the chassis motors
	uint16_t in_steering = RCControllerTask::getChannelAnalogState(
		RCControllerTask::RC_RG_H);
	uint16_t in_throttle = RCControllerTask::getChannelAnalogState(
		RCControllerTask::RC_RG_V);

	if ((in_steering == 0) || (in_throttle == 0))
	{
		// connection lost just keep the robot stable indefinitely
		MotorL298NDriver::go(0, 0);
	}
	else
	{
		int16_t leftSpeed = map(in_throttle, RC_CHANNEL_DOWN, RC_CHANNEL_UP,
								-MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
		int16_t rightSpeed = leftSpeed;

		int16_t turnRate = map(in_steering, RC_CHANNEL_DOWN, RC_CHANNEL_UP,
							   MOTOR_MAX_SPEED, -MOTOR_MAX_SPEED);

		leftSpeed += turnRate;
		rightSpeed -= turnRate;

		if (leftSpeed > MOTOR_MAX_SPEED)
		{
			int16_t excess = leftSpeed - MOTOR_MAX_SPEED;
			leftSpeed = MOTOR_MAX_SPEED;
			rightSpeed -= excess;
		}
		else if (rightSpeed > MOTOR_MAX_SPEED)
		{
			int16_t excess = rightSpeed - MOTOR_MAX_SPEED;
			rightSpeed = MOTOR_MAX_SPEED;
			leftSpeed -= excess;
		}
		MotorL298NDriver::go(leftSpeed, rightSpeed);
	}

	// change the arm position if needed
	// Get the state of the SWC - it selects the level of servos to be changed
	uint8_t servoLowerID = 0;
	uint8_t servoUpperID = 0;
	switch (RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWC))
	{
	case RCControllerTask::SW_UP:
		servoLowerID = 4;
		servoUpperID = 5;
		break;
	case RCControllerTask::SW_MID:
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
								 RCControllerTask::getChannelAnalogState(
									 RCControllerTask::RC_LG_H)));
	ArmController::turnServo(servoUpperID,
							 calcServoSpeed(
								 RCControllerTask::getChannelAnalogState(
									 RCControllerTask::RC_LG_V)));

	// Control the lights with the channel #5 switch (SWB)
	if ((RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWB) == RCControllerTask::SW_MID) && (currentLightsMode != 1))
	{
		LightsController::lightsON(LightsController::LIGHTS_BOTH);
		LightsController::rgbOFF(LightsController::RGB_BOTH);
		currentLightsMode = 1;
	}
	else if ((RCControllerTask::getChannelDiscreteState(RCControllerTask::RC_SWB) == RCControllerTask::SW_UP) && (currentLightsMode != 2))
	{
		LightsController::lightsON(LightsController::LIGHTS_BOTH);
		LightsController::rgbSet(LightsController::RGB_BOTH,
								 rgbColor[0], rgbColor[1], rgbColor[2]);
		currentLightsMode = 2;
	}
	else
	{
		if (currentLightsMode != 0)
		{
			LightsController::allOff();
			currentLightsMode = 0;
		}
	}
}

void AITask::animateRGB()
{
	if (rgbColor[decreasingColor] == 0)
	{
		// time to switch the direction
		decreasingColor = increasingColor;
		increasingColor = (decreasingColor == 2) ? 0 : decreasingColor + 1;
	}
	else
	{
		rgbColor[decreasingColor] -= 1;
		rgbColor[increasingColor] += 1;
	}
	LightsController::rgbSet(LightsController::RGB_BOTH, rgbColor[0],
							 rgbColor[1], rgbColor[2]);
}

int16_t AITask::calcServoSpeed(uint16_t channelState)
{
	// are we in dead zone?
	if ((channelState > (RC_SERVO_MID - SERVO_DEAD_ZONE)) && (channelState < (RC_SERVO_MID + SERVO_DEAD_ZONE)))
	{
		return 0;
	}
	else
		// are we in slow zone?
		if ((channelState > (RC_SERVO_MID - SERVO_SLOW_ZONE)) && (channelState < (RC_SERVO_MID + SERVO_SLOW_ZONE)))
		{
			return (channelState < RC_SERVO_MID) ? -SERVO_SLOW_SPEED : SERVO_SLOW_SPEED;
		}
		else
			return (channelState < RC_SERVO_MID) ? -SERVO_FAST_SPEED : SERVO_FAST_SPEED;
}

void AITask::init()
{
	// Start the listener task
	xTaskCreate(AItaskfun, AI_TASK_NAME, TASK_STACK_SIZE, NULL,
				AI_TASK_PRIORITY, &handle);
	if (handle == NULL)
	{
		ESP_LOGE(LOG_TAG, "AI task creation failed");
		return;
	}

	ESP_LOGI(LOG_TAG, "AI initiated");

	// Play the hello sound
	SoundModuleController::playSound(3); // The hello sound
}
