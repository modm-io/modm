// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AD7928_HPP
#	error "Don't include this file directly! Use 'ad7928.hpp' instead."
#endif

// ----------------------------------------------------------------------------
namespace xpcc
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
ResumableResult<void>
Ad7928<SpiMaster, Cs>::initialize()
{
	RF_BEGIN();

	Cs::setOutput(xpcc::Gpio::High);
	delayMicroseconds(1);

	// reset device
	RF_CALL(transfer(Register_t(0xFF)));
	RF_CALL(transfer(Register_t(0xFF)));

	config |= ControlRegister::WriteControlReg;
	config |= ControlRegister::UnsignedOutput;

	SequenceMode_t::set(config, SequenceMode::NoSequence);
	PowerMode_t::set(config, PowerMode::Normal);
	InputChannel_t::reset(config);

	// write configuration
	RF_CALL(transfer(config));
	currentPowerMode = PowerMode::Normal;

	RF_END();
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<ad7928::Data>
Ad7928<SpiMaster, Cs>::singleConversion(ad7928::InputChannel channel)
{
	RF_BEGIN();

	SequenceMode_t::set(config, SequenceMode::NoSequence);

	if(currentPowerMode == PowerMode::FullShutdown) {
		// power up device, will be in normal mode afterwards
		RF_CALL(wakeup());
	} else if(currentPowerMode == PowerMode::AutoShutdown) {
		// dummy conversion to power up device
		// does not alter control register (write bit = 0)
		RF_CALL(transfer(ControlRegister(0)));
	}

	InputChannel_t::set(config, channel);
	RF_CALL(transfer(config));
	InputChannel_t::reset(config);

	currentPowerMode = PowerMode_t::get(config);

	RF_END_RETURN(data);
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<void>
Ad7928<SpiMaster, Cs>::startSequence(ad7928::SequenceChannels_t channels1,
									 ad7928::SequenceChannels_t channels2)
{
	RF_BEGIN();

	if(currentPowerMode == PowerMode::FullShutdown) {
		// power up device, will be in normal mode afterwards
		RF_CALL(wakeup());
	} else if(currentPowerMode == PowerMode::AutoShutdown) {
		// dummy conversion to power up device
		// does not alter control register (write bit = 0)
		RF_CALL(transfer(ControlRegister_t(0)));
	}

	PowerMode_t::set(config, PowerMode::Normal);
	SequenceMode_t::set(config, SequenceMode::ProgramShadowRegister);
	RF_CALL(transfer(config));
	currentPowerMode = PowerMode::Normal;

	// The next transfer writes to the shadow register
	Sequence1Channels_t::set(sequenceChannels, static_cast<SequenceChannels>(channels1.value));
	Sequence2Channels_t::set(sequenceChannels, static_cast<SequenceChannels>(channels2.value));
	RF_CALL(transfer(sequenceChannels));

	RF_END();
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<ad7928::Data>
Ad7928<SpiMaster, Cs>::nextSequenceConversion()
{
	RF_BEGIN();

	SequenceMode_t::set(config, SequenceMode::ContinueSequence);

	// invalid if device is not in normal mode
	if(currentPowerMode != PowerMode::Normal) {
		RF_RETURN(data);
	}

	RF_CALL(transfer(config));
	currentPowerMode = PowerMode_t::get(config);

	RF_END_RETURN(data);
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
ResumableResult<void>
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
ResumableResult<void>
Ad7928<SpiMaster, Cs>::wakeup()
{
	RF_BEGIN();
	SequenceMode_t::set(config, SequenceMode::NoSequence);
	PowerMode_t::set(config, PowerMode::Normal);

	RF_CALL(transfer(config));

	// Wait for the device to power up
	delayMicroseconds(1);

	currentPowerMode = PowerMode::Normal;

	RF_END();
}

// ----------------------------------------------------------------------------
template <typename SpiMaster, typename Cs>
ResumableResult<void>
Ad7928<SpiMaster, Cs>::transfer(Register_t reg)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());

	SpiMaster::setDataMode(SpiMaster::DataMode::Mode1);

	Cs::reset();

	outBuffer[0] = (reg.value & 0xFF00) >> 8;
	outBuffer[1] = reg.value & 0xFF;

	RF_CALL(SpiMaster::transfer(outBuffer, data.data, 2));

	if (this->releaseMaster())
		Cs::set();

	RF_END();
}

} // namespace xpcc
