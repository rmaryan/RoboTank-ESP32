/*
 * PWMBoardController.h
 *
 * A class which manages PCA9685 16-Channel 12-bit PWM Extender I2C.
 *
 * Reimplemented as a native class for ESP32 architecture.
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

#ifndef MAIN_PWMBOARDCONTROLLER_H_
#define MAIN_PWMBOARDCONTROLLER_H_

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class PWMBoardController {
public:
	/*
	 * Initialize the board
	 */
	static void init();

	/*
	 * Sets desired PWM pulses on the specified board pin.
	 * This method is multithreading-safe.
	 * num - pin #
	 * on - moment to turn signal ON
	 * off - moment to turn signal OFF
	 */
	static void setPWM(uint8_t num, uint16_t on, uint16_t off);

	/*
	 * Turns the specified board pin ON or OFF
	 * This method is using setPWM and thus is multithreading-safe.
	 * num - pin #
	 * ON - if true - turn on, else turn off
	 */
	static void setPinON(uint8_t num, bool ON=true);

private:
	// semaphore to safeguard the access to PWM board hardware access
	static SemaphoreHandle_t xPWMSemaphore;

	// sends the low-level command to the PWM board (1 byte - register address, 1 byte - data)
	static void sendPWMData(uint8_t addr, uint8_t data);
	// receives data from the PWM board
	static uint8_t receivePWMData(uint8_t addr);
};



#endif /* MAIN_PWMBOARDCONTROLLER_H_ */
