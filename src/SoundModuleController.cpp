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
#include "esp_log.h"
#include "pin_mapping.h"
#include "driver/gpio.h"
#include "RoboTankUtils.h"

static const char* LOG_TAG = "SND";

void SoundModuleController::sendCommand(uint16_t command) {
	//Start bit 0 level pulse.
	gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 0);
	// Wait Start bit length minus 50 us
	delayMicros(1950);
	for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {
		//Clock 0 level pulse.
		gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 0);
		delayMicros(50);
		//Write data setup.
		if (command & mask) {
			gpio_set_level(PIN_ESP32_SOUND_P05_DI, 1);
		} else {
			gpio_set_level(PIN_ESP32_SOUND_P05_DI, 0);
		}
		//Write data hold.
		delayMicros(50);
		//Clock 1 level pulse.
		gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 1);
		delayMicros(100);
		// if (mask>0x0001){
		// 	//Stop bit high level pulse.
		// 	delayMicros(2000);
		// }
	}
	//Busy active high from last data bit latch.
	delayMicros(1900);
}

void SoundModuleController::reset() {
	gpio_set_level(PIN_ESP32_SOUND_RESET, 0);
	delayMicros(5000);
	gpio_set_level(PIN_ESP32_SOUND_RESET, 1);
	delayMicros(5000);
}

void SoundModuleController::init() {
	// Initialize the GPIO pins for the sound module
	gpio_config_t io_conf = {};
	//disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set
	io_conf.pin_bit_mask = ((1ULL << PIN_ESP32_SOUND_P04_CLK) | (1ULL << PIN_ESP32_SOUND_P05_DI) | (1ULL << PIN_ESP32_SOUND_RESET));
	//disable pull-down mode
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	//enable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
	// Set the initial state of the pins
	gpio_set_level(PIN_ESP32_SOUND_P04_CLK, 1);
	gpio_set_level(PIN_ESP32_SOUND_P05_DI, 1);
	gpio_set_level(PIN_ESP32_SOUND_RESET, 1);
	vTaskDelay(300 / portTICK_PERIOD_MS); // wait for the sound module to boot up
	reset();
	SoundModuleController::setVolume(7);
	ESP_LOGI(LOG_TAG, "Sound Module kicked-off");
}

void SoundModuleController::playSound(uint16_t trackID) {
	if(trackID < 513) {		
		reset();
		delayMicros(500000);
		sendCommand(trackID);
	} else {
		ESP_LOGE(LOG_TAG, "Unknown track # %d", trackID);
	}
}

void SoundModuleController::stopSound() {
	sendCommand(STOP);
}

void SoundModuleController::setVolume(uint8_t volume) {
	if(volume<8) {
		sendCommand(VOLUME_MIN + volume);
	}
}

