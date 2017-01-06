/*
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TCS3414_HPP
#	error	"Don't include this file directly, use 'tcs3414.hpp' instead!"
#endif

template<typename I2cMaster>
typename modm::Tcs3414<I2cMaster>::Data modm::Tcs3414<I2cMaster>::data;

template<typename I2cMaster>
typename modm::tcs3414::Rgb modm::Tcs3414<I2cMaster>::color;

template < typename I2cMaster >
modm::Tcs3414<I2cMaster>::Tcs3414(uint8_t address)
: I2cDevice<I2cMaster,2>(address),
  commandBuffer{0,0,0,0}
{
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::configure(
		const Gain gain,
		const Prescaler prescaler,
		const IntegrationMode mode,
		const uint8_t time)
{
	RF_BEGIN();

	if ( RF_CALL(setGain(gain, prescaler)) )
	{
		if ( RF_CALL(setIntegrationTime(mode, time)) )
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::refreshAllColors()
{
	RF_BEGIN();

	if ( RF_CALL(readRegisters(
			RegisterAddress::DATA1LOW,
			data.dataBytes,
			sizeof(data.dataBytes)
	) ) )
	{
		// adapt the values to the overall light intensity
		// so that R + G + B = C
		color.red	= data.red.get();
		color.green	= data.green.get();
		color.blue	= data.blue.get();

		{
			// START --> This part is not really necessary
			// Rationale:
			// Imagine a low band light. For example a green laser. In case the filters
			// of this sensors do not transfer this wavelength well, it might
			// result in all colors being very low. The clear value will not
			// filter colors and thus it will see a bright light (intensity).
			// In order to still have some signal the very low green value can be
			// amplified with the clear value.
			const float c =	static_cast<float>(color.red) +
					static_cast<float>(color.green) +
					static_cast<float>(color.blue);
			const float f = data.clear.get() / c;
			color.red	*= f;
			color.green	*= f;
			color.blue	*= f;
		}

		// <-- END
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::writeRegister(
		const RegisterAddress address,
		const uint8_t value)
{
	RF_BEGIN();

	commandBuffer[0] =
				0x80							// write command
			|	0x40							// with SMB read/write protocol
			|	static_cast<uint8_t>(address);	// at this address
	commandBuffer[2] = value;

	this->transaction.configureWrite(commandBuffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::readRegisters(
		const RegisterAddress address,
		uint8_t* const values,
		const uint8_t count)
{
	RF_BEGIN();

	commandBuffer[0] =
			static_cast<uint8_t>(0x80)		// write command
			| static_cast<uint8_t>(0x40)		// with SMB read/write protocol
			| static_cast<uint8_t>(address);	// at this address

	this->transaction.configureWriteRead(commandBuffer, 1, values, count);

	RF_END_RETURN_CALL( this->runTransaction() );
}
