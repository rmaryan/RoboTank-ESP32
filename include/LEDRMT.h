/*
 * LEDRMT.h
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

#pragma once

#include <stdint.h>
#include "driver/rmt_tx.h"

class LEDRMT {
public:
    LEDRMT(gpio_num_t gpio, uint32_t led_num);
    ~LEDRMT();

    bool init();
    void setPixel(uint32_t index, uint8_t r, uint8_t g, uint8_t b);
    void show();
    void clear();

private:
    gpio_num_t _gpio;
    uint32_t _led_num;
    rmt_channel_handle_t _rmt_channel;
    uint8_t* _buffer;
    bool _initialized;

    rmt_encoder_handle_t led_data_encoder = NULL;
};