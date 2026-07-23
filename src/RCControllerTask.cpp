/*
 * RCController.cpp
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

#include "RCControllerTask.h"

#include "rtank_esp_log.h"
static const char* LOG_TAG = LOG_TAG_RC;

#include "driver/uart.h"
#include "esp_timer.h"
#include "pin_mapping.h"
#include "RoboTankUtils.h"

// task parameters for FreeRTOS
#define RC_TASK_NAME "RC_CONTROLLER"
#define RC_CONTROLLER_PRIORITY 5
#define TASK_STACK_SIZE 2048

// the UART number to be used for the RC
#define RC_UART_NUM UART_NUM_1

uint8_t RCControllerTask::state;
RCControllerTask::BUFFER_UNION RCControllerTask::buffer;
uint8_t RCControllerTask::ptr;
volatile uint16_t RCControllerTask::channel[PROTOCOL_CHANNELS];
uint16_t RCControllerTask::chksum;
uint8_t RCControllerTask::deadZone;
int64_t RCControllerTask::lastValidPacketTimeMs;
TaskHandle_t RCControllerTask::handle = NULL;

void RCControllerTask::taskFunction() {
	while (1) {
		size_t bufferLength = 0;
		esp_err_t ret = ESP_OK;
		
		ret = uart_get_buffered_data_len(RC_UART_NUM, &bufferLength);

		if(ret != ESP_OK) {
			// some glitch in the UART interface
			ESP_LOGE(LOG_TAG, "RS UART failed");
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		
		if(bufferLength > 0) {
			uint8_t value = 0;
			if(uart_read_bytes(RC_UART_NUM, &value, 1, 0) > 0) {				
				switch (state) {
				case STATE_SEARCHING_FOR_LENGTH:
					if(value == PROTOCOL_LENGTH) {
						state = STATE_SEARCHING_FOR_COMMAND;
						chksum = 0xFFFF - PROTOCOL_LENGTH;
					}
					break;
				case STATE_SEARCHING_FOR_COMMAND:
					if(value == PROTOCOL_COMMAND40) {
						// promote to the next stage only if the command is correct
						state = STATE_FILLING_BUFFER;
						ptr = 0;
						chksum -= PROTOCOL_COMMAND40;
					} else {
						// otherwise - start searching for the next frame
						state = STATE_SEARCHING_FOR_LENGTH;
					}
					break;
				case STATE_FILLING_BUFFER:
					if(ptr < PROTOCOL_DATA_LENGTH) {
						// update checksum only with the channel bytes
						if(ptr < (PROTOCOL_DATA_LENGTH - 2)) {
							chksum -= value;
						}

						buffer.bytes[ptr++] = value;

						// we are at the end of the buffer
						if(ptr == PROTOCOL_DATA_LENGTH) {							
							processBuffer();
							state = STATE_SEARCHING_FOR_LENGTH;
						}
					} else {
						// something went wrong, resetting the state
						state = STATE_SEARCHING_FOR_LENGTH;
					}
				} // switch
			}
		} else {
			// the buffer is empty - let's wait a bit
			delay_ms(PROTOCOL_TIMEGAP);
		}
	}
}

void RCControllerTask::init(uint8_t deadZoneValue) {
	lastValidPacketTimeMs = 0;

	// Set the dead zone value
	deadZone = deadZoneValue;

	// Initialize the UART interface
	uart_config_t rcUartConfig = {};
	rcUartConfig.baud_rate = 115200;
	rcUartConfig.data_bits = UART_DATA_8_BITS;
	rcUartConfig.parity = UART_PARITY_DISABLE;
	rcUartConfig.stop_bits = UART_STOP_BITS_1;
	rcUartConfig.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	rcUartConfig.rx_flow_ctrl_thresh = 120;

	// Initialize the protocol state machine
	state = STATE_DISCONNECTED;
	ptr = 0;

	if(uart_param_config(RC_UART_NUM, &rcUartConfig)!=ESP_OK) {
		ESP_LOGE(LOG_TAG, "RC Module UART configuration failed");
		return;
	}

	if(uart_set_pin(RC_UART_NUM,
			UART_PIN_NO_CHANGE, // TX - not used
			PIN_ESP32_RC_RX, // RX
			UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)
			!=ESP_OK) {
		ESP_LOGE(LOG_TAG, "RC Module setting UART pin failed");
		return;
	}

	if(uart_driver_install(RC_UART_NUM, BUFFER_SIZE*2, 0, 10, NULL, 0)!=ESP_OK) {
		ESP_LOGE(LOG_TAG, "RC Module UART driver install failed");
		return;
	}

	state = STATE_SEARCHING_FOR_LENGTH;

	// Start the listener task
	xTaskCreate(taskfun, RC_TASK_NAME, TASK_STACK_SIZE, NULL, RC_CONTROLLER_PRIORITY, &handle);
	if(handle == NULL ) {
		ESP_LOGE(LOG_TAG, "RC Module task creation failed");
		state = STATE_DISCONNECTED;
		return;
	}

	ESP_LOGI(LOG_TAG, "RC Module initiated");
}

void RCControllerTask::processBuffer() {
	// update the channels only if CRC match	
	if(buffer.words[PROTOCOL_DATA_WORDS-1] == chksum) {
		for (uint8_t i = 0; i < PROTOCOL_CHANNELS; i++) {
			channel[i] = buffer.words[i];
		}
		lastValidPacketTimeMs = esp_timer_get_time() / 1000;
		// Ensure all writes to channel[] are visible before subsequent reads on other cores
		__sync_synchronize();
	}
}

uint16_t RCControllerTask::getChannelAnalogState(uint8_t channelID) {
	if ((channelID < PROTOCOL_CHANNELS) && (state != STATE_DISCONNECTED)) {

		bool isDataRecent = (esp_timer_get_time()/1000 - lastValidPacketTimeMs) < RC_TIMEOUT_MS; 
    	if (!isDataRecent) return SW_DISCONNECTED; 

		uint16_t val = channel[channelID];
		if(val <= RC_CHANNEL_UP) {
			return RC_CHANNEL_UP;
		} else if (val >= RC_CHANNEL_DOWN) {
			return RC_CHANNEL_DOWN;
		} else if((val > (RC_CHANNEL_MID - deadZone)) && (val < (RC_CHANNEL_MID + deadZone)))
			return RC_CHANNEL_MID;
		{
			// return the channel value
			return val;
		}
	} else {
		return 0;
	}
}

uint8_t RCControllerTask::getChannelDiscreteState(uint8_t channelID){
	if ((channelID < PROTOCOL_CHANNELS) && (state != STATE_DISCONNECTED)) {

		bool isDataRecent = (esp_timer_get_time()/1000 - lastValidPacketTimeMs) < RC_TIMEOUT_MS; 
    	if (!isDataRecent) return SW_DISCONNECTED; 

		uint16_t val = channel[channelID];
		if(val < (RC_CHANNEL_UP + RC_CHANNEL_HALF_STEP)) {
			return SW_UP; // UP position
		} else if (val > (RC_CHANNEL_DOWN - RC_CHANNEL_HALF_STEP)) {
			return SW_DOWN; // DOWN position
		} else {
			return SW_MID; // MID position
		}
	} else {
		return SW_DISCONNECTED; // disconnected state
	}
}
