/*
 * PWMBoardController.cpp
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

#include "PWMBoardController.h"

#include <driver/i2c.h>
#include "pin_mapping.h"

// Board-specific definitions
#define PCA9685_I2C_ADDRESS 0x40      /* Default PCA9685 I2C Slave Address */
#define FREQUENCY_OSCILLATOR 25000000 /* Int. osc. frequency in datasheet */
#define PCA9685_PRESCALE 0xFE     /* Prescaler for PWM output frequency */
#define PCA9685_MODE1 0x00      /* Mode Register 1 */
#define MODE1_RESTART 0x80 /* Restart enabled */
#define MODE1_SLEEP 0x10   /* Low power mode. Oscillator off */
#define MODE1_AI 0x20      /* Auto-Increment enabled */
#define PCA9685_PRESCALE_MIN 3   /* minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /* maximum prescale value */
#define ACK_CHECK_EN 0x1 /* Check for ACK on I2C bus*/
#define PCA9685_LED0_ON_L 0x06  /* LED0 on tick, low byte*/

// We hardcode 50 Hz - this is what works for the analog servos
#define SERVOFREQ 50

void PWMBoardController::init() {
	// set up the I2C interface
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = PIN_PIN_ESP32_PWM_SDA;
	conf.scl_io_num = PIN_PIN_ESP32_PWM_SCL;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	i2c_param_config(I2C_NUM_0, &conf);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);


	// reset the PWM board
	sendPWMData(PCA9685_MODE1, MODE1_RESTART);

	// set up the desired PWM frequency
	float prescaleval = ((FREQUENCY_OSCILLATOR / (SERVOFREQ * 4096.0)) + 0.5) - 1;
	if (prescaleval < PCA9685_PRESCALE_MIN)
		prescaleval = PCA9685_PRESCALE_MIN;
	if (prescaleval > PCA9685_PRESCALE_MAX)
		prescaleval = PCA9685_PRESCALE_MAX;
	uint8_t prescale = (uint8_t)prescaleval;
	uint8_t oldmode = receivePWMData(PCA9685_MODE1);
	uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
	sendPWMData(PCA9685_MODE1, newmode);                     // go to sleep
	sendPWMData(PCA9685_PRESCALE, prescale); // set the prescaler
	sendPWMData(PCA9685_MODE1, oldmode);
	vTaskDelay(5/portTICK_PERIOD_MS);
	sendPWMData(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
	vTaskDelay(10/portTICK_PERIOD_MS);
}

void PWMBoardController::sendPWMData(uint8_t addr, uint8_t data) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (PCA9685_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}

void PWMBoardController::setPWM(uint8_t num, uint16_t on, uint16_t off) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (PCA9685_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, PCA9685_LED0_ON_L + 4 * num, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, on, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, on >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, off, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, off >> 8, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}

uint8_t PWMBoardController::receivePWMData(uint8_t addr) {
	uint8_t data = 0;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (PCA9685_I2C_ADDRESS << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	return data;
}

void PWMBoardController::setPinON(uint8_t num, bool ON) {
	// these are the magic combinations
	if(ON) {
		setPWM(num, 4096, 0);
	} else {
		setPWM(num, 0, 4096);
	}
}
