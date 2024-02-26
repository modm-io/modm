/*
 * Copyright (c) 2014, 2016-2017, Sascha Schade
 * Copyright (c) 2014-2016, 2018, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_I2C_HPP
#define MODM_SSD1306_I2C_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>
#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>
#include "ssd1306_common.hpp"
#include <utility>

namespace modm
{

/// @ingroup modm_driver_ssd1306
struct ssd1306_i2c
{
public:
	enum class
	Transfer : uint8_t
	{
		COMMAND_BURST = 0x00,
		DATA_BURST = 0x40,
		COMMAND = 0x80,
		DATA = 0xC0
	};

	/// @cond
	class Ssd1306_I2cWriteTransaction : public modm::I2cWriteTransaction
	{
	public:
		Ssd1306_I2cWriteTransaction(uint8_t address);

		bool
		configureDisplayWrite(const uint8_t *buffer, std::size_t size);

	protected:
		Writing
		writing() override;

		void
		detaching(modm::I2c::DetachCause cause) override;

		inline bool
		isWritable() const
		{ return !transfer_active; }

	private:
		uint8_t transfer_type;
		bool transfer_active;
	};
	/// @endcond
};  // struct ssd1306_i2c

/**
 * Driver for SSD1306 based OLED-displays using I2C.
 * This display is only rated to be driven with 400kHz, which limits
 * the frame rate to about 40Hz.
 *
 * @author	Niklas Hauser
 * @author	Thomas Sommer
 * @ingroup	modm_driver_ssd1306
 */
template<class I2cMaster, uint8_t Height = 64>
class Ssd1306I2c : public Ssd1306Common<Height>,
				   public I2cDevice<I2cMaster, 3, ssd1306_i2c::Ssd1306_I2cWriteTransaction>
{
public:
	Ssd1306I2c(uint8_t address = 0x3C);

	/// Pings the display
	bool inline
	pingBlocking()
	{
		return RF_CALL_BLOCKING(this->ping());
	}

	/// initializes for 3V3 with charge-pump
	bool inline
	initializeBlocking()
	{
		return RF_CALL_BLOCKING(initialize());
	}

	/// Update the display with the content of the RAM buffer.
	void
	update()
	{
		RF_CALL_BLOCKING(startWriteDisplay());
	}

	/// Use this method to synchronize writing to the displays buffer
	/// to avoid tearing.
	/// @return	`true` if the frame buffer is not being copied to the display
	bool
	isWritable() const
	{
		return this->transaction.isWriteable();
	}

	// MARK: - TASKS
	/// initializes for 3V3 with charge-pump asynchronously
	modm::ResumableResult<bool>
	initialize();

	// starts a frame transfer and waits for completion
	virtual modm::ResumableResult<bool>
	writeDisplay();

protected:
	modm::ResumableResult<bool>
	writeCommands(std::size_t length) override;

	virtual modm::ResumableResult<void>
	initializeMemoryMode();

	virtual modm::ResumableResult<void>
	startWriteDisplay();

	bool transaction_success;
};

// DEPRECATE 2024q4
template<class I2cMaster, uint8_t Height = 64>
using Ssd1306 [[deprecated("Use `modm::Ssd1306I2c` instead!")]] = Ssd1306I2c<I2cMaster, Height>;

}  // namespace modm

#include "ssd1306_i2c_impl.hpp"

#endif  // MODM_SSD1306_I2C_HPP
