/*
 * Copyright (c) 2021, Tomasz Wasilczyk
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "st7586s.hpp"

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/interface/gpio.hpp>

namespace modm
{

template <typename SPI, typename CS, typename RST, typename DC, int Width, int Height>
void
St7586s<SPI, CS, RST, DC, Width, Height>::sendCommand(Command cmd, const void *data, size_t len)
{
	CS::reset();
	DC::reset();  // command mode
	SPI::transferBlocking(static_cast<uint8_t>(cmd));
	DC::set();  // data mode
	if (len > 0) {
		SPI::transferBlocking(reinterpret_cast<const uint8_t*>(data), nullptr, len);
	}
	CS::set();
	// exit with data mode on
}

template <typename SPI, typename CS, typename RST, typename DC, int Width, int Height>
void
St7586s<SPI, CS, RST, DC, Width, Height>::initialize()
{
	namespace payload = detail::st7586s::payload;

	// Configure GPIO
	RST::setOutput(false);
	CS::setOutput(true);
	DC::setOutput();

	// Reset display
	modm::delay(10us);
	RST::set();
	modm::delay(120ms);

	// Power ON operation flow (see datasheet)
	sendCommand(Command::SleepOff);
	sendCommand(Command::DisplayOff);
	modm::delay(50ms);  // t_{ON-V2}
	sendCommand(Command::SetVop, payload::SetVop(13.52f));
	sendCommand(Command::SetBias, payload::SetBias::Ratio_1_11);
	sendCommand(Command::SetBooster, payload::SetBooster::x8);
	sendCommand(Command::AnalogControl, payload::AnalogControl::Enable);
	sendCommand<payload::NLineInversion_t>(Command::NLineInversion,
		payload::LineInversionType_t(payload::LineInversionType::FrameInversion));
	sendCommand(Command::DisplayModeMono);  // TODO: grayscale
	sendCommand(Command::EnableDdram, payload::EnableDdram::Enable);
	sendCommand(Command::ScanDirection, payload::DisplayControl::ComInc
		| payload::DisplayControl::SegInc);
	sendCommand(Command::DisplayDuty, uint8_t(Height - 1));
	sendCommand(Command::InverseOff);
	setClipping(0, 0, Width, Height);
	update();  // clear the display
	sendCommand(Command::DisplayOn);
}

template <typename SPI, typename CS, typename RST, typename DC, int Width, int Height>
void
St7586s<SPI, CS, RST, DC, Width, Height>::setClipping(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	namespace payload = detail::st7586s::payload;

	modm_assert_continue_fail_debug(x < Width, "st4586s.sc.x", "x >= Width", x);
	modm_assert_continue_fail_debug(x + w <= Width, "st4586s.sc.xw", "x + w >= Width", x + w);
	modm_assert_continue_fail_debug(y < Height, "st4586s.sc.y", "y >= Height", y);
	modm_assert_continue_fail_debug(y + h <= Height, "st4586s.sc.yh", "y + h >= Height", y + h);
	modm_assert_continue_fail_debug((x % pixelsPerByte) == 0, "st4586s.sc.x%",
		"x is not a multiply of PBB", x);
	modm_assert_continue_fail_debug((w % pixelsPerByte) == 0, "st4586s.sc.w%",
		"w is not a multiply of PBB", w);

	const payload::SetColumnRow columnRange(x / pixelsPerByte, (x + w) / pixelsPerByte - 1);
	sendCommand(Command::SetColumn, columnRange);

	const payload::SetColumnRow rowRange(y, y + h - 1);
	sendCommand(Command::SetRow, rowRange);
}

template <typename SPI, typename CS, typename RST, typename DC, int Width, int Height>
void
St7586s<SPI, CS, RST, DC, Width, Height>::update()
{
	sendCommand(Command::WriteDisplayData);
	CS::reset();
	// TODO: support windows other than full screen (then make setClipping public)
	// TODO: transfer the whole memory area, not individual pixels
	for (uint16_t y = 0; y < Height; y++) {
		const uint8_t* row = this->buffer[y];
		uint16_t currentByte = 0;
		uint8_t validBits = 0;
		for (uint16_t x = 0; x + 2 < Width; x += 3) {
			if (validBits < 3) {
				currentByte |= (*row++) << validBits;
				validBits += 8;
			}

			uint8_t cell = 0;
			if (currentByte & 0b001) cell |= (0b11 << 6);
			if (currentByte & 0b010) cell |= (0b11 << 3);
			if (currentByte & 0b100) cell |= (0b11 << 0);
			validBits -= 3;
			currentByte >>= 3;

			SPI::transferBlocking(cell);
		}
	}
	CS::set();
}

}  // namespace modm
