/*
 * BufLogger.cpp
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

#include "BufLogger.h"

#include <string.h>

static const char* LOG_TAG = "BLOG";

RingbufHandle_t BufLogger::buf_handle = NULL;
vprintf_like_t BufLogger::original_vprintf = NULL;
SemaphoreHandle_t BufLogger::xBufferSemaphore = NULL;
char BufLogger::line_buffer[LINE_BUFF_SIZE];

void BufLogger::init() {
	// Create ring buffer
	buf_handle = xRingbufferCreate(RING_BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
	if (buf_handle == NULL) {
		ESP_LOGE(LOG_TAG, "Ring buffer creation failed");
		return;
	}

	// Ringbuffer is not thread-safe - need to guard it by a semaphore
	xBufferSemaphore = xSemaphoreCreateMutex();
	if(xBufferSemaphore == NULL) {
		ESP_LOGE(LOG_TAG, "Semaphore creation failed");
	}

	// Take over the default logging function
	original_vprintf = esp_log_set_vprintf(&_log_vprintf);
}

char* BufLogger::pullLines() {
	//Receive data from byte buffer
	size_t item_size;
	char * linesBuffer = NULL;

	if( xSemaphoreTake(xBufferSemaphore, (TickType_t) 100) == pdTRUE ) {
		char *item = (char *)xRingbufferReceiveUpTo(buf_handle, &item_size, 0, RING_BUFFER_SIZE);
		//Check received data
		if (item != NULL) {
			// allocate memory for the output string (+1 char for the trailing zero)
			item_size++;
			linesBuffer = (char *) malloc(item_size);
			memcpy(linesBuffer, item, item_size);
			linesBuffer[item_size-1] = 0;
			//Return Item
			vRingbufferReturnItem(buf_handle, (void *)item);

			// Try again, if we happen to cross the buffer ending
			size_t initial_item_size = item_size;
			item = (char *)xRingbufferReceiveUpTo(buf_handle, &item_size, 0, RING_BUFFER_SIZE);
			if (item != NULL) {
				// expand the message buffer
				// the first part is supposed to be preserved by realloc
				linesBuffer = (char *) realloc(linesBuffer, initial_item_size + item_size);
				memcpy(&(linesBuffer[initial_item_size-1]), item, item_size);
				linesBuffer[initial_item_size + item_size-1] = 0;
				//Return Item
				vRingbufferReturnItem(buf_handle, (void *)item);
			}
		}
		xSemaphoreGive(xBufferSemaphore);
	}
	return linesBuffer;
}

int BufLogger::_log_vprintf(const char *fmt, va_list args) {
	// Build the log line
	int lineLength = vsnprintf (line_buffer, LINE_BUFF_SIZE, fmt, args);
	// Store the log line in the buffer
	if(lineLength > 0) {
		if( xSemaphoreTake(xBufferSemaphore, (TickType_t) 100) == pdTRUE ) {
			size_t currentFreeSize = xRingbufferGetCurFreeSize(buf_handle);

			// if not enough space - let's drop the oldest bytes from the buffer
			if(currentFreeSize < (lineLength)) {
				size_t spaceToRelease = lineLength - currentFreeSize;

				// we can be at the end of the buffer, so might need to release bytes twice
				size_t releasedSize = 0;
				void * releasedItem = xRingbufferReceiveUpTo(buf_handle, &releasedSize, 0, spaceToRelease);
				vRingbufferReturnItem(buf_handle, releasedItem);
				if(releasedSize < spaceToRelease) {
					spaceToRelease-=releasedSize;
					releasedItem = xRingbufferReceiveUpTo(buf_handle, &releasedSize, 0, spaceToRelease);
					vRingbufferReturnItem(buf_handle, releasedItem);
				}
			}

			xRingbufferSend(buf_handle, line_buffer, lineLength, 0);

			xSemaphoreGive(xBufferSemaphore);
		}

		// Still feed the log lines to the default destination
		// No need to parse the string again
		return original_vprintf(line_buffer, va_list());
	} else {
		return 0;
	}
}
