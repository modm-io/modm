// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AD7928_HPP
#	error "Don't include this file directly! Use 'ad7928.hpp' instead."
#endif

// ----------------------------------------------------------------------------
namespace modm
{

template <typename SpiMaster, typename Cs>
Ad7928<SpiMaster, Cs>::Ad7928() : currentPowerMode{PowerMode::Normal}
{
	config = ControlRegister::WriteControlReg | ControlRegister::VrefRange;
	config |= ControlRegister::UnsignedOutput;
	SequenceMode_t::set(config, SequenceMode::NoSequence);
	PowerMode_t::set(config, PowerMode::Normal);
	InputChannel_t::reset(config);
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::initialize()
{
	Cs::setOutput(modm::Gpio::High);
	modm::this_fiber::sleep_for(1us);

	// reset device
	transfer(Register_t(0xFF));
	transfer(Register_t(0xFF));

	config |= ControlRegister::WriteControlReg;
	config |= ControlRegister::UnsignedOutput;

	SequenceMode_t::set(config, SequenceMode::NoSequence);
	PowerMode_t::set(config, PowerMode::Normal);
	InputChannel_t::reset(config);

	// write configuration
	transfer(config);
	currentPowerMode = PowerMode::Normal;
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ad7928::Data
Ad7928<SpiMaster, Cs>::singleConversion(ad7928::InputChannel channel)
{
	SequenceMode_t::set(config, SequenceMode::NoSequence);

	if(currentPowerMode == PowerMode::FullShutdown) {
		// power up device, will be in normal mode afterwards
		wakeup();
	} else if(currentPowerMode == PowerMode::AutoShutdown) {
		// dummy conversion to power up device
		// does not alter control register (write bit = 0)
		transfer(ControlRegister(0));
	}

	InputChannel_t::set(config, channel);
	transfer(config);
	InputChannel_t::reset(config);

	currentPowerMode = PowerMode_t::get(config);

	return data;
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::startSequence(ad7928::SequenceChannels_t channels1,
									 ad7928::SequenceChannels_t channels2)
{
	if(currentPowerMode == PowerMode::FullShutdown) {
		// power up device, will be in normal mode afterwards
		wakeup();
	} else if(currentPowerMode == PowerMode::AutoShutdown) {
		// dummy conversion to power up device
		// does not alter control register (write bit = 0)
		transfer(ControlRegister_t(0));
	}

	PowerMode_t::set(config, PowerMode::Normal);
	SequenceMode_t::set(config, SequenceMode::ProgramShadowRegister);
	transfer(config);
	currentPowerMode = PowerMode::Normal;

	// The next transfer writes to the shadow register
	Sequence1Channels_t::set(sequenceChannels, static_cast<SequenceChannels>(channels1.value));
	Sequence2Channels_t::set(sequenceChannels, static_cast<SequenceChannels>(channels2.value));
	transfer(sequenceChannels);
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ad7928::Data
Ad7928<SpiMaster, Cs>::nextSequenceConversion()
{
	SequenceMode_t::set(config, SequenceMode::ContinueSequence);

	// invalid if device is not in normal mode
	if(currentPowerMode != PowerMode::Normal) {
		return data;
	}

	transfer(config);
	currentPowerMode = PowerMode_t::get(config);

	return data;
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::setExtendedRange(bool enabled)
{
	if(enabled) {
		config &= ~ControlRegister::VrefRange;
	} else {
		config |= ControlRegister::VrefRange;
	}
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
bool
Ad7928<SpiMaster, Cs>::isExtendedRange()
{
	return !(config & ControlRegister::VrefRange);
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::setAutoShutdownEnabled(bool enabled)
{
	if(enabled) {
		PowerMode_t::set(config, PowerMode::AutoShutdown);
	} else {
		PowerMode_t::set(config, PowerMode::Normal);
	}
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
bool
Ad7928<SpiMaster, Cs>::isAutoShutdownEnabled()
{
	return PowerMode_t::get(config) == PowerMode::AutoShutdown;
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::fullShutdown()
{
	SequenceMode_t::set(config, SequenceMode::NoSequence);

	ControlRegister_t controlRegister = config;
	PowerMode_t::set(controlRegister, PowerMode::FullShutdown);

	currentPowerMode = PowerMode::FullShutdown;

	return transfer(controlRegister);
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::wakeup()
{
	SequenceMode_t::set(config, SequenceMode::NoSequence);
	PowerMode_t::set(config, PowerMode::Normal);

	transfer(config);

	// Wait for the device to power up
	modm::this_fiber::sleep_for(1us);

	currentPowerMode = PowerMode::Normal;
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
void
Ad7928<SpiMaster, Cs>::transfer(Register_t reg)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });

	SpiMaster::setDataMode(SpiMaster::DataMode::Mode1);

	Cs::reset();

	outBuffer[0] = (reg.value & 0xFF00) >> 8;
	outBuffer[1] = reg.value & 0xFF;

	SpiMaster::transfer(outBuffer, data.data, 2);

	if (this->releaseMaster())
		Cs::set();
}

} // namespace modm
