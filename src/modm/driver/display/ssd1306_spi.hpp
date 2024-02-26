/*
 * Copyright (c) 2014, 2016-2017, Sascha Schade
 * Copyright (c) 2014-2016, 2018, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_SPI_HPP
#define MODM_SSD1306_SPI_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>
#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>
#include "ssd1306_common.hpp"
#include <utility>

namespace modm
{

/**
 * Driver for SSD1306 based OLED-displays using SPI 4-wire mode.
 *
 * @author	Raphael Lehmann
 * @ingroup	modm_driver_ssd1306
 */
template<class SpiMaster, class Cs, class Dc, uint8_t Height = 64>
class Ssd1306Spi : public Ssd1306Common<Height>,
				   public SpiDevice<SpiMaster>,
				   protected modm::NestedResumable<2>
{
public:
	enum class
	InitSequences : uint8_t
	{
		Hp12832_02,  /// HP12832-02 display module, 0.91inch, 128x32px, 4-wire
					 /// SPI, blue, external 7.5V supply
	};

	modm::ResumableResult<bool>
	initialize(InitSequences initSeq = InitSequences::Hp12832_02);

	/// Update the display with the content of the RAM buffer
	modm::ResumableResult<bool>
	writeDisplay();

	/// Update the display with the content of the RAM buffer
	/// @warning This function is blocking as long as the transfer of the
	/// whole display buffer via SPI takes.
	// Better use @see writeDisplay() instead.
	void
	update()
	{
		RF_CALL_BLOCKING(writeDisplay());
	}

protected:
	modm::ResumableResult<bool>
	writeCommands(std::size_t length) override;
};

}  // namespace modm

#include "ssd1306_spi_impl.hpp"

#endif  // MODM_SSD1306_SPI_HPP
