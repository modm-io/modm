/*
 * Copyright (c) 2021 - 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MA12070P_HPP
#define MODM_MA12070P_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <optional>

namespace modm
{

/// @ingroup modm_driver_ma12070p
struct ma12070p
{
	enum class
	Register : uint8_t
	{
		PowerModeControl = 0x00,
		ThresholdForPowerModeChange12 = 0x01,
		ThresholdForPowerModeChange21 = 0x02,
		ThresholdForPowerModeChange23 = 0x03,
		ThresholdForPowerModeChange32 = 0x04,
		DisableAmplifier = 0x07,
		SoftClippingAndOcpLatching = 0x0a,
		SelectPowerModeProfile = 0x1d,
		PowerModeProfileConfiguration = 0x1e,
		OcpLatchClear = 0x20,
		AudioInputMode = 0x25,
		DcProtection = 0x26,
		AudioInputModeOverwrite = 0x27,
		ErrorHandlerClear = 0x2d,
		PcmFormat = 0x35,
		PcmConfiguration = 0x36,
		LimiterConfiguration = 0x35,
		MuteAndLimiterMux = 0x36,
		MasterVolumeDbInteger = 0x40,
		MasterVolumeDbFract = 0x41,
		Ch0LVolumeDbInteger = 0x42,
		Ch0RVolumeDbInteger = 0x43,
		Ch1LVolumeDbInteger = 0x44,
		Ch1RVolumeDbInteger = 0x45,
		ChxxVolumeDbFract = 0x46,
		Ch0LLimiterDbfsInteger = 0x47,
		Ch0RLimiterDbfsInteger = 0x48,
		Ch1LLimiterDbfsInteger = 0x49,
		Ch1RLimiterDbfsInteger = 0x4a,
		ChxxLimiterDbfsFract = 0x4b,
		VolumeClippingIndicator = 0x7e,
		MonitorCh0FreqPower = 0x60,
		MonitorCh0 = 0x61,
		MonitorCh0ModulationIndex = 0x62,
		MonitorCh1FreqPower = 0x64,
		MonitorCh1 = 0x65,
		MonitorCh1ModulationIndex = 0x66,
		ErrorAccumulated = 0x6d,
		MonitorMsel = 0x75,
		Error = 0x7c,
		VlpMonitor = 0x7e,
	};


	enum class
	PcmWordFormat : uint8_t
	{
		I2s					= 0b000,
		LeftJustifed		= 0b001, /// default
		RightJustifed16b	= 0b100,
		RightJustifed18b	= 0b101,
		RightJustifed20b	= 0b110,
		RightJustifed24b	= 0b111,
	};

	enum class
	ClockPolarity : uint8_t
	{
		FallingEdge = 0b0, /// Capture data at the falling edge of the serial clock signal SCK
		RisingEdge = 0b1, /// Capture data at the rising edge of the serial clock signal SCK
	};

	enum class
	FrameSize : uint8_t
	{
		Bits64 = (0b00 << 3),
		Bits48 = (0b01 << 3),
		Bits32 = (0b10 << 3),
		//Reserved = (0b11 << 3),
	};

	enum class
	WordSelectPolarity : uint8_t
	{
		Low = (0b0 << 1), /// First word of a simultaneously sampled PCM data pair is transmitted while word select (WS) is low.
		High = (0b1 << 1), /// First word of a simultaneously sampled PCM data pair is transmitted while word select (WS) is high.
	};

	enum class
	DataOrder : uint8_t
	{
		MsbFirst = (0b0 << 2),
		LsbFirst = (0b1 << 2),
	};

	enum class
	RightLeftOrder : uint8_t
	{
		LeftFirst = (0b0 << 5),
		RightFirst = (0b1 << 5),
	};

	enum class
	LimiterTiming : uint8_t
	{
		Slow	= 0b00,
		Normal	= 0b01,
		Fast	= 0b10,
	};

	struct I2sAndVlpConfig
	{
		const PcmWordFormat pcmWordFormat = PcmWordFormat::LeftJustifed;
		const ClockPolarity clockPolarity = ClockPolarity::FallingEdge;
		const FrameSize frameSize = FrameSize::Bits64;
		const WordSelectPolarity wordSelectPolarity = WordSelectPolarity::Low;
		const DataOrder dataOrder = DataOrder::MsbFirst;
		const RightLeftOrder rightLeftOrder = RightLeftOrder::LeftFirst;
		const bool useVlp = true;
		const bool useLimiter = true;
		const LimiterTiming limiterReleaseTime = LimiterTiming::Normal;
		const LimiterTiming limiterAttackTime = LimiterTiming::Normal;
	};

	enum class
	VlpMonitor : uint8_t
	{
		LimiterCh0L		= Bit0,
		LimiterCh0R		= Bit1,
		LimiterCh1L		= Bit2,
		LimiterCh1R		= Bit3,
		ClippingCh0L	= Bit4,
		ClippingCh0R	= Bit5,
		ClippingCh1L	= Bit6,
		ClippingCh1R	= Bit7,
	};
	MODM_FLAGS8(VlpMonitor);

#if MODM_HAS_IOSTREAM
	friend IOStream&
	operator << (IOStream& os, const VlpMonitor_t& c)
	{
		os << "VlpMonitor(";
		if(c & VlpMonitor::LimiterCh0L)
			os << "LimiterCh0L ";
		if(c & VlpMonitor::LimiterCh0R)
			os << "LimiterCh0R ";
		if(c & VlpMonitor::LimiterCh1L)
			os << "LimiterCh1L ";
		if(c & VlpMonitor::LimiterCh1R)
			os << "LimiterCh1R ";
		if(c & VlpMonitor::ClippingCh0L)
			os << "ClippingCh0L ";
		if(c & VlpMonitor::ClippingCh0R)
			os << "ClippingCh0R ";
		if(c & VlpMonitor::ClippingCh1L)
			os << "ClippingCh1L ";
		if(c & VlpMonitor::ClippingCh1R)
			os << "ClippingCh1R ";
		os << ")";
		return os;
	}
#endif

	enum class
	ErrorRegister : uint8_t
	{
		FlyingCapOverVolt		= Bit0,
		OverCurrent				= Bit1,
		Pll						= Bit2,
		PvddUnderVolt			= Bit3,
		OverTempWarning			= Bit4,
		OverTempError			= Bit5,
		PinToPinLowImpedance	= Bit6,
		DcProtection			= Bit7,
	};
	MODM_FLAGS8(ErrorRegister);

#if MODM_HAS_IOSTREAM
	friend IOStream&
	operator << (IOStream& os, const ErrorRegister_t& c)
	{
		os << "ErrorRegister(";
		if(c & ErrorRegister::FlyingCapOverVolt)
			os << "FlyingCapOverVolt ";
		if(c & ErrorRegister::OverCurrent)
			os << "OverCurrent ";
		if(c & ErrorRegister::Pll)
			os << "Pll ";
		if(c & ErrorRegister::PvddUnderVolt)
			os << "PvddUnderVolt ";
		if(c & ErrorRegister::OverTempWarning)
			os << "OverTempWarning ";
		if(c & ErrorRegister::OverTempError)
			os << "OverTempError ";
		if(c & ErrorRegister::PinToPinLowImpedance)
			os << "PinToPinLowImpedance ";
		if(c & ErrorRegister::DcProtection)
			os << "DcProtection ";
		os << ")";
		return os;
	}
#endif


	// quarter_decibel type
	enum class
	quarter_decibel_t : int16_t
	{};

	template<typename T>
	static constexpr quarter_decibel_t
	quarter_decibel(T value)
	{
		return quarter_decibel_t(int16_t(value * 4ul));
	}
	
	static constexpr float
	quarterDecibelToFloat(quarter_decibel_t value)
	{
		return int16_t(value) / 4.f;
	}

	static constexpr quarter_decibel_t MaxVolume = static_cast<quarter_decibel_t>(24*4);
	static constexpr quarter_decibel_t MinVolume = static_cast<quarter_decibel_t>(-144*4);
	// static constexpr quarter_decibel_t MaxVolume = quarter_decibel(24);
	// static constexpr quarter_decibel_t MinVolume = quarter_decibel(-144);
};  // struct ma12070p

constexpr ma12070p::quarter_decibel_t
operator-(ma12070p::quarter_decibel_t v)
{
	return static_cast<ma12070p::quarter_decibel_t>(-static_cast<int16_t>(v));
}

namespace literals
{
	constexpr ma12070p::quarter_decibel_t
	operator""_q_db(long double value)
	{
		return ma12070p::quarter_decibel(value);
	}

	constexpr ma12070p::quarter_decibel_t
	operator""_q_db(unsigned long long int value)
	{
		return ma12070p::quarter_decibel(value);
	}
}

/**
 * Infineon MA12070P, Portable Audio DAC with Integrated Class D Speaker Driver
 *
 * @tparam I2cMaster I2C interface
 *
 * @author Raphael Lehmann
 * @ingroup modm_driver_ma12070p
 */
template<typename I2cMaster>
class Ma12070p : public ma12070p, public modm::I2cDevice<I2cMaster, 5>
{
public:
	/**
	 * Constructor.
	 * 
	 * \param i2cAddress The I²C address depends on the AD1 and AD0 pins: 
	 *                   AD1=0, AD0=0 -> 0x20
	 *                   AD1=0, AD0=1 -> 0x21
	 *                   AD1=1, AD0=0 -> 0x22
	 *                   AD1=1, AD0=1 -> 0x23
	 */
	Ma12070p(uint8_t i2cAddress);

	/// Initialize device, call before using any other function
	ResumableResult<bool>
	initialize();

	/// Configure I2S input format and the volume and limiter processor (VLP)
	ResumableResult<bool>
	configureI2sAndVlp(I2sAndVlpConfig config);

	/// Set the limiter treshold for individual channels (-144db to +24db)
	ResumableResult<bool>
	setLimiterTreshold(quarter_decibel_t ch0l, quarter_decibel_t ch0r, quarter_decibel_t ch1l = -144, quarter_decibel_t ch1r = -144);

	/// Set the master volume (-144db to +24db)
	ResumableResult<bool>
	setMasterVolume(quarter_decibel_t volume);

	/// Set the volume for individual channels (-144db to +24db)
	ResumableResult<bool>
	setChannelVolume(quarter_decibel_t ch0l, quarter_decibel_t ch0r, quarter_decibel_t ch1l = -144, quarter_decibel_t ch1r = -144);

	/// Read if limiters are active or if clipping occurs on the VLP output signals
	ResumableResult<std::optional<VlpMonitor_t>>
	readVlpMonitor();

	/// Disable amplifier channels
	/// true means disabled
	ResumableResult<bool>
	disableAmplifier(bool ch0, bool ch1);

	/// Enable DC protection. Default on.
	ResumableResult<bool>
	enableDcProtection(bool enable = true);

	/// Read error monitor register. Gives the live status of every potential error source.
	ResumableResult<std::optional<ErrorRegister_t>>
	readErrors();

	/// Read error monitor register.
	/// Gives the accumulated status of every potential error source.
	/// This register should be cleared by using `clearErrorHandler()`.
	ResumableResult<std::optional<ErrorRegister_t>>
	readAccumulatedErrors();

	/// Clears the error handler
	ResumableResult<bool>
	clearErrorHandler();

private:
	uint8_t tx_buffer[6];
	uint8_t rx_buffer;
	bool success;

	ResumableResult<bool>
	writeRegister(Register reg, uint8_t value);

	ResumableResult<std::optional<uint8_t>>
	readRegister(Register reg);
};

}	// namespace modm

#include "ma12070p_impl.hpp"

#endif // MODM_MA12070P_HPP

