/*
 * OTAManager.cpp
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

#include "OTAManager.h"

#include <sys/param.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"

#define REBOOT_TASK_NAME "REBOOT_TASK"
#define RECONNECT_RETRY_MAX 10

// Embedded Files. To add or remove make changes is component.mk file as well.
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");
extern const uint8_t favicon_png_start[] asm("_binary_favicon_png_start");
extern const uint8_t favicon_png_end[]   asm("_binary_favicon_png_end");

static const char* LOG_TAG = "OTA";

EventGroupHandle_t OTAManager::reboot_event_group;
uint8_t OTAManager::s_retry_num;
httpd_handle_t OTAManager::server;
int8_t OTAManager::flash_status;
httpd_uri_t OTAManager::http_index_html_struct  = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = http_index_html_handler,
		.user_ctx = NULL
};
httpd_uri_t OTAManager::http_favicon_png_struct = {
		.uri = "/favicon.png",
		.method = HTTP_GET,
		.handler = http_favicon_png_handler,
		.user_ctx = NULL
};
httpd_uri_t OTAManager::http_update_struct = {
		.uri = "/update",
		.method = HTTP_POST,
		.handler = http_update_post_handler,
		.user_ctx = NULL
};
httpd_uri_t OTAManager::http_status_struct = {
		.uri = "/status",
		.method = HTTP_POST,
		.handler = http_update_status_handler,
		.user_ctx = NULL
};

void OTAManager::init() {
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	esp_log_level_set("wifi", ESP_LOG_NONE);     // disable wifi driver logging

	// Need this task to spin up, see why in task
	xTaskCreate(systemRebootWrapper, REBOOT_TASK_NAME, 2048, NULL, 5, NULL);

	// Initialize the WiFi connection and HTTP server
	server = NULL;
	flash_status = 0;
	s_retry_num = 0;
	initWiFiStation();
}

void OTAManager::systemRebootTaskFunction()
{
	// Initialize the event group
	reboot_event_group = xEventGroupCreate();

	// Clear the bit
	xEventGroupClearBits(reboot_event_group, REBOOT_BIT);


	for (;;)
	{
		// Wait here until the bit gets set for reboot
		EventBits_t staBits = xEventGroupWaitBits(reboot_event_group, REBOOT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

		// Did portMAX_DELAY ever timeout, not sure so lets just check to be sure
		if ((staBits & REBOOT_BIT) != 0)
		{
			ESP_LOGI(LOG_TAG, "Restarting...");
			vTaskDelay(2000 / portTICK_PERIOD_MS);

			esp_restart();
		}
	}
}

void OTAManager::initWiFiStation() {
	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	cfg.nvs_enable = 0;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

	wifi_config_t wifi_config = {};
	memcpy ( wifi_config.sta.ssid, CONFIG_STATION_SSID, sizeof(CONFIG_STATION_SSID) );
	memcpy ( wifi_config.sta.password, CONFIG_STATION_PASSPHRASE, sizeof(CONFIG_STATION_PASSPHRASE) );

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(LOG_TAG, "WiFi kicked-off");
}

void OTAManager::wifi_event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) {

	if(event_base == WIFI_EVENT) {
		switch (event_id) {
		case WIFI_EVENT_STA_START:			/**< ESP32 station start */
			ESP_LOGI(LOG_TAG, "WIFI_EVENT_STA_START");
			esp_wifi_connect();
			ESP_LOGI(LOG_TAG, "Connecting to SSID: %s | %s", CONFIG_STATION_SSID, CONFIG_STATION_PASSPHRASE);
			break;

		case WIFI_EVENT_STA_DISCONNECTED:		/**< ESP32 station disconnected from AP */
			ESP_LOGI(LOG_TAG, "WIFI_EVENT_STA_DISCONNECTED");
			if (s_retry_num < RECONNECT_RETRY_MAX) {
				esp_wifi_connect();
				s_retry_num++;
				ESP_LOGI(LOG_TAG, "Trying to reconnect...");
			} else {
				ESP_LOGE(LOG_TAG, "STA Connection Failed");
				/* Stop the web server */
				httpStopServer();
			}
			break;
		}
	} else
		if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
			/**< ESP32 station got IP from connected AP */
			ESP_LOGI(LOG_TAG, "IP_EVENT_STA_GOT_IP");
			s_retry_num = 0;
			/* Start the web server */
			httpStartServer();
		}
}

void OTAManager::httpStartServer() {
	if (server == NULL) {
		ESP_LOGI(LOG_TAG, "Starting HTTP Server...");
		httpd_config_t config = HTTPD_DEFAULT_CONFIG();
		config.stack_size = 8192; // (default is 4096)

		// Start the httpd server
		if (httpd_start(&server, &config) == ESP_OK) {
			ESP_LOGI(LOG_TAG, "Establishing URI Handlers");
			// Set URI handlers
			httpd_register_uri_handler(server, &http_index_html_struct);
			httpd_register_uri_handler(server, &http_favicon_png_struct);
			httpd_register_uri_handler(server, &http_update_struct);
			httpd_register_uri_handler(server, &http_status_struct);
		} else {
			ESP_LOGE(LOG_TAG, "HTTP Server start failed");
		}
	}
}

void OTAManager::httpStopServer() {
	if (server!=NULL) {
		ESP_LOGI(LOG_TAG, "Stopping HTTP Server");
		httpd_stop(server);
		server = NULL;
	}
}

esp_err_t OTAManager::http_index_html_handler(httpd_req_t *req) {
	// Clear this every time page is requested
	flash_status = 0;
	ESP_LOGI(LOG_TAG, "Request received: index.html");
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}

esp_err_t OTAManager::http_favicon_png_handler(httpd_req_t *req) {
	ESP_LOGI(LOG_TAG, "Request received: favicon.png");
	httpd_resp_set_type(req, "image/png");
	httpd_resp_send(req, (const char *)favicon_png_start, favicon_png_end - favicon_png_start);
	return ESP_OK;
}

esp_err_t OTAManager::http_update_status_handler(httpd_req_t *req) {
	ESP_LOGI(LOG_TAG, "Request received: update status");

	char ledJSON[100];

	sprintf(ledJSON, "{\"status\":%d,\"compile_time\":\"%s\",\"compile_date\":\"%s\"}", flash_status, __TIME__, __DATE__);
	httpd_resp_set_type(req, "application/json");
	httpd_resp_send(req, ledJSON, strlen(ledJSON));

	// This gets set when upload is complete
	if (flash_status == 1) {
		// We cannot directly call reboot here because we need the
		// browser to get the ack back.
		ESP_LOGI(LOG_TAG, "Sending signal to restart");
		xEventGroupSetBits(reboot_event_group, REBOOT_BIT);
	}

	return ESP_OK;
}

esp_err_t OTAManager::http_update_post_handler(httpd_req_t *req) {
	esp_ota_handle_t ota_handle;

	char ota_buff[1024];
	int content_length = req->content_len;
	int content_received = 0;
	int recv_len;
	bool is_req_body_started = false;
	const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

	ESP_LOGI(LOG_TAG, "HTTP POST Processing");

	// Unsuccessful Flashing
	flash_status = -1;

	do {
		/* Read the data for the request */
		if ((recv_len = httpd_req_recv(req, ota_buff, MIN(content_length, sizeof(ota_buff)))) < 0) {
			if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
				/* Retry receiving if timeout occurred */
				continue;
			}
			ESP_LOGE(LOG_TAG, "HTTP POST Receiving data failed");
			return ESP_FAIL;
		}

		// Is this the first data we are receiving
		// If so, it will have the information in the header we need.
		if (!is_req_body_started) {
			is_req_body_started = true;

			// Lets find out where the actual data starts after the header info
			char *body_start_p = strstr(ota_buff, "\r\n\r\n") + 4;
			int body_part_len = recv_len - (body_start_p - ota_buff);

			esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
			if (err != ESP_OK)
			{
				ESP_LOGE(LOG_TAG, "HTTP POST Error With OTA Begin, Cancelling OTA");
				return ESP_FAIL;
			}
			else {
				ESP_LOGI(LOG_TAG, "HTTP POST Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
			}

			// Lets write this first part of data out
			esp_ota_write(ota_handle, body_start_p, body_part_len);
		} else {
			ESP_LOGI(LOG_TAG, "HTTP POST Writing OTA data");
			esp_ota_write(ota_handle, ota_buff, recv_len);

			content_received += recv_len;
		}

	} while (recv_len > 0 && content_received < content_length);

	if (esp_ota_end(ota_handle) == ESP_OK) {
		// Lets update the partition
		if(esp_ota_set_boot_partition(update_partition) == ESP_OK) {
			// Webpage will request status when complete
			// This is to let it know it was successful
			flash_status = 1;
		} else {
			ESP_LOGE(LOG_TAG, "HTTP POST Set boot partition error");
		}

	} else {
		ESP_LOGE(LOG_TAG, "HTTP POST OTA End Error");
	}
	ESP_LOGI(LOG_TAG, "HTTP POST flashing succeeded");
	return ESP_OK;
}
