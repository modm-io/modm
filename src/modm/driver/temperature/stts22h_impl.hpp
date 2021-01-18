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

#ifndef MODM_STTS22H_HPP
#error "Don't include this file directly, use 'stts22h.hpp' instead!"
#endif

namespace modm
{

template < typename I2cMaster >
Stts22h<I2cMaster>::Stts22h(Data &data, uint8_t address) :
	I2cDevice<I2cMaster, 3>{address}, data_{data}
{
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::write(Register reg, RegisterValue value)
{
	RF_BEGIN();

	buffer_[0] = static_cast<uint8_t>(reg);
	buffer_[1] = value.value;

	this->transaction.configureWrite(&buffer_[0], 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::read(Register reg, uint8_t& value)
{
	return read(reg, &value, 1);
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::read(Register reg, uint8_t* data, uint8_t length)
{
	RF_BEGIN();

	buffer_[0] = static_cast<uint8_t>(reg);
	this->transaction.configureWriteRead(&buffer_[0], 1, data, length);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::initialize()
{
	RF_BEGIN();
	if (!RF_CALL(this->ping())) {
		RF_RETURN(false);
	}
	if (!RF_CALL(write(Register::SoftwareReset, SoftwareReset::SwReset))) {
		RF_RETURN(false);
	}
	if (!RF_CALL(write(Register::SoftwareReset, SoftwareReset{}))) {
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(write(Register::Ctrl, Ctrl::FreeRun | Ctrl::IfAddInc));
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::ping()
{
	RF_BEGIN();
	// It's ok here to use buffer_[1] as temporary storage
	// since read() only uses buffer_[0]
	if (!RF_CALL(read(Register::WhoAmI, buffer_[1]))) {
		RF_RETURN(false);
	}
	RF_END_RETURN(buffer_[1] == DeviceId);
}

template<class I2cMaster>
ResumableResult<bool>
Stts22h<I2cMaster>::readTemperature()
{
	return read(Register::TempLOut, data_.data, 2);
}

} // namespace modm
