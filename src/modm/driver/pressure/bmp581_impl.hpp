/*
 * Copyright (c) 2026, Joel Schulz-Andres
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP581_HPP
#error "Don't include this file directly, use 'bmp581.hpp' instead!"
#endif

#include <chrono>

namespace modm
{

using namespace std::chrono_literals;

template<Bmp581Transport Transport>
template<typename... Args>
Bmp581<Transport>::Bmp581(Args... transportArgs) : Transport{transportArgs...}
{}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::initialize()
{
	if (!Transport::initialize()) { return false; }

	if (!reset()) { return false; }

	const auto chipId = readChipId();
	if (!chipId || *chipId != ChipId) { return false; }

	const auto status = readStatus();
	if (!status) { return false; }
	// Check NvmReady is set and NvmError/NvmCmdError are clear
	if (!(*status & Status::NvmReady) || (*status & Status::NvmError) ||
		(*status & Status::NvmCmdError))
	{
		return false;
	}

	const auto intStatus = readIntStatus();
	if (!intStatus || !(*intStatus & IntStatus::PowerOnReset)) { return false; }

	// Disable deep standby mode
	if (!updateRegister(Register::OdrConfig, uint8_t(OdrConfig::DeepDis),
						uint8_t(OdrConfig::DeepDis)))
	{
		return false;
	}

	return true;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::reset()
{
	if (!writeRegister(Register::Cmd, ResetCommand)) { return false; }

	// Wait for reset to complete (datasheet: 2ms typical, use 5ms for margin)
	modm::this_fiber::sleep_for(5ms);

	// Re-initialize transport (needed for SPI mode)
	if (!Transport::initialize()) { return false; }

	// Dummy read to prime SPI state machine after reset (Bosch recommendation)
	// First SPI transaction after reset can be unreliable; discard result
	(void)readRegister(Register::ChipId);

	return true;
}

template<Bmp581Transport Transport>
std::optional<uint8_t>
Bmp581<Transport>::readChipId()
{
	return readRegister(Register::ChipId);
}

template<Bmp581Transport Transport>
std::optional<bmp581::Status_t>
Bmp581<Transport>::readStatus()
{
	const auto value = readRegister(Register::Status);
	if (!value) { return std::nullopt; }
	return Status_t{*value};
}

template<Bmp581Transport Transport>
std::optional<bmp581::IntStatus_t>
Bmp581<Transport>::readIntStatus()
{
	const auto value = readRegister(Register::IntStatus);
	if (!value) { return std::nullopt; }
	return IntStatus_t{*value};
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setPowerMode(PowerMode mode)
{
	const uint8_t mask = uint8_t(OdrConfig::Mode0) | uint8_t(OdrConfig::Mode1);
	const uint8_t targetMode = static_cast<uint8_t>(mode);
	const uint8_t standbyMode = static_cast<uint8_t>(PowerMode::Standby);

	const auto current = readRegister(Register::OdrConfig);
	if (!current) { return false; }

	// Per datasheet, active mode transitions should go through STANDBY first
	if ((*current & mask) != standbyMode)
	{
		if (!updateRegister(Register::OdrConfig, mask, standbyMode)) { return false; }
		// Maximum transition time to STANDBY (tstandby = 2.5ms)
		modm::this_fiber::sleep_for(2500us);
	}

	if (targetMode != standbyMode)
	{
		if (!updateRegister(Register::OdrConfig, mask, targetMode)) { return false; }
	}

	return true;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setOdr(Odr odr)
{
	const uint8_t mask = uint8_t(OdrConfig::Odr0) | uint8_t(OdrConfig::Odr1) |
						 uint8_t(OdrConfig::Odr2) | uint8_t(OdrConfig::Odr3) |
						 uint8_t(OdrConfig::Odr4);
	const uint8_t value = static_cast<uint8_t>(odr) << 2;

	return updateRegister(Register::OdrConfig, mask, value);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setOversampling(Osr pressOsr, Osr tempOsr, bool enablePressure)
{
	uint8_t value = (static_cast<uint8_t>(tempOsr) << 0) | (static_cast<uint8_t>(pressOsr) << 3);
	if (enablePressure) { value |= uint8_t(OsrConfig::PressEn); }

	return writeRegister(Register::OsrConfig, value);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIirFilter(IirFilter pressIir, IirFilter tempIir)
{
	// IIR writes require STANDBY mode per datasheet section 4.3.8
	// Save current ODR config, switch to standby, write IIR, restore mode
	const auto odrConfig = readRegister(Register::OdrConfig);
	if (!odrConfig) { return false; }

	// Switch to standby mode
	const uint8_t modeMask = uint8_t(OdrConfig::Mode0) | uint8_t(OdrConfig::Mode1);
	if (!writeRegister(Register::OdrConfig, (*odrConfig & ~modeMask) | uint8_t(PowerMode::Standby)))
	{
		return false;
	}

	// Wait for STANDBY transition (tstandby = 2.5ms max per datasheet)
	modm::this_fiber::sleep_for(2500us);

	// Temperature IIR at [2:0], Pressure IIR at [5:3]
	const uint8_t value =
		(static_cast<uint8_t>(tempIir) << 0) | (static_cast<uint8_t>(pressIir) << 3);

	const bool iirOk = writeRegister(Register::DspIir, value);

	// Restore original power mode
	const bool restoreOk = writeRegister(Register::OdrConfig, *odrConfig);

	return iirOk && restoreOk;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIntConfig(IntConfig_t config)
{
	// Read-modify-write to preserve upper nibble (pad drive strength)
	constexpr uint8_t intConfigMask = uint8_t(IntConfig::Mode) | uint8_t(IntConfig::Polarity) |
									  uint8_t(IntConfig::OpenDrain) | uint8_t(IntConfig::Enable);
	return updateRegister(Register::IntConfig, intConfigMask, config.value);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIntSource(IntSource_t sources)
{
	return writeRegister(Register::IntSource, sources.value);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::readData(Data& data)
{
	// Read temperature (3 bytes) and pressure (3 bytes) in one transaction
	// Registers are contiguous: TEMP_XLSB(0x1D) to PRESS_MSB(0x22)
	uint8_t buffer[6];
	if (!this->read(i(Register::TempDataXlsb), buffer, 6)) { return false; }

	data.rawTemp[0] = buffer[0];
	data.rawTemp[1] = buffer[1];
	data.rawTemp[2] = buffer[2];
	data.rawPress[0] = buffer[3];
	data.rawPress[1] = buffer[4];
	data.rawPress[2] = buffer[5];

	return true;
}

template<Bmp581Transport Transport>
std::optional<float>
Bmp581<Transport>::readTemperature()
{
	Data data;
	if (!this->read(i(Register::TempDataXlsb), data.rawTemp.data(), 3)) { return std::nullopt; }
	return data.getTemperature();
}

template<Bmp581Transport Transport>
std::optional<float>
Bmp581<Transport>::readPressure()
{
	Data data;
	if (!this->read(i(Register::PressDataXlsb), data.rawPress.data(), 3)) { return std::nullopt; }
	return data.getPressure();
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::isDataReady()
{
	const auto status = readIntStatus();
	return status && (*status & IntStatus::DataReady);
}

// -----------------------------------------------------------------------------
// FIFO Functions
// -----------------------------------------------------------------------------

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setFifoEnabled(bool enable)
{
	return updateRegister(Register::FifoConfig, uint8_t(FifoConfig::Mode),
						  enable ? uint8_t(FifoConfig::Mode) : 0);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setFifoThreshold(uint8_t threshold)
{
	if (threshold > 31) { return false; }
	constexpr uint8_t thresholdMask =
		uint8_t(FifoConfig::Threshold0) | uint8_t(FifoConfig::Threshold1) |
		uint8_t(FifoConfig::Threshold2) | uint8_t(FifoConfig::Threshold3) |
		uint8_t(FifoConfig::Threshold4);
	return updateRegister(Register::FifoConfig, thresholdMask, threshold << 1);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setFifoSelect(FifoFrameSelect frameSelect, FifoDecimation decimation)
{
	const auto odrConfig = readRegister(Register::OdrConfig);
	if (!odrConfig) { return false; }

	const uint8_t modeMask = uint8_t(OdrConfig::Mode0) | uint8_t(OdrConfig::Mode1);
	const uint8_t oldMode = *odrConfig & modeMask;

	// Datasheet: FIFO_SEL must be changed in STANDBY mode.
	if (oldMode != uint8_t(PowerMode::Standby))
	{
		if (!writeRegister(Register::OdrConfig,
						   (*odrConfig & ~modeMask) | uint8_t(PowerMode::Standby)))
		{
			return false;
		}
		modm::this_fiber::sleep_for(2500us);
	}

	// FIFO_SEL(0x18): frame select in bits [1:0], decimation in bits [4:2]
	const uint8_t value =
		static_cast<uint8_t>(frameSelect) | (static_cast<uint8_t>(decimation) << 2);
	const bool selectOk = writeRegister(Register::FifoSel, value);
	if (!selectOk) { return false; }

	if (oldMode != uint8_t(PowerMode::Standby))
	{
		return writeRegister(Register::OdrConfig, (*odrConfig & ~modeMask) | oldMode);
	}

	return true;
}

template<Bmp581Transport Transport>
std::optional<uint8_t>
Bmp581<Transport>::getFifoCount()
{
	const auto value = readRegister(Register::FifoCount);
	if (!value) { return std::nullopt; }
	// FIFO_COUNT uses bits [5:0]
	return *value & 0x3f;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::readFifoFrame(Data& data)
{
	const auto fifoSel = readRegister(Register::FifoSel);
	if (!fifoSel) { return false; }

	const auto frameSelect = static_cast<FifoFrameSelect>(*fifoSel & 0x03);
	switch (frameSelect)
	{
		case FifoFrameSelect::PressureAndTemperature: {
			uint8_t buffer[6];
			if (!this->read(i(Register::FifoData), buffer, 6)) { return false; }
			data.rawTemp[0] = buffer[0];
			data.rawTemp[1] = buffer[1];
			data.rawTemp[2] = buffer[2];
			data.rawPress[0] = buffer[3];
			data.rawPress[1] = buffer[4];
			data.rawPress[2] = buffer[5];
			return true;
		}
		case FifoFrameSelect::TemperatureOnly: {
			if (!this->read(i(Register::FifoData), data.rawTemp.data(), 3)) { return false; }
			data.rawPress = {0, 0, 0};
			return true;
		}
		case FifoFrameSelect::PressureOnly: {
			if (!this->read(i(Register::FifoData), data.rawPress.data(), 3)) { return false; }
			data.rawTemp = {0, 0, 0};
			return true;
		}
		case FifoFrameSelect::Disabled:
		default:
			return false;
	}
}

template<Bmp581Transport Transport>
uint8_t
Bmp581<Transport>::readFifoFrames(Data* data, uint8_t count)
{
	const auto available = getFifoCount();
	if (!available) { return 0; }

	const uint8_t toRead = (*available < count) ? *available : count;
	for (uint8_t i = 0; i < toRead; ++i)
	{
		if (!readFifoFrame(data[i])) { return i; }
	}
	return toRead;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::flushFifo()
{
	// Writing 0xB0 to CMD register flushes the FIFO
	static constexpr uint8_t FifoFlushCommand = 0xB0;
	return writeRegister(Register::Cmd, FifoFlushCommand);
}

template<Bmp581Transport Transport>
std::optional<uint8_t>
Bmp581<Transport>::readRegister(Register reg)
{
	uint8_t value;
	if (!this->read(i(reg), &value, 1)) { return std::nullopt; }
	return value;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::writeRegister(Register reg, uint8_t value)
{
	return this->write(i(reg), value);
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::updateRegister(Register reg, uint8_t mask, uint8_t value)
{
	const auto current = readRegister(reg);
	if (!current) { return false; }
	const uint8_t newValue = (*current & ~mask) | (value & mask);
	return writeRegister(reg, newValue);
}

}  // namespace modm
