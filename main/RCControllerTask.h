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
		STATE_DISCONNECTED,
		STATE_SEARCHING_FOR_LENGTH,
		STATE_SEARCHING_FOR_COMMAND,
		STATE_FILLING_BUFFER,
	};

	static const uint8_t PROTOCOL_LENGTH = 0x20;    // The pack length, also, the first byte in the pack
	static const uint8_t PROTOCOL_COMMAND40 = 0x40; // Command is always 0x40
	static const uint8_t PROTOCOL_DATA_LENGTH = PROTOCOL_LENGTH - 2; // length of the protocol without the first 2 bytes header
	static const uint8_t PROTOCOL_DATA_WORDS = PROTOCOL_DATA_LENGTH/2; // 2-byte words cound in the protocol data
	static const uint8_t PROTOCOL_TIMEGAP = 3; // Packets are received every ~7ms so use ~half that for the gap
	static const uint8_t PROTOCOL_CHANNELS = 10;
	static const uint16_t BUFFER_SIZE = 256;  // Size for the UART reading buffer
	static const uint16_t TASK_STACK_SIZE = 2048;  // Size for the task

	union BUFFER_UNION {
		uint8_t  bytes[PROTOCOL_DATA_LENGTH];
		uint16_t words[PROTOCOL_DATA_WORDS];
	};
	static BUFFER_UNION buffer;

	static uint8_t state;
	// protocol buffer, excludes the leading 0x20 and 0x10
	static uint8_t ptr;
	static uint16_t channel[PROTOCOL_CHANNELS];
	static uint16_t chksum;

	// Reference to the task created
	static xTaskHandle handle;

	// The task code is here
	static void taskFunction();

	// Wrapper for the xTaskCreate parameter
	static void taskfun(void* parm) {
		static_cast<RCControllerTask *>(parm)->taskFunction();
	}

	// this method processes the content of the buffer
	// if the checksum is OK - the channels data is populated with the parsed values
	static void processBuffer();
public:
	// Launch the RC listening thread
	static void init();

	// Get the current channel state
	// Returns 0 on any error or disconnected state
	static uint16_t getChannelState(uint8_t channedID);
};

#endif /* MAIN_RCCONTROLLERTASK_H_ */
