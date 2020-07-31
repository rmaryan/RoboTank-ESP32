/*
 * ArmController.h
 *
 * A class which manages the 6-servos robotic arm
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

#ifndef MAIN_ARMCONTROLLER_H_
#define MAIN_ARMCONTROLLER_H_

/**
 * If you uncomment the line below - a special servo calibration mode is engaged.
 * It hijacks the main execution thread and opens a serial CLI interface with the computer.
 * You can change positions of the arm servos sending commands in the serial terminal.
 * Command format: SDDD, where S - is a servo ID (0-6) DDD - position in degree from 0 to 180
 */
//#define SERVO_CALIBRATION_MODE 1

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include "pin_mapping.h"

class ArmController {
private:
	// the number of the servos in the arm
	static const uint8_t SERVOS_COUNT = 6;
	// mapping between servo ID and the actual PWM board port
	static const uint8_t SERVO_PWM_PORTS[SERVOS_COUNT];

	static const uint16_t SERVOMIN_PULSE = 100; // the 'minimum' pulse length count (out of 4096)
	static const uint16_t SERVOMAX_PULSE = 500; // the 'maximum' pulse length count (out of 4096)

	// The constants below are defined for each servo in the arm.
	// These are minimum possible servos angle, maximum possible angle and the parking position angle (DEG).
	// See moveServo explanation for more information on possible degrees range.
	// You should find the exact values for your servos using a try and error or smart calculations
	// as they depend on the arm geometry and servos parameters.
	// You can use SERVO_CALIBRATION_MODE definition above to make the calibration easier
	static const uint8_t SERVO_MIN_DEG[SERVOS_COUNT];
	static const uint8_t SERVO_MAX_DEG[SERVOS_COUNT];
	static const uint8_t SERVO_PARK_DEG[SERVOS_COUNT];

	// Servo state storage
	static uint16_t servo_current_position[SERVOS_COUNT]; // actually - current PWM pulse length
	static int16_t  servo_current_speed[SERVOS_COUNT];    // can be positive or negative, depending on the rotation direction
	static uint16_t servo_target_position[SERVOS_COUNT];  // desired PWM pulse length
	static uint8_t servo_target_positionDEG[SERVOS_COUNT]; // desired DEG position

	// Servo moving smoothness parameters
	static const uint16_t SERVO_STEP_DURATION = 100; // milliseconds
	static const uint8_t SERVO_SPEED_MAX = 8;     // the max speed of rotation (change in pulse length per step)
	static const uint8_t SERVO_SPEED_MIN = 4;   // the starting speed delta (in pulse length measure)
	static const uint8_t SERVO_SPEED_ACCELERATION = 2;   // speed will be multiplied by this amount until reaches SERVO_SPEED_MAX
	static const uint16_t SERVO_SLOW_DOWN_DISTANCE =  // distance to the target position when we should start decelerating from the max speed
			(SERVO_SPEED_MIN - SERVO_SPEED_MAX*SERVO_SPEED_ACCELERATION)/(1 - SERVO_SPEED_ACCELERATION);
			// these are a simple geometric progression calculations
	// This timer is used to feed new servos positions on regular basis
	static TimerHandle_t xServoUpdateTimerHandle;
	// Servos update timer calls this function every SERVO_STEP_DURATION milliseconds
	// Servo new positions are calculated and commanded here
	static void servoTimerCallback(TimerHandle_t pxTimer);

public:
	// Initialize the arm controller
	static void init();

	// Control the servo position directly
	// Please note - this function just sets a new target for the servo.
	// Actual servo management is performed by servoTimerCallback().
	// @param servoID
	// @param servoPositionDEG [0..180]
	// Please note - servo position degree is indicative. For the 270 degree servos it will be scaled up to the whole servo rotation range.
	// So consider 0 and 180 as the opposite edge positions and 90 as a mid point
	static void setServo(uint8_t servoID, uint8_t servoPositionDEG);

	// Change the servo position relatively
	// @param servoID
	// @param turningAngleDEG - how much to turn the servo in DEG. Positive values turn the servo right
	static void turnServo(uint8_t servoID, int16_t turningAngleDEG);

	// Move arm to the static parking position
	static void parkArm();

	// This function is used only if you engage the SERVO_CALIBRATION_MODE
#ifdef SERVO_CALIBRATION_MODE
	static void servoCalibration();
#endif
};

#endif /* MAIN_ARMCONTROLLER_H_ */
