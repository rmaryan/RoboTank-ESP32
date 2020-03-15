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
#include "driver/uart.h"
#include "pin_mapping.h"

// task name for FreeRTOS
#define RC_TASK_NAME "RC_CONTROLLER"
// the UART number to be used for the RC
#define UART_NUM UART_NUM_1

void RCControllerTask::taskFunction() {
	while (1) {
		size_t bufferLength = 0;
		ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM, (size_t*)&bufferLength));

		if(bufferLength > 0) {

			uint32_t now = esp_timer_get_time() / 1000ULL;
			if (now - last >= PROTOCOL_TIMEGAP) {
				state = GET_LENGTH;
			}
			last = now;

			uint8_t v = 0;
			if(uart_read_bytes(UART_NUM, &v, 1, 100) != 1) continue;

			switch (state) {
			case GET_LENGTH:
				if (v <= PROTOCOL_LENGTH) {
					ptr = 0;
					len = v - PROTOCOL_OVERHEAD;
					chksum = 0xFFFF - v;
					state = GET_DATA;
				} else {
					state = DISCARD;
				}
				break;

			case GET_DATA:
				buffer[ptr++] = v;
				chksum -= v;
				if (ptr == len)	{
					state = GET_CHKSUML;
				}
				break;

			case GET_CHKSUML:
				lchksum = v;
				state = GET_CHKSUMH;
				break;

			case GET_CHKSUMH:
				// Validate checksum
				if (chksum == (v << 8) + lchksum) {
					// Execute command - we only know command 0x40
					switch (buffer[0])	{
					case PROTOCOL_COMMAND40:
						if( xSemaphoreTake(channelSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {
							// Valid - extract channel data
							for (uint8_t i = 1; i < PROTOCOL_CHANNELS * 2 + 1; i += 2) {
								channel[i / 2] = buffer[i] | (buffer[i + 1] << 8);
							}
							xSemaphoreGive(channelSemaphore);
						}
						break;

					default:
						break;
					}
				}
				state = DISCARD;
				break;

			case DISCARD:
			default:
				break;
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
	state = DISCARD;
	last = esp_timer_get_time() / 1000ULL;
	ptr = 0;
	len = 0;
	chksum = 0;
	lchksum = 0;

	// We'll use UART_NUM_1 for this channel
	ESP_ERROR_CHECK(uart_param_config(UART_NUM, &rcUartConfig));

	ESP_ERROR_CHECK(uart_set_pin(UART_NUM,
			0, // TX
			PIN_PIN_ESP32_RC_RX, // RX
			UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 2048, 0, 10, NULL, 0));

	// Creating binary semaphore to protect channel data list
	channelSemaphore = xSemaphoreCreateBinary();

	// Start the listener task
	xTaskCreate(taskfun, RC_TASK_NAME, configMINIMAL_STACK_SIZE, NULL, RC_CONTROLLER_PRIORITY, &handle);
}

uint16_t RCControllerTask::getChannelState(uint8_t channedID) {
	if (channedID < PROTOCOL_CHANNELS) {
		if( xSemaphoreTake(channelSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {
			uint16_t result = channel[channedID];
			xSemaphoreGive(channelSemaphore);
			return result;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}
