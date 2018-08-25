/*
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef THREAD_DISPLAY
#define THREAD_DISPLAY

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/display/ssd1306.hpp>

#include "hardware.hpp"

class DisplayThread: public modm::pt::Protothread
{
public:
	DisplayThread();

	bool
	update();

	bool
	setSeq(int32_t seq) {
		_dirty = true;
		_seq = seq;
		return true;
	}

	bool
	setTemp(int32_t temp) {
		_dirty = true;
		_temp = temp;
		return true;
	}

	bool
	setPres(int32_t pres) {
		_dirty = true;
		_pres = pres;
		return true;
	}

	bool
	setHumi(int32_t humi) {
		_dirty = true;
		_humi = humi;
		return true;
	}

protected:
	modm::Ssd1306<MyI2cMaster, 64> display;
	modm::ShortTimeout boot_timeout;
	bool _dirty;
	int32_t _seq;
	int32_t _temp;
	int32_t _pres;
	int32_t _humi;
};

#endif
