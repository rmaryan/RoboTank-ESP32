/*
 * RCController.cpp
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

#include "RCControllerTask.h"

#include "esp_log.h"
static const char* LOG_TAG = "RC";

#include "driver/uart.h"
#include "pin_mapping.h"

// task parameters for FreeRTOS
#define RC_TASK_NAME "RC_CONTROLLER"
#define RC_CONTROLLER_PRIORITY 5
#define TASK_STACK_SIZE 2048

// the UART number to be used for the RC
#define UART_NUM UART_NUM_1

uint8_t RCControllerTask::state;
RCControllerTask::BUFFER_UNION RCControllerTask::buffer;
uint8_t RCControllerTask::ptr;
uint16_t RCControllerTask::channel[PROTOCOL_CHANNELS];
uint16_t RCControllerTask::chksum;
xTaskHandle RCControllerTask::handle = NULL;

void RCControllerTask::taskFunction() {
	while (1) {
		size_t bufferLength = 0;
		ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM, &bufferLength));

		if(bufferLength > 0) {
			uint8_t value = 0;
			if(uart_read_bytes(UART_NUM, &value, 1, 0) > 0) {
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
			vTaskDelay(PROTOCOL_TIMEGAP / portTICK_RATE_MS);
		}
	}
}

void RCControllerTask::init() {
	// Initialize the UART interface
	uart_config_t rcUartConfig;
	rcUartConfig.baud_rate = 115200;
	rcUartConfig.data_bits = UART_DATA_8_BITS;
	rcUartConfig.parity = UART_PARITY_DISABLE;
	rcUartConfig.stop_bits = UART_STOP_BITS_1;
	rcUartConfig.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	rcUartConfig.rx_flow_ctrl_thresh = 120;

	// Initialize the protocol state machine
	state = STATE_DISCONNECTED;
	ptr = 0;

	if(uart_param_config(UART_NUM, &rcUartConfig)!=ESP_OK) {
		ESP_LOGE(LOG_TAG, "RC Module UART configuration failed");
		return;
	}

	if(uart_set_pin(UART_NUM,
			0, // TX
			PIN_PIN_ESP32_RC_RX, // RX
			UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)
			!=ESP_OK) {
		ESP_LOGE(LOG_TAG, "RC Module setting UART pin failed");
		return;
	}

	if(uart_driver_install(UART_NUM, BUFFER_SIZE*2, 0, 10, NULL, 0)!=ESP_OK) {
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
	}
}

uint16_t RCControllerTask::getChannelState(uint8_t channedID) {
	if ((channedID < PROTOCOL_CHANNELS) && (state != STATE_DISCONNECTED)) {
		return channel[channedID];
	} else {
		return 0;
	}
}
