/*
* Copyright (c) 2021, Christopher Durand
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#ifndef MODM_MCP990X_HPP
#error "Don't include this file directly, use 'mcp990x.hpp' instead!"
#endif

namespace modm
{

template < typename I2cMaster >
Mcp990x<I2cMaster>::Mcp990x(Data &data, uint8_t address) :
	I2cDevice<I2cMaster, 3>{address}, data_{data}
{
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::write(Register reg, uint8_t value)
{
	RF_BEGIN();

	buffer_[0] = static_cast<uint8_t>(reg);
	buffer_[1] = value;

	this->transaction.configureWrite(&buffer_[0], 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::read(Register reg, uint8_t& value)
{
	RF_BEGIN();

	buffer_[0] = static_cast<uint8_t>(reg);
	this->transaction.configureWriteRead(&buffer_[0], 1, &value, 1);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::initialize()
{
	RF_BEGIN();
	if (!RF_CALL(this->ping())) {
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(write(Register::Config, uint8_t(Config::ExtendedRange)));
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::ping()
{
	RF_BEGIN();
	// It's ok here to use buffer_[1] as temporary storage
	// since read() only uses buffer_[0]
	if (!RF_CALL(read(Register::ProductId, buffer_[1]))) {
		RF_RETURN(false);
	}
	RF_END_RETURN(buffer_[1] == DeviceIds[0] || buffer_[1] == DeviceIds[1] || buffer_[1] == DeviceIds[2]);
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::readInternalTemperature()
{
	return readTemperature(Register::IntTempHigh, Register::IntTempLow);
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::readExternalDiodeTemperature(ExternalDiode diode)
{
	const auto index = static_cast<uint8_t>(diode);
	return readTemperature(ExtHighReg[index], ExtLowReg[index]);
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::setExternalDiodeIdealityFactor(ExternalDiode diode, uint8_t idealitySetting)
{
	const auto index = static_cast<uint8_t>(diode);
	return write(ExtIdealityReg[index], idealitySetting & 0b0011'1111);
}

template<class I2cMaster>
ResumableResult<bool>
Mcp990x<I2cMaster>::readTemperature(Register highReg, Register lowReg)
{
	RF_BEGIN();
	if (!RF_CALL(read(highReg, data_.data[1]))) {
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(read(lowReg, data_.data[0]));
}

} // namespace modm
