/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2013, Fabian Greif
 * Copyright (c) 2012-2014, Sascha Schade
 * Copyright (c) 2013, Daniel Krebs
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>

#include <modm/driver/display/parallel_tft.hpp>
#include <modm/driver/bus/tft_memory_bus.hpp>
#include <modm/ui/display/image.hpp>
#include <modm/driver/touch/ads7843.hpp>

#include "touchscreen_calibrator.hpp"


// ----------------------------------------------------------------------------

/* FSMC
 *
 * * Use A16 as Command / Data pin
 * 0x60000000 is the base address for FSMC's first memory bank.
 * When accessing 0x60000000 A16 is low.
 *
 * Why offset +0x20000 for A16?
 * (1 << 16) is 0x10000.
 *
 * But the TftMemoryBus16Bit uses the FSMC in 16 bit mode.
 * Then, according to Table 184 (External memory address) of
 * reference manual (p1317) address HADDR[25:1] >> 1 are issued to
 * the address pins A24:A0.
 * So when writing to offset +((1 << 16) << 1) pin A16 is high.
 */
modm::TftMemoryBus16Bit parallelBus(
		(volatile uint16_t *) 0x60000000,
		(volatile uint16_t *) 0x60020000);

modm::ParallelTft<modm::TftMemoryBus16Bit> tft(parallelBus);

// ----------------------------------------------------------------------------
// Touchscreen

typedef GpioOutputC4 CsTouchscreen;
typedef GpioInputC5  IntTouchscreen;

modm::Ads7843<SpiMaster2, CsTouchscreen, IntTouchscreen> ads7843;
modm::TouchscreenCalibrator touchscreen;

typedef GpioOutputD7 CS;

static void
initDisplay()
{

	Fsmc::initialize();
	GpioConnector<Peripheral::Fsmc,
		GpioD14::D0,
		GpioD15::D1,
		GpioD0::D2,
		GpioD1::D3,
		GpioE7::D4,
		GpioE8::D5,
		GpioE9::D6,
		GpioE10::D7,
		GpioE11::D8,
		GpioE12::D9,
		GpioE13::D10,
		GpioE14::D11,
		GpioE15::D12,
		GpioD8::D13,
		GpioD9::D14,
		GpioD10::D15,
		GpioD4::Noe,
		GpioD5::Nwe,
		GpioD11::A16>::connect();

	CS::setOutput();
	CS::reset();

	fsmc::NorSram::AsynchronousTiming timing = {
		// read
		15,
		0,
		15,

		// write
		15,
		0,
		15,

		// bus turn around
		0
	};

	fsmc::NorSram::configureAsynchronousRegion(
			fsmc::NorSram::CHIP_SELECT_1,
			fsmc::NorSram::NO_MULTIPLEX_16BIT,
			fsmc::NorSram::SRAM_ROM,
			fsmc::NorSram::MODE_A,
			timing);

	fsmc::NorSram::enableRegion(fsmc::NorSram::CHIP_SELECT_1);

	tft.initialize();
}

static void
initTouchscreen()
{
	CsTouchscreen::setOutput();
	CsTouchscreen::set();

	IntTouchscreen::setInput(Gpio::InputType::PullUp);

	SpiMaster2::connect<GpioB13::Sck, GpioB14::Miso, GpioB15::Mosi>();
	SpiMaster2::initialize<Board::systemClock, 1312500ul>();
	SpiMaster2::setDataMode(SpiMaster2::DataMode::Mode0);

}

// ----------------------------------------------------------------------------
static void
drawCross(modm::GraphicDisplay& display, modm::glcd::Point center)
{
	display.setColor(modm::glcd::Color::red());
	display.drawLine(center.x - 15, center.y, center.x - 2, center.y);
	display.drawLine(center.x + 2, center.y, center.x + 15, center.y);
	display.drawLine(center.x, center.y - 15, center.x, center.y - 2);
	display.drawLine(center.x, center.y + 2, center.x, center.y + 15);

	display.setColor(modm::glcd::Color::white());
	display.drawLine(center.x - 15, center.y + 15, center.x - 7, center.y + 15);
	display.drawLine(center.x - 15, center.y + 7, center.x - 15, center.y + 15);

	display.drawLine(center.x - 15, center.y - 15, center.x - 7, center.y - 15);
	display.drawLine(center.x - 15, center.y - 7, center.x - 15, center.y - 15);

	display.drawLine(center.x + 7, center.y + 15, center.x + 15, center.y + 15);
	display.drawLine(center.x + 15, center.y + 7, center.x + 15, center.y + 15);

	display.drawLine(center.x + 7, center.y - 15, center.x + 15, center.y - 15);
	display.drawLine(center.x + 15, center.y - 15, center.x + 15, center.y - 7);
}

static void
calibrateTouchscreen(modm::GraphicDisplay& display)
{
	modm::glcd::Point calibrationPoint[3] = { { 45, 45 }, { 270, 90 }, { 100, 190 } };
	modm::glcd::Point sample[3];

	for (uint8_t i = 0; i < 3; i++)
	{
		display.clear();

		display.setColor(modm::glcd::Color::yellow());
		display.setCursor(50, 5);
		display << "Touch crosshair to calibrate";

		drawCross(display, calibrationPoint[i]);
		modm::delayMilliseconds(500);

		while (!ads7843.read(&sample[i])) {
			// wait until a valid sample can be taken
		}
	}

	touchscreen.calibrate(calibrationPoint, sample);

	display.clear();
}

void
drawPoint(modm::GraphicDisplay& display, modm::glcd::Point point)
{
	if (point.x < 0 || point.y < 0) {
		return;
	}

	display.drawPixel(point.x, point.y);
	display.drawPixel(point.x + 1, point.y);
	display.drawPixel(point.x, point.y + 1);
	display.drawPixel(point.x + 1, point.y + 1);
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Board::LedOrange::set();

	initDisplay();
	initTouchscreen();

	calibrateTouchscreen(tft);

	tft.setColor(modm::glcd::Color::lime());

	while (1)
	{
		modm::glcd::Point raw;
		if (ads7843.read(&raw))
		{
			modm::glcd::Point point;

			touchscreen.translate(&raw, &point);
			drawPoint(tft, point);

			Board::LedGreen::set();
		}
		else {
			Board::LedGreen::reset();
		}

		// clear screen if the user button is pressed
		if (Board::Button::read()) {
			tft.clear();
		}
	}

	return 0;
}
