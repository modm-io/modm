/*
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
#include <modm/debug.hpp>
#include <modm/processing.hpp>
#include <modm/driver/display/ili9341_spi.hpp>
#include <modm/driver/touch/touch2046.hpp>

#include <lvgl/lvgl.h>


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


static lv_disp_draw_buf_t disp_buf;
static constexpr size_t buf_size = LV_HOR_RES_MAX * LV_VER_RES_MAX / 8;
static lv_color_t buf[buf_size];

void my_touchpad_read(lv_indev_drv_t*, lv_indev_data_t* data)
{
	data->state = RF_CALL_BLOCKING(touchController.isTouched()) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
	if(data->state == LV_INDEV_STATE_PR) {
		auto xy = RF_CALL_BLOCKING(touchController.getTouchPosition());
		data->point.x = std::get<0>(xy);
		data->point.y = std::get<1>(xy);
	}
}

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	tftController.drawRaw(
		{area->x1, area->y1},
		(area->x2 - area->x1 +1),
		(area->y2 - area->y1 + 1),
		(modm::color::Rgb565*)color_p);
	lv_disp_flush_ready(disp_drv);
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

	MODM_LOG_INFO << "modm LVGL example on Nucleo-L452RE board!\n\n";

	lv_init();

	// Initialize the display buffer
	lv_disp_draw_buf_init(&disp_buf, buf, NULL, buf_size);

	// Initialize the display:
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = my_flush_cb;
	disp_drv.hor_res = LV_HOR_RES_MAX;
	disp_drv.ver_res = LV_VER_RES_MAX;
	lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

	// Initialize touchscreen driver:
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_touchpad_read;
	lv_indev_drv_register(&indev_drv);

	lv_obj_t* scr = lv_disp_get_scr_act(disp); // Get the current screen

	lv_obj_t* labelA =  lv_label_create(scr);
	lv_label_set_text(labelA, "Hello world!");
	lv_obj_set_pos(labelA, 60, 10);
	lv_obj_set_size(labelA, 120, 50);

	lv_obj_t* btn = lv_btn_create(lv_scr_act());
	lv_obj_set_pos(btn, 60, 135);
	lv_obj_set_size(btn, 120, 50);
	lv_obj_t* btnLabel = lv_label_create(btn);
	lv_label_set_text(btnLabel, "Button");
	lv_obj_add_event_cb(btn, [](lv_event_t *event) {
		static uint16_t btnCounter = 0;
		lv_label_set_text_fmt(lv_obj_get_child(event->target, 0), "Button: %d", ++btnCounter);
	}, LV_EVENT_PRESSED, NULL);

	lv_obj_t* labelB =  lv_label_create(scr);
	lv_label_set_text(labelB, "Big Font");
	lv_obj_set_pos(labelB, 40, 260);
	lv_obj_set_style_text_font(labelB, &lv_font_montserrat_36, LV_PART_MAIN);

	uint16_t counter = 0;

	modm::ShortPeriodicTimer tmr{10ms};

	while (true)
	{
		lv_task_handler();

		if (tmr.execute())
		{
			lv_label_set_text_fmt(labelA, "counter=%d", ++counter);
		}
	}

	return 0;
}
