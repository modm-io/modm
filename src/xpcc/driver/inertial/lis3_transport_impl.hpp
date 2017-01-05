/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LIS3_TRANSPORT_HPP
#	error  "Don't include this file directly, use 'lis3_transport.hpp' instead!"
#endif

// ============================================================================
// MARK: I2C TRANSPORT
template < class I2cMaster >
modm::Lis3TransportI2c<I2cMaster>::Lis3TransportI2c(uint8_t address)
:	I2cDevice<I2cMaster, 2>(address)
{
}

// MARK: - register access
// MARK: write register
template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3TransportI2c<I2cMaster>::write(uint8_t reg, uint8_t value)
{
	RF_BEGIN();

	buffer[0] = reg;
	buffer[1] = value;

	this->transaction.configureWrite(buffer, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read register
template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3TransportI2c<I2cMaster>::read(uint8_t reg, uint8_t *buffer, uint8_t length)
{
	RF_BEGIN();

	this->buffer[0] = reg;
	this->transaction.configureWriteRead(this->buffer, 1, buffer, length);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// ============================================================================
// MARK: SPI TRANSPORT
template < class SpiMaster, class Cs >
modm::Lis3TransportSpi<SpiMaster, Cs>::Lis3TransportSpi(uint8_t /*address*/)
:	whoAmI(0)
{
	Cs::setOutput(modm::Gpio::High);
}

// MARK: ping
template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Lis3TransportSpi<SpiMaster, Cs>::ping()
{
	RF_BEGIN();

	whoAmI = 0;

	RF_CALL(read(0x0F, whoAmI));

	RF_END_RETURN(whoAmI != 0);
}

// MARK: - register access
// MARK: write register
template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Lis3TransportSpi<SpiMaster, Cs>::write(uint8_t reg, uint8_t value)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(reg | Write));
	RF_CALL(SpiMaster::transfer(value));

	if (this->releaseMaster())
		Cs::set();

	RF_END_RETURN(true);
}

// MARK: read register
template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Lis3TransportSpi<SpiMaster, Cs>::read(uint8_t reg, uint8_t *buffer, uint8_t length)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(reg | Read));

	RF_CALL(SpiMaster::transfer(nullptr, buffer, length));

	if (this->releaseMaster())
		Cs::set();

	RF_END_RETURN(true);
}

