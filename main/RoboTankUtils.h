/*
 * RoboTankUtils.h
 *
 * Utility functions used widely in the project.
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
 */

#ifndef MAIN_ROBOTANKUTILS_H_
#define MAIN_ROBOTANKUTILS_H_

#include <stdint.h>

// Arduino map function port
long map(long x, long in_min, long in_max, long out_min, long out_max);

// Short-cut for the delay in milliseconds
void delay_ms(uint32_t ms);

#endif /* MAIN_ROBOTANKUTILS_H_ */
