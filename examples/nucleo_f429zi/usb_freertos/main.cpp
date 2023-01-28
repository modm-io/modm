/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <tusb.h>

#include <modm/board.hpp>
#include <modm/processing/rtos.hpp>
#include <modm/processing/timer/periodic_timer.hpp>

using namespace Board;

class SomeTask : modm::rtos::Thread
{
public:
	SomeTask() : Thread(2, 2048, "some_task") {}

	void
	run()
	{
		modm::PeriodicTimer tmr{2.5s};
		uint8_t counter{0};

		while (1)
		{
			if (tmr.execute())
			{
				MODM_LOG_INFO << "SomeTask: Hello via Uart, counter=" << (counter++) << modm::endl;
			}

			vTaskDelay(1);
		}

		vTaskDelete(0);
	}
};

class UsbTask : modm::rtos::Thread
{
public:
	UsbTask() : Thread(1, 2048, "usb_task"), usb_io_device0{}, usb_stream0{usb_io_device0} {}

	void
	run()
	{
		MODM_LOG_INFO << "USBTask: Calling Board::initializeUsbFs() ..." << modm::endl;
		Board::initializeUsbFs(4);
		MODM_LOG_INFO << "USBTask: Calling tusb_init() ..." << modm::endl;
		tusb_init();
		MODM_LOG_INFO << "... done!" << modm::endl;

		modm::PeriodicTimer tmr{2.5s};
		uint8_t counter{0};

		while (1)
		{
			tud_task();
			if (tmr.execute())
			{
				MODM_LOG_INFO << "UsbTask: Hello via Uart, counter=" << counter << modm::endl;
				usb_stream0 << "UsbTask: Hello via USB CDC, counter=" << counter << modm::endl;
				counter++;
			}

			vTaskDelay(1);
		}

		vTaskDelete(0);
	}

private:
	modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::DiscardIfFull> usb_io_device0;
	modm::IOStream usb_stream0;
};

SomeTask someTask;
UsbTask usbTask;

int
main()
{
	Board::initialize();
	Leds::setOutput();
	MODM_LOG_INFO << "Nucleo-F429ZI: TinyUSB & FreeRTOS example" << modm::endl;

	MODM_LOG_INFO << "WARNING!" << modm::endl;
	MODM_LOG_INFO << "TinyUSB in modm does not currently use the FreeRTOS abstraction layer";
	MODM_LOG_INFO << " and is not thread-safe with FreeRTOS threads." << modm::endl;

	modm::rtos::Scheduler::schedule();

	// we should never get here
	return 0;
}
