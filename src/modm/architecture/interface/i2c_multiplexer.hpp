/*
 * Copyright (c) 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_I2C_MULTIPLEXER_HPP
#define MODM_I2C_MULTIPLEXER_HPP

#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * Interface of an I2C Multiplexer.
 *
 * All I2C multiplexer driver should implement this interface.
 * @ingroup modm_architecture_i2c_multiplexer
 */
template <
	auto &multiplexer,
	typename std::remove_reference_t<decltype(multiplexer)>::Channel channel >
	class I2cMultiplexerChannel : public modm::I2cMaster
{
public:
	static constexpr auto &i2cMultiplexer = multiplexer;

public:
	static bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr);
};

/// @ingroup modm_architecture_i2c_multiplexer
struct i2cMultiplexer
{
public:
	enum class
	Channel : uint8_t
	{
		Ch0 = 0,
		Ch1 = 1,
		Ch2 = 2,
		Ch3 = 3,
		Ch4 = 4,
		Ch5 = 5,
		Ch6 = 6,
		Ch7 = 7
	};
}; // struct i2cMultiplexer


/// @ingroup modm_architecture_i2c_multiplexer
template < class I2cMaster, class I2cMultiplexerDevice >
class I2cMultiplexer : public i2cMultiplexer, public modm::I2c
{
public:
	/// Constructor, sets address to default of 0x70
	I2cMultiplexer();

	static bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr)
	{
		return I2cMaster::start(transaction, handler);
	}

public:
	/// Alias-templates for simpler use of the Channels
	/// @{
	template < auto &object >
	using Ch0 = I2cMultiplexerChannel< object, Channel::Ch0 >;
	template < auto &object >
	using Ch1 = I2cMultiplexerChannel< object, Channel::Ch1 >;
	template < auto &object >
	using Ch2 = I2cMultiplexerChannel< object, Channel::Ch2 >;
	template < auto &object >
	using Ch3 = I2cMultiplexerChannel< object, Channel::Ch3 >;
	template < auto &object >
	using Ch4 = I2cMultiplexerChannel< object, Channel::Ch4 >;
	template < auto &object >
	using Ch5 = I2cMultiplexerChannel< object, Channel::Ch5 >;
	template < auto &object >
	using Ch6 = I2cMultiplexerChannel< object, Channel::Ch6 >;
	template < auto &object >
	using Ch7 = I2cMultiplexerChannel< object, Channel::Ch7 >;
	/// @}

	I2cMultiplexerDevice multiplexerDevice;
};

} // modm namespace

template < class I2cMaster, class I2cMultiplexerDevice  >
modm::I2cMultiplexer<I2cMaster, I2cMultiplexerDevice >::I2cMultiplexer()
{
}

template <
	auto &multiplexer,
	typename std::remove_reference_t<decltype(multiplexer)>::Channel channel >
bool
modm::I2cMultiplexerChannel<multiplexer, channel>::start(modm::I2cTransaction *transaction, ConfigurationHandler handler)
{
	// If call to multiplexer failed, return without doing the actual transaction
	if (RF_CALL_BLOCKING(multiplexer.multiplexerDevice.setActiveChannel(static_cast<uint8_t>(channel))) == false) {
		return false;
	}

	return multiplexer.start(transaction, handler);
}

#endif // MODM_I2C_MULTIPLEXER_HPP
