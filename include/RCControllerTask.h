/*
 * RCController.h
 *
 * A class which manages the Radio Controller commands handling.
 *
 * Core code was borrowed from here: https://gitlab.com/timwilkinson/FlySkyIBus/-/tree/master
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

#ifndef MAIN_RCCONTROLLERTASK_H_
#define MAIN_RCCONTROLLERTASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define RC_CHANNEL_UP 1000
#define RC_CHANNEL_MID 1500
#define RC_CHANNEL_DOWN 2000
#define RC_CHANNEL_HALF_STEP 250

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
	static const uint8_t PROTOCOL_CHANNELS = 10; // Number of channels in the protocol
	static const uint16_t BUFFER_SIZE = 256;  // Size for the UART reading buffer

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

	static uint8_t deadZone;

	// Reference to the task created
	static TaskHandle_t handle;

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

	enum SwitchState {
		SW_DISCONNECTED,
		SW_UP,
		SW_MID,
		SW_DOWN
	};

	// RC control channels mapping
	static const uint8_t RC_RG_H = 0; // Right gimbal horizontal axis
	static const uint8_t RC_RG_V = 1; // Right gimbal vertical axis
	static const uint8_t RC_LG_H = 2; // Left gimbal horizontal axis
	static const uint8_t RC_LG_V = 3; // Left gimbal vertical axis
	static const uint8_t RC_SWA = 4;
	static const uint8_t RC_SWB = 5;
	static const uint8_t RC_SWC = 6;
	static const uint8_t RC_SWD = 7;
	static const uint8_t RC_SWE = 8;
	static const uint8_t RC_SWF = 9;

	// Launch the RC listening thread
	static void init(uint8_t deadZoneValue = 20);

    // Get the current channel state
	// Returns 0 on any error or disconnected state
	static uint16_t getChannelAnalogState(uint8_t channedID);

	// Get the switch position
	// Returns SW_DISCONNECTED if the channel is not connected
	// Returns SW_UP, SW_MID or SW_DOWN if the channel is connected
	static uint8_t getChannelDiscreteState(uint8_t channedID);

};

#endif /* MAIN_RCCONTROLLERTASK_H_ */
