/*
 * SensorsController.cpp
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

#include "SensorsController.h"

#include <math.h>

#include "esp_log.h"
static const char *LOG_TAG = "SENS";

#include "driver/gpio.h"
#include "driver/rmt.h"

#include "pin_mapping.h"
#include "RoboTankUtils.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_3 /* RMT channel for transmitter - pay attention channels 0 and 1 are used by LED module */
#define RMT_RX_CHANNEL RMT_CHANNEL_2 /* RMT channel for receiver */
#define RMT_CLK_DIV 100 /* RMT counter clock divider */
#define RMT_TX_CARRIER_EN 0 /* Disable carrier */
#define RMT_ITEM32_TIMEOUT_US 9500 /* RMT receiver timeout value(us) */
#define RMT_TICK_10_US (80000000/RMT_CLK_DIV/100000) /* RMT counter value for 10 us.(Source clock is APB clock) */
#define ITEM_DURATION(d) ((d & 0x7fff)*10/RMT_TICK_10_US)

// task definitions for FreeRTOS
#define SENS_TASK_NAME "SENS_TASK"
#define SENS_TASK_PRIORITY 4
#define TASK_STACK_SIZE 2048

bool SensorsController::usInitialized = false;
uint32_t SensorsController::lastUSEchoDuration = 0;
rmt_item32_t SensorsController::triggerRMTItem;
xTaskHandle SensorsController::handle;

void SensorsController::init() {
	// Initialize GPIO IR pins the same way
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = ((1ULL << PIN_PIN_ESP32_IR_FL)
			| (1ULL << PIN_PIN_ESP32_IR_FR) | (1ULL << PIN_PIN_ESP32_IR_DL)
			| (1ULL << PIN_PIN_ESP32_IR_DR));
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	gpio_config(&io_conf);

	// Initialize the RMT module to handle the Ultrasonic distance sensor
	initUS_RMT();

	// Start the sensors polling task
	if (usInitialized) {
		xTaskCreate(sensorsTaskfun, SENS_TASK_NAME, TASK_STACK_SIZE, NULL,
		SENS_TASK_PRIORITY, &handle);
		if (handle == NULL) {
			usInitialized = false;
			ESP_LOGE(LOG_TAG, "Sensors task creation failed");
		}
	}

	ESP_LOGI(LOG_TAG, "Sensors initiated");
}

void SensorsController::initUS_RMT() {
	// The receiving part
	ESP_LOGI(LOG_TAG, "Init RMT rx");
	rmt_config_t rmt_rx;
	rmt_rx.channel = RMT_RX_CHANNEL;
	rmt_rx.gpio_num = PIN_PIN_ESP32_US_ECHO;
	rmt_rx.clk_div = RMT_CLK_DIV;
	rmt_rx.mem_block_num = 1;
	rmt_rx.rmt_mode = RMT_MODE_RX;
	rmt_rx.rx_config.filter_en = true;
	rmt_rx.rx_config.filter_ticks_thresh = 100;
	rmt_rx.rx_config.idle_threshold = RMT_ITEM32_TIMEOUT_US / 10
			* (RMT_TICK_10_US);
	if (rmt_config(&rmt_rx) != ESP_OK) {
		ESP_LOGE(LOG_TAG, "US Sensor RX configuration failed");
		return;
	}
	if (rmt_driver_install(rmt_rx.channel, 1000, 0) != ESP_OK) {
		ESP_LOGE(LOG_TAG, "US Sensor RX driver failed");
		return;
	}

	// The transmitting part
	ESP_LOGI(LOG_TAG, "Init RMT tx");
	GPIO_IS_VALID_OUTPUT_GPIO(PIN_PIN_ESP32_US_TRIG);
	rmt_config_t rmt_tx;
	rmt_tx.channel = RMT_TX_CHANNEL;
	rmt_tx.gpio_num = PIN_PIN_ESP32_US_TRIG;
	rmt_tx.mem_block_num = 1;
	rmt_tx.clk_div = RMT_CLK_DIV;
	rmt_tx.tx_config.loop_en = false;
	rmt_tx.tx_config.carrier_duty_percent = 50;
	rmt_tx.tx_config.carrier_freq_hz = 3000;
	rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
	rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
	rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
	rmt_tx.tx_config.idle_output_en = true;
	rmt_tx.rmt_mode = RMT_MODE_TX;
	if (rmt_config(&rmt_tx) != ESP_OK) {
		ESP_LOGE(LOG_TAG, "US Sensor TX configuration failed");
		return;
	}
	if (rmt_driver_install(rmt_tx.channel, 0, 0) != ESP_OK) {
		ESP_LOGE(LOG_TAG, "US Sensor TX driver failed");
		return;
	}

	// prepare the trigger signal definition
	triggerRMTItem.level0 = 1;
	triggerRMTItem.duration0 = RMT_TICK_10_US;
	triggerRMTItem.level1 = 0;
	triggerRMTItem.duration1 = RMT_TICK_10_US;

	usInitialized = true;
}

void SensorsController::sensorsTaskFunction() {
	ESP_LOGI(LOG_TAG, "Sensors task start");
	// complete the echo receiving preparations
	size_t rx_size = 0;
	RingbufHandle_t rb = NULL;
	rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
	rmt_rx_start(RMT_RX_CHANNEL, true);
	ESP_LOGI(LOG_TAG, "Sensors task engaging");
	while (true) {
		rmt_item32_t *rxItem;
		// wipe all items in the buffer (most probably - noise)
		while(true) {
			rxItem = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 0); // no wait
			if(rxItem != NULL) {
				vRingbufferReturnItem(rb, (void*) rxItem);
			} else {
				break;
			}
		}

		// send the trigger signal
		if (rmt_write_items(RMT_TX_CHANNEL, &triggerRMTItem, 1, true) != ESP_OK) {
			ESP_LOGE(LOG_TAG, "rmt_write_items failed");
			continue;
		}
		// wait for the sending completion
		if (rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY) != ESP_OK) {
			ESP_LOGE(LOG_TAG, "rmt_wait_tx_done failed");
			continue;
		}

		rxItem = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size,
				pdMS_TO_TICKS(200));

		if (rxItem != NULL) {
			lastUSEchoDuration = rxItem->duration0;
			vRingbufferReturnItem(rb, (void*) rxItem);
		} else {
			lastUSEchoDuration = 0;
		}

		// refresh the US sensor data every second
		delay_ms(1000);
	}
}

void SensorsController::getSensorsState(SensorsStateStruct &s) {
	// Read the IR sensors state directly
	s.ir_fl = gpio_get_level(PIN_PIN_ESP32_IR_FL);
	s.ir_fr = gpio_get_level(PIN_PIN_ESP32_IR_FR);
	s.ir_dl = gpio_get_level(PIN_PIN_ESP32_IR_DL);
	s.ir_dr = gpio_get_level(PIN_PIN_ESP32_IR_DR);

	// get the last known US sensor measure
	// we can't get it instantly as it might require
	s.us_forward =
			usInitialized ?
					round(
							340.29 * ITEM_DURATION(lastUSEchoDuration)
									/ (10 * 1000 * 2)) // distance in cm
									:
					0;
}
