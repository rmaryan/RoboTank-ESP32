/*
 * RCController.h
 *
 * A class which manages the Radio Controller commands handling.
 *
 * Core code was borrowed from here: https://gitlab.com/timwilkinson/FlySkyIBus/-/tree/master
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

#ifndef MAIN_RCCONTROLLERTASK_H_
#define MAIN_RCCONTROLLERTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// RC Controller task priority. Must be less than configMAX_PRIORITIES
#define RC_CONTROLLER_PRIORITY 5

class RCControllerTask {
private:
	// Protocol parsing definitions
	enum State {
		GET_LENGTH,
		GET_DATA,
		GET_CHKSUML,
		GET_CHKSUMH,
		DISCARD,
	};

	static const uint8_t PROTOCOL_LENGTH = 0x20;
	static const uint8_t PROTOCOL_OVERHEAD = 3; // <len><cmd><data....><chkl><chkh>
	static const uint8_t PROTOCOL_TIMEGAP = 3; // Packets are received very ~7ms so use ~half that for the gap
	static const uint8_t PROTOCOL_CHANNELS = 6;
	static const uint8_t PROTOCOL_COMMAND40 = 0x40; // Command is always 0x40

	static uint8_t state;
	static uint32_t last;
	static uint8_t buffer[PROTOCOL_LENGTH];
	static uint8_t ptr;
	static uint8_t len;
	static uint16_t channel[PROTOCOL_CHANNELS];
	static uint16_t chksum;
	static uint8_t lchksum;


	// Reference to the task created
	static xTaskHandle handle;

	// Internal semaphore to access buffers in a thread-safe way
	static SemaphoreHandle_t channelSemaphore;

	// The task code is here
	static void taskFunction();

	// Wrapper for the xTaskCreate parameter
	static void taskfun(void* parm) {
		static_cast<RCControllerTask *>(parm)->taskFunction();
	}

public:
	// Launch the RC listening thread
	// Returns the reference to the RC controller incoming messages buffer
	static void init();

	// get the current channel state
	static uint16_t getChannelState(uint8_t channedID);
};

#endif /* MAIN_RCCONTROLLERTASK_H_ */
