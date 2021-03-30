/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <tusb.h>

#include <modm/board.hpp>
#include <modm/io.hpp>
#include <modm/processing.hpp>

using namespace Board;

#if CFG_TUD_CDC
modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::DiscardIfFull> usb_io_device0;
modm::IOStream usb_stream0(usb_io_device0);
#endif

modm::PeriodicTimer tmr{2.5s};

// Invoked when device is mounted
void tud_mount_cb() { tmr.restart(1s); }
// Invoked when device is unmounted
void tud_umount_cb() { tmr.restart(250ms); }
// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool) { tmr.restart(2.5s); }
// Invoked when usb bus is resumed
void tud_resume_cb() { tmr.restart(1s); }
void midi_task();

int main()
{
	Board::initialize();
	Board::initializeUsbFs();
	tusb_init();

	while (true)
	{
		tud_task();
		midi_task();

		if (tmr.execute())
		{
			Leds::toggle();
			static uint8_t counter{0};
#ifdef MODM_BOARD_HAS_LOGGER
			MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
#endif
#if CFG_TUD_CDC
			usb_stream0 << "Hello World from USB: " << (counter++) << modm::endl;
#endif
		}
	}

	return 0;
}



void midi_task()
{
#if CFG_TUD_MIDI
	static modm::PeriodicTimer tmr{286ms};

	if (tmr.execute())
	{
		// The MIT License (MIT)
		// Copyright (c) 2019 Ha Thach (tinyusb.org)

		// Store example melody as an array of note values
		static uint8_t note_sequence[] =
		{
			74,78,81,86,90,93,98,102,57,61,66,69,73,78,81,85,88,92,97,100,97,92,88,85,81,78,
			74,69,66,62,57,62,66,69,74,78,81,86,90,93,97,102,97,93,90,85,81,78,73,68,64,61,
			56,61,64,68,74,78,81,86,90,93,98,102
		};
		// Variable that holds the current position in the sequence.
		static uint32_t note_pos = 0;

		// Previous positions in the note sequence.
		int previous = note_pos - 1;

		// If we currently are at position 0, set the
		// previous position to the last note in the sequence.
		if (previous < 0) previous = sizeof(note_sequence) - 1;

		// Send Note On for current position at full velocity (127) on channel 1.
		tud_midi_write24(0, 0x90, note_sequence[note_pos], 127);

		// Send Note Off for previous note.
		tud_midi_write24(0, 0x80, note_sequence[previous], 0);

		// Increment position
		note_pos++;

		// If we are at the end of the sequence, start over.
		if (note_pos >= sizeof(note_sequence)) note_pos = 0;
	}
#endif
}
