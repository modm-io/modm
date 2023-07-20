/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMI088_TRANSPORT_HPP
#error "Don't include this file directly, use 'bmi088_transport.hpp' instead!"
#endif

#include <modm/processing/fiber.hpp>

namespace modm
{

template<typename SpiMaster, typename AccCs, typename GyroCs>
void
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::initialize()
{
	// rising edge on CS required to enable accelerometer SPI mode
	AccCs::setOutput(false);
	modm::delay_ns(100);
	AccCs::setOutput(true);

	GyroCs::setOutput(true);
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
std::span<uint8_t>
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::readRegisters(AccRegister startReg,
										uint8_t count)
{
	return readRegisters<AccCs>(static_cast<uint8_t>(startReg), count, true);
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
std::span<uint8_t>
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::readRegisters(GyroRegister startReg,
										uint8_t count)
{
	return readRegisters<GyroCs>(static_cast<uint8_t>(startReg), count, false);
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
template<typename Cs>
std::span<uint8_t>
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::readRegisters(uint8_t startReg,
										uint8_t count, bool dummyByte)
{
	if (count > MaxRegisterSequence) {
		return {};
	}

	while (!this->acquireMaster()) {
		modm::fiber::yield();
	}
	Cs::reset();

	const uint8_t dataOffset = (dummyByte ? 2 : 1);

	txBuffer_[0] = startReg | ReadFlag;
	txBuffer_[1] = 0;

	SpiMaster::transfer(&txBuffer_[0], &rxBuffer_[0], count + dataOffset);

	if (this->releaseMaster()) {
		Cs::set();
	}

	return std::span{&rxBuffer_[dataOffset], count};
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
bool
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::writeRegister(AccRegister reg, uint8_t data)
{
	return writeRegister<AccCs>(static_cast<uint8_t>(reg), data);
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
bool
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::writeRegister(GyroRegister reg, uint8_t data)
{
	return writeRegister<GyroCs>(static_cast<uint8_t>(reg), data);
}

template<typename SpiMaster, typename AccCs, typename GyroCs>
template<typename Cs>
bool
Bmi088SpiTransport<SpiMaster, AccCs, GyroCs>::writeRegister(uint8_t reg, uint8_t data)
{
	while (!this->acquireMaster()) {
		modm::fiber::yield();
	}
	Cs::reset();

	txBuffer_[0] = reg;
	txBuffer_[1] = data;
	SpiMaster::transfer(&txBuffer_[0], nullptr, 2);

	if (this->releaseMaster()) {
		Cs::set();
	}

	return true;
}

// ------------------------------------------------------------------------------------------------

template<typename I2cMaster>
Bmi088I2cTransport<I2cMaster>::Bmi088I2cTransport(uint8_t accAddress, uint8_t gyroAddress)
	: I2cDevice<I2cMaster>{accAddress}, accAddress_{accAddress}, gyroAddress_{gyroAddress}
{
}

template<typename I2cMaster>
void
Bmi088I2cTransport<I2cMaster>::initialize()
{
}

template<typename I2cMaster>
std::span<uint8_t>
Bmi088I2cTransport<I2cMaster>::readRegisters(AccRegister startReg, uint8_t count)
{
	this->transaction.setAddress(accAddress_);
	return readRegisters(static_cast<uint8_t>(startReg), count);
}

template<typename I2cMaster>
std::span<uint8_t>
Bmi088I2cTransport<I2cMaster>::readRegisters(GyroRegister startReg, uint8_t count)
{
	this->transaction.setAddress(gyroAddress_);
	return readRegisters(static_cast<uint8_t>(startReg), count);
}

template<typename I2cMaster>
std::span<uint8_t>
Bmi088I2cTransport<I2cMaster>::readRegisters(uint8_t startReg, uint8_t count)
{
	if (count > MaxRegisterSequence) {
		return {};
	}

	this->transaction.configureWriteRead(&startReg, 1, &buffer_[0], count);
	const bool success = this->runTransaction();

	if (success) {
		return std::span{&buffer_[0], count};
	} else {
		return {};
	}
}

template<typename I2cMaster>
bool
Bmi088I2cTransport<I2cMaster>::writeRegister(AccRegister reg, uint8_t data)
{
	this->transaction.setAddress(accAddress_);
	return writeRegister(static_cast<uint8_t>(reg), data);
}

template<typename I2cMaster>
bool
Bmi088I2cTransport<I2cMaster>::writeRegister(GyroRegister reg, uint8_t data)
{
	this->transaction.setAddress(gyroAddress_);
	return writeRegister(static_cast<uint8_t>(reg), data);
}

template<typename I2cMaster>
bool
Bmi088I2cTransport<I2cMaster>::writeRegister(uint8_t reg, uint8_t data)
{
	buffer_[0] = reg;
	buffer_[1] = data;
	this->transaction.configureWrite(&buffer_[0], 2);

	return this->runTransaction();
}

} // namespace modm
