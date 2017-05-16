/*
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2012, 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX6966_HPP
#	error "Don't include this file directly, use 'max6966.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::initialize(max6966::Current current, uint8_t config)
{
	Cs::setOutput(modm::Gpio::High);

	setAllConfiguration(config | max6966::CONFIG_RUN_MODE);
	setAllCurrent(current);
}

// MARK: configuration
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::setAllConfiguration(uint8_t config)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking(max6966::REGISTER_CONFIGURATION);
		Spi::transferBlocking(config);
	}
	Cs::set();
}

// MARK: channels and currents
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::setChannel(uint16_t channel, uint8_t value)
{
	if (channel >= DRIVERS*10)
		return;

	uint8_t driver = channel / 10;
	uint8_t reg = channel % 10;

	writeToDriver(driver, static_cast<max6966::Register>(reg), value);
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::setChannels(uint8_t * values)
{
	// for all channels
	for (uint_fast8_t ch = 0; ch < 10; ++ch)
	{
		Cs::reset();
		// for all drivers
		for (uint_fast8_t dr = 0; dr < DRIVERS; ++dr)
		{
			Spi::transferBlocking(max6966::REGISTER_PORT0 + ch);
			Spi::transferBlocking(values[ch + dr * 10]);
		}
		Cs::set();
	}
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::setAllChannels(uint8_t value)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking(max6966::REGISTER_PORT0_9);
		Spi::transferBlocking(value);
	}
	Cs::set();
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
uint8_t
modm::MAX6966<Spi, Cs, DRIVERS>::getChannel(uint16_t channel)
{
	if (channel >= DRIVERS*10)
		return 0;

	uint8_t driver = channel / 10;
	uint8_t reg = channel % 10;

	return readFromDriver(driver, static_cast<max6966::Register>(reg));
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::setHalfCurrent(uint16_t channel, bool full)
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
modm::MAX6966<Spi, Cs, DRIVERS>::setAllCurrent(max6966::Current current)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking(max6966::REGISTER_GLOBAL_CURRENT);
		Spi::transferBlocking(current);
	}
	Cs::set();
}

// MARK: protected
template<typename Spi, typename Cs, uint8_t DRIVERS>
void
modm::MAX6966<Spi, Cs, DRIVERS>::writeToDriver(uint8_t driver, max6966::Register reg, uint8_t data)
{
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking((i == driver) ? (reg | max6966::WRITE) : max6966::REGISTER_NO_OP);
		Spi::transferBlocking(data);
	}
	Cs::set();
}

template<typename Spi, typename Cs, uint8_t DRIVERS>
uint8_t
modm::MAX6966<Spi, Cs, DRIVERS>::readFromDriver(uint8_t driver, max6966::Register reg)
{
	// write the register we want to read
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking((i == driver) ? (reg | max6966::READ) : max6966::REGISTER_NO_OP);
		Spi::transferBlocking(0xff);
	}
	Cs::set();

	// TODO: Add delay here?
//	modm::delayMilliseconds(1);

	// send dummy data and get the right register
	uint8_t data=0;
	uint8_t buffer;
	Cs::reset();
	for (uint_fast8_t i=0; i < DRIVERS; ++i)
	{
		Spi::transferBlocking(max6966::REGISTER_NO_OP);
		buffer = Spi::transferBlocking(0xff);
		if (i == driver) data = buffer;
	}
	Cs::set();

	return data;
}
