/*
 * SensorsController.cpp
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

#include "SensorsController.h"

#include <math.h>

#include "rtank_esp_log.h"
static const char *LOG_TAG = LOG_TAG_SENS;

#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

#include "pin_mapping.h"
#include "RoboTankUtils.h"
#include "freertos/queue.h"

#define SENSORS_REFRESH_RATE_MS 1000 /* Sensors refresh rate in ms */

#define RMT_CLK_DIV 100 /* RMT counter clock divider */
#define RMT_RESOLUTION_HZ 1000000   // 1 MHz resolution, 1 tick = 1 us
#define RMT_ITEM32_TIMEOUT_US 9500 /* RMT receiver timeout value(us) */
#define RMT_TICK_10_US (80000000 / RMT_CLK_DIV / 100000) /* RMT counter value for 10 us.(Source clock is APB clock) */
#define ITEM_DURATION(d) ((d & 0x7fff)*10/RMT_TICK_10_US)

// task definitions for FreeRTOS
#define SENS_TASK_NAME "SENS_TASK"
#define SENS_TASK_PRIORITY 4
#define TASK_STACK_SIZE 2048

bool SensorsController::usInitialized = false;
uint32_t SensorsController::lastUSEchoDuration = 0;
rmt_channel_handle_t SensorsController::tx_channel = NULL;
rmt_channel_handle_t SensorsController::rx_channel = NULL;
rmt_encoder_handle_t SensorsController::hcsr04_trig_encoder = NULL;
const rmt_symbol_word_t SensorsController::trig_symbol = {
        .duration0 = 10, // 10 us pulse
        .level0 = 1,
        .duration1 = 0,  // No second pulse, ends after first duration
        .level1 = 0,
};

QueueHandle_t SensorsController::receive_queue = NULL;

TaskHandle_t SensorsController::handle;

void SensorsController::init() {
	// Initialize GPIO IR pins the same way
	gpio_config_t io_conf = {};
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = ((1ULL << PIN_ESP32_IR_FL)
			| (1ULL << PIN_ESP32_IR_FR) | (1ULL << PIN_ESP32_IR_DL)
			| (1ULL << PIN_ESP32_IR_DR));
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
    ESP_LOGI(LOG_TAG, "Init RMT rx");
    rmt_rx_channel_config_t rx_config = {
        .gpio_num = PIN_ESP32_US_ECHO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .flags = 0,
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_config, &rx_channel));

    receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue); 
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = rx_data_ready_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

    ESP_ERROR_CHECK(rmt_enable(rx_channel));

    //--------------------------------------------------------------------------------------

    ESP_LOGI(LOG_TAG, "Init RMT tx");

    rmt_tx_channel_config_t tx_config = {
        .gpio_num = PIN_ESP32_US_TRIG,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4, 
        .flags = 0,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &tx_channel));
    ESP_ERROR_CHECK(rmt_enable(tx_channel));

    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_encoder_config, &hcsr04_trig_encoder));

    usInitialized = true;
}

bool SensorsController::rx_data_ready_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    // return whether any task is woken up
    return high_task_wakeup == pdTRUE;
}

void SensorsController::sensorsTaskFunction() {
    ESP_LOGI(LOG_TAG, "Sensors task start");

    while (true) {
        // Send the trigger signal
        rmt_transmit_config_t tx_cfg = {
            .loop_count = 0,
        };
        ESP_ERROR_CHECK(rmt_transmit(tx_channel, hcsr04_trig_encoder, &trig_symbol, sizeof(trig_symbol), &tx_cfg));
        ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_channel, portMAX_DELAY));

        // Receive echo
        rmt_receive_config_t rx_cfg = {
            .signal_range_min_ns = 500,
            .signal_range_max_ns = RMT_ITEM32_TIMEOUT_US * 1000, 
        };        
        rmt_symbol_word_t rx_symbols[10];
        rmt_rx_done_event_data_t rx_data;

        esp_err_t ret = rmt_receive(rx_channel, rx_symbols, sizeof(rx_symbols), &rx_cfg);
        if (ret == ESP_OK) {
            while(1)
            {
                // wait for RX done signal
                if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)
                {
                    lastUSEchoDuration = rx_data.received_symbols[0].duration0;
                    break;
                }
            }
        }
        else
        {
            lastUSEchoDuration = 0;
        }
        delay_ms(SENSORS_REFRESH_RATE_MS);
    }
}

void SensorsController::getSensorsState(SensorsStateStruct &s) {
	// Read the IR sensors state directly
	s.ir_fl = gpio_get_level(PIN_ESP32_IR_FL);
	s.ir_fr = gpio_get_level(PIN_ESP32_IR_FR);
	s.ir_dl = gpio_get_level(PIN_ESP32_IR_DL);
	s.ir_dr = gpio_get_level(PIN_ESP32_IR_DR);

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
