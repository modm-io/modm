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
	writeRegister(device, Register(uint8_t(Register::OVW_OUT1) + 2 * uint8_t(output)),
		voltageThresholdCode(upperVoltage));
	writeRegister(device, Register(uint8_t(Register::UVW_OUT1) + 2 * uint8_t(output)),
		voltageThresholdCode(lowerVoltage));

	const uint8_t shift = 2 * uint8_t(output);
	uint8_t wv = readRegister(device, Register::WV_CTRL);
	// OutputOn (VOUTx_ON, from CMM) rather than Continuous (VOUTx, from OVM):
	// VOUTx is refreshed even while the output is off, so it reads ~0 V there
	// and would immediately trip UndervoltageWarning against any threshold.
	// VOUTx_ON only updates while the output is actually on, and - like
	// IOUTx - is blanked by the first measurement delay (M_SETUP).
	wv = (wv & ~(0b11 << shift)) | (uint8_t(VoltageWarningSource::OutputOn) << shift);
	writeRegister(device, Register::WV_CTRL, wv);
}


template <class SpiMaster, class Cs, size_t MaxDevices>
void
Mc33xs2410<SpiMaster, Cs, MaxDevices>::setCurrentThreshold(Output output, float lowerCurrent, float upperCurrent, uint8_t device)
{
	writeRegister(device, Register(uint8_t(Register::OCW_OUT1) + 2 * uint8_t(output)),
		overcurrentWarningCode(upperCurrent));
	writeRegister(device, Register(uint8_t(Register::UCW_OUT1) + 2 * uint8_t(output)),
		undercurrentWarningCode(lowerCurrent));

	const uint8_t shift = 2 * uint8_t(output);
	uint8_t wc = readRegister(device, Register::WC_CTRL);
	wc = (wc & ~(0b11 << shift)) | (uint8_t(CurrentWarningSource::Continuous) << shift);
	writeRegister(device, Register::WC_CTRL, wc);
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
	setCommand(device, buildWriteFrame(reg, data));
	transferChain();
}

template <class SpiMaster, class Cs, size_t MaxDevices>
uint16_t
Mc33xs2410<SpiMaster, Cs, MaxDevices>::readRegister(uint8_t device, uint8_t address, bool controlBank)
{
	setCommand(device, buildReadFrame(address, controlBank));

	// The requested register is only reported in the *next* SS window for
	// the whole chain, so the same command must be shifted through twice.
	transferChain();
	transferChain();
	return rx[device] & 0x3FFF;
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

	SpiMaster::transfer(buffer, buffer, 2 * devices_);

	if (this->releaseMaster()) Cs::set();

	for (size_t ii = 0; ii < devices_; ii++)
	{
		rx[devices_ - 1 - ii] = (uint16_t(buffer[2 * ii]) << 8) | buffer[2 * ii + 1];
	}
}

} // namespace modm
