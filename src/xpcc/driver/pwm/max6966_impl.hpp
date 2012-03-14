// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MAX6966_HPP
#	error "Don't include this file directly, use 'max6966.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::initialize(max6966::Current current, uint8_t config)
{
	Cs::setOutput(xpcc::gpio::HIGH);
	
	setAllConfiguration(config | max6966::CONFIG_RUN_MODE);
	setAllCurrent(current);
}

// MARK: configuration
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::setAllConfiguration(uint8_t config)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write(max6966::REGISTER_CONFIGURATION);
		Spi::write(config);
	}
	Cs::set();
}

// MARK: channels and currents
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::setChannel(uint16_t channel, uint8_t value)
{
	if (channel >= DRIVERS*10)
		return;
	
	uint8_t driver = channel / 10;
	uint8_t reg = channel % 10;
	
	writeToDriver(driver, static_cast<max6966::Register>(reg), value);
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::setAllChannels(uint8_t value)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write(max6966::REGISTER_PORT0_9);
		Spi::write(value);
	}
	Cs::set();
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
uint8_t
xpcc::MAX6966<Spi, Cs, DRIVERS>::getChannel(uint16_t channel)
{
	if (channel >= DRIVERS*10)
		return;
	
	uint8_t driver = channel / 10;
	uint8_t reg = channel % 10;
	
	return readFromDriver(driver, static_cast<max6966::Register>(reg));
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::setHalfCurrent(uint16_t channel, bool full)
{
	if (channel >= DRIVERS*10)
		return;
	
	uint8_t driver = channel / 10;
	uint8_t driverChannel = channel % 10;
	uint8_t mask;
	max6966::Register reg;
	
	if (driverChannel <= 7)
	{
		mask = (1 << driverChannel);
		reg = max6966::REGISTER_CURRENT7_0;
	}
	else {
		mask = (1 << (driverChannel - 8));
		reg = max6966::REGISTER_CURRENT9_8;
	}
	uint8_t bit = full ? mask : 0;
	
	writeToDriverMasked(driver, reg, bit, mask);
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::setAllCurrent(max6966::Current current)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write(max6966::REGISTER_GLOBAL_CURRENT);
		Spi::write(current);
	}
	Cs::set();
}

// MARK: protected
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
xpcc::MAX6966<Spi, Cs, DRIVERS>::writeToDriver(uint8_t driver, max6966::Register reg, uint8_t data)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write((i == driver) ? reg : max6966::REGISTER_NO_OP);
		Spi::write(data);
	}
	Cs::set();
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
uint8_t
xpcc::MAX6966<Spi, Cs, DRIVERS>::readFromDriver(uint8_t driver, max6966::Register reg)
{
	// write the register we want to read
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write((i == driver) ? (reg | max6966::READ) : max6966::REGISTER_NO_OP);
		Spi::write(0xff);
	}
	Cs::set();
	
	// TODO: Add delay here?
//	xpcc::delay_ms(1);
	
	// send dummy data and get the right register
	uint8_t data=0;
	uint8_t buffer;
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::write(max6966::REGISTER_NO_OP);
		buffer = Spi::write(0xff);
		if (i == driver) data = buffer;
	}
	Cs::set();
	
	return data;
}
