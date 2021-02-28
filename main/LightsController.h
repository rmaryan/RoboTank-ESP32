/*
 * LightsController.h
 *
 * A class which manages all robots lights - head, rear, bottom RGB.
 * RGB code uses ESP32 DLED Controller library from  CalinRadoni (https://github.com/CalinRadoni/ESP32DLEDController)
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

#ifndef MAIN_LIGHTSCONTROLLER_H_
#define MAIN_LIGHTSCONTROLLER_H_

#include <stdint.h>

// Includes for the ESP32DLEDController library
#include "ESP32RMTChannel.h"
#include "DStrip.h"
#include "DLEDController.h"

class LightsController {
private:
	static const uint16_t MAX_POWER = 4096;
	static const uint8_t  RMT_CHANNEL_LEFT   = 0;     // ESP32 RMT's channel [0 ... 7]
	static const uint8_t  RMT_CHANNEL_RIGHT  = 1;     // ESP32 RMT's channel [0 ... 7]
	static const uint8_t  BYTES_PER_LED  = 3;     // ESP32 RMT's channel [0 ... 7]
	static const uint16_t LED_COUNT  = 3;             // 3 LEDS per strip
	static const uint8_t  MAX_COLOR_VALUE    = 255;   // maximum value allowed for color component


	static DStrip stripRGBLeft;
	static DStrip stripRGBRight;
	static DLEDController LEDcontroller;
	static ESP32RMTChannel rmtChannelLeft;
	static ESP32RMTChannel rmtChannelRight;



public:
	enum MainLightsDirection {
		LIGHTS_HEAD,
		LIGHTS_REAR,
		LIGHTS_BOTH
	};

	enum RGBLightsSide {
		RGB_LEFT,
		RGB_RIGHT,
		RGB_BOTH
	};

	// Initialize the lights subsystem
	static void init();

	// Turn on lights at specified power
	// direction - head, rear or both
	// power - from 0 to MAX_POWER. Default: MAX_POWER
	static void lightsON(MainLightsDirection direction, uint16_t power=MAX_POWER);

	// Turn off main lights
	static void lightsOFF(MainLightsDirection direction) {
		lightsON(direction, 0);
	}

	// Turn on RGB lights
	// side - left, right or both
	// r, g, b - color
	static void rgbSet(RGBLightsSide side, uint8_t r, uint8_t g, uint8_t b);

	// Turn off RGB lights
	static void rgbOFF(RGBLightsSide side) {
		rgbSet(side, 0, 0, 0);
	}

	// turn all lights off
	static void allOff() {
		lightsOFF(LIGHTS_BOTH);
		rgbOFF(RGB_BOTH);
	}

};

#endif /* MAIN_LIGHTSCONTROLLER_H_ */
