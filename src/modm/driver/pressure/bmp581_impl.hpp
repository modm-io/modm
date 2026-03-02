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

namespace modm
{

template<Bmp581Transport Transport>
template<typename... Args>
Bmp581<Transport>::Bmp581(Args... transportArgs)
	: Transport{transportArgs...}
{
}

template<Bmp581Transport Transport>
void
Bmp581<Transport>::waitForCommandGap()
{
	if (timer_.isArmed()) {
		timer_.wait();
	}
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::initialize()
{
	if (!Transport::initialize()) {
		return false;
	}

	if (!reset()) {
		return false;
	}

	const auto chipId = readChipId();
	if (!chipId || *chipId != ChipId) {
		return false;
	}

	const auto status = readStatus();
	if (!status) {
		return false;
	}
	// Check NvmReady is set and NvmError/NvmCmdError are clear
	if (!(*status & Status::NvmReady) ||
	    (*status & Status::NvmError) ||
	    (*status & Status::NvmCmdError)) {
		return false;
	}

	const auto intStatus = readIntStatus();
	if (!intStatus || !(*intStatus & IntStatus::PowerOnReset)) {
		return false;
	}

	// Disable deep standby mode
	if (!updateRegister(Register::OdrConfig, uint8_t(OdrConfig::DeepDis), uint8_t(OdrConfig::DeepDis))) {
		return false;
	}

	return true;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::reset()
{
	waitForCommandGap();

	if (!writeRegister(Register::Cmd, ResetCommand)) {
		return false;
	}

	// Wait for reset to complete (datasheet: 2ms typical)
	timer_.restart(std::chrono::milliseconds{5});
	timer_.wait();

	// Re-initialize transport (needed for SPI mode)
	if (!Transport::initialize()) {
		return false;
	}

	// Dummy read to prime SPI state machine after reset (Bosch recommendation)
	// First SPI transaction after reset can be unreliable; discard result
	(void)readRegister(Register::ChipId);

	timer_.restart(std::chrono::microseconds{2});
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
	if (!value) {
		return std::nullopt;
	}
	return Status_t{*value};
}

template<Bmp581Transport Transport>
std::optional<bmp581::IntStatus_t>
Bmp581<Transport>::readIntStatus()
{
	const auto value = readRegister(Register::IntStatus);
	if (!value) {
		return std::nullopt;
	}
	return IntStatus_t{*value};
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setPowerMode(PowerMode mode)
{
	waitForCommandGap();

	const uint8_t mask = uint8_t(OdrConfig::Mode0) | uint8_t(OdrConfig::Mode1);
	const uint8_t targetMode = static_cast<uint8_t>(mode);
	const uint8_t standbyMode = static_cast<uint8_t>(PowerMode::Standby);

	const auto current = readRegister(Register::OdrConfig);
	if (!current) {
		return false;
	}

	// Per datasheet, active mode transitions should go through STANDBY first.
	if ((*current & mask) != standbyMode) {
		if (!updateRegister(Register::OdrConfig, mask, standbyMode)) {
			return false;
		}
		// Maximum transition time to STANDBY.
		timer_.restart(std::chrono::microseconds{2500});
		timer_.wait();
	}

	if (targetMode != standbyMode) {
		if (!updateRegister(Register::OdrConfig, mask, targetMode)) {
			return false;
		}
	}

	timer_.restart(std::chrono::microseconds{2});
	return true;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setOdr(Odr odr)
{
	waitForCommandGap();

	const uint8_t mask = uint8_t(OdrConfig::Odr0) | uint8_t(OdrConfig::Odr1) |
	                     uint8_t(OdrConfig::Odr2) | uint8_t(OdrConfig::Odr3) |
	                     uint8_t(OdrConfig::Odr4);
	const uint8_t value = static_cast<uint8_t>(odr) << 2;

	const bool ok = updateRegister(Register::OdrConfig, mask, value);
	timer_.restart(std::chrono::microseconds{2});
	return ok;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setOversampling(Osr pressOsr, Osr tempOsr, bool enablePressure)
{
	waitForCommandGap();

	uint8_t value = (static_cast<uint8_t>(tempOsr) << 0) |
	                (static_cast<uint8_t>(pressOsr) << 3);
	if (enablePressure) {
		value |= uint8_t(OsrConfig::PressEn);
	}

	const bool ok = writeRegister(Register::OsrConfig, value);
	timer_.restart(std::chrono::microseconds{2});
	return ok;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIirFilter(IirFilter pressIir, IirFilter tempIir)
{
	// IIR writes require STANDBY mode per datasheet section 4.3.8
	// Save current ODR config, switch to standby, write IIR, restore mode
	const auto odrConfig = readRegister(Register::OdrConfig);
	if (!odrConfig) {
		return false;
	}

	// Switch to standby mode
	const uint8_t modeMask = uint8_t(OdrConfig::Mode0) | uint8_t(OdrConfig::Mode1);
	if (!writeRegister(Register::OdrConfig, (*odrConfig & ~modeMask) | uint8_t(PowerMode::Standby))) {
		return false;
	}

	// Wait for STANDBY transition (tstandby = 2.5ms max per datasheet)
	timer_.restart(std::chrono::microseconds{2500});
	waitForCommandGap();

	// Temperature IIR at [2:0], Pressure IIR at [5:3]
	const uint8_t value = (static_cast<uint8_t>(tempIir) << 0) |
	                      (static_cast<uint8_t>(pressIir) << 3);

	const bool iirOk = writeRegister(Register::DspIir, value);

	// Restore original power mode
	const bool restoreOk = writeRegister(Register::OdrConfig, *odrConfig);

	timer_.restart(std::chrono::microseconds{2});
	return iirOk && restoreOk;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIntConfig(IntConfig_t config)
{
	waitForCommandGap();

	// Read-modify-write to preserve upper nibble (pad drive strength)
	constexpr uint8_t intConfigMask = uint8_t(IntConfig::Mode) | uint8_t(IntConfig::Polarity) |
	                                  uint8_t(IntConfig::OpenDrain) | uint8_t(IntConfig::Enable);
	const bool ok = updateRegister(Register::IntConfig, intConfigMask, config.value);

	timer_.restart(std::chrono::microseconds{2});
	return ok;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::setIntSource(IntSource_t sources)
{
	waitForCommandGap();
	const bool ok = writeRegister(Register::IntSource, sources.value);
	timer_.restart(std::chrono::microseconds{2});
	return ok;
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::readData(Data& data)
{
	// Read temperature (3 bytes) and pressure (3 bytes) in one transaction
	// Registers are contiguous: TEMP_XLSB(0x1D) to PRESS_MSB(0x22)
	uint8_t buffer[6];
	if (!this->read(i(Register::TempDataXlsb), buffer, 6)) {
		return false;
	}

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
	if (!this->read(i(Register::TempDataXlsb), data.rawTemp.data(), 3)) {
		return std::nullopt;
	}
	return data.getTemperature();
}

template<Bmp581Transport Transport>
std::optional<float>
Bmp581<Transport>::readPressure()
{
	Data data;
	if (!this->read(i(Register::PressDataXlsb), data.rawPress.data(), 3)) {
		return std::nullopt;
	}
	return data.getPressure();
}

template<Bmp581Transport Transport>
bool
Bmp581<Transport>::isDataReady()
{
	const auto status = readIntStatus();
	return status && (*status & IntStatus::DataReady);
}

template<Bmp581Transport Transport>
std::optional<uint8_t>
Bmp581<Transport>::readRegister(Register reg)
{
	uint8_t value;
	if (!this->read(i(reg), &value, 1)) {
		return std::nullopt;
	}
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
	if (!current) {
		return false;
	}
	const uint8_t newValue = (*current & ~mask) | (value & mask);
	return writeRegister(reg, newValue);
}

} // namespace modm
