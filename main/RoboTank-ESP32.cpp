/*
 *  Tank robot with sensors and manipulators built on ESP32 microcontroller.
 *
 *  THE ENTRY POINT
 *
 *  Minimum ESP-IDF version required: 4.0
 *
 *  Copyright (c) 2020 Maryan Rachynskyy
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"

#include "OTAManager.h"
#include "pin_mapping.h"
#include "PWMBoardController.h"
#include "MotorL298NDriver.h"
#include "SoundModuleController.h"

static const char* LOG_TAG = "ROOT";

extern "C" {
void app_main();
}

void app_main()
{
	ESP_LOGI(LOG_TAG, "Starting the launch sequence...");
	ESP_LOGI(LOG_TAG, "Version: %s %s", __TIME__, __DATE__);

	// Initializing the OTA and web server subsystem
	OTAManager::init();

	// Hello Sound!
	SoundModuleController::init();
	SoundModuleController::playSound(7);

	PWMBoardController::init();
	while(true)	{
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
/*
	// the hello world dance
	MotorL298NDriver::go(160);
	PWMBoardController::setPinON(14,true);
	PWMBoardController::setPinON(15,false);
	vTaskDelay(2000 / portTICK_RATE_MS);
	MotorL298NDriver::go(-160);
	PWMBoardController::setPinON(15,true);
	PWMBoardController::setPinON(14,false);
	vTaskDelay(2000 / portTICK_RATE_MS);
	MotorL298NDriver::go(150, -150);
	PWMBoardController::setPinON(15,true);
	PWMBoardController::setPinON(14,true);
	vTaskDelay(2000 / portTICK_RATE_MS);
	*/
}
