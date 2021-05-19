/*
 * Copyright (c) 2021, Henrik Hose
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/display/ili9341_spi.hpp>

#include <modm/driver/touch/touch2046.hpp>

using namespace Board;
using namespace modm::color::html;

namespace tft
{
	using DmaRx = Dma1::Channel2;
	using DmaTx = Dma1::Channel3;
	using Spi = SpiMaster1_Dma<DmaRx, DmaTx>;
	//using Spi = SpiMaster1;
	using Cs = modm::platform::GpioC8;
	using Sck = modm::platform::GpioA5;
	using Miso = modm::platform::GpioA6;
	using Mosi = modm::platform::GpioA7;
	using DataCommands = modm::platform::GpioC5;
	using Reset = modm::platform::GpioC6;
	using Backlight = modm::platform::GpioC9;
}

modm::Ili9341Spi<
	tft::Spi,
	tft::Cs,
	tft::DataCommands,
	tft::Reset,
	tft::Backlight
> tftController;

namespace touch
{
	using Spi = SpiMaster2;
	using Cs = modm::platform::GpioB3;
	using Sck = modm::platform::GpioB13;
	using Miso = modm::platform::GpioB14;
	using Mosi = modm::platform::GpioB15;
	//using Interrupt = modm::platform::GpioA10;
}

modm::Touch2046<touch::Spi, touch::Cs> touchController;


int
main()
{
	using namespace modm::literals;
	Board::initialize();
	Dma1::enable();

	tft::Spi::connect<
		tft::Sck::Sck,
		tft::Miso::Miso,
		tft::Mosi::Mosi>();
	tft::Spi::initialize<SystemClock, 40_MHz>();
	tftController.initialize();
	tftController.enableBacklight(true);

	touch::Spi::connect<
		touch::Sck::Sck,
		touch::Miso::Miso,
		touch::Mosi::Mosi>();
	touch::Spi::initialize<SystemClock, 2500_kHz>();
	modm::touch2046::Calibration cal{
		.OffsetX = -11,
		.OffsetY = 335,
		.FactorX = 22018,
		.FactorY = -29358,
		.MaxX = 240,
		.MaxY = 320,
		.ThresholdZ = 500,
	};
	touchController.setCalibration(cal);

	LedGreen::set();

	tftController.setColor(Black);
	tftController.setBackgroundColor(Red);
	tftController.clear();

	tftController.setFont(modm::font::ArcadeClassic);
	tftController.setColor(White);
	tftController.setCursor(10,12);
	tftController << "(10,10)";
	tftController.setCursor(150,202);
	tftController << "(150,200)";
	tftController.drawLine(5, 10, 15, 10);
	tftController.drawLine(10, 5, 10, 15);
	tftController.drawLine(145, 200, 155, 200);
	tftController.drawLine(150, 195, 150, 205);

	tftController.setColor(Black);
	tftController.setFont(modm::font::Ubuntu_36);

	int16_t X = 0;
	int16_t Y = 0;
	int16_t Z = 0;

	while (true)
	{
		LedGreen::set();

		std::tie(X, Y, Z) = RF_CALL_BLOCKING(touchController.getRawValues());
		tftController.setColor(Red);
		tftController.fillRectangle({30, 50}, 90, 115);
		tftController.setColor(Black);
		tftController.setCursor(0, 50);
		tftController << "X=" << X;
		tftController.setCursor(0, 90);
		tftController << "Y=" << Y;
		tftController.setCursor(0, 130);
		tftController << "Z=" << Z;

		tftController.setColor(Red);
		tftController.fillRectangle({30, 220}, 120, 35);
		tftController.setColor(Black);
		if(RF_CALL_BLOCKING(touchController.isTouched())) {
			std::tie(X, Y) = RF_CALL_BLOCKING(touchController.getTouchPosition());
			tftController.setCursor(5, 220);
			tftController << "> (" << X << "," << Y << ")";
		}

		LedGreen::reset();
		modm::delay(300ms);
	}

	return 0;
}
