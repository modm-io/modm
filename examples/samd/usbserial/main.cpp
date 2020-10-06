/*
 * Copyright (c) 2020, Erik Henriksson
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

modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::BlockIfFull> usb_io_device;
modm::IOStream usb_stream(usb_io_device);

// Arduino style hack that resets to DFU on disconnect at 1200bps
modm_extern_c void
tud_cdc_line_state_cb(uint8_t, bool dtr, bool)
{
	if (!dtr)  // disconnected
	{
		cdc_line_coding_t coding;
		tud_cdc_get_line_coding(&coding);
		if (coding.bit_rate == 1200)
		{
			static volatile uint32_t *DBL_TAP_PTR = (volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4);
			constexpr uint32_t DBL_TAP_MAGIC = 0xf01669ef;  // For UF2 bootloader
			// constexpr uint32_t DBL_TAP_MAGIC = 0x07738135; // For Arduino Zero bootloader
			*DBL_TAP_PTR = DBL_TAP_MAGIC;
			NVIC_SystemReset();
		}
	}
}

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

int main()
{
	Board::initialize();
	Board::initializeUsbFs();

	// Output 500hz PWM on D12 so we can validate the GCLK0 clock speed.
	PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
	TCC0->PER.bit.PER = SystemClock::Frequency / 1000;
	TCC0->CC[3].bit.CC = SystemClock::Frequency / 2000;
	TCC0->CTRLA.bit.ENABLE = true;
	using Tcc = Peripherals::Tcc<0>;
	D12::As<PeripheralPin::Wo>::Connector<Tcc, Tcc::Wo<3>>::connect();
	GenericClockController::connect<ClockPeripheral::Tcc0>(ClockGenerator::System);

	tusb_init();

	while (true)
	{
		tud_task();

		if (tmr.execute())
		{
			Leds::toggle();
			static uint8_t counter{0};
			usb_stream << "Hello World from USB: " << (counter++) << modm::endl;
		}
	}

	return 0;
}
