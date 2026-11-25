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
#include <modm/driver/inertial/l3gd20.hpp>
#include <lvgl/lvgl.h>
#include <algorithm>
#include <cstring>

using namespace Board;

// Two tabs for the 240x320 LCD: the live rotation rates of the on-board gyroscope,
// and a thermostat that you set by dragging its knob on the resistive touch screen.

namespace palette
{
constexpr uint32_t Background = 0x0b0f1c;
constexpr uint32_t Card       = 0x161d33;
constexpr uint32_t Border     = 0x252f52;
constexpr uint32_t Text       = 0xe8ecff;
constexpr uint32_t Muted      = 0x8790b5;
constexpr uint32_t Coral      = 0xff5c7a;
constexpr uint32_t Lime       = 0xa6ff3b;
constexpr uint32_t Cyan       = 0x3bdcff;
constexpr uint32_t Mint       = 0x2ee6a6;
constexpr uint32_t Amber      = 0xffb547;
}
static lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

constexpr int32_t Width{240};
constexpr int32_t Height{320};

// ============================================================================
// Display: two framebuffers in SDRAM, swapped by the LTDC during vertical blanking
static void
display_initialize()
{
	constexpr size_t Size = Width * Height * sizeof(uint16_t);
	auto *buffer1 = new (modm::MemoryExternal) uint8_t[Size];
	auto *buffer2 = new (modm::MemoryExternal) uint8_t[Size];
	std::memset(buffer2, 0, Size);
	Board::initializeDisplay();
	Board::setDisplayBuffer(buffer2);

	lv_display_t *display = lv_display_create(Width, Height);
	lv_display_set_buffers(display, buffer1, buffer2, Size, LV_DISPLAY_RENDER_MODE_DIRECT);
	lv_display_set_flush_cb(display, [](lv_display_t *display, const lv_area_t *, uint8_t *buffer)
	{
		if (lv_display_flush_is_last(display)) Board::setDisplayBuffer(buffer);
	});
	lv_display_set_flush_wait_cb(display, [](lv_display_t *)
	{
		while (LTDC->SRCR & LTDC_SRCR_VBR) ;
	});
	lv_display_set_theme(display, lv_theme_default_init(display, color(palette::Mint),
							color(palette::Cyan), true, &lv_font_montserrat_12));
}

// ============================================================================
// Resistive touch: the raw 12-bit positions are mapped linearly to pixels.
// These calibration values are from the ST BSP and may need tuning for your panel.
constexpr int32_t TouchLeft{3870}, TouchRight{270};
constexpr int32_t TouchTop{360}, TouchBottom{3880};

static lcd::Touch touch{lcd::TouchAddress};

static void
touch_initialize()
{
	Board::initializeTouchscreen();
	touch.initialize();

	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, [](lv_indev_t *, lv_indev_data_t *data)
	{
		static lv_point_t last{};
		lcd::Touch::Point raw;
		if (touch.read(raw))
		{
			last.x = std::clamp<int32_t>((raw.x - TouchLeft) * Width / (TouchRight - TouchLeft), 0, Width - 1);
			last.y = std::clamp<int32_t>((raw.y - TouchTop) * Height / (TouchBottom - TouchTop), 0, Height - 1);
			data->state = LV_INDEV_STATE_PRESSED;
		}
		else data->state = LV_INDEV_STATE_RELEASED;
		data->point = last;
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
card(lv_obj_t *parent, int32_t height)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_remove_style_all(obj);
	lv_obj_set_size(obj, LV_PCT(100), height);
	lv_obj_set_style_bg_color(obj, color(palette::Card), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_style_border_color(obj, color(palette::Border), 0);
	lv_obj_set_style_border_width(obj, 1, 0);
	lv_obj_set_style_radius(obj, 14, 0);
	lv_obj_set_style_pad_all(obj, 10, 0);
	lv_obj_set_scrollable(obj, false);
	return obj;
}

// ============================================================================
// Motion: rotation rates of the L3GD20 or I3G4250D gyroscope
using GyroTransport = modm::Lis3TransportSpi< l3g::SpiMaster, l3g::Cs >;
static modm::l3gd20::Data gyroData;
static modm::L3gd20< GyroTransport > gyro{gyroData};

constexpr uint32_t AxisColors[] = {palette::Coral, palette::Lime, palette::Cyan};
constexpr int32_t MaxRate{250}; // degrees per second

static float
gyro_rate(int axis)
{
	auto &data = gyro.getData();
	return axis == 0 ? data.getX() : axis == 1 ? data.getY() : data.getZ();
}

static struct
{
	lv_obj_t *chart;
	lv_chart_series_t *series[3];
	lv_obj_t *bars[3];
	lv_obj_t *values[3];
	float bias[3];
	bool present;
} motion;

static void
motion_create(lv_obj_t *tab)
{
	lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_gap(tab, 8, 0);

	lv_obj_t *obj = card(tab, 160);
	label(obj, &lv_font_montserrat_12, palette::Muted, "ROTATION RATE");
	motion.chart = lv_chart_create(obj);
	lv_obj_set_size(motion.chart, LV_PCT(100), 124);
	lv_obj_align(motion.chart, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(motion.chart, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(motion.chart, 0, 0);
	lv_obj_set_style_pad_all(motion.chart, 0, 0);
	lv_obj_set_style_line_color(motion.chart, color(palette::Border), LV_PART_MAIN);
	lv_obj_set_style_line_width(motion.chart, 2, LV_PART_ITEMS);
	lv_obj_set_style_size(motion.chart, 0, 0, LV_PART_INDICATOR);
	lv_chart_set_div_line_count(motion.chart, 5, 0);
	lv_chart_set_point_count(motion.chart, 50);
	lv_chart_set_axis_range(motion.chart, LV_CHART_AXIS_PRIMARY_Y, -MaxRate, MaxRate);
	for (int ii = 0; ii < 3; ii++)
	{
		motion.series[ii] = lv_chart_add_series(motion.chart, color(AxisColors[ii]), LV_CHART_AXIS_PRIMARY_Y);
		lv_chart_set_all_values(motion.chart, motion.series[ii], 0);
	}

	obj = card(tab, 96);
	static constexpr const char *axes[] = {"X", "Y", "Z"};
	for (int ii = 0; ii < 3; ii++)
	{
		lv_obj_t *name = label(obj, &lv_font_montserrat_12, AxisColors[ii], axes[ii]);
		lv_obj_align(name, LV_ALIGN_TOP_LEFT, 0, ii * 26);
		motion.bars[ii] = lv_bar_create(obj);
		lv_obj_set_size(motion.bars[ii], 120, 8);
		lv_bar_set_range(motion.bars[ii], -MaxRate, MaxRate);
		lv_bar_set_mode(motion.bars[ii], LV_BAR_MODE_SYMMETRICAL);
		lv_obj_set_style_bg_color(motion.bars[ii], color(palette::Border), LV_PART_MAIN);
		lv_obj_set_style_bg_opa(motion.bars[ii], LV_OPA_COVER, LV_PART_MAIN);
		lv_obj_set_style_bg_color(motion.bars[ii], color(AxisColors[ii]), LV_PART_INDICATOR);
		lv_obj_align(motion.bars[ii], LV_ALIGN_TOP_LEFT, 18, ii * 26 + 4);
		motion.values[ii] = label(obj, &lv_font_montserrat_12, palette::Text);
		lv_obj_set_width(motion.values[ii], 64);
		lv_obj_set_style_text_align(motion.values[ii], LV_TEXT_ALIGN_RIGHT, 0);
		lv_obj_align(motion.values[ii], LV_ALIGN_TOP_RIGHT, 0, ii * 26);
	}

	motion.present = gyro.ping();
	if (motion.present)
	{
		// Average the zero-rate offset while the board lies still after reset
		gyro.configure(gyro.Scale::Dps250, gyro.MeasurementRate::Hz95);
		modm::delay(100ms);
		constexpr int Samples{32};
		for (int sample = 0; sample < Samples; sample++)
		{
			gyro.readRotation();
			for (int ii = 0; ii < 3; ii++) motion.bias[ii] += gyro_rate(ii) / Samples;
			modm::delay(11ms);
		}
	}
	else
	{
		lv_obj_t *missing = label(tab, &lv_font_montserrat_12, palette::Coral, "Gyroscope not found");
		lv_obj_set_ignore_layout(missing, true);
		lv_obj_center(missing);
	}

	lv_timer_create([](lv_timer_t *)
	{
		if (not motion.present) return;
		gyro.readRotation();
		int32_t rates[3];
		for (int ii = 0; ii < 3; ii++) rates[ii] = gyro_rate(ii) - motion.bias[ii];
		for (int ii = 0; ii < 3; ii++)
		{
			lv_chart_set_next_value(motion.chart, motion.series[ii], rates[ii]);
			lv_bar_set_value(motion.bars[ii], rates[ii], LV_ANIM_OFF);
			lv_label_set_text_fmt(motion.values[ii], "%" LV_PRId32 " dps", rates[ii]);
		}
	}, 40, nullptr);
}

// ============================================================================
// Climate: a thermostat knob in half degrees with three modes
static struct
{
	lv_obj_t *arc;
	lv_obj_t *value;
	lv_obj_t *mode;
} climate;

constexpr const char *ModeNames[] = {"HEATING", "COOLING", "AUTO"};
constexpr uint32_t ModeColors[] = {palette::Coral, palette::Cyan, palette::Mint};

static void
climate_update()
{
	const int32_t half_degrees = lv_arc_get_value(climate.arc);
	lv_label_set_text_fmt(climate.value, "%" LV_PRId32 ".%" LV_PRId32,
						  half_degrees / 2, (half_degrees % 2) * 5);
}

static void
climate_set_mode(uint32_t mode)
{
	lv_obj_set_style_arc_color(climate.arc, color(ModeColors[mode]), LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(climate.arc, color(ModeColors[mode]), LV_PART_KNOB);
	lv_obj_set_style_text_color(climate.mode, color(ModeColors[mode]), 0);
	lv_label_set_text(climate.mode, ModeNames[mode]);
}

static void
climate_create(lv_obj_t *tab)
{
	climate.arc = lv_arc_create(tab);
	lv_obj_set_size(climate.arc, 200, 200);
	lv_obj_align(climate.arc, LV_ALIGN_TOP_MID, 0, 0);
	lv_arc_set_rotation(climate.arc, 135);
	lv_arc_set_bg_angles(climate.arc, 0, 270);
	lv_arc_set_range(climate.arc, 2 * 16, 2 * 30);
	lv_arc_set_value(climate.arc, 2 * 21 + 1);
	lv_obj_set_style_arc_width(climate.arc, 14, LV_PART_MAIN);
	lv_obj_set_style_arc_width(climate.arc, 14, LV_PART_INDICATOR);
	lv_obj_set_style_arc_color(climate.arc, color(palette::Border), LV_PART_MAIN);
	lv_obj_set_style_pad_all(climate.arc, 6, LV_PART_KNOB);
	lv_obj_add_event_cb(climate.arc, [](lv_event_t *) { climate_update(); }, LV_EVENT_VALUE_CHANGED, nullptr);

	climate.value = label(climate.arc, &lv_font_montserrat_48, palette::Text);
	lv_obj_align(climate.value, LV_ALIGN_CENTER, 0, -6);
	lv_obj_t *unit = label(climate.arc, &lv_font_montserrat_12, palette::Muted, "TARGET  C");
	lv_obj_align(unit, LV_ALIGN_CENTER, 0, 30);
	climate.mode = label(climate.arc, &lv_font_montserrat_12, palette::Coral);
	lv_obj_align(climate.mode, LV_ALIGN_BOTTOM_MID, 0, -4);

	static const char *buttons[] = {"Heat", "Cool", "Auto", ""};
	lv_obj_t *modes = lv_buttonmatrix_create(tab);
	lv_buttonmatrix_set_map(modes, buttons);
	lv_buttonmatrix_set_one_checked(modes, true);
	lv_buttonmatrix_set_button_ctrl_all(modes, LV_BUTTONMATRIX_CTRL_CHECKABLE);
	lv_buttonmatrix_set_button_ctrl(modes, 0, LV_BUTTONMATRIX_CTRL_CHECKED);
	lv_obj_set_size(modes, LV_PCT(100), 48);
	lv_obj_align(modes, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_style_bg_opa(modes, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(modes, 0, 0);
	lv_obj_set_style_pad_all(modes, 0, 0);
	lv_obj_set_style_bg_color(modes, color(palette::Card), LV_PART_ITEMS);
	lv_obj_set_style_bg_color(modes, color(palette::Border), LV_PART_ITEMS | LV_STATE_CHECKED);
	lv_obj_add_event_cb(modes, [](lv_event_t *e)
	{
		climate_set_mode(lv_buttonmatrix_get_selected_button(lv_event_get_target_obj(e)));
	}, LV_EVENT_VALUE_CHANGED, nullptr);

	climate_update();
	climate_set_mode(0);
}

// ============================================================================
static void
ui_create()
{
	lv_obj_t *screen = lv_screen_active();
	lv_obj_set_style_bg_color(screen, color(palette::Background), 0);

	lv_obj_t *tabview = lv_tabview_create(screen);
	lv_obj_set_style_bg_opa(tabview, LV_OPA_TRANSP, 0);
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_BOTTOM);
	lv_tabview_set_tab_bar_size(tabview, 44);
	lv_obj_t *bar = lv_tabview_get_tab_bar(tabview);
	lv_obj_set_style_bg_color(bar, color(palette::Card), 0);
	lv_obj_set_style_text_color(bar, color(palette::Muted), 0);

	lv_obj_t *tabs[] = {
		lv_tabview_add_tab(tabview, LV_SYMBOL_REFRESH " Motion"),
		lv_tabview_add_tab(tabview, LV_SYMBOL_HOME " Climate"),
	};
	for (auto *tab : tabs)
	{
		lv_obj_set_style_pad_all(tab, 8, 0);
		lv_obj_set_scrollable(tab, false);
	}
	for (uint32_t ii = 0; ii < 2; ii++)
	{
		lv_obj_t *button = lv_tabview_get_tab_button(tabview, ii);
		lv_obj_set_style_text_color(button, color(palette::Text), LV_STATE_CHECKED);
		lv_obj_set_style_border_side(button, LV_BORDER_SIDE_TOP, LV_STATE_CHECKED);
	}
	motion_create(tabs[0]);
	climate_create(tabs[1]);
}

int
main()
{
	Board::initialize();
	display_initialize();
	touch_initialize();
	ui_create();

	while (true) lv_timer_handler();
	return 0;
}
