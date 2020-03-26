/*
 * SoundModuleController.cpp
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

#include "SoundModuleController.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pin_mapping.h"
#include <driver/gpio.h>

static const char* LOG_TAG = "SND";

#define NOP() asm volatile ("nop")

void SoundModuleController::sendCommand(uint16_t command) {
	//Start bit 0 level pulse.
	gpio_set_level(PIN_PIN_ESP32_SOUND_P04_CLK, 0);
	// Wait Start bit length minus 50 us
	delayMicros(2000);
	for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {
		//Clock 0 level pulse.
		gpio_set_level(PIN_PIN_ESP32_SOUND_P04_CLK, 0);
		delayMicros(50);
		//Write data setup.
		if (command & mask) {
			gpio_set_level(PIN_PIN_ESP32_SOUND_P05_DI, 1);
		} else {
			gpio_set_level(PIN_PIN_ESP32_SOUND_P05_DI, 0);
		}
		//Write data hold.
		delayMicros(50);
		//Clock 1 level pulse.
		gpio_set_level(PIN_PIN_ESP32_SOUND_P04_CLK, 1);
		delayMicros(100);
		if (mask>0x0001){
			//Stop bit high level pulse.
			delayMicros(2000);
		}
	}
	//Busy active high from last data bit latch.
	vTaskDelay(20 / portTICK_RATE_MS);
}

void SoundModuleController::init() {
	gpio_pad_select_gpio(PIN_PIN_ESP32_SOUND_P04_CLK);
	gpio_pad_select_gpio(PIN_PIN_ESP32_SOUND_P05_DI);
	gpio_set_direction(PIN_PIN_ESP32_SOUND_P04_CLK, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_PIN_ESP32_SOUND_P05_DI, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_PIN_ESP32_SOUND_P04_CLK, 1);
	gpio_set_level(PIN_PIN_ESP32_SOUND_P05_DI, 1);
	vTaskDelay(300 / portTICK_RATE_MS);
	ESP_LOGI(LOG_TAG, "Sound Module kicked-off");
}

void SoundModuleController::playSound(uint16_t trackID) {
	if((trackID >0) && (trackID < 513)) {
		ESP_LOGI(LOG_TAG, "Playing sound %d", trackID);
		SoundModuleController::sendCommand(trackID);
	} else {
		ESP_LOGE(LOG_TAG, "Unknown track # %d", trackID);
	}
}

void SoundModuleController::stopSound() {
	SoundModuleController::sendCommand(STOP);
}

void SoundModuleController::setVolume(uint8_t volume) {
	if(volume<8) {
		SoundModuleController::sendCommand(VOLUME_MIN + volume);
	}
}

void SoundModuleController::delayMicros(uint32_t us) {\
    uint32_t m = esp_timer_get_time();
    if(us){
        uint32_t e = (m + us);
        if(m > e){ //overflow
            while(esp_timer_get_time() > e){
                NOP();
            }
        }
        while(esp_timer_get_time() < e){
            NOP();
        }
    }
}
