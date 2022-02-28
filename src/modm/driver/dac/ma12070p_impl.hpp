/*
 * Copyright (c) 2021 - 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MA12070P_HPP
#	error "Don't include this file directly, use 'ma12070p.hpp' instead!"
#endif

namespace {
template<typename T>
auto
resumableResultOptionalTfromOptionalUint(modm::ResumableResult<std::optional<uint8_t>> value)
-> modm::ResumableResult<std::optional<T>>
{
	if (value.getResult()) {
		return modm::ResumableResult<std::optional<T>>(value.getState(), static_cast<std::optional<T>>(value.getResult()));
	}
	else {
		return modm::ResumableResult<std::optional<T>>(value.getState(), static_cast<std::optional<T>>(std::nullopt));
	}
}
}

namespace modm
{

template<typename I2cMaster>
Ma12070p<I2cMaster>::Ma12070p(uint8_t i2cAddress)
	: I2cDevice<I2cMaster, 5>(i2cAddress)
{
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::initialize()
{
	RF_BEGIN();
	RF_END_RETURN(true);
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::writeRegister(Register reg, uint8_t value)
{
	RF_BEGIN();
	tx_buffer[0] = static_cast<uint8_t>(reg);
	tx_buffer[1] = value;
	this->transaction.configureWrite(tx_buffer, 2);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<std::optional<uint8_t>>
Ma12070p<I2cMaster>::readRegister(Register reg)
{
	RF_BEGIN();
	tx_buffer[0] = static_cast<uint8_t>(reg);
	this->transaction.configureWriteRead(&tx_buffer[0], 1, &rx_buffer, 1);
	success = RF_CALL(this->runTransaction());
	if (!success) {
		RF_RETURN(std::nullopt);
	}
	RF_END_RETURN(rx_buffer);
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::configureI2sAndVlp(I2sAndVlpConfig c)
{
	RF_BEGIN();
	tx_buffer[0] = uint8_t(Register::LimiterConfiguration); // and the following Register::MuteAndLimiterMux
	tx_buffer[1] /* register 0x35 */ = (uint8_t(c.limiterReleaseTime) << 6) |
									   (uint8_t(c.limiterAttackTime) << 4) |
									   (uint8_t(c.useVlp) << 3) |
									   (uint8_t(c.pcmWordFormat) << 0);
	tx_buffer[2] /* register 0x36 */ =
		(uint16_t(c.useLimiter) << 6) |
		(uint8_t(c.rightLeftOrder) << 5) |
		(uint8_t(c.frameSize) << 3) |
		(uint8_t(c.dataOrder) << 2) |
		(uint8_t(c.wordSelectPolarity) << 1) |
		(uint8_t(c.clockPolarity) << 0);
	this->transaction.configureWrite(tx_buffer, 3);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::setLimiterTreshold(quarter_decibel_t ch0l, quarter_decibel_t ch0r, quarter_decibel_t ch1l, quarter_decibel_t ch1r)
{
	RF_BEGIN();
	tx_buffer[0] = uint8_t(Register::Ch0LVolumeDbInteger);
	tx_buffer[1] = uint16_t(ch0l) >> 2;
	tx_buffer[2] = uint16_t(ch0r) >> 2;
	tx_buffer[3] = uint16_t(ch1l) >> 2;
	tx_buffer[4] = uint16_t(ch1r) >> 2;
	tx_buffer[5] = (uint16_t(ch0l) & 0b11) | ((uint16_t(ch0r) & 0b11) << 2) | ((uint16_t(ch1l) & 0b11) << 4) | ((uint16_t(ch1r) & 0b11) << 6);
	this->transaction.configureWrite(tx_buffer, 6);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::setMasterVolume(quarter_decibel_t volume)
{
	RF_BEGIN();
	tx_buffer[0] = uint8_t(Register::MasterVolumeDbInteger);
	tx_buffer[1] = uint16_t(volume) >> 2;
	tx_buffer[2] = (uint16_t(volume) & 0b11);
	this->transaction.configureWrite(tx_buffer, 3);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::setChannelVolume(quarter_decibel_t ch0l, quarter_decibel_t ch0r, quarter_decibel_t ch1l, quarter_decibel_t ch1r)
{
	RF_BEGIN();
	tx_buffer[0] = uint8_t(Register::Ch0LLimiterDbfsInteger);
	tx_buffer[1] = uint16_t(ch0l) >> 2;
	tx_buffer[2] = uint16_t(ch0r) >> 2;
	tx_buffer[3] = uint16_t(ch1l) >> 2;
	tx_buffer[4] = uint16_t(ch1r) >> 2;
	tx_buffer[5] = (uint16_t(ch0l) & 0b11) | ((uint16_t(ch0r) & 0b11) << 2) | ((uint16_t(ch1l) & 0b11) << 4) | ((uint16_t(ch1r) & 0b11) << 6);
	this->transaction.configureWrite(tx_buffer, 6);
	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
ResumableResult<std::optional<ma12070p::VlpMonitor_t>>
Ma12070p<I2cMaster>::readVlpMonitor()
{
	return resumableResultOptionalTfromOptionalUint<VlpMonitor_t>(readRegister(Register::VlpMonitor));
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::disableAmplifier(bool ch0, bool ch1)
{
	return writeRegister(Register::DisableAmplifier, (uint8_t(ch0) << 7) | (uint8_t(ch1) << 6));
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::enableDcProtection(bool enable)
{
	return writeRegister(Register::DcProtection, (uint8_t(enable) << 2));
}

template<typename I2cMaster>
ResumableResult<std::optional<ma12070p::ErrorRegister_t>>
Ma12070p<I2cMaster>::readErrors()
{
	return resumableResultOptionalTfromOptionalUint<ErrorRegister_t>(readRegister(Register::Error));
}

template<typename I2cMaster>
ResumableResult<std::optional<ma12070p::ErrorRegister_t>>
Ma12070p<I2cMaster>::readAccumulatedErrors()
{
	return resumableResultOptionalTfromOptionalUint<ErrorRegister_t>(readRegister(Register::ErrorAccumulated));
}

template<typename I2cMaster>
ResumableResult<bool>
Ma12070p<I2cMaster>::clearErrorHandler()
{
	return writeRegister(Register::ErrorHandlerClear, (0b1 << 2));
}


}	// namespace modm
