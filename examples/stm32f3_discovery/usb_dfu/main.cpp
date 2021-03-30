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

modm::PeriodicTimer tmr{1s};

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

modm_section(".noinit") uint64_t dfu_mode;
constexpr uint64_t DFU_MAGIC{0xf318ea89313f2be8};

/* 1. Do scons program once
 * 2. Power-cycle by disconnecting USB cable from ST-Link
 * 3. Force USB re-enumeration by connecting USB cable to UsbFs
 * 4. Do `scons program-dfu delay=3` as many times as you want
 *
 * Rebooting the chip into DFU and enumerating USB again takes a few seconds.
 * You may have to increase the `delay` argument.
 */

int main()
{
	if (dfu_mode == DFU_MAGIC)
	{
		dfu_mode = 0; __DSB();
		// Jump to SystemFlash *before* initializing any peripherals!
		asm volatile
		(
			// Address of SystemFlash differs between devices!!!
			"ldr r0, =0x1FFFD800"	"\n\t"
			"ldr sp,[r0, #0]"		"\n\t"
			"ldr r0,[r0, #4]"		"\n\t"
			"bx  r0"
		);
	}
	Board::initialize();
	Board::initializeUsbFs();
	tusb_init();

	while (true)
	{
		tud_task();

		if (tmr.execute())
		{
			if (dfu_mode == DFU_MAGIC) NVIC_SystemReset();
			LedNorth::toggle();
		}
	}

	return 0;
}

// Invoked on DFU_DETACH request to reboot to the bootloader
void tud_dfu_runtime_reboot_to_dfu_cb(void)
{
	dfu_mode = DFU_MAGIC;
	// You must delay SystemReset so that TinyUSB can finish!!!
	tmr.restart(100ms);
}
