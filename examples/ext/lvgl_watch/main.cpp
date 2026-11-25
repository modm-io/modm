/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <lvgl/lvgl.h>
#include <cinttypes>

using namespace Board;

// A smartwatch style demo for the 240x240 LCD: swipe between a watch face,
// activity rings and a music player, or press the user button.

namespace palette
{
constexpr uint32_t Background = 0x05070d;
constexpr uint32_t Face       = 0x141a2e;
constexpr uint32_t Border     = 0x262f52;
constexpr uint32_t Text       = 0xeef1ff;
constexpr uint32_t Muted      = 0x8790b5;
constexpr uint32_t Coral      = 0xff4d6d;
constexpr uint32_t Lime       = 0xa6ff3b;
constexpr uint32_t Cyan       = 0x3bdcff;
constexpr uint32_t Mint       = 0x2ee6a6;
constexpr uint32_t Violet     = 0x8f6bff;
}
static lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

constexpr int32_t Size{240};
constexpr lv_point_t Center{Size / 2, Size / 2};

// ============================================================================
// Display: LVGL renders 80 lines at a time into internal RAM, which the CPU
// then writes into the display RAM of the ST7789 over the FMC.
static void
display_flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
	const uint16_t width = lv_area_get_width(area);
	const uint16_t height = lv_area_get_height(area);
	getDisplay().setClipping(area->x1, area->y1, width, height);
	lcd::Interface::writePixels(reinterpret_cast<const uint16_t *>(pixels), width * height);
	lv_display_flush_ready(display);
}

static uint32_t frames;

static void
display_initialize()
{
	Board::initializeDisplay();
	alignas(4) static uint8_t buffer[Size * 80 * 2];
	lv_display_t *display = lv_display_create(Size, Size);
	lv_display_set_buffers(display, buffer, nullptr, sizeof(buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
	lv_display_set_flush_cb(display, display_flush);
	lv_display_add_event_cb(display, [](lv_event_t *) { frames++; }, LV_EVENT_RENDER_READY, nullptr);
	lv_display_set_theme(display, lv_theme_default_init(display, color(palette::Mint),
							color(palette::Violet), true, &lv_font_montserrat_12));
}

// ============================================================================
static ft6::Touch::Data touchData;
static ft6::Touch touch{touchData, ft6::TouchAddress};

static void
touch_initialize()
{
	Board::initializeTouchscreen();
	touch.configure(ft6::Touch::InterruptMode::Trigger, 60, 60);

	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, [](lv_indev_t *, lv_indev_data_t *data)
	{
		touch.readTouches();
		ft6::Touch::touch_t tp;
		touch.getData().getTouch(&tp, 0);
		if (tp.event == ft6::Touch::Event::Contact)
		{
			// The touch panel is rotated by 90 degrees against the display
			data->point.x = tp.y;
			data->point.y = Size - 1 - tp.x;
			data->state = LV_INDEV_STATE_PRESSED;
		}
		else data->state = LV_INDEV_STATE_RELEASED;
	});
}

// ============================================================================
static lv_obj_t *
label(lv_obj_t *parent, const lv_font_t *font, uint32_t text_color, const char *text = "")
{
	lv_obj_t *obj = lv_label_create(parent);
	lv_obj_set_style_text_font(obj, font, 0);
	lv_obj_set_style_text_color(obj, color(text_color), 0);
	lv_label_set_text(obj, text);
	return obj;
}

static lv_obj_t *
circle(lv_obj_t *parent, int32_t size, uint32_t fill)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_remove_style_all(obj);
	lv_obj_set_size(obj, size, size);
	lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_color(obj, color(fill), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_scrollable(obj, false);
	lv_obj_set_clickable(obj, false);
	return obj;
}

static lv_obj_t *
ring(lv_obj_t *parent, int32_t size, int32_t width, uint32_t ring_color)
{
	lv_obj_t *arc = lv_arc_create(parent);
	lv_obj_set_size(arc, size, size);
	lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
	lv_obj_set_clickable(arc, false);
	lv_arc_set_bg_angles(arc, 0, 360);
	lv_arc_set_rotation(arc, 270);
	lv_arc_set_value(arc, 0);
	lv_obj_set_style_arc_width(arc, width, LV_PART_MAIN);
	lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
	lv_obj_set_style_arc_color(arc, color(ring_color), LV_PART_MAIN);
	lv_obj_set_style_arc_opa(arc, LV_OPA_20, LV_PART_MAIN);
	lv_obj_set_style_arc_color(arc, color(ring_color), LV_PART_INDICATOR);
	lv_obj_center(arc);
	return arc;
}

static void
animate(void *var, int32_t from, int32_t to, uint32_t duration, uint32_t delay, lv_anim_exec_xcb_t cb)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, var);
	lv_anim_set_values(&a, from, to);
	lv_anim_set_duration(&a, duration);
	lv_anim_set_delay(&a, delay);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
	lv_anim_set_exec_cb(&a, cb);
	lv_anim_start(&a);
}

// ============================================================================
// Watch face with smooth second hand
struct Hand
{
	lv_obj_t *line;
	lv_point_precise_t points[2];
	int32_t length;
	int32_t angle{-1};

	void
	create(lv_obj_t *parent, int32_t hand_length, int32_t width, uint32_t hand_color)
	{
		length = hand_length;
		line = lv_line_create(parent);
		lv_obj_set_style_line_width(line, width, 0);
		lv_obj_set_style_line_rounded(line, true, 0);
		lv_obj_set_style_line_color(line, color(hand_color), 0);
	}

	// Only redraw when the hand actually moved, and keep the line object tight
	// around the hand, so that only a small area needs to be redrawn.
	void
	set(int32_t degrees)
	{
		if (degrees == angle) return;
		angle = degrees;
		const int32_t dx = (length * lv_trigo_cos(degrees - 90)) >> LV_TRIGO_SHIFT;
		const int32_t dy = (length * lv_trigo_sin(degrees - 90)) >> LV_TRIGO_SHIFT;
		const int32_t left = std::min<int32_t>(0, dx), top = std::min<int32_t>(0, dy);
		points[0] = {lv_value_precise_t(-left), lv_value_precise_t(-top)};
		points[1] = {lv_value_precise_t(dx - left), lv_value_precise_t(dy - top)};
		lv_obj_set_pos(line, Center.x + left, Center.y + top);
		lv_line_set_points(line, points, 2);
	}
};

static struct
{
	Hand hour, minute, second;
	lv_obj_t *digital;
} watch;

// The clock starts at the classic watch advertisement time
constexpr uint32_t StartTime_ms{(10 * 3600 + 8 * 60 + 30) * 1000};

static void
watch_create(lv_obj_t *tile)
{
	lv_obj_t *face = circle(tile, 236, palette::Face);
	lv_obj_set_style_border_color(face, color(palette::Border), 0);
	lv_obj_set_style_border_width(face, 2, 0);
	lv_obj_center(face);

	lv_obj_t *scale = lv_scale_create(tile);
	lv_obj_set_size(scale, 220, 220);
	lv_obj_center(scale);
	lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
	lv_scale_set_range(scale, 0, 60);
	lv_scale_set_angle_range(scale, 360);
	lv_scale_set_rotation(scale, 270);
	lv_scale_set_total_tick_count(scale, 61);
	lv_scale_set_major_tick_every(scale, 5);
	lv_scale_set_label_show(scale, true);
	static const char *labels[] = {"12", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "", nullptr};
	lv_scale_set_text_src(scale, labels);
	lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
	lv_obj_set_style_line_width(scale, 3, LV_PART_INDICATOR);
	// Short 1px ticks look skewed, since their ends are rounded to whole pixels
	lv_obj_set_style_line_opa(scale, LV_OPA_TRANSP, LV_PART_ITEMS);
	lv_obj_set_style_line_color(scale, color(palette::Text), LV_PART_INDICATOR);
	lv_obj_set_style_text_color(scale, color(palette::Muted), LV_PART_INDICATOR);
	lv_obj_set_style_text_font(scale, &lv_font_montserrat_20, LV_PART_INDICATOR);
	lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);

	// Mark the minutes with dots instead
	lv_obj_add_event_cb(face, [](lv_event_t *e)
	{
		lv_area_t coords;
		lv_obj_get_coords(lv_event_get_target_obj(e), &coords);
		const int32_t cx = (coords.x1 + coords.x2) / 2, cy = (coords.y1 + coords.y2) / 2;
		lv_draw_rect_dsc_t dot;
		lv_draw_rect_dsc_init(&dot);
		dot.bg_color = color(palette::Muted);
		dot.radius = LV_RADIUS_CIRCLE;
		for (int32_t minute = 0; minute < 60; minute++)
		{
			if (minute % 5 == 0) continue;
			const int32_t x = cx + ((106 * lv_trigo_cos(minute * 6)) >> LV_TRIGO_SHIFT);
			const int32_t y = cy + ((106 * lv_trigo_sin(minute * 6)) >> LV_TRIGO_SHIFT);
			const lv_area_t area{x - 1, y - 1, x + 1, y + 1};
			lv_draw_rect(lv_event_get_layer(e), &dot, &area);
		}
	}, LV_EVENT_DRAW_MAIN_END, nullptr);

	lv_obj_t *brand = label(tile, &lv_font_montserrat_12, palette::Mint, "modm");
	lv_obj_align(brand, LV_ALIGN_CENTER, 0, -42);
	watch.digital = label(tile, &lv_font_montserrat_12, palette::Muted);
	lv_obj_align(watch.digital, LV_ALIGN_CENTER, 0, 44);

	watch.hour.create(tile, 52, 7, palette::Text);
	watch.minute.create(tile, 78, 5, palette::Text);
	watch.second.create(tile, 92, 2, palette::Coral);
	lv_obj_t *hub = circle(tile, 12, palette::Coral);
	lv_obj_center(hub);

	lv_timer_create([](lv_timer_t *)
	{
		const uint32_t ms = StartTime_ms + modm::Clock::now().time_since_epoch().count();
		const uint32_t seconds = ms / 1000;
		watch.second.set((ms % 60'000) * 360 / 60'000);
		watch.minute.set((seconds % 3600) * 360 / 3600);
		watch.hour.set((seconds % 43'200) * 360 / 43'200);
		static uint32_t shown{~0ul};
		if (shown == seconds) return;
		shown = seconds;
		lv_label_set_text_fmt(watch.digital, "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32,
							  (seconds / 3600) % 24, (seconds / 60) % 60, seconds % 60);
	}, 30, nullptr);
}

// ============================================================================
// Activity rings that fill up whenever the tile is shown
static struct
{
	lv_obj_t *rings[3];
	lv_obj_t *steps;
} activity;

static void
activity_create(lv_obj_t *tile)
{
	activity.rings[0] = ring(tile, 228, 22, palette::Coral);
	activity.rings[1] = ring(tile, 180, 22, palette::Lime);
	activity.rings[2] = ring(tile, 132, 22, palette::Cyan);

	activity.steps = label(tile, &lv_font_montserrat_28, palette::Text, "0");
	lv_obj_align(activity.steps, LV_ALIGN_CENTER, 0, -8);
	lv_obj_t *unit = label(tile, &lv_font_montserrat_12, palette::Muted, "STEPS");
	lv_obj_align(unit, LV_ALIGN_CENTER, 0, 18);
}

static void
activity_animate()
{
	static constexpr int32_t goals[] = {86, 62, 94};
	for (int ii = 0; ii < 3; ii++)
	{
		animate(activity.rings[ii], 0, goals[ii], 1400, ii * 150, [](void *arc, int32_t value)
		{
			lv_arc_set_value(static_cast<lv_obj_t *>(arc), value);
		});
	}
	animate(activity.steps, 0, 8432, 1600, 0, [](void *steps, int32_t value)
	{
		lv_label_set_text_fmt(static_cast<lv_obj_t *>(steps), "%" LV_PRId32, value);
	});
}

// ============================================================================
// Music player with play/pause button
static struct
{
	lv_obj_t *spinner;
	lv_obj_t *progress;
	lv_obj_t *play;
	bool playing{true};
} music;

static lv_obj_t *
music_button(lv_obj_t *parent, const char *symbol, int32_t size, uint32_t fill, lv_opa_t opa)
{
	lv_obj_t *button = lv_button_create(parent);
	lv_obj_set_size(button, size, size);
	lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_color(button, color(fill), 0);
	lv_obj_set_style_bg_opa(button, opa, 0);
	lv_obj_set_style_shadow_width(button, 0, 0);
	lv_obj_t *icon = label(button, &lv_font_montserrat_20, palette::Text, symbol);
	lv_obj_center(icon);
	return button;
}

static void
music_create(lv_obj_t *tile)
{
	lv_obj_t *album = circle(tile, 84, palette::Violet);
	lv_obj_set_style_bg_grad_color(album, color(palette::Coral), 0);
	lv_obj_set_style_bg_grad_dir(album, LV_GRAD_DIR_VER, 0);
	lv_obj_align(album, LV_ALIGN_TOP_MID, 0, 22);
	lv_obj_center(label(album, &lv_font_montserrat_28, palette::Text, LV_SYMBOL_AUDIO));

	music.spinner = lv_spinner_create(tile);
	lv_obj_set_size(music.spinner, 104, 104);
	lv_spinner_set_anim_params(music.spinner, 2400, 90);
	lv_obj_set_style_arc_width(music.spinner, 4, LV_PART_MAIN);
	lv_obj_set_style_arc_width(music.spinner, 4, LV_PART_INDICATOR);
	lv_obj_set_style_arc_opa(music.spinner, LV_OPA_10, LV_PART_MAIN);
	lv_obj_set_style_arc_color(music.spinner, color(palette::Mint), LV_PART_INDICATOR);
	lv_obj_align(music.spinner, LV_ALIGN_TOP_MID, 0, 12);

	lv_obj_t *title = label(tile, &lv_font_montserrat_20, palette::Text,
							"Embedded Symphony No. 9 in C++   ");
	lv_label_set_long_mode(title, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
	lv_obj_set_width(title, 180);
	lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 124);
	lv_obj_t *artist = label(tile, &lv_font_montserrat_12, palette::Muted, "the modm ensemble");
	lv_obj_align(artist, LV_ALIGN_TOP_MID, 0, 150);

	music.progress = lv_bar_create(tile);
	lv_obj_set_size(music.progress, 160, 4);
	lv_bar_set_range(music.progress, 0, 1000);
	lv_obj_set_style_bg_color(music.progress, color(palette::Border), LV_PART_MAIN);
	lv_obj_set_style_bg_color(music.progress, color(palette::Mint), LV_PART_INDICATOR);
	lv_obj_align(music.progress, LV_ALIGN_TOP_MID, 0, 172);

	lv_obj_t *previous = music_button(tile, LV_SYMBOL_PREV, 36, palette::Face, LV_OPA_COVER);
	lv_obj_align(previous, LV_ALIGN_TOP_MID, -54, 188);
	lv_obj_t *next = music_button(tile, LV_SYMBOL_NEXT, 36, palette::Face, LV_OPA_COVER);
	lv_obj_align(next, LV_ALIGN_TOP_MID, 54, 188);
	music.play = music_button(tile, LV_SYMBOL_PAUSE, 44, palette::Mint, LV_OPA_COVER);
	lv_obj_align(music.play, LV_ALIGN_TOP_MID, 0, 184);

	lv_obj_add_event_cb(music.play, [](lv_event_t *e)
	{
		music.playing = not music.playing;
		lv_obj_t *icon = lv_obj_get_child(lv_event_get_target_obj(e), 0);
		lv_label_set_text(icon, music.playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
		lv_obj_set_hidden(music.spinner, not music.playing);
	}, LV_EVENT_CLICKED, nullptr);
	const auto skip = [](lv_event_t *) { lv_bar_set_value(music.progress, 0, LV_ANIM_OFF); };
	lv_obj_add_event_cb(previous, skip, LV_EVENT_CLICKED, nullptr);
	lv_obj_add_event_cb(next, skip, LV_EVENT_CLICKED, nullptr);

	lv_timer_create([](lv_timer_t *)
	{
		if (not music.playing) return;
		lv_bar_set_value(music.progress, (lv_bar_get_value(music.progress) + 4) % 1000, LV_ANIM_OFF);
	}, 250, nullptr);
}

// ============================================================================
static lv_obj_t *dots[3];

static void
dots_update(uint32_t active)
{
	for (uint32_t ii = 0; ii < 3; ii++)
	{
		lv_obj_set_width(dots[ii], ii == active ? 14 : 5);
		lv_obj_set_style_bg_opa(dots[ii], ii == active ? LV_OPA_COVER : LV_OPA_40, 0);
	}
}

static void
ui_create()
{
	lv_obj_t *screen = lv_screen_active();
	lv_obj_set_style_bg_color(screen, color(palette::Background), 0);

	lv_obj_t *tiles = lv_tileview_create(screen);
	lv_obj_set_style_bg_opa(tiles, LV_OPA_TRANSP, 0);
	lv_obj_set_scrollbar_mode(tiles, LV_SCROLLBAR_MODE_OFF);
	watch_create(lv_tileview_add_tile(tiles, 0, 0, LV_DIR_RIGHT));
	activity_create(lv_tileview_add_tile(tiles, 1, 0, LV_DIR_HOR));
	music_create(lv_tileview_add_tile(tiles, 2, 0, LV_DIR_LEFT));

	// Page indicator on the top layer, so it does not scroll with the tiles
	lv_obj_t *pager = lv_obj_create(lv_layer_top());
	lv_obj_remove_style_all(pager);
	lv_obj_set_size(pager, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(pager, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_column(pager, 4, 0);
	lv_obj_align(pager, LV_ALIGN_BOTTOM_MID, 0, -3);
	for (auto *&dot : dots)
	{
		dot = circle(pager, 5, palette::Text);
		lv_obj_set_style_radius(dot, 3, 0);
	}
	dots_update(0);

	lv_obj_add_event_cb(tiles, [](lv_event_t *e)
	{
		lv_obj_t *tiles = lv_event_get_target_obj(e);
		const uint32_t index = lv_obj_get_index(lv_tileview_get_tile_active(tiles));
		dots_update(index);
		if (index == 1) activity_animate();
	}, LV_EVENT_VALUE_CHANGED, nullptr);

	// The user button cycles through the tiles
	lv_timer_create([](lv_timer_t *timer)
	{
		static bool pressed{false};
		if (pressed == Button::read()) return;
		pressed = not pressed;
		if (not pressed) return;
		auto *tiles = static_cast<lv_obj_t *>(lv_timer_get_user_data(timer));
		const uint32_t index = lv_obj_get_index(lv_tileview_get_tile_active(tiles));
		lv_tileview_set_tile_by_index(tiles, (index + 1) % 3, 0, LV_ANIM_ON);
	}, 20, tiles);

	lv_timer_create([](lv_timer_t *)
	{
		MODM_LOG_INFO << frames / 5 << " fps" << modm::endl;
		frames = 0;
	}, 5000, nullptr);
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "modm LVGL watch demo on the STM32F723E Discovery board" << modm::endl;

	display_initialize();
	touch_initialize();
	ui_create();

	while (true) lv_timer_handler();
	return 0;
}
