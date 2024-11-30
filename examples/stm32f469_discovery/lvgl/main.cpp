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


Touch::Data touchData;
Touch touch{touchData, TouchAddress};

void my_touchpad_read(lv_indev_t*, lv_indev_data_t* data)
{
	touch.readTouches();
	Touch::touch_t tp;
	touch.getData().getTouch(&tp, 0);
	// mirror and rotate correctly
	uint16_t x{tp.y}, y{uint16_t(480 - tp.x)};
	if (tp.event == Touch::Event::Contact)
	{
		data->state = LV_INDEV_STATE_PRESSED;
		data->point.x = x;
		data->point.y = y;
	}
	else data->state = LV_INDEV_STATE_RELEASED;
}

uint8_t *buf_1, *buf_2;
void disp_flush(lv_display_t* disp, const lv_area_t*, uint8_t* px_map)
{
	setDisplayBuffer((uint16_t*) px_map);
	lv_display_flush_ready(disp);
}


int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

	MODM_LOG_INFO << "modm LVGL example on STM32F469-Discovery board!\n\n";

	touch.configure(Touch::InterruptMode::Trigger, 60, 60);

	// Initialize the display:
	lv_display_t *disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_display_set_flush_cb(disp, disp_flush);

	static constexpr size_t BufSize = LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t);
	buf_1 = new(modm::MemoryExternal) uint8_t[BufSize];
	buf_2 = new(modm::MemoryExternal) uint8_t[BufSize];
	lv_display_set_buffers(disp, buf_1, buf_2, BufSize, LV_DISPLAY_RENDER_MODE_DIRECT);

	// Initialize touchscreen driver:
	lv_indev_t* indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, my_touchpad_read);

	lv_obj_t *labelA =  lv_label_create(lv_screen_active());
	lv_label_set_text(labelA, "Hello world!");
	lv_obj_set_pos(labelA, 10, 10);
	lv_obj_set_size(labelA, 120, 50);

	lv_obj_t* btn2 = lv_button_create(lv_screen_active());
	lv_obj_set_pos(btn2, 140, 10);
	lv_obj_set_size(btn2, 120, 50);

	lv_obj_t* label2 = lv_label_create(btn2);
	lv_label_set_text(label2, "Button2");

	lv_obj_add_event_cb(btn2, [](lv_event_t *event)
	{
		static uint16_t btn2Counter = 0;
		lv_label_set_text_fmt((lv_obj_t*) lv_event_get_user_data(event),
							  "Button 2: %d", ++btn2Counter);
	}, LV_EVENT_PRESSED, label2);

	uint16_t counter = 0;

	modm::ShortPeriodicTimer tmr{20ms};
	while (true)
	{
		lv_timer_handler();

		if (tmr.execute())
		{
			lv_label_set_text_fmt(labelA, "counter=%d", ++counter);
		}
	}

	return 0;
}
