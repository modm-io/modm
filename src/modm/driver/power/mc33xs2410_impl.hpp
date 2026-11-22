// coding: utf-8
/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MC33XS2410_HPP
#	error "Don't include this file directly! Use 'mc33xs2410.hpp' instead."
#endif

// ----------------------------------------------------------------------------
namespace modm
{

template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::initialize()
{
	Cs::setOutput(modm::Gpio::High);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setMode(TransitMode mode, uint8_t device)
{
	GlobalControl_t control(readRegister(device, Register::GLB_CTRL));
	TransitMode_t::set(control, mode);
	writeRegister(device, Register::GLB_CTRL, control.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setParallelMode(Output output, bool enable, uint8_t device)
{
	const auto bit = uint8_t(output) < uint8_t(Output::Output3) ?
			GlobalControl::ParallelOut12 : GlobalControl::ParallelOut34;

	GlobalControl_t control(readRegister(device, Register::GLB_CTRL));
	// The transit bits are not retained, so writing back what was read must
	// not request a transition, least of all 11b, which resets the device.
	TransitMode_t::set(control, TransitMode::None);
	control.update(bit, enable);
	writeRegister(device, Register::GLB_CTRL, control.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
bool
Mc33xs2410<SpiMaster, Cs, MaxDevices>::getParallelMode(Output output, uint8_t device)
{
	const auto bit = uint8_t(output) < uint8_t(Output::Output3) ?
			GlobalControl::ParallelOut12 : GlobalControl::ParallelOut34;
	return GlobalControl_t(readRegister(device, Register::GLB_CTRL)).any(bit);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setStrongMisoDrive(bool enable, uint8_t device)
{
	GlobalControl_t control(readRegister(device, Register::GLB_CTRL));
	TransitMode_t::set(control, TransitMode::None); // see setParallelMode()
	control.update(GlobalControl::DriveStrengthMedium, not enable);
	writeRegister(device, Register::GLB_CTRL, control.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setOutput(Output output, bool enable, uint8_t device)
{
	const auto bit = OutputControl(uint8_t(OutputControl::Output1) << uint8_t(output));

	OutputControl_t control(readRegister(device, Register::OUT1_4_CTRL));
	if (enable) control |= bit;
	else control &= ~bit;

	writeRegister(device, Register::OUT1_4_CTRL, control.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setWatchdogSafeModeOnTimeout(bool enable, uint8_t device)
{
	WatchdogControl_t wdt(readRegister(device, Register::WDT_REG));
	wdt.update(WatchdogControl::SafeModeOnTimeout, enable);
	writeRegister(device, Register::WDT_REG, wdt.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
bool
Mc33xs2410<SpiMaster, Cs, MaxDevices>::getOutput(Output output, uint8_t device)
{
	const InputOutputStatus_t status{readRegister(device, DiagnosticRegister::IN_OUT_STA)};
	return status.any(InputOutputStatus(uint16_t(InputOutputStatus::Output1) << uint8_t(output)));
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setVoltageThreshold(Output output, float lowerVoltage, float upperVoltage, uint8_t device)
{
	const uint8_t shift = 2 * uint8_t(output);
	uint16_t wv{};
	// The value of the read arrives during the first write
	const Command first[]{
		Command::read(Register::WV_CTRL, &wv, device),
		Command::write(Register(uint8_t(Register::OVW_OUT1) + shift), voltageThresholdCode(upperVoltage), device),
	};
	transfer(first);
	// OutputOn (VOUTx_ON, from CMM) rather than Continuous (VOUTx, from OVM):
	// VOUTx is refreshed even while the output is off, so it reads ~0 V there
	// and would immediately trip UndervoltageWarning against any threshold.
	// VOUTx_ON only updates while the output is actually on, and - like
	// IOUTx - is blanked by the first measurement delay (M_SETUP).
	wv = (wv & ~(0b11 << shift)) | (uint8_t(VoltageWarningSource::OutputOn) << shift);
	const Command second[]{
		Command::write(Register(uint8_t(Register::UVW_OUT1) + shift), voltageThresholdCode(lowerVoltage), device),
		Command::write(Register::WV_CTRL, uint8_t(wv), device),
	};
	transfer(second);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setCurrentThreshold(Output output, float lowerCurrent, float upperCurrent, uint8_t device)
{
	const uint8_t shift = 2 * uint8_t(output);
	uint16_t wc{};
	// The value of the read arrives during the first write
	const Command first[]{
		Command::read(Register::WC_CTRL, &wc, device),
		Command::write(Register(uint8_t(Register::OCW_OUT1) + shift), overcurrentWarningCode(upperCurrent), device),
	};
	transfer(first);
	wc = (wc & ~(0b11 << shift)) | (uint8_t(CurrentWarningSource::Continuous) << shift);
	const Command second[]{
		Command::write(Register(uint8_t(Register::UCW_OUT1) + shift), undercurrentWarningCode(lowerCurrent), device),
		Command::write(Register::WC_CTRL, uint8_t(wc), device),
	};
	transfer(second);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setOverloadProtection(Output output, float currentLimit,
	OverloadTimeout timeout, uint8_t device)
{
	writeRegister(device, Register(uint8_t(Register::OCL_OUT1) + uint8_t(output)),
		overcurrentWarningCode(currentLimit));

	// DisableLatchOff and TransitSafeMode bits are left clear: latch-off
	// stays enabled (reset default) and only the overloaded output is
	// affected, the rest of the chained device keeps running.
	const OverloadProtectionControl_t olp{OverloadTimeout_t(timeout)};
	writeRegister(device, Register::OLP_CTRL, olp.value);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::writeRegister(uint8_t device, Register reg, uint8_t data)
{
	const Command command = Command::write(reg, data, device);
	transfer({&command, 1});
}

template <class SpiMaster, class Cs, size_t MaxDevices>
uint16_t
Mc33xs2410<SpiMaster, Cs, MaxDevices>::readRegister(uint8_t device, uint8_t address, bool controlBank)
{
	uint16_t value{};
	const Command command{buildReadFrame(address, controlBank), &value, device};
	transfer({&command, 1});
	return value;
}


template <class SpiMaster, class Cs, size_t MaxDevices>
bool
Mc33xs2410<SpiMaster, Cs, MaxDevices>::transfer(std::span<const Command> commands)
{
	const auto find = [&](uint8_t device, size_t from)
	{
		while (from < commands.size() and commands[from].device != device) from++;
		return from;
	};
	size_t next[MaxDevices];
	uint16_t* results[MaxDevices] = {};
	// A read only takes effect, also on read-to-clear bits, in the next frame
	bool reading[MaxDevices] = {};
	for (uint8_t ii = 0; ii < devices_; ii++) next[ii] = find(ii, 0);

	bool valid{true};
	while (true)
	{
		bool work{false};
		for (uint8_t ii = 0; ii < devices_; ii++)
		{
			const bool queued = next[ii] < commands.size();
			// A NOP, since repeating a read would clear its register again
			tx[ii] = queued ? commands[next[ii]].frame : buildNopFrame();
			work |= queued or reading[ii];
		}
		if (not work) return valid;

		transferChain();

		for (uint8_t ii = 0; ii < devices_; ii++)
		{
			if ((rx[ii] >> 14) == 0b11) valid = false;
			// This frame answers the command of the previous one
			if (results[ii]) *results[ii] = rx[ii] & 0x3FFF;
			results[ii] = nullptr;
			reading[ii] = false;
			if (next[ii] < commands.size())
			{
				const Command& command = commands[next[ii]];
				results[ii] = command.result;
				reading[ii] = not (command.frame & 0x8000);
				next[ii] = find(ii, next[ii] + 1);
			}
		}
	}
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::transferChain()
{
	// Devices are wired MISO->MOSI in series (device 0 nearest the host), so
	// bits shift away from the host-input side: at the end of an SS-low
	// window, device 0's slot holds the *last* 16 bits clocked in and
	// device (devices()-1)'s slot holds the *first* 16 bits clocked in. The
	// wire buffer must therefore be packed farthest-device-first.
	for (size_t ii = 0; ii < devices_; ii++)
	{
		const uint16_t word = tx[devices_ - 1 - ii];
		buffer[2 * ii] = uint8_t(word >> 8);
		buffer[2 * ii + 1] = uint8_t(word);
	}

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();
	modm::delay_ns(100); // TLEAD

	SpiMaster::transfer(buffer, buffer, 2 * devices_);

	if (this->releaseMaster())
	{
		Cs::set();
		modm::delay_ns(300); // TW_SS
	}

	for (size_t ii = 0; ii < devices_; ii++)
	{
		rx[devices_ - 1 - ii] = (uint16_t(buffer[2 * ii]) << 8) | buffer[2 * ii + 1];
	}
}

} // namespace modm
