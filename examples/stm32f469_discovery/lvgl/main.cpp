/*
 * Copyright (c) 2020, Raphael Lehmann
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

#include <lvgl/lvgl.h>


// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

using namespace Board;
using namespace modm::glcd;
using namespace Board::ft6;
using namespace modm::literals;


static uint16_t* displayBuffer;
static lv_disp_draw_buf_t disp_buf;
static lv_color_t* buf;
static constexpr size_t buf_size = LV_HOR_RES_MAX * LV_VER_RES_MAX;

Touch::Data touchData;
Touch touch{touchData};

void my_touchpad_read(lv_indev_drv_t*, lv_indev_data_t* data)
{
	RF_CALL_BLOCKING(touch.readTouches());
	Touch::touch_t tp;
	touch.getData().getTouch(&tp, 0);
	// mirror and rotate correctly
	uint16_t x{tp.y}, y{uint16_t(480 - tp.x)};
	data->state = (tp.event == Touch::Event::Contact) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
	if(data->state == LV_INDEV_STATE_PR) {
		data->point.x = x;
		data->point.y = y;
	}
}

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	for(lv_coord_t y = area->y1; y <= area->y2; y++) {
		std::memcpy(
			&displayBuffer[(800*y) + area->x1],
			&color_p[(y - area->y1) * (area->x2 - area->x1 + 1)],
			(area->x2 - area->x1 + 1) * sizeof(lv_color_t)
			);
	}
	lv_disp_flush_ready(disp_drv);
}


int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

	MODM_LOG_INFO << "modm LVGL example on STM32F469-Discovery board!\n\n";

	RF_CALL_BLOCKING(touch.configure(Touch::InterruptMode::Trigger, 60, 60));

	lv_init();

	buf = new(modm::MemoryExternal) lv_color_t[buf_size];
	displayBuffer = new(modm::MemoryExternal) uint16_t[buf_size];

	setDisplayBuffer(displayBuffer);

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
	lv_obj_set_pos(labelA, 10, 10);
	lv_obj_set_size(labelA, 120, 50);

	lv_obj_t* btn2 = lv_btn_create(lv_scr_act());
	lv_obj_set_pos(btn2, 140, 10);
	lv_obj_set_size(btn2, 120, 50);
	lv_obj_t* label2 = lv_label_create(btn2);
	lv_label_set_text(label2, "Button2");
	lv_obj_add_event_cb(btn2, [](lv_event_t *event) {
		static uint16_t btn2Counter = 0;
		lv_label_set_text_fmt(lv_obj_get_child(event->target, 0), "Button 2: %d", ++btn2Counter);
	}, LV_EVENT_PRESSED, NULL);

	uint16_t counter = 0;

	modm::ShortPeriodicTimer tmr{20ms};
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
