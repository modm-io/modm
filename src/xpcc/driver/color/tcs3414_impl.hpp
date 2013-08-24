/*
 * \file	tcs3414_impl.hpp
 * \date	2 Mar 2013
 * \author	David Hebbeker
 */

#ifndef XPCC__TCS3414_HPP
#	error	"Don't include this file directly, use 'software_i2c.hpp' instead!"
#endif

template<typename I2cMaster>
xpcc::i2c::WriteReadAdapter xpcc::Tcs3414<I2cMaster>::i2cWRadapter;

template<typename I2cMaster>
typename xpcc::Tcs3414<I2cMaster>::Data xpcc::Tcs3414<I2cMaster>::data;

template<typename I2cMaster>
typename xpcc::tcs3414::Rgb xpcc::Tcs3414<I2cMaster>::color;

template<typename I2cMaster>
xpcc::tcs3414::OperationSuccess xpcc::Tcs3414<I2cMaster>::configure(const Gain gain,
	const Prescaler prescaler, const IntegrationMode mode,
	const uint8_t time) {
	if(!setGain(gain, prescaler)) {
		return false; }
	return setIntegrationTime(mode, time);
}

template<typename I2cMaster>
xpcc::tcs3414::OperationSuccess xpcc::Tcs3414<I2cMaster>::writeRegister(
	const RegisterAddress address, const uint8_t value) {
	uint8_t buffer_write[3] = { 0, 1 };	// set the number of bytes to 1
	buffer_write[0] =
				0x80							// write command
			|	0x40							// with SMB read/write protocol
			|	static_cast<uint8_t>(address);	// at this address
	buffer_write[2] = value;
	i2cWRadapter.initialize(ADDRESS, buffer_write, 3, NULL, 0);
	if(!I2cMaster::startBlocking(&i2cWRadapter))
		return false;
	if(i2cWRadapter.getState() != xpcc::i2c::adapter::State::NO_ERROR)
		return false;
	return true;
}

template<typename I2cMaster>
xpcc::tcs3414::OperationSuccess xpcc::Tcs3414<I2cMaster>::readRegisters(
	const RegisterAddress address, uint8_t* const values, const uint8_t count) {
	const uint8_t buffer_write =
				static_cast<uint8_t>(0x80)		// write command
			|	static_cast<uint8_t>(0x40)		// with SMB read/write protocol
			|	static_cast<uint8_t>(address);	// at this address
	i2cWRadapter.initialize(&buffer_write, 1, values, count);	// read registers
	if(!I2cMaster::startBlocking(&i2cWRadapter))
		return false;
	if(i2cWRadapter.getState() != xpcc::i2c::adapter::State::NO_ERROR)
		return false;
	return true;
}




