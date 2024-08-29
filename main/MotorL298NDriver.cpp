/*
 * MotorL298NDriver.cpp
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
 */

#include "MotorL298NDriver.h"

#include "driver/gpio.h"

#include "pin_mapping.h"
#include "PWMBoardController.h"

int16_t MotorL298NDriver::currentLeftSpeed = 0;
int16_t MotorL298NDriver::currentRightSpeed = 0;

void MotorL298NDriver::go(int16_t speed) {
	go(speed, speed);
}

void MotorL298NDriver::go(int16_t leftSpeed, int16_t rightSpeed) {
	bool goLeftForward = true;
	bool goRightForward = true;

	// normalize the speed per motors characteristics
	if(leftSpeed < -MAX_SPEED)
		leftSpeed = -MAX_SPEED;
	else if(leftSpeed > MAX_SPEED)
		leftSpeed = MAX_SPEED;
	else if(abs(leftSpeed)<MIN_SPEED)
		leftSpeed = 0;

	if(rightSpeed < -MAX_SPEED)
		rightSpeed = -MAX_SPEED;
	else if(rightSpeed > MAX_SPEED)
		rightSpeed = MAX_SPEED;
	else if(abs(rightSpeed)<MIN_SPEED)
		rightSpeed = 0;

	if (leftSpeed != currentLeftSpeed) {
		currentLeftSpeed = leftSpeed;
		if (leftSpeed < 0) {
			goLeftForward = false;
			leftSpeed = -leftSpeed;
		}

		PWMBoardController::setPinON(PIN_PWM_MOTOR_IN1, goLeftForward);
		PWMBoardController::setPinON(PIN_PWM_MOTOR_IN2, !goLeftForward);
		PWMBoardController::setPWM(PIN_PWM_MOTOR_ENA, 0, leftSpeed);
	}

	if (rightSpeed != currentRightSpeed) {
		currentRightSpeed = rightSpeed;
		if (rightSpeed < 0) {
			goRightForward = false;
			rightSpeed = -rightSpeed;
		}

		PWMBoardController::setPinON(PIN_PWM_MOTOR_IN3, goRightForward);
		PWMBoardController::setPinON(PIN_PWM_MOTOR_IN4, !goRightForward);
		PWMBoardController::setPWM(PIN_PWM_MOTOR_ENB, 0, rightSpeed);
	}
}
