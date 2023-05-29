/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2022, Christopher Durand
 * Copyright (c) 2023, Thomas Rush
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;

modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::BlockIfFull> usb_io_device;
modm::IOStream usb_stream(usb_io_device);

int
main()
{
	initialize();
	initializeUsbFs();

	tusb_init();

	uint32_t counter(0);

	modm::PeriodicTimer timer{500ms};

	while (not timer.execute()) {
		tud_task();
	}

	// it can be hard to catch this without the preceding delay
	usb_stream << "Hello from USB" << modm::endl;

	while (true)
	{
		tud_task();
		if (timer.execute()) {
			Led::toggle();
			usb_stream << "loop: " << counter++ << modm::endl;
		}
	}

	return 0;
}
