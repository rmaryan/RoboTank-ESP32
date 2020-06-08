/*
 * BufLogger.h
 *
 * This class takes over the regular esp_log framework collecting the logged lines in a ring buffer.
 * Once buffer overflows - the oldest messages are dropped, releasing space for the new messages.
 *
 * Can be used to redirect log messages to the alternative receivers (in our case - a web page)
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#ifndef MAIN_BUFLOGGER_H_
#define MAIN_BUFLOGGER_H_

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/ringbuf.h>
#include <freertos/semphr.h>

class BufLogger {
private:
	static const uint16_t RING_BUFFER_SIZE = 1024;
	static const uint16_t LINE_BUFF_SIZE = 250;
	static char line_buffer[LINE_BUFF_SIZE];

	static RingbufHandle_t buf_handle;
	static vprintf_like_t original_vprintf;
	static SemaphoreHandle_t xBufferSemaphore;
	static int _log_vprintf(const char *fmt, va_list args);

public:
	// Initialize the subsystem, override a standard logger
	static void init();
	// Get the log buffer content
	// The calling function should take care about releasing the memory of the string received (free(ptr))
	// Returns NULL if the buffer is empty
	static char* pullLines();
};

#endif /* MAIN_BUFLOGGER_H_ */
