/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#ifndef MODM_SH1106_HPP
#define MODM_SH1106_HPP

#include "ssd1306.hpp"

namespace modm
{

/**
 * SH1106 is said to be 'compatible' with SSD1306. However there's a relevant
 * difference: SH1106 does only support MemoryMode::PAGE. This requires a little
 * more extensive writeDisplay() routine. We have to alternate between setting
 * Page-address and sending page-data instead of sending the whole buffer at
 * once like is for SSD1306 in MemoryMode::HORIZONTAL / MemoryMode::VERTICAL
 *
 * @ingroup modm_driver_sh1106
 */

template<class I2cMaster, uint16_t H = 64>
class Sh1106 : public Ssd1306<I2cMaster, H>
{
public:
	using ColorType = Ssd1306<I2cMaster, H>::ColorType;
	using Buffer = Ssd1306<I2cMaster, H>::Buffer;

	Sh1106(uint8_t address = 0x3C) : Ssd1306<I2cMaster, H>(address) {}

    // TODO Abstract these write(...) cause is common to all I2c Displays!
	// Write BufferInterface
	modm::ResumableResult<bool>
	write(graphic::BufferInterface<ColorType> &buffer, shape::Point placement = {0, 0}) {
		RF_BEGIN();
		RF_END_RETURN_CALL(writeImage(graphic::ImageAccessor<ColorType, modm::accessor::Ram>(&buffer, placement)));
	}

	// Write Flash Image
	modm::ResumableResult<bool>
	write(const uint8_t *addr, shape::Point placement = {0, 0}) {
		RF_BEGIN();
		// TODO Support other than ColorType
		RF_END_RETURN_CALL(writeImage(graphic::ImageAccessor<ColorType, modm::accessor::Flash>(addr, placement)));
	}

protected:
	modm::ResumableResult<bool>
	initializeMemoryMode() override;

	modm::ResumableResult<bool>
	updateClipping() final;

	// Write monochrome Image
	// Caution: placement.y rounds to multiples of 8
	template<template<typename> class Accessor>
	modm::ResumableResult<bool>
	writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor);

	// Static variables for Resumable Functions
	size_t yd; // vertical index in display (page)
	size_t yd_start; // first vertical index in display
	size_t yd_end; // last vertical index in display
	size_t yb; // vertical position in buffer
};

}  // namespace modm

#include "sh1106_impl.hpp"

#endif // MODM_SH1106_HPP