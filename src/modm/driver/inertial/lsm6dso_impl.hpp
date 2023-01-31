/*
 * Copyright (c) 2014-2015, Niklas Hauser
 * Copyright (c) 2022-2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LSM6DSO_HPP
#	error  "Don't include this file directly, use 'lsm6dso.hpp' instead!"
#endif

namespace modm
{

template<class Transport>
Lsm6dso<Transport>::Lsm6dso(uint8_t address)
:	Transport(address), buffer{}
{
}

template<class Transport>
modm::ResumableResult<bool>
Lsm6dso<Transport>::initialize()
{
	RF_BEGIN();
	// nothing
	RF_END_RETURN(true);
}

template<class Transport>
modm::ResumableResult<std::optional<uint8_t>>
Lsm6dso<Transport>::readRegister(Register reg)
{
	RF_BEGIN();
	if (!RF_CALL(this->read(i(reg), buffer[0])))
	{
		RF_RETURN(std::nullopt);
	}
	RF_END_RETURN(buffer[0]);
}

template<class Transport>
modm::ResumableResult<bool>
Lsm6dso<Transport>::readRegisters(Register reg, std::span<uint8_t> values)
{
	return this->read(i(reg), &values[0], values.size());
}

template<class Transport>
modm::ResumableResult<bool>
Lsm6dso<Transport>::writeRegister(Register reg, uint8_t value)
{
	return this->write(i(reg), value);
}

template<class Transport>
template<frequency_t outputDataRate, percent_t tolerance>
modm::ResumableResult<bool>
Lsm6dso<Transport>::setOutputDataRate()
{
	constexpr auto actualOutputDataRate = getClosestOdr<outputDataRate>();
	modm::PeripheralDriver::assertBaudrateInTolerance<std::lround(actualOutputDataRate.first * 100), outputDataRate * 100, tolerance>();

	// Set linear and angular ODR
	RF_BEGIN();
	if (! RF_CALL(this->read(i(Register::CTRL1_XL), buffer, 2)))
	{
		RF_RETURN(false);
	}

	buffer[0] = (actualOutputDataRate.second << 4) | (buffer[0] & ~Ctrl1XlOutputDataRateMask);
	buffer[1] = (actualOutputDataRate.second << 4) | (buffer[1] & ~Ctrl2GOutputDataRateMask);

	if (!RF_CALL(writeRegister(Register::CTRL1_XL, buffer[0])))
	{
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(writeRegister(Register::CTRL2_G, buffer[1]));
}

template<class Transport>
modm::ResumableResult<bool>
Lsm6dso<Transport>::setRange(LinearRange lr, AngularRange ar)
{
	RF_BEGIN();
	if (! RF_CALL(this->read(i(Register::CTRL1_XL), buffer, 2)))
	{
		RF_RETURN(false);
	}

	buffer[0] = i(lr) | (buffer[0] & ~Ctrl1XlLinearRangeMask);
	buffer[1] = i(ar) | (buffer[1] & ~Ctrl2GAngularRangeMask);

	if (!RF_CALL(writeRegister(Register::CTRL1_XL, buffer[0])))
	{
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(writeRegister(Register::CTRL2_G, buffer[1]));
}

template<class Transport>
template<frequency_t outputDataRate, percent_t tolerance>
modm::ResumableResult<bool>
Lsm6dso<Transport>::setOutputDataRateAndRange(LinearRange lr, AngularRange ar)
{
	constexpr auto actualOutputDataRate = getClosestOdr<outputDataRate>();
	modm::PeripheralDriver::assertBaudrateInTolerance<std::lround(actualOutputDataRate.first * 100), outputDataRate * 100, tolerance>();

	RF_BEGIN();
	if (!RF_CALL(writeRegister(Register::CTRL1_XL, (actualOutputDataRate.second << 4) | i(lr))))
	{
		RF_RETURN(false);
	}
	RF_END_RETURN_CALL(writeRegister(Register::CTRL2_G, (actualOutputDataRate.second << 4) | i(ar)));
}

} // namespace modm
