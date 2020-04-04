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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ArmController.h"
#include "PWMBoardController.h"
#include "RoboTankUtils.h"

#define SERVOMIN  100 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  500 // this is the 'maximum' pulse length count (out of 4096)

const uint8_t ArmController::SERVO_PWM_PORTS[SERVOS_COUNT] = {
		PIN_PWM_ARM_SERVO0,
		PIN_PWM_ARM_SERVO1,
		PIN_PWM_ARM_SERVO2,
		PIN_PWM_ARM_SERVO3,
		PIN_PWM_ARM_SERVO4,
		PIN_PWM_ARM_SERVO5
};
const uint16_t ArmController::SERVO_MIN_DEG[SERVOS_COUNT] = {0, 0, 0, 0, 0, 0};
const uint16_t ArmController::SERVO_MAX_DEG[SERVOS_COUNT] = {180, 180, 180, 180, 180, 180};
const uint16_t ArmController::SERVO_PARK_DEG[SERVOS_COUNT] = {80, 60, 20, 20, 70, 70};


void ArmController::parkArm() {
	for(int8_t i = (SERVOS_COUNT-1); i>=0; i--) {
		moveServo(i, SERVO_PARK_DEG[i]);
	}
}

void ArmController::moveServo(uint8_t servoID, uint8_t servoPositionDEG) {
	// Check the parameters validity not to harm innocent PWM lines
	if(servoID < SERVOS_COUNT) {
		if((servoPositionDEG>= SERVO_MIN_DEG[servoID]) &&
				(servoPositionDEG<= SERVO_MAX_DEG[servoID])) {
			uint16_t pulse = map(servoPositionDEG, 0, 180, SERVOMIN, SERVOMAX);
			PWMBoardController::setPWM(SERVO_PWM_PORTS[servoID], 0, pulse);
		}
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
			moveServo(servoID, servoPosition);
		}
	}
}

#endif
