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
#define MODM_MC33XS2410_HPP

#include <cstddef>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

/// @ingroup modm_driver_mc33xs2410
struct mc33xs2410
{
	/// One of the four high-side switch channels
	enum class
	Output : uint8_t
	{
		Output1 = 0,
		Output2 = 1,
		Output3 = 2,
		Output4 = 3,
	};

	/// Control bank registers (8-bit, read/write)
	enum class
	Register : uint8_t
	{
		GLB_CTRL = 0x00,
		READBACK = 0x01,
		OUT1_4_CTRL = 0x02,
		IN_CTRL1 = 0x03,
		IN_CTRL2 = 0x04,
		PWM_CTRL1 = 0x05,
		PWM_CTRL2 = 0x06,
		PWM_CTRL3 = 0x07,
		PWM_FREQ1 = 0x08,
		PWM_FREQ2 = 0x09,
		PWM_FREQ3 = 0x0A,
		PWM_FREQ4 = 0x0B,
		PWM_DC1 = 0x0C,
		PWM_DC2 = 0x0D,
		PWM_DC3 = 0x0E,
		PWM_DC4 = 0x0F,
		EN_IRQ_SPI = 0x10,
		EN_IRQ_PIN = 0x11,
		EN_WARN_SPI = 0x12,
		EN_WARN_PIN = 0x13,
		WDT_REG = 0x14,
		M_SETUP = 0x15,
		C_CTRL = 0x16,
		WC_CTRL = 0x17,
		OCW_OUT1 = 0x18,
		UCW_OUT1 = 0x19,
		OCW_OUT2 = 0x1A,
		UCW_OUT2 = 0x1B,
		OCW_OUT3 = 0x1C,
		UCW_OUT3 = 0x1D,
		OCW_OUT4 = 0x1E,
		UCW_OUT4 = 0x1F,
		WV_CTRL = 0x20,
		OVW_OUT1 = 0x21,
		UVW_OUT1 = 0x22,
		OVW_OUT2 = 0x23,
		UVW_OUT2 = 0x24,
		OVW_OUT3 = 0x25,
		UVW_OUT3 = 0x26,
		OVW_OUT4 = 0x27,
		UVW_OUT4 = 0x28,
		TEMP_WT = 0x29,
		DV_STVB = 0x2A,
		DT_STVB = 0x2B,
		OPD_CTRL1 = 0x2C,
		OPD_CTRL2 = 0x2D,
		I_OLD1 = 0x2E,
		I_OLD2 = 0x2F,
		I_OLD3 = 0x30,
		I_OLD4 = 0x31,
		ACL_CTRL1 = 0x32,
		ACL_CTRL2 = 0x33,
		SSC_CTRL = 0x34,
		OLP_CTRL = 0x35,
		OCL_OUT1 = 0x36,
		OCL_OUT2 = 0x37,
		OCL_OUT3 = 0x38,
		OCL_OUT4 = 0x39,
		PI_CTRL1 = 0x3A,
		PI_CTRL2 = 0x3B,
		I_SET1 = 0x3C,
		I_SET2 = 0x3D,
		I_SET3 = 0x3E,
		I_SET4 = 0x3F,
	};

	/// Diagnostic bank registers (14-bit, read-only)
	enum class
	DiagnosticRegister : uint8_t
	{
		GLB_STA = 0x00,
		IN_OUT_STA = 0x01,
		OUT1_STA = 0x02,
		OUT2_STA = 0x03,
		OUT3_STA = 0x04,
		OUT4_STA = 0x05,
		ISR_IRQ = 0x06,
		ISR_WARN = 0x07,
		OD_IOUT1 = 0x08,
		OD_IOUT2 = 0x09,
		OD_IOUT3 = 0x0A,
		OD_IOUT4 = 0x0B,
		IOUT1 = 0x0C,
		IOUT2 = 0x0D,
		IOUT3 = 0x0E,
		IOUT4 = 0x0F,
		INI_IOUT1 = 0x10,
		INI_IOUT2 = 0x11,
		INI_IOUT3 = 0x12,
		INI_IOUT4 = 0x13,
		FIN_IOUT1 = 0x14,
		FIN_IOUT2 = 0x15,
		FIN_IOUT3 = 0x16,
		FIN_IOUT4 = 0x17,
		VOUT1_ON = 0x18,
		VOUT2_ON = 0x19,
		VOUT3_ON = 0x1A,
		VOUT4_ON = 0x1B,
		OD_VOUT1 = 0x1C,
		OD_VOUT2 = 0x1D,
		OD_VOUT3 = 0x1E,
		OD_VOUT4 = 0x1F,
		VOUT1 = 0x20,
		VOUT2 = 0x21,
		VOUT3 = 0x22,
		VOUT4 = 0x23,
		VPWR_M = 0x25,
		TS_0_TEMP = 0x26,
		PI_DC1 = 0x27,
		PI_DC2 = 0x28,
		PI_DC3 = 0x29,
		PI_DC4 = 0x2A,
		FB_IOUT1 = 0x2B,
		FB_IOUT2 = 0x2C,
		FB_IOUT3 = 0x2D,
		FB_IOUT4 = 0x2E,
		TS_1_TEMP = 0x2F,
		TS_2_TEMP = 0x30,
		TS_3_TEMP = 0x31,
		TS_4_TEMP = 0x32,
	};


	/// Global control register (GLB_CTRL, 00h control bank)
	enum class
	GlobalControl : uint8_t
	{
		Transit1 = Bit7,
		Transit0 = Bit6,
		DriveStrengthMedium = Bit5,
		ParallelOut12 = Bit4,
		ParallelOut34 = Bit3,
		OscillatorModulationEnable = Bit2,
		FrequencyModulationBand2 = Bit1,
		CmosInputLogic = Bit0,
	};
	MODM_FLAGS8(GlobalControl);

	/// Mode transition requested via GLB_CTRL[7:6]
	enum class
	TransitMode : uint8_t
	{
		None = 0b00,
		ToNormal = 0b01,
		ToSafe = 0b10,
		Reset = 0b11,
	};
	typedef modm::Configuration<GlobalControl_t, TransitMode, 0b11, 6> TransitMode_t;


	/// Output control register (OUT1_4_CTRL, 02h control bank)
	enum class
	OutputControl : uint8_t
	{
		SlewRateFast4 = Bit7,
		SlewRateFast3 = Bit6,
		SlewRateFast2 = Bit5,
		SlewRateFast1 = Bit4,
		Output4 = Bit3,
		Output3 = Bit2,
		Output2 = Bit1,
		Output1 = Bit0,
	};
	MODM_FLAGS8(OutputControl);


	/// Global status register (GLB_STA, 00h diagnostic bank)
	enum class
	GlobalStatus : uint16_t
	{
		SafeMode = Bit13,
		ChargePumpFault = Bit11,
		VpwrLowVoltage = Bit10,
		VpwrUndervoltage = Bit9,
		VddUndervoltage = Bit8,
		Output4Warning = Bit7,
		Output3Warning = Bit6,
		Output2Warning = Bit5,
		Output1Warning = Bit4,
		Output4LatchOff = Bit3,
		Output3LatchOff = Bit2,
		Output2LatchOff = Bit1,
		Output1LatchOff = Bit0,
	};
	MODM_FLAGS16(GlobalStatus);


	/// Output pin states inside the input/output status register
	/// (IN_OUT_STA, 01h diagnostic bank)
	enum class
	InputOutputStatus : uint16_t
	{
		Output4 = Bit7,
		Output3 = Bit6,
		Output2 = Bit5,
		Output1 = Bit4,
	};
	MODM_FLAGS16(InputOutputStatus);


	/// Output status register (OUTx_STA, 02h to 05h diagnostic bank)
	enum class
	OutputStatus : uint16_t
	{
		ThermalShutdown = Bit13,
		CurrentLimitRetriesExceeded = Bit12,
		SevereShortCircuit = Bit11,
		OverloadCondition = Bit10,
		ActiveCurrentLimit = Bit9,
		OvertemperatureWarning = Bit8,
		OvercurrentWarning = Bit7,
		UndercurrentWarning = Bit6,
		OvervoltageWarning = Bit5,
		UndervoltageWarning = Bit4,
		ShortToVBat = Bit3,
		OpenLoad = Bit2,
	};
	MODM_FLAGS16(OutputStatus);


	/// Interrupt request flags (EN_IRQ_SPI/EN_IRQ_PIN/ISR_IRQ)
	enum class
	IrqFlag : uint8_t
	{
		ChargePumpFault = Bit7,
		WatchdogTimeout = Bit5,
		SpiToggleBitError = Bit4,
		SafeModeTransition = Bit3,
		Overtemperature = Bit2,
		CurrentLimitRetryOrSevereShortCircuit = Bit1,
		OverloadCondition = Bit0,
	};
	MODM_FLAGS8(IrqFlag);

	/// Warning flags (EN_WARN_SPI/EN_WARN_PIN/ISR_WARN)
	enum class
	WarnFlag : uint8_t
	{
		Watchdog = Bit7,
		Overtemperature = Bit6,
		Overcurrent = Bit5,
		Undercurrent = Bit4,
		Overvoltage = Bit3,
		Undervoltage = Bit2,
		VddUndervoltage = Bit1,
		VpwrUndervoltage = Bit0,
	};
	MODM_FLAGS8(WarnFlag);


	/// Watchdog timer control register (WDT_REG, 14h control bank)
	enum class
	WatchdogControl : uint8_t
	{
		SafeModeOnTimeout = Bit6,
	};
	MODM_FLAGS8(WatchdogControl);


	/// Comparison source for the per-output voltage warning band, two bits
	/// per output packed into WV_CTRL (20h control bank)
	enum class
	VoltageWarningSource : uint8_t
	{
		Disabled = 0b00,
		OutputOn = 0b01,
		Continuous = 0b10,
		OnDemand = 0b11,
	};

	/// Comparison source for the per-output current warning band, two bits
	/// per output packed into WC_CTRL (17h control bank)
	enum class
	CurrentWarningSource : uint8_t
	{
		Disabled = 0b00,
		Initial = 0b01,
		Continuous = 0b10,
		InitialAndContinuous = 0b11,
	};


	/// First measurement delay (M_SETUP[3:2], 15h control bank), applies to
	/// all four outputs of a device: how long after an output turns on the
	/// continuous current/voltage measurement (CMM) is masked, so inrush
	/// current isn't misreported as an overcurrent/undervoltage warning
	enum class
	FirstMeasurementDelay : uint8_t
	{
		Time1ms = 0b00,
		Time8ms = 0b01,
		Time16ms = 0b10,
		Time64ms = 0b11,
	};

	/// Periodic measurement time (M_SETUP[1:0], 15h control bank), applies
	/// to all four outputs of a device: refresh interval of the continuous
	/// current/voltage measurement once the first measurement delay elapses
	enum class
	PeriodicMeasurementTime : uint8_t
	{
		Time1ms = 0b00,
		Time2ms = 0b01,
		Time4ms = 0b10,
		Time8ms = 0b11,
	};


	/// Overload protection control register (OLP_CTRL, 35h control bank);
	/// applies to all four outputs of a device, unlike OCL_OUTx which is
	/// per-output
	enum class
	OverloadProtectionControl : uint8_t
	{
		TransitSafeMode = Bit7,
		DisableLatchOff = Bit6,
	};
	MODM_FLAGS8(OverloadProtectionControl);

	/// Overcurrent timeout period (OLP_CTRL[5:3]) an output must exceed its
	/// OCL_OUTx threshold for before an overload condition latches it off
	enum class
	OverloadTimeout : uint8_t
	{
		Time125ms = 0b000,
		Time250ms = 0b001,
		Time500ms = 0b010,
		Time1s = 0b011,
		Time2s = 0b100,
		Time4s = 0b101,
		Time8s = 0b110,
		Time16s = 0b111,
	};
	typedef modm::Configuration<OverloadProtectionControl_t, OverloadTimeout, 0b111, 3> OverloadTimeout_t;


	/// Encodes a control bank write command (16-bit SPI frame)
	static constexpr uint16_t
	buildWriteFrame(Register reg, uint8_t data)
	{ return 0x8000 | (uint16_t(uint8_t(reg) & 0x3F) << 8) | data; }

	/// Encodes a register read command (16-bit SPI frame); the value is only
	/// reported in the *next* SPI frame, see the class documentation
	static constexpr uint16_t
	buildReadFrame(uint8_t address, bool controlBank)
	{ return (uint16_t(address & 0x3F) << 8) | (controlBank ? 0x0080 : 0x0000); }

	/// A read of the global status register, which is the only entirely
	/// side-effect free frame, used to pad the chain slots of the devices that
	/// a command does not address
	static constexpr uint16_t
	buildNopFrame()
	{ return buildReadFrame(0x00, false); }

	/// Scaling of the embedded 12-bit ADC measurements reported in the
	/// diagnostic bank, which carries them as raw unitless codes.
	/// @{
	static constexpr float CurrentPerLsb{5.0f / 5115.0f};	///< A, IOUTx over 0 A to 5.0 A
	static constexpr float VoltagePerLsb{0.0634f};			///< V, VOUTx/VPWR_M over 0 V to 65 V
	static constexpr float TemperaturePerLsb{0.25f};		///< degree Celsius, TS_x_TEMP
	static constexpr float TemperatureOffset{-40.0f};		///< degree Celsius, TS_x_TEMP at code 0
	/// @}

	/// Encodes a voltage threshold in Volt as used by OVW_OUTx/UVW_OUTx (LSB = 255 mV, 0 V to 65 V)
	static constexpr uint8_t
	voltageThresholdCode(float volts)
	{
		if (volts <= 0.0f) return 0x00;
		if (volts >= 65.0f) return 0xFF;
		return uint8_t(volts / 0.255f + 0.5f);
	}

	/// Encodes an overcurrent warning threshold in Ampere as used by OCW_OUTx (LSB = 19.6 mA, 0 A to 5.0 A)
	static constexpr uint8_t
	overcurrentWarningCode(float amperes)
	{
		if (amperes <= 0.0f) return 0x00;
		if (amperes >= 5.0f) return 0xFF;
		return uint8_t(amperes / (5.0f / 255.0f) + 0.5f);
	}

	/// Encodes an undercurrent warning threshold in Ampere as used by UCW_OUTx (LSB = 3.9 mA, 0 A to 1.0 A)
	static constexpr uint8_t
	undercurrentWarningCode(float amperes)
	{
		if (amperes <= 0.0f) return 0x00;
		if (amperes >= 1.0f) return 0xFF;
		return uint8_t(amperes / (1.0f / 255.0f) + 0.5f);
	}

}; // struct mc33xs2410

/**
 * NXP MC33XS2410 quad 100 mOhm / dual 50 mOhm, 3.0 V to 60 V high-side switch.
 *
 * Four channel self-protected high-side switch with 16-bit SPI interface for
 * output control and diagnostics, and an embedded 12-bit ADC for per-channel
 * current, voltage and temperature monitoring.
 *
 * The SPI bus must be configured for mode 1 (CPOL=0, CPHA=1) and a maximum
 * frequency of 10 MHz.
 *
 * Multiple devices can be daisy-chained on one SPI bus: their SS pins are
 * tied together and MISO of device N is connected to MOSI of device N+1
 * (device 0 being nearest to the host). `MaxDevices` only sizes the static
 * buffers, the actual chain length is passed to the constructor, so it may be
 * discovered at runtime. Every accessor below takes a `device` index (0-based,
 * defaults to 0) to select which chained device it addresses. A single SPI
 * transfer always shifts data through the *whole* chain, and because a
 * register's value is only reported in the SPI frame *following* the one that
 * requested it, `readRegister()` performs two whole-chain transfers per call.
 *
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin, shared by all devices in the chain
 * @tparam	MaxDevices	Buffer capacity: longest chain this instance can drive
 *
 * [Datasheet](https://www.nxp.com/docs/en/data-sheet/MC33XS2410.pdf)
 *
 * @ingroup modm_driver_mc33xs2410
 */
template <class SpiMaster, class Cs, size_t MaxDevices = 1>
class Mc33xs2410 : public mc33xs2410, public modm::SpiDevice<SpiMaster>
{
public:
	/// @param	devices		Length of the daisy-chain, clamped to [1, MaxDevices]
	explicit
	Mc33xs2410(uint8_t devices = MaxDevices)
	:	devices_(devices ? (devices < MaxDevices ? devices : MaxDevices) : 1) {}

	/// Length of the daisy-chain this instance drives
	uint8_t
	devices() const
	{ return devices_; }

	/// Call this function once before using the device(s)
	void
	initialize();

	/// Executes a full SPI-triggered device reset
	void
	reset(uint8_t device = 0)
	{ setMode(TransitMode::Reset, device); }

	/// Transitions the device between normal and safe operation mode
	void
	setMode(TransitMode mode, uint8_t device = 0)
	{ writeRegister(device, Register::GLB_CTRL, GlobalControl_t(TransitMode_t(mode)).value); }

	/// Configures switching slew rate and on/off state for all four outputs at once
	void
	setOutputControl(OutputControl_t control, uint8_t device = 0)
	{ writeRegister(device, Register::OUT1_4_CTRL, control.value); }

	/// Enables or disables the automatic transition to safe mode when the
	/// SPI watchdog times out; disable when SPI frame timing cannot be
	/// guaranteed (e.g. bit-banged SPI)
	void
	setWatchdogSafeModeOnTimeout(bool enable, uint8_t device = 0);

	/// Turns a single output on or off, preserving the state of the other outputs
	void
	setOutput(Output output, bool enable, uint8_t device = 0);

	/// Reads back the logic state of an output pin from IN_OUT_STA
	bool
	getOutput(Output output, uint8_t device = 0);

	/// Configures the under-/overvoltage warning band for an output from
	/// explicit bounds (Volt) and switches the comparison source to VOUTx_ON
	/// (only updated while the output is on, see FirstMeasurementDelay), so
	/// the device raises the warning autonomously instead of the MCU polling
	/// and computing it. Compares against the output pin voltage, not VPWR.
	void
	setVoltageThreshold(Output output, float lowerVoltage, float upperVoltage, uint8_t device = 0);

	/// Configures the under-/overcurrent warning band for an output from
	/// explicit bounds (Ampere; pass 0 for \c lowerCurrent to disable the
	/// undercurrent warning) and switches the comparison source to
	/// continuous monitoring.
	void
	setCurrentThreshold(Output output, float lowerCurrent, float upperCurrent, uint8_t device = 0);

	/// Configures the hardware overload protection (OLP, see datasheet
	/// section 8.14.7/8.14.3): if the output current exceeds
	/// \c currentLimit for longer than \c timeout, the device autonomously
	/// latches the output off (reported as OutputStatus::OverloadCondition)
	/// with no MCU intervention required. Unlike setCurrentThreshold(),
	/// this is a genuine hardware shutdown - not just a warning flag. Note
	/// that OCL_OUTx is per-output, but the timeout/enable in OLP_CTRL
	/// apply to all four outputs of a device, so the last call for any
	/// output on a given device wins for the timeout setting. There is no
	/// equivalent autonomous latch-off for over-/undervoltage; use
	/// protectOutput() for that.
	void
	setOverloadProtection(Output output, float currentLimit,
		OverloadTimeout timeout = OverloadTimeout::Time1s, uint8_t device = 0);

	/// Configures how long the continuous current/voltage measurement
	/// (IOUTx/VOUTx_ON) is blanked after an output turns on (M_SETUP), to
	/// ride out inrush current/voltage sag on the load instead of it
	/// tripping the Over-/UndercurrentWarning or Over-/UndervoltageWarning
	/// set up by setCurrentThreshold()/setVoltageThreshold(). Applies to
	/// all four outputs of \c device at once - there is no per-output field.
	void
	setMeasurementDelay(FirstMeasurementDelay fmd,
		PeriodicMeasurementTime pmt = PeriodicMeasurementTime::Time1ms, uint8_t device = 0)
	{ writeRegister(device, Register::M_SETUP, (uint8_t(fmd) << 2) | uint8_t(pmt)); }

	/// Global status: mode of operation, supply faults and output warnings/latch-offs
	GlobalStatus_t
	readGlobalStatus(uint8_t device = 0)
	{ return GlobalStatus_t(readRegister(device, DiagnosticRegister::GLB_STA)); }

	/// Detailed fault status of a single output. Bits are latched: a bit
	/// reads 1 if that condition occurred at least once since the *last*
	/// read, not that it is true right now, and reading this register
	/// clears it. Discard a stale read (or call clearOutputStatus()) before
	/// an operation whose result you want to check, otherwise unrelated
	/// history (e.g. ShortToVBat latched while the output was off, or the
	/// brief open-load/inrush window right at turn-on) will still be set.
	OutputStatus_t
	readOutputStatus(Output output, uint8_t device = 0)
	{
		return OutputStatus_t(readRegister(device,
			DiagnosticRegister(uint8_t(DiagnosticRegister::OUT1_STA) + uint8_t(output))));
	}

	/// Latched IRQ-type interrupt status, cleared by reading
	IrqFlag_t
	readInterruptStatus(uint8_t device = 0)
	{ return IrqFlag_t(uint8_t(readRegister(device, DiagnosticRegister::ISR_IRQ))); }

	/// Latched WARN-type interrupt status, cleared by reading
	WarnFlag_t
	readWarningStatus(uint8_t device = 0)
	{ return WarnFlag_t(uint8_t(readRegister(device, DiagnosticRegister::ISR_WARN))); }

	/// Continuously monitored output current in Ampere (0 A to 5.0 A)
	float
	readOutputCurrent(Output output, uint8_t device = 0)
	{
		return float(readRegister(device,
			DiagnosticRegister(uint8_t(DiagnosticRegister::IOUT1) + uint8_t(output)))) * CurrentPerLsb;
	}

	/// Continuously monitored output voltage in Volt (0 V to 65 V)
	float
	readOutputVoltage(Output output, uint8_t device = 0)
	{
		return float(readRegister(device,
			DiagnosticRegister(uint8_t(DiagnosticRegister::VOUT1) + uint8_t(output)))) * VoltagePerLsb;
	}

	/// VPWR supply voltage in Volt (0 V to 65 V)
	float
	readSupplyVoltage(uint8_t device = 0)
	{ return float(readRegister(device, DiagnosticRegister::VPWR_M)) * VoltagePerLsb; }

	/// Central die temperature in degree Celsius
	float
	readDieTemperature(uint8_t device = 0)
	{ return float(readRegister(device, DiagnosticRegister::TS_0_TEMP)) * TemperaturePerLsb + TemperatureOffset; }

private:
	/// Places one command in the chain and makes every other slot a NOP, so
	/// that the *previous* command is not shifted into those devices again.
	void
	setCommand(uint8_t device, uint16_t frame)
	{
		for (uint8_t ii = 0; ii < devices_; ii++) tx[ii] = buildNopFrame();
		tx[device] = frame;
	}

	void
	writeRegister(uint8_t device, Register reg, uint8_t data);

	uint8_t
	readRegister(uint8_t device, Register reg)
	{ return uint8_t(readRegister(device, uint8_t(reg), true)); }

	uint16_t
	readRegister(uint8_t device, DiagnosticRegister reg)
	{ return readRegister(device, uint8_t(reg), false); }

	uint16_t
	readRegister(uint8_t device, uint8_t address, bool controlBank);

	/// Shifts the current contents of `tx` through the whole chain and
	/// updates `rx`; this is the only place that touches `Cs`.
	void
	transferChain();

	uint16_t tx[MaxDevices] = {};
	uint16_t rx[MaxDevices] = {};
	uint8_t buffer[2 * MaxDevices];
	uint8_t devices_;
};


#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

/// Prints the set flag names separated by '|', or "None" if the value is
/// zero, e.g. "ShortToVBat|OpenLoad|OvertemperatureWarning".
/// @ingroup modm_driver_mc33xs2410
inline IOStream&
operator<<(IOStream& stream, mc33xs2410::OutputStatus_t status)
{
	using OutputStatus = mc33xs2410::OutputStatus;
	if (not status) return stream << "None";

	bool first = true;
	const auto put = [&stream, &first](bool set, const char* name)
	{
		if (not set) return;
		if (not first) stream << "|";
		stream << name;
		first = false;
	};
	put(status.any(OutputStatus::ThermalShutdown), "ThermalShutdown");
	put(status.any(OutputStatus::CurrentLimitRetriesExceeded), "CurrentLimitRetriesExceeded");
	put(status.any(OutputStatus::SevereShortCircuit), "SevereShortCircuit");
	put(status.any(OutputStatus::OverloadCondition), "OverloadCondition");
	put(status.any(OutputStatus::ActiveCurrentLimit), "ActiveCurrentLimit");
	put(status.any(OutputStatus::OvertemperatureWarning), "OvertemperatureWarning");
	put(status.any(OutputStatus::OvercurrentWarning), "OvercurrentWarning");
	put(status.any(OutputStatus::UndercurrentWarning), "UndercurrentWarning");
	put(status.any(OutputStatus::OvervoltageWarning), "OvervoltageWarning");
	put(status.any(OutputStatus::UndervoltageWarning), "UndervoltageWarning");
	put(status.any(OutputStatus::ShortToVBat), "ShortToVBat");
	put(status.any(OutputStatus::OpenLoad), "OpenLoad");
	return stream;
}

#endif

} // namespace modm

#include "mc33xs2410_impl.hpp"

#endif // MODM_MC33XS2410_HPP
