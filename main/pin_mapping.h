/*
 * pin_mapping.h
 *
 * This file defines which the real hardware IO pins were used for different functions.
 * They may differ to what is shown on the schematics.
 *
 *  Copyright (c) 2020 Maryan Rachynskyy
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PIN_MAPPING_H_
#define PIN_MAPPING_H_

#include <driver/gpio.h>

/* ESP32 Pins */
/*============*/

// ESP32 pins used to control the sound module (WTV020M01)
#define PIN_PIN_ESP32_SOUND_P04_CLK GPIO_NUM_13
#define PIN_PIN_ESP32_SOUND_P05_DI GPIO_NUM_26

// pins used for I2C connection to the PWM multiplexing board
#define PIN_PIN_ESP32_PWM_SCL GPIO_NUM_27
#define PIN_PIN_ESP32_PWM_SDA GPIO_NUM_14

// pin used to connect to the RC receiver port
#define PIN_PIN_ESP32_RC_RX GPIO_NUM_25

// bottom RGB LED controlling pins
#define PIN_PIN_ESP32_RGB_LEFT GPIO_NUM_32
#define PIN_PIN_ESP32_RGB_RIGHT GPIO_NUM_33

// ulstrasonic distance sensor pins
#define PIN_PIN_ESP32_US_TRIG GPIO_NUM_34
#define PIN_PIN_ESP32_US_ECHO GPIO_NUM_35

// Infrared obstacle detectors pins
#define PIN_PIN_ESP32_IR_FL GPIO_NUM_18
#define PIN_PIN_ESP32_IR_FR GPIO_NUM_17
#define PIN_PIN_ESP32_IR_DL GPIO_NUM_19
#define PIN_PIN_ESP32_IR_DR GPIO_NUM_5


/* PWM Multiplexer pins (PCA9685 board) */
/*======================================*/
#define PIN_PWM_ARM_SERVO0 7
#define PIN_PWM_ARM_SERVO1 8
#define PIN_PWM_ARM_SERVO2 9
#define PIN_PWM_ARM_SERVO3 10
#define PIN_PWM_ARM_SERVO4 12
#define PIN_PWM_ARM_SERVO5 13

#define PIN_PWM_LIGHTS_HEAD 14
#define PIN_PWM_LIGHTS_REAR 11

#define PIN_PWM_US_SERVO 0

#define PIN_PWM_MOTOR_ENA 3
#define PIN_PWM_MOTOR_IN1 1
#define PIN_PWM_MOTOR_IN2 2
#define PIN_PWM_MOTOR_IN3 5
#define PIN_PWM_MOTOR_IN4 6
#define PIN_PWM_MOTOR_ENB 4

#endif /* PIN_MAPPING_H_ */
