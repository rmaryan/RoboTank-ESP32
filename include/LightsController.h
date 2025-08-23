/*
 * LightsController.h
 *
 * A class which manages all robots lights - head, rear, bottom RGB.
 *
 * Copyright (c) 2025 Maryan Rachynskyy
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
#include "LEDRMT.h"

class LightsController {
private:
	static const uint16_t MAX_POWER = 4096;
	static const uint16_t LED_COUNT  = 3;    // 3 LEDS per strip

	static LEDRMT stripRGBLeft;
	static LEDRMT stripRGBRight;

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
