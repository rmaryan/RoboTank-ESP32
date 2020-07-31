/*
 * ArmController.cpp
 *
 * Copyright (c) 2020 Maryan Rachynskyy
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <esp_log.h>
#include "ArmController.h"
#include "PWMBoardController.h"
#include "RoboTankUtils.h"

const uint8_t ArmController::SERVO_PWM_PORTS[SERVOS_COUNT] = {
		PIN_PWM_ARM_SERVO0,
		PIN_PWM_ARM_SERVO1,
		PIN_PWM_ARM_SERVO2,
		PIN_PWM_ARM_SERVO3,
		PIN_PWM_ARM_SERVO4,
		PIN_PWM_ARM_SERVO5
};

const uint8_t ArmController::SERVO_MIN_DEG[SERVOS_COUNT] = 	 {2,   15,  10,  30,  10,  45};
const uint8_t ArmController::SERVO_MAX_DEG[SERVOS_COUNT] =   {178, 165, 180, 170, 170, 85};
const uint8_t ArmController::SERVO_PARK_DEG[SERVOS_COUNT] =  {90,  20,  10,  50,  90,  71}; // arm parking positions. Set 255 to skip

uint16_t ArmController::servo_current_position[SERVOS_COUNT] = {};
int16_t  ArmController::servo_current_speed[SERVOS_COUNT] = {};
uint16_t ArmController::servo_target_position[SERVOS_COUNT] =  {};
uint8_t ArmController::servo_target_positionDEG[SERVOS_COUNT] =  {};

#define SERVO_TIMER_TASK_NAME "SERVO TIMER"
TimerHandle_t ArmController::xServoUpdateTimerHandle;

static const char* LOG_TAG = "ARM";

void ArmController::init() {
	// Make sure the arm is initialized in the parked position
	// Be careful about what you have in your SERVO_PARK_DEG
	// or uninitialized arm can damage lots of stuff around the robot
	parkArm();
	for(int8_t i = (SERVOS_COUNT-1); i>=0; i--) {
		servo_current_position[i] = servo_target_position[i];
		PWMBoardController::setPWM(SERVO_PWM_PORTS[i], 0, servo_current_position[i]);
	}

	// start the arm moving timer task
	// we use it to move smoothly all servos of the arm simultaneously
	xServoUpdateTimerHandle = xTimerCreate(
			SERVO_TIMER_TASK_NAME,
			pdMS_TO_TICKS(SERVO_STEP_DURATION), /* period/time */
			pdTRUE, /* auto reload */
			(void*)0, /* timer ID */
			servoTimerCallback); /* callback */

	if (xServoUpdateTimerHandle!=NULL) {
		if(xTimerStart(xServoUpdateTimerHandle, 0) == pdPASS ) {
			ESP_LOGI(LOG_TAG, "ARM Controller initiated");
		} else {
			ESP_LOGE(LOG_TAG, "ARM Controller Timer start failed");
		}
	} else {
		ESP_LOGE(LOG_TAG, "ARM Controller Timer creation failed");
	}
}

void ArmController::turnServo(uint8_t servoID, int16_t turningAngleDEG) {
	if(servoID < SERVOS_COUNT) {
		int16_t desiredPositionDEG = servo_target_positionDEG[servoID] + turningAngleDEG;
		if(desiredPositionDEG< SERVO_MIN_DEG[servoID])
			desiredPositionDEG = SERVO_MIN_DEG[servoID];
		else
			if(desiredPositionDEG > SERVO_MAX_DEG[servoID])
				desiredPositionDEG = SERVO_MAX_DEG[servoID];

		setServo(servoID, desiredPositionDEG);
	}
}

void ArmController::parkArm() {
	for(int8_t i = (SERVOS_COUNT-1); i>=0; i--) {
		setServo(i, SERVO_PARK_DEG[i]);
	}
}

void ArmController::setServo(uint8_t servoID, uint8_t servoPositionDEG) {
	// Check the parameters validity not to harm innocent PWM lines
	if(servoID < SERVOS_COUNT) {
		if((servoPositionDEG>= SERVO_MIN_DEG[servoID]) &&
				(servoPositionDEG<= SERVO_MAX_DEG[servoID])) {
			servo_target_positionDEG[servoID] = servoPositionDEG;
			servo_target_position[servoID] = map(servoPositionDEG, 0, 180, SERVOMIN_PULSE, SERVOMAX_PULSE);
		}
	}
}

void ArmController::servoTimerCallback(TimerHandle_t pxTimer) {
	for(int servoID = 0; servoID<SERVOS_COUNT; servoID++) {
		if(servo_current_position[servoID] == servo_target_position[servoID]) {
			// nothing to do for this servo
			continue;
		}

		// detect the desired rotation direction
		bool positiveDirection = servo_current_position[servoID] < servo_target_position[servoID];

		// should we change rotation direction?
		if(servo_current_speed[servoID] != 0) {
			if((servo_current_speed[servoID]>0)!=positiveDirection) {
				// stop the movement and wait for one cycle
				servo_current_speed[servoID] = 0;
				ESP_LOGI(LOG_TAG, "CALLBACK REDIRECT %d c=%d t=%d", servoID, servo_current_position[servoID], servo_target_position[servoID]);
				continue;
			}
		}

		// being here means the servo either does not move, or moves to the expected direction
		uint16_t absDistance = abs(servo_target_position[servoID] - servo_current_position[servoID]);
		uint16_t absSpeed = abs(servo_current_speed[servoID]);

		if(servoID == 1) {
			ESP_LOGI(LOG_TAG, "CALLBACK %d c=%d t=%d ad=%d as=%d", servoID, servo_current_position[servoID], servo_target_position[servoID], absDistance, absSpeed);
		}

		// Can we reach the destination within one tick?
		if(absDistance <= SERVO_SPEED_MIN) {
			servo_current_position[servoID] = servo_target_position[servoID];
			servo_current_speed[servoID] = 0;
		} else {
			// should we start decreasing the speed because approaching the destination?
			if(absDistance <= SERVO_SLOW_DOWN_DISTANCE) {
				absSpeed /=SERVO_SPEED_ACCELERATION;
				if(absSpeed<SERVO_SPEED_MIN) {
					absSpeed = SERVO_SPEED_MIN;
				}
			} else
				// destination is far. should we increase the speed to the maximum
				if(absSpeed < SERVO_SPEED_MAX) {
					if(absSpeed == 0) {
						// if we were at zero speed - set a minimum speed
						absSpeed = SERVO_SPEED_MIN;
					} else {
						// if we were moving already - double the speed
						absSpeed*=SERVO_SPEED_ACCELERATION;
					}
				}
			servo_current_speed[servoID] = positiveDirection? absSpeed : -absSpeed;
			servo_current_position[servoID] += servo_current_speed[servoID];
		}

		PWMBoardController::setPWM(SERVO_PWM_PORTS[servoID], 0, servo_current_position[servoID]);
	}
}

#ifdef SERVO_CALIBRATION_MODE
void ArmController::servoCalibration() {
	const uint8_t BUFF_SIZE = 4;
	char buffer[BUFF_SIZE+1] = "1234";
	uint8_t buffLen = 0;

	vTaskDelay(2000 / portTICK_RATE_MS);

	printf("Submit commands in format: SDDD, where S - servo ID (0-8) DDD - position in degree\n");

	// commands level loop
	while(1) {
		// characters reading loop
		while(1) {
			int ch;
			ch = getchar();
			if (ch == EOF) {
				// wait a bit for the characters to be available
				vTaskDelay(200 / portTICK_RATE_MS);
			} else
				if (ch == '\n') {
					// this is an end of line character - process the command
					buffer[buffLen] = 0;
					break;
				} else
					if ((ch >= '0') && (ch <='9')) {
						// this is a numeric character for our command
						if(buffLen < BUFF_SIZE) {
							buffer[buffLen] = ch;
							printf("%c", ch);
							buffLen++;
						} else {
							// overflow
							printf("\nToo long\n");
							buffLen = 0;
						}
					} else {
						// just ignore other characters
					}
		}
		// buffer contains some command
		if(buffLen > 0) {
			uint8_t servoID = buffer[0] - '0';
			char *end;
			uint8_t servoPosition = strtoul(&(buffer[1]), &end, 10);

			buffLen = 0;

			printf("\nSetting servo %d to position %d\n", servoID, servoPosition);
			setServo(servoID, servoPosition);
		}
	}
}

#endif
