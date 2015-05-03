// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC58X3_HPP
#	error "Don't include this file directly, use 'hmc58x3.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Hmc58x3<I2cMaster>::Hmc58x3(Data &data, uint8_t address)
:	I2cDevice<I2cMaster,2>(address), data(data),
	rawBuffer{0x10, 0x20, 0x01, 0,0,0,0,0,0, 0x00}
{
}

// MARK: - Tasks
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::readMagneticField()
{
	RF_BEGIN();

	if (RF_CALL(read(Register::DataX_Lsb, rawBuffer+3, 7)))
	{
		std::memcpy(data.data, rawBuffer+3, 6);
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - base methods
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::configureRaw(uint8_t rate, uint8_t gain, const uint8_t* gainValues, uint8_t average)
{
	RF_BEGIN();

	rawBuffer[0] = rate | average;
	rawBuffer[1] = gain;
	rawBuffer[2] = uint8_t(OperationMode::ContinousConversion);

	if (RF_CALL(write(Register::ConfigA, rawBuffer, 3)))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::setGainRaw(uint8_t gain, const uint8_t* gainValues)
{
	RF_BEGIN();

	rawBuffer[1] = (rawBuffer[1] & ~uint8_t(ConfigB::GN_Mask)) | gain;

	if (RF_CALL(write(Register::ConfigB, rawBuffer[1])))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - register access
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::readStatus()
{
	return read(Register::Status, rawBuffer[9]);
}

// MARK: update register
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	RF_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;

	RF_END_RETURN_CALL(write(Register(index), rawBuffer[index]));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length)
{
	RF_BEGIN();

	if (length > 5)
		RF_RETURN(false);

	rawBuffer[3] = uint8_t(reg);
	std::memcpy(rawBuffer+4, buffer, length);

	this->transaction.configureWrite(rawBuffer+3, length+1);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Hmc58x3<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	RF_BEGIN();

	rawBuffer[3] = uint8_t(reg);
	this->transaction.configureWriteRead(rawBuffer+3, 1, buffer, length);

	RF_END_RETURN_CALL( this->runTransaction() );
}
