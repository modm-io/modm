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
#include <cmath>
#include <cstring>

using namespace Board;

// A studio mixer for the 480x272 touch screen: six channel strips with faders,
// LED level meters and mute/solo buttons, and a master section with a volume
// knob, a stereo meter and a spectrum. The audio is simulated from a drum
// pattern at 120 BPM and follows the faders and buttons.

namespace palette
{
constexpr uint32_t Background = 0x0d0f14;
constexpr uint32_t Panel      = 0x171b24;
constexpr uint32_t Border     = 0x2a3040;
constexpr uint32_t Text       = 0xe6e9f2;
constexpr uint32_t Muted      = 0x7c8499;
constexpr uint32_t Green      = 0x3ddc84;
constexpr uint32_t Yellow     = 0xffcc33;
constexpr uint32_t Red        = 0xff4455;
constexpr uint32_t Accent     = 0x4da3ff;
constexpr uint32_t Off        = 0x232836;
}
static lv_color_t color(uint32_t hex) { return lv_color_hex(hex); }

constexpr int32_t Width{lcd::Width};
constexpr int32_t Height{lcd::Height};

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
	lv_display_set_theme(display, lv_theme_default_init(display, color(palette::Accent),
							color(palette::Green), true, &lv_font_montserrat_12));
}

static ft6::Touch::Data touchData;
static ft6::Touch touch{touchData, ft6::TouchAddress};

static void
touch_initialize()
{
	Board::initializeTouchscreen();
	touch.configure(ft6::Touch::InterruptMode::Polling, 60, 60);

	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, [](lv_indev_t *, lv_indev_data_t *data)
	{
		touch.readTouches();
		ft6::Touch::touch_t tp;
		touch.getData().getTouch(&tp, 0);
		if (tp.event == ft6::Touch::Event::Contact)
		{
			// The touch controller reports the axes swapped
			data->point.x = tp.y;
			data->point.y = tp.x;
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
panel(lv_obj_t *parent, int32_t x, int32_t y, int32_t w, int32_t h)
{
	lv_obj_t *obj = lv_obj_create(parent);
	lv_obj_remove_style_all(obj);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);
	lv_obj_set_style_bg_color(obj, color(palette::Panel), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_style_border_color(obj, color(palette::Border), 0);
	lv_obj_set_style_border_width(obj, 1, 0);
	lv_obj_set_style_radius(obj, 6, 0);
	lv_obj_set_scrollable(obj, false);
	return obj;
}

// An LED level meter made of segments that only redraw when they change
struct Meter
{
	static constexpr int MaxSegments{16};
	lv_obj_t *segments[MaxSegments];
	int count;
	int lit{-1};
	int peak{0};
	uint32_t peak_time{0};

	void
	create(lv_obj_t *parent, int32_t x, int32_t y, int number, bool vertical, int32_t length, int32_t thickness)
	{
		count = number;
		const int32_t pitch = length / count;
		for (int ii = 0; ii < count; ii++)
		{
			lv_obj_t *led = segments[ii] = lv_obj_create(parent);
			lv_obj_remove_style_all(led);
			if (vertical)
			{
				lv_obj_set_size(led, thickness, pitch - 2);
				lv_obj_set_pos(led, x, y + length - (ii + 1) * pitch);
			}
			else
			{
				lv_obj_set_size(led, pitch - 2, thickness);
				lv_obj_set_pos(led, x + ii * pitch, y);
			}
			lv_obj_set_style_radius(led, 1, 0);
			lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
			lv_obj_set_style_bg_color(led, color(palette::Off), 0);
			lv_obj_set_clickable(led, false);
		}
	}

	uint32_t
	segment_color(int ii) const
	{
		if (ii >= count * 7 / 8) return palette::Red;
		if (ii >= count * 5 / 8) return palette::Yellow;
		return palette::Green;
	}

	void
	set(float level, uint32_t now)
	{
		const int on = std::clamp(int(level * count + 0.5f), 0, count);
		// The highest segment stays lit for a moment as peak hold
		if (on - 1 >= peak or now - peak_time > 700) { peak = on - 1; peak_time = now; }
		const int previous = lit;
		lit = on;
		for (int ii = 0; ii < count; ii++)
		{
			const bool was = ii < previous or ii == previous_peak;
			const bool is = ii < on or ii == peak;
			if (was != is or previous < 0)
				lv_obj_set_style_bg_color(segments[ii], color(is ? segment_color(ii) : palette::Off), 0);
		}
		previous_peak = peak;
	}

	int previous_peak{-1};
};

// ============================================================================
struct Channel
{
	const char *name;
	lv_obj_t *fader;
	lv_obj_t *value;
	lv_obj_t *mute;
	lv_obj_t *solo;
	Meter meter;
	float envelope;
	float level;
};

static Channel channels[] = {
	{"KICK"}, {"SNARE"}, {"HATS"}, {"BASS"}, {"SYNTH"}, {"VOX"},
};

static struct
{
	lv_obj_t *knob;
	lv_obj_t *value;
	Meter left, right;
	lv_obj_t *spectrum;
	lv_chart_series_t *bins;
	lv_obj_t *bpm;
	lv_obj_t *record;
} master;

static void
fader_label(Channel &channel)
{
	// Map the fader position to decibels: 0..100 is -60..+6 dB
	const int32_t position = lv_slider_get_value(channel.fader);
	if (position == 0) lv_label_set_text(channel.value, "-inf");
	else lv_label_set_text_fmt(channel.value, "%+" LV_PRId32, position * 66 / 100 - 60);
}

static lv_obj_t *
toggle(lv_obj_t *parent, const char *text, uint32_t checked_color, int32_t x, int32_t y)
{
	lv_obj_t *button = lv_button_create(parent);
	lv_obj_set_size(button, 22, 18);
	lv_obj_set_pos(button, x, y);
	lv_obj_set_checkable(button, true);
	lv_obj_set_style_radius(button, 3, 0);
	lv_obj_set_style_shadow_width(button, 0, 0);
	lv_obj_set_style_pad_all(button, 0, 0);
	lv_obj_set_style_bg_color(button, color(palette::Off), 0);
	lv_obj_set_style_bg_color(button, color(checked_color), LV_STATE_CHECKED);
	lv_obj_t *text_label = label(button, &lv_font_montserrat_12, palette::Text, text);
	lv_obj_center(text_label);
	return button;
}

static void
strip_create(lv_obj_t *screen, Channel &channel, int32_t x)
{
	lv_obj_t *strip = panel(screen, x, 30, 54, 238);
	lv_obj_t *name = label(strip, &lv_font_montserrat_12, palette::Muted, channel.name);
	lv_obj_align(name, LV_ALIGN_TOP_MID, 0, 4);

	channel.meter.create(strip, 8, 24, 14, true, 154, 8);

	channel.fader = lv_slider_create(strip);
	lv_obj_set_size(channel.fader, 6, 146);
	lv_obj_set_pos(channel.fader, 32, 28);
	lv_slider_set_range(channel.fader, 0, 100);
	lv_slider_set_value(channel.fader, 55 + lv_rand(0, 30), LV_ANIM_OFF);
	lv_obj_set_style_bg_color(channel.fader, color(palette::Off), LV_PART_MAIN);
	lv_obj_set_style_bg_color(channel.fader, color(palette::Accent), LV_PART_INDICATOR);
	// A wide flat fader cap instead of a round knob
	lv_obj_set_style_bg_color(channel.fader, color(palette::Text), LV_PART_KNOB);
	lv_obj_set_style_radius(channel.fader, 2, LV_PART_KNOB);
	lv_obj_set_style_pad_hor(channel.fader, 8, LV_PART_KNOB);
	lv_obj_set_style_pad_ver(channel.fader, -1, LV_PART_KNOB);
	lv_obj_set_ext_click_area(channel.fader, 12);
	lv_obj_add_event_cb(channel.fader, [](lv_event_t *e)
	{
		fader_label(*static_cast<Channel *>(lv_event_get_user_data(e)));
	}, LV_EVENT_VALUE_CHANGED, &channel);

	channel.value = label(strip, &lv_font_montserrat_12, palette::Text);
	lv_obj_align(channel.value, LV_ALIGN_TOP_MID, 0, 186);
	fader_label(channel);

	channel.mute = toggle(strip, "M", palette::Red, 4, 210);
	channel.solo = toggle(strip, "S", palette::Yellow, 28, 210);
}

static void
master_create(lv_obj_t *screen)
{
	lv_obj_t *section = panel(screen, 346, 30, 130, 238);
	lv_obj_t *title = label(section, &lv_font_montserrat_12, palette::Muted, "MASTER");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 4);

	master.knob = lv_arc_create(section);
	lv_obj_set_size(master.knob, 96, 96);
	lv_obj_align(master.knob, LV_ALIGN_TOP_MID, 0, 22);
	lv_arc_set_rotation(master.knob, 135);
	lv_arc_set_bg_angles(master.knob, 0, 270);
	lv_arc_set_range(master.knob, 0, 100);
	lv_arc_set_value(master.knob, 80);
	lv_obj_set_style_arc_width(master.knob, 8, LV_PART_MAIN);
	lv_obj_set_style_arc_width(master.knob, 8, LV_PART_INDICATOR);
	lv_obj_set_style_arc_color(master.knob, color(palette::Off), LV_PART_MAIN);
	lv_obj_set_style_arc_color(master.knob, color(palette::Accent), LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(master.knob, color(palette::Text), LV_PART_KNOB);
	lv_obj_set_style_pad_all(master.knob, 3, LV_PART_KNOB);
	master.value = label(master.knob, &lv_font_montserrat_20, palette::Text);
	lv_obj_center(master.value);
	const auto update = [](lv_event_t *)
	{
		const int32_t value = lv_arc_get_value(master.knob);
		lv_label_set_text_fmt(master.value, "%" LV_PRId32, value);
	};
	lv_obj_add_event_cb(master.knob, update, LV_EVENT_VALUE_CHANGED, nullptr);
	update(nullptr);

	lv_obj_t *l = label(section, &lv_font_montserrat_12, palette::Muted, "L");
	lv_obj_set_pos(l, 6, 124);
	lv_obj_t *r = label(section, &lv_font_montserrat_12, palette::Muted, "R");
	lv_obj_set_pos(r, 6, 138);
	master.left.create(section, 18, 127, 16, false, 104, 8);
	master.right.create(section, 18, 141, 16, false, 104, 8);

	master.spectrum = lv_chart_create(section);
	lv_obj_set_size(master.spectrum, 118, 62);
	lv_obj_align(master.spectrum, LV_ALIGN_BOTTOM_MID, 0, -26);
	lv_obj_set_style_bg_opa(master.spectrum, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(master.spectrum, 0, 0);
	lv_obj_set_style_pad_all(master.spectrum, 0, 0);
	lv_obj_set_style_pad_column(master.spectrum, 1, 0);
	lv_obj_set_style_radius(master.spectrum, 0, LV_PART_ITEMS);
	lv_chart_set_type(master.spectrum, LV_CHART_TYPE_BAR);
	lv_chart_set_div_line_count(master.spectrum, 0, 0);
	lv_chart_set_point_count(master.spectrum, 24);
	lv_chart_set_axis_range(master.spectrum, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
	master.bins = lv_chart_add_series(master.spectrum, color(palette::Green), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_all_values(master.spectrum, master.bins, 0);

	master.bpm = label(section, &lv_font_montserrat_12, palette::Muted, "120 BPM");
	lv_obj_align(master.bpm, LV_ALIGN_BOTTOM_RIGHT, -6, -6);
	master.record = lv_obj_create(section);
	lv_obj_remove_style_all(master.record);
	lv_obj_set_size(master.record, 10, 10);
	lv_obj_set_style_radius(master.record, LV_RADIUS_CIRCLE, 0);
	lv_obj_set_style_bg_color(master.record, color(palette::Red), 0);
	lv_obj_set_style_bg_opa(master.record, LV_OPA_COVER, 0);
	lv_obj_align(master.record, LV_ALIGN_BOTTOM_LEFT, 8, -8);
	lv_obj_t *rec = label(section, &lv_font_montserrat_12, palette::Red, "REC");
	lv_obj_align_to(rec, master.record, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
}

// ============================================================================
// A drum machine at 120 BPM in 16th notes drives the channel envelopes
static void
audio_step(uint32_t now)
{
	const uint32_t sixteenth = now / 125;
	static uint32_t last{~0ul};
	const bool tick = sixteenth != last;
	last = sixteenth;
	const uint32_t step = sixteenth % 16;

	if (tick)
	{
		if (step % 4 == 0 or step == 10) channels[0].envelope = 1.f;			// kick
		if (step == 4 or step == 12) channels[1].envelope = 0.95f;				// snare
		if (step % 2 == 0) channels[2].envelope = step % 4 ? 0.55f : 0.7f;		// hats
		if (step % 4 == 2 or step == 0) channels[3].envelope = 0.85f;			// bass
		if (step % 3 == 0) channels[4].envelope = 0.5f + lv_rand(0, 40) / 100.f;// synth
		if (sixteenth % 64 < 40 and lv_rand(0, 3)) channels[5].envelope = 0.6f + lv_rand(0, 35) / 100.f; // vox
	}
	static constexpr float Decay[] = {0.80f, 0.84f, 0.70f, 0.93f, 0.90f, 0.92f};

	bool solo = false;
	for (auto &channel : channels) solo |= lv_obj_has_state(channel.solo, LV_STATE_CHECKED);

	float left = 0, right = 0;
	for (size_t ii = 0; ii < std::size(channels); ii++)
	{
		Channel &channel = channels[ii];
		channel.envelope *= Decay[ii];
		const bool audible = not lv_obj_has_state(channel.mute, LV_STATE_CHECKED) and
							 (not solo or lv_obj_has_state(channel.solo, LV_STATE_CHECKED));
		const float gain = lv_slider_get_value(channel.fader) / 85.f;
		channel.level = audible ? std::min(1.f, channel.envelope * gain) : 0;
		channel.meter.set(channel.level, now);
		// Pan the channels a little apart
		const float pan = (ii - 2.5f) / 6;
		left += channel.level * (0.5f - pan) * 0.55f;
		right += channel.level * (0.5f + pan) * 0.55f;
	}
	const float volume = lv_arc_get_value(master.knob) / 100.f;
	master.left.set(std::min(1.f, left * volume), now);
	master.right.set(std::min(1.f, right * volume), now);

	// Low bins follow kick and bass, the middle snare and synth, the top the hats
	int32_t *bins = lv_chart_get_series_y_array(master.spectrum, master.bins);
	for (int bin = 0; bin < 24; bin++)
	{
		const float low = std::max(channels[0].level * (bin < 5 ? 1.f : 0.f), channels[3].level * (bin < 8 ? 0.8f : 0.1f));
		const float mid = std::max(channels[1].level, std::max(channels[4].level, channels[5].level)) * (bin >= 6 and bin < 18 ? 0.8f : 0.2f);
		const float high = channels[2].level * (bin >= 16 ? 0.9f : 0.1f);
		const float target = std::min(1.f, std::max(low, std::max(mid, high)) * volume) * (85 + lv_rand(0, 15));
		bins[bin] = std::max(int32_t(target), bins[bin] - 6);
	}
	lv_chart_refresh(master.spectrum);
	lv_obj_set_style_bg_opa(master.record, now % 1000 < 500 ? LV_OPA_COVER : LV_OPA_20, 0);
}

static void
ui_create()
{
	lv_obj_t *screen = lv_screen_active();
	lv_obj_set_style_bg_color(screen, color(palette::Background), 0);
	lv_obj_set_scrollable(screen, false);

	lv_obj_t *logo = label(screen, &lv_font_montserrat_20, palette::Accent, "modm");
	lv_obj_set_pos(logo, 8, 3);
	lv_obj_t *title = label(screen, &lv_font_montserrat_12, palette::Muted, "STUDIO MIXER  " LV_SYMBOL_AUDIO "  48 kHz / 24 bit");
	lv_obj_align_to(title, logo, LV_ALIGN_OUT_RIGHT_MID, 10, 2);

	for (size_t ii = 0; ii < std::size(channels); ii++) strip_create(screen, channels[ii], 4 + ii * 57);
	master_create(screen);

	lv_timer_create([](lv_timer_t *)
	{
		audio_step(modm::Clock::now().time_since_epoch().count());
	}, 30, nullptr);
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "modm LVGL studio mixer" << modm::endl;
	display_initialize();
	touch_initialize();
	ui_create();

	while (true) lv_timer_handler();
	return 0;
}
