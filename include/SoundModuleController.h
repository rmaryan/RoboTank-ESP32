/*
 * SoundModuleController.h
 *
 * A class which manages the WTV020SD16P-based sound board.
 * Commands are executed asynchronously in a dedicated worker task,
 * so calling methods never block the caller.
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

#ifndef SOUNDMODULECONTROLLER_H_
#define SOUNDMODULECONTROLLER_H_

#include <stdint.h>

class SoundModuleController {
private:
	static const uint16_t PLAY_PAUSE = 0xFFFE;
	static const uint16_t STOP   = 0xFFFF;
	static const uint16_t VOLUME_MIN = 0xFFF0;

	// Internal helpers (called from worker task only)
	static void sendCommand(uint16_t command);
	static void reset();

	// Worker task entry point
	static void soundWorkerTask(void* param);

public:
	// Initialize the sound subsystem and start the background worker task.
	static void init();
	// Queue a track to play (track # 0..512) - non-blocking.
	static void playSound(uint16_t trackID);
	// Queue a stop command - non-blocking.
	static void stopSound();
	// Set volume level from 0 (mute) to 7 (max) - non-blocking.
	static void setVolume(uint8_t volume);
};

#endif /* SOUNDMODULECONTROLLER_H_ */
