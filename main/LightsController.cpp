/*
 * LightsController.cpp
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

#include "LightsController.h"

#include "pin_mapping.h"
#include "PWMBoardController.h"
#include "RoboTankUtils.h"

DStrip LightsController::stripRGBLeft;
DStrip LightsController::stripRGBRight;
DLEDController LightsController::LEDcontroller;
ESP32RMTChannel LightsController::rmtChannelLeft;
ESP32RMTChannel LightsController::rmtChannelRight;

void LightsController::init() {

	stripRGBLeft.Create(BYTES_PER_LED, LED_COUNT, MAX_COLOR_VALUE);
	stripRGBRight.Create(BYTES_PER_LED, LED_COUNT, MAX_COLOR_VALUE);

	rmtChannelLeft.Initialize((rmt_channel_t) RMT_CHANNEL_LEFT,
			(gpio_num_t) PIN_PIN_ESP32_RGB_LEFT, LED_COUNT * 24);
	rmtChannelLeft.ConfigureForWS2812x();
	rmtChannelRight.Initialize((rmt_channel_t) RMT_CHANNEL_RIGHT,
			(gpio_num_t) PIN_PIN_ESP32_RGB_RIGHT, LED_COUNT * 24);
	rmtChannelRight.ConfigureForWS2812x();

	LEDcontroller.SetLEDType(LEDType::WS2812B);

	allOff();
}

void LightsController::lightsON(MainLightsDirection direction, uint16_t power) {
	if (power >= MAX_POWER) {
		if ((direction == LIGHTS_BOTH) || (direction == LIGHTS_HEAD)) {
			PWMBoardController::setPinON(PIN_PWM_LIGHTS_HEAD);
		}

		if ((direction == LIGHTS_BOTH) || (direction == LIGHTS_REAR)) {
			PWMBoardController::setPinON(PIN_PWM_LIGHTS_REAR);
		}
	} else {
		if ((direction == LIGHTS_BOTH) || (direction == LIGHTS_HEAD)) {
			PWMBoardController::setPWM(PIN_PWM_LIGHTS_HEAD, 0, power);
		}

		if ((direction == LIGHTS_BOTH) || (direction == LIGHTS_REAR)) {
			PWMBoardController::setPWM(PIN_PWM_LIGHTS_REAR, 0, power);
		}
	}
}

void LightsController::rgbSet(RGBLightsSide side, uint8_t r, uint8_t g,
		uint8_t b) {

	if ((side == RGB_LEFT) || (side == RGB_BOTH)) {
		for (int i = 0; i < LED_COUNT; i++) {
			stripRGBLeft.SetPixel(i, r, g, b);
			LEDcontroller.SetLEDs(stripRGBLeft.description.data,
					stripRGBLeft.description.dataLen, &rmtChannelLeft);
		}
	}

	if ((side == RGB_RIGHT) || (side == RGB_BOTH)) {
		for (int i = 0; i < LED_COUNT; i++) {
			stripRGBRight.SetPixel(i, r, g, b);
			LEDcontroller.SetLEDs(stripRGBRight.description.data,
					stripRGBRight.description.dataLen, &rmtChannelRight);
		}
	}
}
