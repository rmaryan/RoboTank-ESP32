/*
 * RoboTankUtils.cpp
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
 */

#include "RoboTankUtils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "rtank_esp_log.h"

#define NOP() asm volatile ("nop")

// Maps a number from one range to another
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Delays for a specified number of milliseconds
void delay_ms(uint32_t ms) {
    if (ms != 0) {
        vTaskDelay(ms / portTICK_PERIOD_MS);
    }
}

// Delays for a specified number of microseconds
void delay_mks(uint32_t us) {
    if(us){
		uint64_t m = esp_timer_get_time();
		uint64_t e = (m + us);
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

// Initialize the logging subsystem with default log levels
void rtank_esp_log_init()
{
    // Set the default log level for all modules to INFO
    // Use CONFIG_LOG_DEFAULT_LEVEL for default level from sdkconfig
    esp_log_level_set(LOG_TAG_AI, ESP_LOG_INFO);
    esp_log_level_set(LOG_TAG_ARM, ESP_LOG_INFO);
    esp_log_level_set(LOG_TAG_BLOG, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_LED, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_MTR, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_OTA, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_PWM, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_RC, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_ROOT, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_SENS, ESP_LOG_WARN);
    esp_log_level_set(LOG_TAG_SND, ESP_LOG_INFO);
}