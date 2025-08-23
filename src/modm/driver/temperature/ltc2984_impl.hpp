// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LTC2984_HPP
	#error	"Don't include this file directly, use 'ltc2984.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < class SpiMaster, class Cs >
modm::Ltc2984<SpiMaster, Cs>::Ltc2984()
    : enabledChannels(0)
{
	Cs::setOutput(modm::Gpio::High);
}

template < class SpiMaster, class Cs >
bool
modm::Ltc2984<SpiMaster, Cs>::ping()
{
	// Store first byte of Custom Sensor Table Data into buffer[4]
	readByte(Register::CustomDataTable, buffer[4]);
	buffer[5] = 0x42;
	writeData(Register::CustomDataTable, &buffer[5], 1);
	readByte(Register::CustomDataTable, buffer[6]);
	// Restore first byte of Custom Sensor Table Data from buffer[4]
	writeData(Register::CustomDataTable, &buffer[4], 1);
	return buffer[5] == buffer[6];
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::configure(ltc2984::Configuration::Rejection rejection,
                                        ltc2984::Configuration::TemperatureUnit temperatureUnit,
                                        uint8_t muxDelay)
{
	buffer[0] = static_cast<uint8_t>(rejection) | static_cast<uint8_t>(temperatureUnit);
	writeData(ltc2984::Register::GlobalConfiguration, buffer, 1);
	return writeData(ltc2984::Register::MuxConfigDelay, muxDelay, 1);
}

template < class SpiMaster, class Cs >
bool
modm::Ltc2984<SpiMaster, Cs>::isBusy()
{
	readByte(Register::CommandStatus, buffer[0]);
	return (static_cast<CommandStatus>(buffer[0]) & CommandStatus::DoneAndStart) == CommandStatus::Start;
}

// ----------------------------------------------------------------------------
template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::configureChannel(ltc2984::Channel channel, uint32_t config)
{
	return writeData(ltc2984::Register::ChannelConfig + channel, reinterpret_cast<uint8_t*>(&config), 4);
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::initiateSingleMeasurement(ltc2984::Channel channel)
{
	return writeData(ltc2984::Register::CommandStatus, ltc2984::CommandStatus::Start | channel);
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::initiateMeasurements()
{
	return writeData(ltc2984::Register::CommandStatus, CommandStatus::Start);
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::enableChannel(ltc2984::Configuration::MuxChannel channel)
{
	enabledChannels |= channel;
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::disableChannel(ltc2984::Configuration::MuxChannel channel)
{
	enabledChannels &= ~channel;
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::setChannels()
{
	return writeData(ltc2984::Register::MuxChannels, reinterpret_cast<uint8_t*>(&enabledChannels), 4);
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::enterSleepMode()
{
	return writeData(ltc2984::Register::CommandStatus, CommandStatus::Sleep);
}

// ----------------------------------------------------------------------------
template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::readChannel(ltc2984::Channel channel, ltc2984::Data& value)
{
	return readFourBytes(ltc2984::Register::Results + channel, reinterpret_cast<uint8_t*>(&value.data));
}

// ----------------------------------------------------------------------------
template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::writeData(Register address, uint8_t* data, size_t length)
{
	if (length > 4) {
		return;
	}

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = Write;

	buffer[1] = static_cast<uint16_t>(address) >> 8;
	buffer[2] = static_cast<uint16_t>(address);

	// swap byte order
	for (uint8_t i = 0; i < length; i++) {
		buffer[3+i] = data[length-1-i];
	}

	SpiMaster::transfer(buffer, nullptr, length+3);

	if (this->releaseMaster()) {
		Cs::set();
	}
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::writeData(Register address, CommandStatus command)
{
	return writeData(address, reinterpret_cast<uint8_t*>(&command), 1);
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::readFourBytes(Register address, uint8_t* data)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = Read;

	buffer[1] = static_cast<uint16_t>(address) >> 8;
	buffer[2] = static_cast<uint16_t>(address);

	SpiMaster::transfer(buffer, nullptr, 3);
	SpiMaster::transfer(nullptr, buffer, 4);

	// swap byte order
	data[0] = buffer[3];
	data[1] = buffer[2];
	data[2] = buffer[1];
	data[3] = buffer[0];

	if (this->releaseMaster()) {
		Cs::set();
	}
}

template < class SpiMaster, class Cs >
void
modm::Ltc2984<SpiMaster, Cs>::readByte(Register address, uint8_t& command)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = Read;

	buffer[1] = static_cast<uint16_t>(address) >> 8;
	buffer[2] = static_cast<uint16_t>(address);

	SpiMaster::transfer(buffer, nullptr, 3);
	SpiMaster::transfer(nullptr, &command, 1);

	if (this->releaseMaster()) {
		Cs::set();
	}
}
