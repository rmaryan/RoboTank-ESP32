/*
 * SensorsController.h
 *
 * The sensors controller class.
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

#ifndef MAIN_SENSORSCONTROLLER_H_
#define MAIN_SENSORSCONTROLLER_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "rtank_esp_log.h"

struct SensorsStateStruct {
	// Infrared obstacle sensors: forward-right, forward-left, down-right, down-left
	// 1 - sensor was triggered, 0 - not triggered
	uint8_t ir_fr :1, ir_fl :1, ir_dr :1, ir_dl :1;
	// Ultrasonic sensor
	// Distance in cm
	// 0 - no data
	uint16_t us_forward;
};

class SensorsController {
private:
	// The ultrasonic sensor initialization state
	static bool usInitialized;

	// The last known ultrasonic sensor echo duration
	static uint32_t lastUSEchoDuration;
	static rmt_channel_handle_t tx_channel;
	static rmt_channel_handle_t rx_channel;
	
	static rmt_encoder_handle_t hcsr04_trig_encoder;
    IRAM_ATTR static const rmt_symbol_word_t trig_symbol;

	static QueueHandle_t receive_queue;

	// Reference to the task created
	static TaskHandle_t handle;

	// The RMT RX callback
	static IRAM_ATTR bool rx_data_ready_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data);

	// The task code is here
	static void sensorsTaskFunction();

	// Wrapper for the xTaskCreate parameter
	static void sensorsTaskfun(void* parm) {
		static_cast<SensorsController *>(parm)->sensorsTaskFunction();
	}

	// initialize the RMT module for the ultrasonic signals processing
	static void initUS_RMT();
public:
	// Initialize the sensors
	static void init();

	// get sensors state
	static void getSensorsState(SensorsStateStruct& s);
};

#endif /* MAIN_SENSORSCONTROLLER_H_ */
