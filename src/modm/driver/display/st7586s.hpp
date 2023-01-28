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

#include <modm/ui/display/monochrome_graphic_display_horizontal.hpp>

#include "st7586s_protocol.hpp"

namespace modm
{

// TODO: this controller has pixels packed by 3, not 8 per byte
/// @ingroup modm_driver_st7586s
template <typename SPI, typename CS, typename RST, typename DC, int Width = 384, int Height = 160>
class St7586s : public MonochromeGraphicDisplayHorizontal<Width, Height>
{
	using Command = detail::st7586s::Command;
	static constexpr uint8_t pixelsPerByte = 3;

	void sendCommand(Command cmd, const void *data = nullptr, size_t len = 0);
	template <typename Data>
	void sendCommand(Command cmd, Data data) {
		sendCommand(cmd, &data, sizeof(data));
	}

	void setClipping(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

public:
	void initialize();
	virtual void update() override;
};

}  // namespace modm

#include "st7586s_impl.hpp"
