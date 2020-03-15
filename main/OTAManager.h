/*
 * OTAManager.h
 *
 * OTA Manager takes care about all aspects of accepting Over The Air updates.
 *
 * This includes:
 * 1. Connection to the WiFi network as specified in the section
 *    "OTA WiFi Configuration" in make menuconfig.
 * 2. Launch of the on-board web server which serves the configuration web pages.
 * 3. Acceptance of the *.bin file with the new firmware.
 * 4. Updating the firmware and rebooting the robot.
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

#ifndef MAIN_OTAMANAGER_H_
#define MAIN_OTAMANAGER_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_http_server.h"

class OTAManager {
private:
	static EventGroupHandle_t reboot_event_group;
	static const int REBOOT_BIT = BIT0;

	static EventGroupHandle_t wifi_event_group;
	static const int WIFI_STA_CONNECTED_BIT = BIT0;
	static const int WIFI_STA_DISCONNECTED_BIT = BIT1;

	static httpd_handle_t server;
	static int8_t flash_status;

	// This task kicks-off when a new firmware arrived and loaded
	// (the function + the wrapper)
	static void systemRebootTaskFunction();
	static void systemRebootWrapper(void* parm) {
		static_cast<OTAManager *>(parm)->systemRebootTaskFunction();
	}

	// WiFi connection routines
	static void initWiFiStation();
	static void wifi_event_handler(void* arg, esp_event_base_t event_base,
            int32_t event_id, void* event_data);

	// HTTP Server
	static void httpStartServer();
	static void httpStopServer();
	// Sends index.html page
	static esp_err_t http_index_html_handler(httpd_req_t *req);
	// Sends favicon
	static esp_err_t http_favicon_png_handler(httpd_req_t *req);
	// Sends status information
	static esp_err_t http_update_status_handler(httpd_req_t *req);
	// Receives .bin file
	static esp_err_t http_update_post_handler(httpd_req_t *req);
	// URI handler structures
	static httpd_uri_t http_index_html_struct;
	static httpd_uri_t http_favicon_png_struct;
	static httpd_uri_t http_update_struct;
	static httpd_uri_t http_status_struct;

public:
	static void init();
};

#endif /* MAIN_OTAMANAGER_H_ */
