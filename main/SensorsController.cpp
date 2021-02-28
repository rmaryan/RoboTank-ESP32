/*
 * SensorsController.cpp
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

#include "SensorsController.h"

#include <driver/gpio.h>

#include "pin_mapping.h"


void SensorsController::init() {
	//FIXME to be finished
	gpio_pad_select_gpio(PIN_PIN_ESP32_IR_FL);
	gpio_set_direction(PIN_PIN_ESP32_IR_FL, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_PIN_ESP32_IR_FL, GPIO_PULLDOWN_ONLY);
	gpio_pad_select_gpio(PIN_PIN_ESP32_IR_FR);
	gpio_set_direction(PIN_PIN_ESP32_IR_FR, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_PIN_ESP32_IR_FR, GPIO_PULLDOWN_ONLY);
	gpio_pad_select_gpio(PIN_PIN_ESP32_IR_DL);
	gpio_set_direction(PIN_PIN_ESP32_IR_DL, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_PIN_ESP32_IR_DL, GPIO_PULLDOWN_ONLY);
	gpio_pad_select_gpio(PIN_PIN_ESP32_IR_DR);
	gpio_set_direction(PIN_PIN_ESP32_IR_DR, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_PIN_ESP32_IR_DR, GPIO_PULLDOWN_ONLY);
}
