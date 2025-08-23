/*
 * LEDRMT.cpp
 *
 * A simple RMT-based LED strip controller for ESP32.
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

#include "LEDRMT.h"
#include <string.h>
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "freertos/FreeRTOS.h"

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000) // 10MHz resolution
#define LED_STRIP_RMT_MEM_BLOCK_SYMBOLS 64
#define LED_STRIP_RMT_TICK_DUR 100

static const char* TAG = "LedStripController";

// WS2812 timing (in ns)
#define T0H_NS  400
#define T0L_NS  850
#define T1H_NS  800
#define T1L_NS  450

LEDRMT::LEDRMT(gpio_num_t gpio, uint32_t led_num)
    : _gpio(gpio), _led_num(led_num), _buffer(nullptr), _initialized(false), _rmt_channel(nullptr){

    };

LEDRMT::~LEDRMT() {
    if (_buffer) {
        delete[] _buffer;
    }
    if (_rmt_channel) {
        rmt_disable(_rmt_channel);
        rmt_del_channel(_rmt_channel);
    }
}

bool LEDRMT::init() {
    if (_initialized) return true;

    _buffer = new uint8_t[_led_num * 3];
    memset(_buffer, 0, _led_num * 3);

    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = _gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .mem_block_symbols = LED_STRIP_RMT_MEM_BLOCK_SYMBOLS,
        .trans_queue_depth = 4,
        .flags = 0,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &_rmt_channel));
    ESP_ERROR_CHECK(rmt_enable(_rmt_channel));

    rmt_copy_encoder_config_t copy_encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&copy_encoder_config, &led_data_encoder));

    _initialized = true;
    return true;
}

void LEDRMT::setPixel(uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
    if (index >= _led_num) return;
    _buffer[index * 3 + 0] = g;
    _buffer[index * 3 + 1] = r;
    _buffer[index * 3 + 2] = b;
}

void LEDRMT::show() {
    if (!_initialized) return;

    // Each bit is encoded as a pair of RMT symbols (high, low)
    size_t num_bits = _led_num * 24;
    rmt_symbol_word_t symbols[num_bits];

    size_t symbol_idx = 0;
    for (uint32_t i = 0; i < _led_num * 3; ++i) {
        uint8_t val = _buffer[i];
        for (int bit = 7; bit >= 0; --bit) {
            bool is_one = val & (1 << bit);
            if (is_one) {
                symbols[symbol_idx].level0 = 1;
                symbols[symbol_idx].duration0 = T1H_NS / LED_STRIP_RMT_TICK_DUR;
                symbols[symbol_idx].level1 = 0;
                symbols[symbol_idx].duration1 = T1L_NS  / LED_STRIP_RMT_TICK_DUR;
            } else {
                symbols[symbol_idx].level0 = 1;
                symbols[symbol_idx].duration0 = T0H_NS / LED_STRIP_RMT_TICK_DUR;
                symbols[symbol_idx].level1 = 0;
                symbols[symbol_idx].duration1 = T0L_NS  / LED_STRIP_RMT_TICK_DUR;
            }
            symbol_idx++;
        }
    }

    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0,
    };
    ESP_ERROR_CHECK(rmt_transmit(_rmt_channel, led_data_encoder, symbols, sizeof(symbols), &tx_cfg));    
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(_rmt_channel, portMAX_DELAY));
}

void LEDRMT::clear() {
    if (!_initialized) return;
    memset(_buffer, 0, _led_num * 3);
    show();
}