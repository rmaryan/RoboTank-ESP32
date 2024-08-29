/*
 * AITask.h
 *
 * The robot's AI which integrates everything and takes decisions.
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

#ifndef MAIN_AITASK_H_
#define MAIN_AITASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

enum AI_STATE {AI_STATE_IDLE, AI_STATE_RC_PRE, AI_STATE_RC};

class AITask {
private:
	static AI_STATE ai_mode;

	// Reference to the task created
	static xTaskHandle handle;

	// Variables for the RGB lights animation
	static uint8_t rgbColor[3];
	static uint8_t increasingColor;
	static uint8_t decreasingColor;

	// The task code is here
	static void AItaskFunction();
	static void processTickAI();
	static void processTickRC();

	// Wrapper for the xTaskCreate parameter
	static void AItaskfun(void* parm) {
		static_cast<AITask *>(parm)->AItaskFunction();
	}

	// the helper method which calculates the servo rotations speed depending on the RC gimbal position
	static int16_t calcServoSpeed(uint16_t channelState);

	// the helper method which animates the RGB lights and pushes changes to the Lights Controller
	static void animateRGB();

public:
	// Launch the AI thread
	// It is assumed that all the other modules were already loaded
	static void init();
};

#endif /* MAIN_AITASK_H_ */
