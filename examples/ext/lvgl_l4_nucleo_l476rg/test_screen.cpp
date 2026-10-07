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

#include "test_screen.h"
#include <lvgl/lvgl.h>

static lv_obj_t* screen;
static uint16_t counter = 0;
static lv_obj_t* labelA;
static lv_obj_t* labelTouch;
static lv_obj_t* labelRawTouch;

void btn_event_cb(lv_event_t *event)
{
	static uint16_t btnCounter = 0;
	lv_label_set_text_fmt((lv_obj_t*) lv_event_get_user_data(event),
							"Button: %d", ++btnCounter);
}

void drawTestScreen(void)
{
	screen = lv_obj_create(NULL);

	labelA =  lv_label_create(screen);
	lv_label_set_text(labelA, "Hello world!");
	lv_obj_set_pos(labelA, 60, 10);
	lv_obj_set_size(labelA, 120, 50);

	labelTouch =  lv_label_create(screen);
	lv_label_set_text_fmt(labelTouch, "Pos Touch: x = %d, y = %d", 0, 0);
	lv_obj_set_pos(labelTouch, 60, 30);
	lv_obj_set_size(labelTouch, 120, 50);

	labelRawTouch =  lv_label_create(screen);
	lv_label_set_text_fmt(labelRawTouch, "Raw Touch: x = %d, y = %d", 0, 0);
	lv_obj_set_pos(labelRawTouch, 60, 80);
	lv_obj_set_size(labelRawTouch, 120, 50);

	lv_obj_t* btn = lv_button_create(screen);
	lv_obj_set_pos(btn, 60, 135);
	lv_obj_set_size(btn, 120, 50);

	lv_obj_t* btnLabel = lv_label_create(btn);
	lv_label_set_text(btnLabel, "Button");

	static lv_style_t style_btn_pressed;
	lv_style_init(&style_btn_pressed);
	lv_style_set_bg_color(&style_btn_pressed, lv_palette_main(LV_PALETTE_ORANGE));

	lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_PRESSED, btnLabel);
	lv_obj_t* labelB =  lv_label_create(screen);
	lv_label_set_text(labelB, "Big Font");
	lv_obj_set_pos(labelB, 40, 260);
	lv_obj_set_style_text_font(labelB, &lv_font_montserrat_36, LV_PART_MAIN);

	// Load screen
	lv_scr_load(screen);
}

void setLblText(void)
{
	lv_label_set_text_fmt(labelA, "counter=%d", ++counter);
}

void setTouchText(int16_t x, int16_t y, int16_t rawX, int16_t rawY)
{
	lv_label_set_text_fmt(labelTouch, "Pos Touch: x = %d, y = %d", x, y);
	lv_label_set_text_fmt(labelRawTouch, "Raw Touch: x = %d, y = %d", rawX, rawY);
}
