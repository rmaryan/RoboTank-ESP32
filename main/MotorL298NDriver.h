/*
 * MotorL298NDriver.h
 *
 * A class which manages L298N-based motor driver board
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

#ifndef MAIN_MOTORL298NDRIVER_H_
#define MAIN_MOTORL298NDRIVER_H_

#include <stdint.h>

class MotorL298NDriver {
private:
	static int16_t currentLeftSpeed;
	static int16_t currentRightSpeed;

public:
	// Run both motors at the speed specified. Speed can be between -255 (backward) and 255 (forward)
	static void go(int16_t speed);
	// Run each motor at different speed
	static void go(int16_t leftSpeed, int16_t rightSpeed);
};

#endif /* MAIN_MOTORL298NDRIVER_H_ */
