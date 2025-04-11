/*
 * Copyright (c) 2024, Frank Altheim
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug.hpp>
#include <modm/processing.hpp>
#include <modm/driver/display/ili9341_spi.hpp>
#include <modm/driver/touch/touch2046.hpp>

#include <lvgl/lvgl.h>

// USER INCLUDES
#include "test_screen.h"


// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

using namespace Board;
using namespace modm::literals;


namespace tft
{
	using DmaRx = Dma1::Channel2;
	using DmaTx = Dma1::Channel3;
	using Spi = SpiMaster1_Dma<DmaRx, DmaTx>;
	//using Spi = SpiMaster1;
	using Cs = Board::D10;
	using Sck = Board::D13;
	using Miso = Board::D12;
	using Mosi = Board::D11;
	using DataCommands = Board::D9;
	using Reset = Board::D8;
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
	using Spi = SpiMaster3;
	using Cs = modm::platform::GpioD2;
	using Sck = modm::platform::GpioC10;
	using Miso = modm::platform::GpioC11;
	using Mosi = modm::platform::GpioC12;
	using Interrupt = modm::platform::GpioC9;
}

modm::Touch2046<touch::Spi, touch::Cs> touchController;

// Define display buffer 1
static constexpr size_t buf_size = 240 * 320 / 8;
static lv_color_t modm_aligned(4) buf[buf_size];

// Define display buffer 2
static constexpr size_t buf2_size = 240 * 320 / 8;
static lv_color_t modm_aligned(4) buf2[buf2_size];

void my_touchpad_read(lv_indev_t*, lv_indev_data_t* data)
{
	data->state = touchController.isTouched() ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	if(data->state == LV_INDEV_STATE_PRESSED) {
		std::tuple xy = touchController.getTouchPosition();
		data->point.x = std::get<0>(xy);
		data->point.y = std::get<1>(xy);

		std::tuple xyRAW = touchController.getRawValues();
		int16_t rawX = std::get<0>(xyRAW);
		int16_t rawY = std::get<1>(xyRAW);
		// Display on test screen
		setTouchText(data->point.x, data->point.y, rawX, rawY);

	}
}

void disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
	tftController.drawRaw(
		{uint16_t(area->x1), uint16_t(area->y1)},
		(area->x2 - area->x1 +1),
		(area->y2 - area->y1 + 1),
		(modm::color::Rgb565*)px_map);
	lv_display_flush_ready(disp);
}

int
main()
{
	Board::initialize();
	Dma1::enable();

	tft::Spi::connect<
		tft::Sck::Sck,
		tft::Miso::Miso,
		tft::Mosi::Mosi>();
	tft::Spi::initialize<Board::SystemClock, 24_MHz>();
	tftController.initialize();
	tftController.enableBacklight(true);

	touch::Spi::connect<
		touch::Sck::Sck,
		touch::Miso::Miso,
		touch::Mosi::Mosi>();
	touch::Spi::initialize<Board::SystemClock, 1.5_MHz>();
	modm::touch2046::Calibration cal{
		.OffsetX = -11,
		.OffsetY = 335,
		.FactorX = 22018,
		.FactorY = -29358,
		.MaxX = 240,
		.MaxY = 320,
		.ThresholdZ = 100,
	};
	touchController.setCalibration(cal);

	MODM_LOG_INFO << "reflow-display on nucleo-l476rg!\n\n";

	lv_display_t *disp = lv_display_create(240, 320);
	lv_display_set_flush_cb(disp, disp_flush);
	lv_display_set_buffers(disp, buf, buf2, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

	// Initialize touchscreen driver:
	lv_indev_t* indev = lv_indev_create();
	// Assert touchscreen driver was created successfully:
	if (indev == NULL) {
		MODM_LOG_ERROR << "Failed to create input device\n";
		while (1) {}
	}

	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, my_touchpad_read);

	drawTestScreen();

	modm::ShortPeriodicTimer tmr{20ms};

	while (true)
	{
		lv_timer_handler();

		if (tmr.execute())
		{
			setLblText();
		}
	}

	return 0;
}
