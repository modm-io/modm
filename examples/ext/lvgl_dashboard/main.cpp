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
#include <algorithm>
#include <cinttypes>
#include <cstdlib>
#include <cstring>

using namespace Board;
using namespace Board::ft6;

// ============================================================================
// Palette
namespace palette
{
constexpr uint32_t Background = 0x0b0f1c;
constexpr uint32_t Header     = 0x10162a;
constexpr uint32_t Card       = 0x161d33;
constexpr uint32_t Border     = 0x252f52;
constexpr uint32_t Text       = 0xe8ecff;
constexpr uint32_t Muted      = 0x8790b5;
constexpr uint32_t Blue       = 0x4f8cff;
constexpr uint32_t Mint       = 0x2ee6a6;
constexpr uint32_t Amber      = 0xffb547;
constexpr uint32_t Coral      = 0xff5c7a;
constexpr uint32_t Violet     = 0xa67bff;
}
static lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

// ============================================================================
// Display: two RGB565 framebuffers in SDRAM, LVGL renders directly into the
// hidden one and the LTDC swaps them during the vertical blanking period.
constexpr int32_t Width{800};
constexpr int32_t Height{480};
constexpr size_t FramebufferSize{Width * Height * sizeof(uint16_t)};

static void
display_flush(lv_display_t *display, const lv_area_t *, uint8_t *framebuffer)
{
	if (not lv_display_flush_is_last(display)) return;
	LTDC_Layer1->CFBAR = uint32_t(framebuffer);
	LTDC->SRCR = LTDC_SRCR_VBR;
}

static void
display_flush_wait(lv_display_t *)
{
	// The reload bit clears once the LTDC shows the new framebuffer
	while (LTDC->SRCR & LTDC_SRCR_VBR) ;
}

// Performance counters shown in the UI
static struct
{
	modm::PreciseClock::time_point render_start;
	uint32_t render_us{};
	uint32_t frames{};
	uint32_t fps{};
} stats;

static void
display_initialize()
{
	Board::initializeDisplay();
	auto *buffer1 = new (modm::MemoryExternal) uint8_t[FramebufferSize];
	auto *buffer2 = new (modm::MemoryExternal) uint8_t[FramebufferSize];
	std::memset(buffer1, 0, FramebufferSize);
	std::memset(buffer2, 0, FramebufferSize);
	// LVGL renders into buffer1 first, so show buffer2 meanwhile
	Board::setDisplayBuffer(buffer2);

	lv_display_t *display = lv_display_create(Width, Height);
	lv_display_set_buffers(display, buffer1, buffer2, FramebufferSize, LV_DISPLAY_RENDER_MODE_DIRECT);
	lv_display_set_flush_cb(display, display_flush);
	lv_display_set_flush_wait_cb(display, display_flush_wait);

	lv_display_add_event_cb(display, [](lv_event_t *)
	{
		stats.render_start = modm::PreciseClock::now();
	}, LV_EVENT_RENDER_START, nullptr);
	lv_display_add_event_cb(display, [](lv_event_t *)
	{
		const uint32_t us = (modm::PreciseClock::now() - stats.render_start).count();
		stats.render_us = (stats.render_us * 7 + us) / 8;
		stats.frames++;
	}, LV_EVENT_RENDER_READY, nullptr);

	lv_theme_t *theme = lv_theme_default_init(display, color(palette::Blue), color(palette::Mint),
											  true, &lv_font_montserrat_14);
	lv_display_set_theme(display, theme);
}

// ============================================================================
// Touchscreen
static Touch::Data touchData;
static Touch touch{touchData, TouchAddress};

static void
touch_initialize()
{
	Board::initializeTouchscreen();
	touch.configure(Touch::InterruptMode::Trigger, 60, 60);

	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, [](lv_indev_t *, lv_indev_data_t *data)
	{
		touch.readTouches();
		Touch::touch_t tp;
		touch.getData().getTouch(&tp, 0);
		if (tp.event == Touch::Event::Contact)
		{
			// The touch panel is rotated against the display
			data->point.x = tp.y;
			data->point.y = Height - tp.x;
			data->state = LV_INDEV_STATE_PRESSED;
		}
		else data->state = LV_INDEV_STATE_RELEASED;
	});
}

// ============================================================================
// Styles and helpers
static lv_style_t style_card;
static lv_style_t style_title;
static lv_style_t style_muted;

static void
styles_initialize()
{
	lv_style_init(&style_card);
	lv_style_set_bg_color(&style_card, color(palette::Card));
	lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
	lv_style_set_border_color(&style_card, color(palette::Border));
	lv_style_set_border_width(&style_card, 1);
	lv_style_set_radius(&style_card, 18);
	lv_style_set_pad_all(&style_card, 16);
	lv_style_set_shadow_width(&style_card, 0);

	lv_style_init(&style_title);
	lv_style_set_text_color(&style_title, color(palette::Muted));
	lv_style_set_text_font(&style_title, &lv_font_montserrat_14);
	lv_style_set_text_letter_space(&style_title, 1);

	lv_style_init(&style_muted);
	lv_style_set_text_color(&style_muted, color(palette::Muted));
}

static lv_obj_t *
card(lv_obj_t *parent, int32_t width, int32_t height, const char *title)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_remove_style_all(obj);
	lv_obj_add_style(obj, &style_card, 0);
	lv_obj_set_size(obj, width, height);
	lv_obj_set_scrollable(obj, false);
	lv_obj_t *label = lv_label_create(obj);
	lv_obj_add_style(label, &style_title, 0);
	lv_label_set_text(label, title);
	return obj;
}

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
ring(lv_obj_t *parent, int32_t size, int32_t width, uint32_t ring_color)
{
	lv_obj_t *arc = lv_arc_create(parent);
	lv_obj_set_size(arc, size, size);
	lv_obj_remove_style(arc, nullptr, LV_PART_KNOB);
	lv_obj_set_clickable(arc, false);
	lv_obj_set_style_arc_width(arc, width, LV_PART_MAIN);
	lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
	lv_obj_set_style_arc_color(arc, color(ring_color), LV_PART_MAIN);
	lv_obj_set_style_arc_opa(arc, LV_OPA_20, LV_PART_MAIN);
	lv_obj_set_style_arc_color(arc, color(ring_color), LV_PART_INDICATOR);
	return arc;
}

template< typename Callback >
static void
animate(void *var, int32_t from, int32_t to, uint32_t duration, Callback cb)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, var);
	lv_anim_set_values(&a, from, to);
	lv_anim_set_duration(&a, duration);
	lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
	lv_anim_set_exec_cb(&a, cb);
	lv_anim_start(&a);
}

// ============================================================================
// Header with title, render statistics and uptime
static lv_obj_t *header_stats;
static lv_obj_t *header_uptime;

static void
header_create(lv_obj_t *parent)
{
	lv_obj_t *header = lv_obj_create(parent);
	lv_obj_remove_style_all(header);
	lv_obj_set_size(header, Width, 56);
	lv_obj_set_style_bg_color(header, color(palette::Header), 0);
	lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
	lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
	lv_obj_set_style_border_color(header, color(palette::Border), 0);
	lv_obj_set_style_border_width(header, 1, 0);
	lv_obj_set_style_pad_hor(header, 20, 0);

	lv_obj_t *logo = label(header, &lv_font_montserrat_28, palette::Blue, "modm");
	lv_obj_align(logo, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_t *subtitle = label(header, &lv_font_montserrat_14, palette::Muted);
	lv_label_set_text_fmt(subtitle, "LVGL %d.%d  " LV_SYMBOL_RIGHT "  STM32F469 Discovery",
						  LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR);
	lv_obj_align_to(subtitle, logo, LV_ALIGN_OUT_RIGHT_MID, 14, 3);

	header_uptime = label(header, &lv_font_montserrat_20, palette::Text, "00:00:00");
	lv_obj_align(header_uptime, LV_ALIGN_RIGHT_MID, 0, 0);

	// A chip that tells which renderer draws the fills and images
	lv_obj_t *chip = lv_obj_create(header);
	lv_obj_remove_style_all(chip);
	lv_obj_set_size(chip, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_pad_hor(chip, 10, 0);
	lv_obj_set_style_pad_ver(chip, 4, 0);
	lv_obj_set_style_radius(chip, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_opa(chip, LV_OPA_20, 0);
#if LV_USE_DRAW_DMA2D
	lv_obj_set_style_bg_color(chip, color(palette::Mint), 0);
	label(chip, &lv_font_montserrat_14, palette::Mint, LV_SYMBOL_CHARGE " DMA2D");
#else
	lv_obj_set_style_bg_color(chip, color(palette::Amber), 0);
	label(chip, &lv_font_montserrat_14, palette::Amber, "Software");
#endif
	lv_obj_align(chip, LV_ALIGN_RIGHT_MID, -120, 0);

	header_stats = label(header, &lv_font_montserrat_14, palette::Muted);
	lv_obj_align(header_stats, LV_ALIGN_RIGHT_MID, -240, 0);
}

// ============================================================================
// Overview: gauge, system rings and a sparkline
constexpr int32_t DialSize{246};
constexpr lv_point_t DialCenter{134, 26 + DialSize / 2};
constexpr int32_t NeedleLength{76};
constexpr int32_t MaxRpm{8000};

static struct
{
	lv_obj_t *needle;
	lv_point_precise_t points[2];
	lv_obj_t *arc;
	lv_obj_t *value;
	int32_t rpm;
	uint32_t color;
} gauge;

static struct
{
	lv_obj_t *rings[3];
	lv_obj_t *labels[3];
} system_card;

static struct
{
	lv_obj_t *chart;
	lv_chart_series_t *series;
	lv_obj_t *value;
	int32_t pressure{10130};
} pressure_card;

static void
gauge_set(int32_t rpm)
{
	gauge.rpm = rpm;
	// Place the needle object tightly around the line, so that moving it only redraws a small area
	const int32_t angle = 135 + 270 * rpm / MaxRpm;
	const int32_t dx = (NeedleLength * lv_trigo_cos(angle)) >> LV_TRIGO_SHIFT;
	const int32_t dy = (NeedleLength * lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT;
	const int32_t left = std::min<int32_t>(0, dx), top = std::min<int32_t>(0, dy);
	gauge.points[0] = {lv_value_precise_t(-left), lv_value_precise_t(-top)};
	gauge.points[1] = {lv_value_precise_t(dx - left), lv_value_precise_t(dy - top)};
	lv_obj_set_pos(gauge.needle, DialCenter.x + left, DialCenter.y + top);
	lv_line_set_points(gauge.needle, gauge.points, 2);

	lv_arc_set_value(gauge.arc, rpm);
	lv_label_set_text_fmt(gauge.value, "%" LV_PRId32, rpm);
	// Changing a style redraws the whole widget, so only do it when the zone changes
	const uint32_t zone_color = rpm > 6500 ? palette::Coral : rpm > 5000 ? palette::Amber : palette::Blue;
	if (zone_color == gauge.color) return;
	gauge.color = zone_color;
	lv_obj_set_style_line_color(gauge.needle, color(zone_color), 0);
	lv_obj_set_style_arc_color(gauge.arc, color(zone_color), LV_PART_INDICATOR);
}

static lv_obj_t *
dial_create(lv_obj_t *parent)
{
	lv_obj_t *dial = lv_obj_create(parent);
	lv_obj_remove_style_all(dial);
	lv_obj_set_size(dial, DialSize, DialSize);

	lv_obj_t *track = ring(dial, DialSize, 8, palette::Blue);
	lv_arc_set_rotation(track, 135);
	lv_arc_set_bg_angles(track, 0, 270);
	lv_obj_set_style_arc_opa(track, LV_OPA_TRANSP, LV_PART_INDICATOR);

	lv_obj_t *scale = lv_scale_create(dial);
	lv_obj_set_size(scale, 206, 206);
	lv_obj_center(scale);
	lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
	lv_scale_set_range(scale, 0, MaxRpm);
	lv_scale_set_angle_range(scale, 270);
	lv_scale_set_rotation(scale, 135);
	lv_scale_set_total_tick_count(scale, 41);
	lv_scale_set_major_tick_every(scale, 5);
	lv_scale_set_label_show(scale, true);
	static const char *labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", nullptr};
	lv_scale_set_text_src(scale, labels);
	lv_obj_set_style_length(scale, 6, LV_PART_ITEMS);
	lv_obj_set_style_length(scale, 12, LV_PART_INDICATOR);
	lv_obj_set_style_line_color(scale, color(palette::Border), LV_PART_ITEMS);
	lv_obj_set_style_line_color(scale, color(palette::Muted), LV_PART_INDICATOR);
	lv_obj_set_style_line_width(scale, 2, LV_PART_INDICATOR);
	lv_obj_set_style_text_color(scale, color(palette::Muted), LV_PART_INDICATOR);
	lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);

	// Red zone
	static lv_style_t style_red;
	lv_style_init(&style_red);
	lv_style_set_line_color(&style_red, color(palette::Coral));
	lv_style_set_text_color(&style_red, color(palette::Coral));
	lv_scale_section_t *section = lv_scale_add_section(scale);
	lv_scale_set_section_range(scale, section, 6500, MaxRpm);
	lv_scale_set_section_style_indicator(scale, section, &style_red);
	lv_scale_set_section_style_items(scale, section, &style_red);

	// Drawing the 41 anti-aliased ticks and the track is the most expensive part of
	// the gauge, so render them once into an image that the DMA2D blends every frame.
	lv_obj_update_layout(dial);
	lv_draw_buf_t *snapshot = lv_snapshot_take(dial, LV_COLOR_FORMAT_ARGB8888);
	if (not snapshot) return dial;
	lv_obj_delete(dial);
	lv_obj_t *image = lv_image_create(parent);
	lv_image_set_src(image, snapshot);
	return image;
}

static void
gauge_create(lv_obj_t *parent)
{
	lv_obj_t *obj = card(parent, 300, LV_PCT(100), "MOTOR SPEED");

	lv_obj_t *dial = dial_create(obj);
	lv_obj_align(dial, LV_ALIGN_TOP_MID, 0, DialCenter.y - DialSize / 2);

	gauge.arc = ring(obj, DialSize, 8, palette::Blue);
	lv_arc_set_rotation(gauge.arc, 135);
	lv_arc_set_bg_angles(gauge.arc, 0, 270);
	lv_arc_set_range(gauge.arc, 0, MaxRpm);
	lv_obj_set_style_arc_opa(gauge.arc, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_align_to(gauge.arc, dial, LV_ALIGN_CENTER, 0, 0);

	gauge.needle = lv_line_create(obj);
	lv_obj_set_style_line_width(gauge.needle, 5, 0);
	lv_obj_set_style_line_rounded(gauge.needle, true, 0);

	lv_obj_t *hub = lv_obj_create(obj);
	lv_obj_remove_style_all(hub);
	lv_obj_set_size(hub, 18, 18);
	lv_obj_set_style_radius(hub, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_color(hub, color(palette::Text), 0);
	lv_obj_set_style_bg_opa(hub, LV_OPA_COVER, 0);
	lv_obj_set_pos(hub, DialCenter.x - 9, DialCenter.y - 9);

	gauge.value = label(obj, &lv_font_montserrat_48, palette::Text);
	lv_obj_align(gauge.value, LV_ALIGN_BOTTOM_MID, 0, -22);
	lv_obj_t *unit = label(obj, &lv_font_montserrat_14, palette::Muted, "RPM");
	lv_obj_align(unit, LV_ALIGN_BOTTOM_MID, 0, 0);
	gauge_set(0);

	// Drive the gauge to a new random speed every few seconds
	lv_timer_create([](lv_timer_t *timer)
	{
		animate(nullptr, gauge.rpm, lv_rand(800, MaxRpm - 200), 1600,
				[](void *, int32_t value) { gauge_set(value); });
		lv_timer_set_period(timer, lv_rand(1800, 3000));
	}, 100, nullptr);
}

static void
system_create(lv_obj_t *parent)
{
	lv_obj_t *obj = card(parent, LV_PCT(100), 196, "SYSTEM");

	static constexpr uint32_t colors[] = {palette::Coral, palette::Mint, palette::Violet};
	for (int ii = 0; ii < 3; ii++)
	{
		system_card.rings[ii] = ring(obj, 150 - ii * 36, 14, colors[ii]);
		lv_arc_set_bg_angles(system_card.rings[ii], 0, 360);
		lv_arc_set_rotation(system_card.rings[ii], 270);
		lv_obj_align(system_card.rings[ii], LV_ALIGN_LEFT_MID, 4 + ii * 18, 10);

		lv_obj_t *dot = lv_obj_create(obj);
		lv_obj_remove_style_all(dot);
		lv_obj_set_size(dot, 10, 10);
		lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
		lv_obj_set_style_bg_color(dot, color(colors[ii]), 0);
		lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
		lv_obj_align(dot, LV_ALIGN_LEFT_MID, 178, -28 + ii * 34);

		system_card.labels[ii] = label(obj, &lv_font_montserrat_20, palette::Text);
		lv_obj_align_to(system_card.labels[ii], dot, LV_ALIGN_OUT_RIGHT_MID, 12, 0);
	}
}

static void
system_update(uint32_t load)
{
	const uint32_t render = stats.render_us;
	lv_arc_set_value(system_card.rings[0], load);
	lv_arc_set_value(system_card.rings[1], std::min<uint32_t>(stats.fps * 100 / 60, 100));
	lv_arc_set_value(system_card.rings[2], std::min<uint32_t>(render / 160, 100));
	lv_label_set_text_fmt(system_card.labels[0], "%" PRIu32 " %% busy", load);
	lv_label_set_text_fmt(system_card.labels[1], "%" PRIu32 " fps", stats.fps);
	lv_label_set_text_fmt(system_card.labels[2], "%" PRIu32 ".%" PRIu32 " ms render",
						  render / 1000, (render / 100) % 10);
}

static void
pressure_create(lv_obj_t *parent)
{
	lv_obj_t *obj = card(parent, LV_PCT(100), LV_PCT(100), "AIR PRESSURE");
	lv_obj_set_flex_grow(obj, 1);

	pressure_card.value = label(obj, &lv_font_montserrat_28, palette::Text);
	lv_obj_align(pressure_card.value, LV_ALIGN_TOP_RIGHT, 0, -6);

	lv_obj_t *chart = pressure_card.chart = lv_chart_create(obj);
	lv_obj_set_size(chart, LV_PCT(100), 110);
	lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(chart, 0, 0);
	lv_obj_set_style_pad_all(chart, 4, 0);
	lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
	lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(chart, 3, 0);
	lv_obj_set_style_line_color(chart, color(palette::Border), LV_PART_MAIN);
	lv_chart_set_point_count(chart, 48);
	lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 10080, 10180);
	pressure_card.series = lv_chart_add_series(chart, color(palette::Amber), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_all_values(chart, pressure_card.series, pressure_card.pressure);

	lv_timer_create([](lv_timer_t *)
	{
		auto &p = pressure_card.pressure;
		p = std::clamp<int32_t>(p + int32_t(lv_rand(0, 12)) - 6, 10090, 10170);
		lv_chart_set_next_value(pressure_card.chart, pressure_card.series, p);
		lv_label_set_text_fmt(pressure_card.value, "%" LV_PRId32 ".%" LV_PRId32 " hPa", p / 10, p % 10);
	}, 250, nullptr);
}

static void
overview_create(lv_obj_t *tab)
{
	lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_gap(tab, 12, 0);
	gauge_create(tab);

	lv_obj_t *column = lv_obj_create(tab);
	lv_obj_remove_style_all(column);
	lv_obj_set_height(column, LV_PCT(100));
	lv_obj_set_flex_grow(column, 1);
	lv_obj_set_flex_flow(column, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(column, 12, 0);
	system_create(column);
	pressure_create(column);
}

// ============================================================================
// Controls: the four user LEDs, a chase animation and the user button
struct LedControl
{
	const char *name;
	uint32_t color;
	void (*set)(bool);
	lv_obj_t *led;
	lv_obj_t *toggle;
};
static LedControl leds[] =
{
	{"GREEN",  palette::Mint,  [](bool on) { LedGreen::set(on); }, nullptr, nullptr},
	{"ORANGE", palette::Amber, [](bool on) { LedOrange::set(on); }, nullptr, nullptr},
	{"RED",    palette::Coral, [](bool on) { LedRed::set(on); }, nullptr, nullptr},
	{"BLUE",   palette::Blue,  [](bool on) { LedBlue::set(on); }, nullptr, nullptr},
};

static void
led_set(LedControl &led, bool on)
{
	led.set(on);
	lv_obj_set_checked(led.toggle, on);
	if (on) lv_led_on(led.led);
	else lv_led_off(led.led);
}

static struct
{
	lv_timer_t *chase;
	lv_obj_t *speed;
	lv_obj_t *button_led;
	lv_obj_t *button_count;
	uint32_t presses;
	uint8_t position;
} controls;

static void
controls_create(lv_obj_t *tab)
{
	lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_pad_gap(tab, 12, 0);

	for (auto &led : leds)
	{
		lv_obj_t *obj = card(tab, 158, 190, led.name);
		led.led = lv_led_create(obj);
		lv_obj_set_size(led.led, 48, 48);
		lv_led_set_color(led.led, color(led.color));
		lv_obj_align(led.led, LV_ALIGN_CENTER, 0, -6);
		led.toggle = lv_switch_create(obj);
		lv_obj_set_style_bg_color(led.toggle, color(led.color), LV_PART_INDICATOR | LV_STATE_CHECKED);
		lv_obj_align(led.toggle, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_obj_add_event_cb(led.toggle, [](lv_event_t *e)
		{
			auto &led = *static_cast<LedControl *>(lv_event_get_user_data(e));
			led_set(led, lv_obj_has_state(led.toggle, LV_STATE_CHECKED));
		}, LV_EVENT_VALUE_CHANGED, &led);
		led_set(led, false);
	}

	lv_obj_t *obj = card(tab, LV_PCT(100), 190, "LED CHASE");
	lv_obj_t *chase = lv_switch_create(obj);
	lv_obj_align(chase, LV_ALIGN_TOP_LEFT, 0, 34);
	lv_obj_t *speed_label = label(obj, &lv_font_montserrat_14, palette::Muted, "Speed");
	lv_obj_align(speed_label, LV_ALIGN_TOP_LEFT, 0, 96);
	controls.speed = lv_slider_create(obj);
	lv_obj_set_width(controls.speed, 300);
	lv_slider_set_range(controls.speed, 1, 20);
	lv_slider_set_value(controls.speed, 8, LV_ANIM_OFF);
	lv_obj_align(controls.speed, LV_ALIGN_TOP_LEFT, 0, 126);

	controls.chase = lv_timer_create([](lv_timer_t *)
	{
		controls.position = (controls.position + 1) % 6;
		// Bounce back and forth: 0 1 2 3 2 1
		const uint8_t active = controls.position < 4 ? controls.position : 6 - controls.position;
		for (uint8_t ii = 0; ii < 4; ii++) led_set(leds[ii], ii == active);
	}, 1000 / 8, nullptr);
	lv_timer_pause(controls.chase);

	lv_obj_add_event_cb(chase, [](lv_event_t *e)
	{
		if (lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
			lv_timer_resume(controls.chase);
		} else {
			lv_timer_pause(controls.chase);
			for (auto &led : leds) led_set(led, false);
		}
	}, LV_EVENT_VALUE_CHANGED, nullptr);
	lv_obj_add_event_cb(controls.speed, [](lv_event_t *)
	{
		lv_timer_set_period(controls.chase, 1000 / lv_slider_get_value(controls.speed));
	}, LV_EVENT_VALUE_CHANGED, nullptr);

	// Mirror the blue user button
	lv_obj_t *button_title = label(obj, &lv_font_montserrat_14, palette::Muted, "USER BUTTON");
	lv_obj_align(button_title, LV_ALIGN_TOP_RIGHT, -60, 0);
	controls.button_led = lv_led_create(obj);
	lv_obj_set_size(controls.button_led, 64, 64);
	lv_led_set_color(controls.button_led, color(palette::Blue));
	lv_led_off(controls.button_led);
	lv_obj_align(controls.button_led, LV_ALIGN_RIGHT_MID, -86, 4);
	controls.button_count = label(obj, &lv_font_montserrat_14, palette::Muted);
	lv_obj_align(controls.button_count, LV_ALIGN_BOTTOM_RIGHT, -60, 0);
	lv_label_set_text(controls.button_count, "0 presses");

	lv_timer_create([](lv_timer_t *)
	{
		static bool pressed{false};
		if (pressed == Button::read()) return;
		pressed = not pressed;
		if (pressed) {
			lv_led_on(controls.button_led);
			lv_label_set_text_fmt(controls.button_count, "%" PRIu32 " presses", ++controls.presses);
		} else lv_led_off(controls.button_led);
	}, 20, nullptr);
}

// ============================================================================
// Trends: frame rate and render time history, and a spectrum analyzer
static struct
{
	lv_obj_t *chart;
	lv_chart_series_t *fps;
	lv_chart_series_t *render;
	lv_obj_t *spectrum;
	lv_chart_series_t *bins;
} trends;

static void
legend(lv_obj_t *parent, uint32_t legend_color, const char *text, int32_t x)
{
	lv_obj_t *dot = lv_obj_create(parent);
	lv_obj_remove_style_all(dot);
	lv_obj_set_size(dot, 10, 10);
	lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_color(dot, color(legend_color), 0);
	lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
	lv_obj_align(dot, LV_ALIGN_TOP_RIGHT, x, 4);
	lv_obj_t *text_label = label(parent, &lv_font_montserrat_14, palette::Muted, text);
	lv_obj_align_to(text_label, dot, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
}

static void
trends_create(lv_obj_t *tab)
{
	lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(tab, 12, 0);

	lv_obj_t *obj = card(tab, LV_PCT(100), 226, "PERFORMANCE  " LV_SYMBOL_RIGHT "  LAST 30 S");
	legend(obj, palette::Mint, "fps", -190);
	legend(obj, palette::Amber, "render ms x4", -90);
	lv_obj_t *chart = trends.chart = lv_chart_create(obj);
	lv_obj_set_size(chart, LV_PCT(100), 166);
	lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(chart, 0, 0);
	lv_obj_set_style_line_color(chart, color(palette::Border), LV_PART_MAIN);
	lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
	lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
	lv_chart_set_div_line_count(chart, 5, 7);
	lv_chart_set_point_count(chart, 60);
	lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 80);
	trends.fps = lv_chart_add_series(chart, color(palette::Mint), LV_CHART_AXIS_PRIMARY_Y);
	trends.render = lv_chart_add_series(chart, color(palette::Amber), LV_CHART_AXIS_PRIMARY_Y);

	obj = card(tab, LV_PCT(100), 162, "SPECTRUM");
	lv_obj_t *spectrum = trends.spectrum = lv_chart_create(obj);
	lv_obj_set_size(spectrum, LV_PCT(100), 112);
	lv_obj_align(spectrum, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(spectrum, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(spectrum, 0, 0);
	lv_obj_set_style_pad_all(spectrum, 0, 0);
	lv_obj_set_style_pad_column(spectrum, 4, 0);
	lv_obj_set_style_radius(spectrum, 3, LV_PART_ITEMS);
	lv_chart_set_type(spectrum, LV_CHART_TYPE_BAR);
	lv_chart_set_div_line_count(spectrum, 0, 0);
	lv_chart_set_point_count(spectrum, 40);
	lv_chart_set_axis_range(spectrum, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
	trends.bins = lv_chart_add_series(spectrum, color(palette::Violet), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_all_values(spectrum, trends.bins, 0);

	// Bins jump up at random and fall back slowly like a peak meter
	lv_timer_create([](lv_timer_t *)
	{
		int32_t *bins = lv_chart_get_series_y_array(trends.spectrum, trends.bins);
		const uint32_t peak = lv_rand(0, 39);
		for (int32_t ii = 0; ii < 40; ii++)
		{
			const int32_t distance = std::abs(ii - int32_t(peak));
			const int32_t target = std::max<int32_t>(0, 95 - distance * 12) * lv_rand(40, 100) / 100;
			bins[ii] = std::max(target, bins[ii] - 6);
		}
		lv_chart_refresh(trends.spectrum);
	}, 50, nullptr);
}

// ============================================================================
static void
ui_create()
{
	styles_initialize();
	lv_obj_t *screen = lv_screen_active();
	lv_obj_set_style_bg_color(screen, color(palette::Background), 0);
	lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_scrollable(screen, false);

	header_create(screen);

	lv_obj_t *tabview = lv_tabview_create(screen);
	lv_obj_set_width(tabview, Width);
	lv_obj_set_flex_grow(tabview, 1);
	lv_obj_set_style_bg_opa(tabview, LV_OPA_TRANSP, 0);
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
	lv_tabview_set_tab_bar_size(tabview, 96);

	lv_obj_t *bar = lv_tabview_get_tab_bar(tabview);
	lv_obj_set_style_bg_color(bar, color(palette::Header), 0);
	lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_RIGHT, 0);
	lv_obj_set_style_border_color(bar, color(palette::Border), 0);
	lv_obj_set_style_border_width(bar, 1, 0);
	lv_obj_set_style_text_color(bar, color(palette::Muted), 0);

	lv_obj_t *tabs[] = {
		lv_tabview_add_tab(tabview, LV_SYMBOL_HOME "\nOverview"),
		lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS "\nControls"),
		lv_tabview_add_tab(tabview, LV_SYMBOL_IMAGE "\nTrends"),
	};
	for (auto *tab : tabs)
	{
		lv_obj_set_style_pad_all(tab, 12, 0);
		lv_obj_set_scrollable(tab, false);
	}
	for (uint32_t ii = 0; ii < 3; ii++)
	{
		lv_obj_t *button = lv_tabview_get_tab_button(tabview, ii);
		lv_obj_set_style_text_align(button, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_color(button, color(palette::Text), LV_STATE_CHECKED);
		lv_obj_set_style_bg_color(button, color(palette::Card), LV_STATE_CHECKED);
		lv_obj_set_style_border_color(button, color(palette::Blue), LV_STATE_CHECKED);
		lv_obj_set_style_border_side(button, LV_BORDER_SIDE_LEFT, LV_STATE_CHECKED);
	}
	overview_create(tabs[0]);
	controls_create(tabs[1]);
	trends_create(tabs[2]);

	// Update all statistics once per second
	lv_timer_create([](lv_timer_t *)
	{
		stats.fps = stats.frames;
		stats.frames = 0;
		const uint32_t render = stats.render_us;
		// Fraction of each second spent rendering
		const uint32_t load = std::min<uint32_t>(render * stats.fps / 10'000, 100);

		system_update(load);
		lv_label_set_text_fmt(header_stats, "%" PRIu32 " fps  " LV_SYMBOL_BULLET "  %" PRIu32 ".%" PRIu32 " ms",
							  stats.fps, render / 1000, (render / 100) % 10);
		const uint32_t seconds = modm::Clock::now().time_since_epoch().count() / 1000;
		lv_label_set_text_fmt(header_uptime, "%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32,
							  seconds / 3600, (seconds / 60) % 60, seconds % 60);
	}, 1000, nullptr);

	// Two points per second for the performance history
	lv_timer_create([](lv_timer_t *)
	{
		lv_chart_set_next_value(trends.chart, trends.fps, stats.fps);
		lv_chart_set_next_value(trends.chart, trends.render, stats.render_us * 4 / 1000);
	}, 500, nullptr);
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "modm LVGL dashboard on the STM32F469 Discovery board" << modm::endl;
#if LV_USE_DRAW_DMA2D
	MODM_LOG_INFO << "Rendering with the DMA2D draw unit" << modm::endl;
#endif

	display_initialize();
	touch_initialize();
	ui_create();

	while (true) lv_timer_handler();
	return 0;
}
