/*
 * SoundModuleController.cpp
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

#include "SoundModuleController.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "rtank_esp_log.h"
#include "pin_mapping.h"
#include "driver/gpio.h"
#include "RoboTankUtils.h"

static const char* LOG_TAG = LOG_TAG_SND;

// ============================================================
// Command types (must be visible outside the class)
// ============================================================
typedef enum {
	CMD_PLAY = 0,
	CMD_STOP,
	CMD_SET_VOLUME,
} command_type_t;

// ============================================================
// Command structure sent through the queue to the worker task
// ============================================================
typedef struct {
	command_type_t type;
	uint32_t param;   // track ID for CMD_PLAY, volume value for CMD_SET_VOLUME
} sound_command_t;

// Static FreeRTOS objects (module-private)
static QueueHandle_t s_cmdQueue = nullptr; ///< queue of sound_command_t items
static TaskHandle_t  s_workerTask = nullptr;


/* ------------------------------------------------------------------ */
// Low-level GPIO helpers -- ONLY the worker task should call these.
// They contain the timing-sensitive pulses required by the WTV020SD16P.
/* ------------------------------------------------------------------ */

void SoundModuleController::sendCommand(uint16_t command) {
	// Start bit: 0 level pulse.
	gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 0);
	delay_mks(1950);                           // start-bit low duration
	for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {
		// Clock 0 level pulse.
		gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 0);
		delay_mks(50);

		// Data setup on P05_DI.
		if (command & mask) {
			gpio_set_level(PIN_ESP32_SOUND_P05_DI, 1);
		} else {
			gpio_set_level(PIN_ESP32_SOUND_P05_DI, 0);
		}

		delay_mks(50);                         // data hold time
		gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 1);   // clock high pulse.
		delay_mks(100);                       // clock-high duration
	}
	vTaskDelay(pdMS_TO_TICKS(2));             // busy wait after last bit
}

void SoundModuleController::reset() {
	gpio_set_level(PIN_ESP32_SOUND_RESET, 0);
	delay_mks(5000);                          // reset low for 5 ms
	gpio_set_level(PIN_ESP32_SOUND_RESET,   1);// release reset (active-high)
}


/* ------------------------------------------------------------------ */
// Worker task -- processes queued commands sequentially with timing.
// ------------------------------------------------------------------ */

void SoundModuleController::soundWorkerTask(void* param) {
	sound_command_t cmd;

	ESP_LOGI(LOG_TAG, "Sound worker task started");

	for (;;) {
		if (xQueueReceive(s_cmdQueue, &cmd, portMAX_DELAY)) {
			switch (cmd.type) {
			case CMD_PLAY:
				reset();
				vTaskDelay(pdMS_TO_TICKS(500));  // wait for module to boot after reset before playing track
				sendCommand(cmd.param);
				break;

			case CMD_STOP:
				sendCommand(STOP);
				break;

			case CMD_SET_VOLUME: {
				uint8_t vol = (uint8_t)(cmd.param & 0xFFuL);    // extract volume value
				if (vol < 8) {
					sendCommand(VOLUME_MIN + vol);
				} else {
					ESP_LOGW(LOG_TAG, "Volume out of range: %d, clamping to 7", vol);
					sendCommand(VOLUME_MIN + 7);
				}
			} break;

			default:
				ESP_LOGE(LOG_TAG, "Unknown command type: %d", cmd.type);
				break;
			}
		}
	}
}


/* ------------------------------------------------------------------ */
// Public API -- these methods post commands to the queue and return.
// The worker task executes them with correct timing.
// ------------------------------------------------------------------ */

void SoundModuleController::init() {
	gpio_config_t io_conf = {};
	io_conf.intr_type    = GPIO_INTR_DISABLE;
	io_conf.mode         = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = ((1ULL << PIN_ESP32_SOUND_P04_CLK) |
	                        (1ULL << PIN_ESP32_SOUND_P05_DI) |
	                        (1ULL << PIN_ESP32_SOUND_RESET));
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 1);
	gpio_set_level(PIN_ESP32_SOUND_P05_DI, 1);
	gpio_set_level(PIN_ESP32_SOUND_RESET,   1);
	delay_ms(300);                            // wait for sound module to boot

	// Create the command queue (depth of 16 commands).
	s_cmdQueue = xQueueCreate(16, sizeof(sound_command_t));
	configASSERT(s_cmdQueue);

	xTaskCreate(&soundWorkerTask, "snd_wk", 2048, nullptr, tskIDLE_PRIORITY + 2, &s_workerTask);
	configASSERT(s_workerTask);

	// Pre-fill: set default volume immediately (queued).
	setVolume(7);

	ESP_LOGI(LOG_TAG, "Sound Module initialized");
}

void SoundModuleController::playSound(uint16_t trackID) {
	if (trackID >= 513u) { // WTV020SD16P supports tracks 0-512 only.
		ESP_LOGE(LOG_TAG, "Unknown track #%d", trackID);
		return;
	}

	sound_command_t cmd = {};
	cmd.type     = CMD_PLAY;
	cmd.param    = (uint32_t)trackID; // WTV020SD16P supports tracks 0-512 only.

	if (xQueueSend(s_cmdQueue, &cmd, portMAX_DELAY) != pdPASS) {
		ESP_LOGE(LOG_TAG, "Failed to queue play command");
	}
}

void SoundModuleController::stopSound() {
	sound_command_t cmd = {};
	cmd.type = CMD_STOP;

	if (xQueueSend(s_cmdQueue, &cmd, portMAX_DELAY) != pdPASS) {
		ESP_LOGE(LOG_TAG, "Failed to queue stop command");
	}
}

void SoundModuleController::setVolume(uint8_t volume) {
	sound_command_t cmd = {};
	cmd.type   = CMD_SET_VOLUME;
	cmd.param  = (uint32_t)volume & 0xFFuL;

	if (xQueueSend(s_cmdQueue, &cmd, portMAX_DELAY) != pdPASS) {
		ESP_LOGE(LOG_TAG, "Failed to queue volume command");
	}
}