// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_IXM42XXX_TRANSPORT_HPP
#	error  "Don't include this file directly, use 'ixm42xxx_transport.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template < class I2cMaster >
modm::Ixm42xxxTransportI2c<I2cMaster>::Ixm42xxxTransportI2c(uint8_t address)
    : I2cDevice<I2cMaster, 2>(address)
{
}

// ----------------------------------------------------------------------------

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportI2c<I2cMaster>::write(uint8_t reg, uint8_t value)
{
    RF_BEGIN();

    buffer[0] = reg;
    buffer[1] = value;

    this->transaction.configureWrite(buffer, 2);

    RF_END_RETURN_CALL( this->runTransaction() );
}

// ----------------------------------------------------------------------------

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportI2c<I2cMaster>::read(uint8_t reg, uint8_t &value)
{
    return read(reg, &value, 1);
}

// ----------------------------------------------------------------------------

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportI2c<I2cMaster>::read(uint8_t reg, uint8_t *buffer, std::size_t length)
{
    RF_BEGIN();

    this->buffer[0] = reg;
    this->transaction.configureWriteRead(this->buffer, 1, buffer, length);

    RF_END_RETURN_CALL( this->runTransaction() );
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::Ixm42xxxTransportSpi<SpiMaster, Cs>::Ixm42xxxTransportSpi(uint8_t /*address*/)
    : whoAmI(0)
{
    Cs::setOutput(modm::Gpio::High);
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportSpi<SpiMaster, Cs>::ping()
{
    RF_BEGIN();

    whoAmI = 0;
    RF_CALL(read(0x75, whoAmI));

    RF_END_RETURN(whoAmI != 0);
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportSpi<SpiMaster, Cs>::write(uint8_t reg, uint8_t value)
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

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportSpi<SpiMaster, Cs>::read(uint8_t reg, uint8_t &value)
{
    return read(reg, &value, 1);
}

// ----------------------------------------------------------------------------

template < class SpiMaster, class Cs >
modm::ResumableResult<bool>
modm::Ixm42xxxTransportSpi<SpiMaster, Cs>::read(uint8_t reg, uint8_t *buffer, std::size_t length)
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
