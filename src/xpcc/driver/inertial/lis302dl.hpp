// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS302DL_HPP
#define XPCC_LIS302DL_HPP

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/processing/resumable.hpp>
#include "lis3_transport.hpp"

namespace xpcc
{

// forward declaration for friending with lis3dsh::Data
template < class Transport >
class Lis302dl;

struct lis302dl
{
protected:
	/// @cond
	enum class
	Register : uint8_t
	{
		WhoAmI = 0x0F,			///< Device identification (0x3B): r

		CtrlReg1 = 0x20,		///< Control register 1: r/w
		CtrlReg2 = 0x21,		///< Control register 2: r/w
		CtrlReg3 = 0x22,		///< Control register 3: r/w
		HpFilterReset = 0x23,	///< Reading this register momentarily sets all filters to 0

		Status = 0x27,			///< Status Register
		OutX = 0x29,			///< Output x-axis
		OutY = 0x2B,			///< Output y-axis
		OutZ = 0x2D,			///< Output z-axis

		FfWuCfg1 = 0x30,		///< Free fall/wake up configuration register 1
		FfWuSrc1 = 0x31,		///< Free fall/wake up source register 1
		FfWuThs1 = 0x32,		///< Free fall/wake up threshold register 1
		FfWuDuration1 = 0x33,	///< Free fall/wake up duration register 1

		FfWuCfg2 = 0x34,		///< Free fall/wake up configuration register 2
		FfWuSrc2 = 0x35,		///< Free fall/wake up source register 2
		FfWuThs2 = 0x36,		///< Free fall/wake up threshold register 2
		FfWuDuration2 = 0x37,	///< Free fall/wake up duration register 2

		ClickCfg = 0x38,		///< Click configuration register
		ClickSrc = 0x39,		///< Click source register
		ClickThsYX = 0x3B,		///< Click threshold register for x- and y-axis
		ClickThsZ = 0x3C,		///< Click threshold register for z-axis
		ClickTimeLimit = 0x3D,	///< Click time limit register
		ClickLatency = 0x3E,	///< Click latency register
		ClickWindow = 0x3F,		///< Click window register
	};
	/// @endcond

public:
	/// CTRL_REG1 default value is 0x07
	enum class
	Control1 : uint8_t
	{
		DR = Bit7,		///< Data rate selection. (0: 100 Hz output data rate; 1: 400 Hz output data rate)
		PD = Bit6,		///< Power Down Control. (0: power down mode; 1: active mode)
		FS = Bit5,		///< Full Scale selection. (refer to Table 3 for typical full scale value)
		STP = Bit4,		///< Self Test Enable. (0: normal mode; 1: self test P enabled)
		STM = Bit3,		///< Self Test Enable. (0: normal mode; 1: self test M enabled)
		Zen = Bit2,		///< Z axis enable. (0: Z axis disabled; 1: Z axis enabled)
		Yen = Bit1,		///< Y axis enable. (0: Y axis disabled; 1: Y axis enabled)
		Xen = Bit0,		///< X axis enable. (0: X axis disabled; 1: X axis enabled)
	};
	XPCC_FLAGS8(Control1);

	/// CTRL_REG2 default value is 0x00
	enum class
	Control2 : uint8_t
	{
		SIM = Bit7,			///< SPI Serial Interface Mode selection. (0: 4-wire interface; 1: 3-wire interface)
		BOOT = Bit6,		///< Reboot memory content. (0: normal mode; 1: reboot memory content)

		FDS = Bit4,			///< Filtered Data Selection. (0: internal filter bypassed; 1: data from internal filter sent to output register)
		HP_FF_WU2 = Bit3,	///< High Pass filter enabled for FreeFall/WakeUp # 2. (0: filter bypassed; 1: filter enabled)
		HP_FF_WU1 = Bit2,	///< High Pass filter enabled for Free-Fall/Wake-Up #1. (0: filter bypassed; 1: filter enabled)
		HP_COEFF2 = Bit1,	///< High pass filter cut-off frequency configuration.
		HP_COEFF1 = Bit0,	///< High pass filter cut-off frequency configuration.
	};
	XPCC_FLAGS8(Control2);

	/// CTRL_REG3 default value is 0x00
	enum class
	Control3 : uint8_t
	{
		IHL = Bit7,		///< Interrupt active high, low. (0: active high; 1: active low)
		PP_OD = Bit6,	///< Push-pull/Open Drain selection on interrupt pad. (0: push-pull; 1: open drain)

		I2CFG2 = Bit5,	///< Data Signal on Int2 pad control bits.
		I2CFG1 = Bit4,	///< Data Signal on Int2 pad control bits.
		I2CFG0 = Bit3,	///< Data Signal on Int2 pad control bits.

		I1CFG2 = Bit2,	///< Data Signal on Int1 pad control bits.
		I1CFG1 = Bit1,	///< Data Signal on Int1 pad control bits.
		I1CFG0 = Bit0,	///< Data Signal on Int1 pad control bits.
	};
	XPCC_FLAGS8(Control3);

	typedef FlagsGroup<Control1_t, Control2_t, Control3_t> Control_t;

	/// STATUS_REG default value is 0x00
	enum class
	Status : uint8_t
	{
		ZYXOR = Bit7,	///< X, Y and Z axis data overrun.
		ZOR = Bit6,		///< Z axis data overrun.
		YOR = Bit5,		///< Y axis data overrun.
		XOR = Bit4,		///< X axis data overrun.

		ZYXDA = Bit3,	///< X, Y and Z axis new data available.
		ZDA = Bit2,		///< Z axis new data available.
		YDA = Bit1,		///< Y axis new data available.
		XDA = Bit0,		///< X axis new data available.
	};
	XPCC_FLAGS8(Status);

	/// FF_WU_CFG default value is 0x00
	enum class
	FreeFallConfig : uint8_t
	{
		AOI = Bit7,		///< And/or combination of Interrupt events. (0: OR combination of interrupt events; 1: AND combination of interrupt events)
		LIR = Bit6,		///< Latch Interrupt request into FF_WU_SRC reg with the FF_WU_SRC reg cleared by reading FF_WU_SRC_1 reg.
		ZHIE = Bit5,	///< Enable interrupt generation on Z high event.
		ZLIE = Bit4,	///< Enable interrupt generation on Z low event.
		YHIE = Bit3,	///< Enable interrupt generation on Y high event.
		YLIE = Bit2,	///< Enable interrupt generation on Y low event.
		XHIE = Bit1,	///< Enable interrupt generation on X high event.
		XLIE = Bit0,	///< Enable interrupt generation on X low event.
	};
	XPCC_FLAGS8(FreeFallConfig);

	/// FF_WU_SRC default value is 0x00
	enum class
	FreeFallSource : uint8_t
	{
		IA = Bit6,	///< Interrupt Active.
		ZH = Bit5,	///< Z high.
		ZL = Bit4,	///< Z low.
		YH = Bit3,	///< Y high.
		YL = Bit2,	///< Y low.
		XH = Bit1,	///< X high.
		XL = Bit0,	///< X low.
	};
	XPCC_FLAGS8(FreeFallSource);

	/// FF_WU_THS default value is 0x00
	enum class
	FreeFallThreshold : uint8_t
	{
		DRCM = Bit7,		///< Resetting mode selection.
		THS_Mask = 0x7F,	///< Free-fall / wake-up Threshold.
	};
	XPCC_FLAGS8(FreeFallThreshold);

	/// CLOCK_CFG default value is 0x00
	enum class
	ClickConfig : uint8_t
	{
		LIR = Bit6,		///< Latch interrupt request into CLICK_SRC reg with the CLICK_SRC reg refreshed by reading CLICK_SRC reg.
		DoubleZ = Bit5,
		SingleZ = Bit4,
		DoubleY = Bit3,
		SingleY = Bit2,
		DoubleX = Bit1,
		SingleX = Bit0,
	};
	XPCC_FLAGS8(ClickConfig);

	/// CLOCK_SRC default value is 0x00
	enum class
	ClickSource : uint8_t
	{
		IA = Bit6,		///< Interrupt Active.
		DoubleZ = Bit5,
		SingleZ = Bit4,
		DoubleY = Bit3,
		SingleY = Bit2,
		DoubleX = Bit1,
		SingleX = Bit0,
	};
	XPCC_FLAGS8(ClickSource);

	typedef FlagsGroup<
			Control1_t, Control2_t, Control3_t, Status_t,
			FreeFallConfig_t, FreeFallSource_t, FreeFallThreshold_t,
			ClickConfig_t, ClickSource_t
	> Register_t;

public:
	enum class
	InterruptSource : uint8_t
	{
		GND = 0x00,
		FF_WU_1 = 0x01,
		FF_WU_2 = 0x02,
		FF_WU_1_OR_2 = 0x03,
		DataReady = 0x04,
		Click = 0x07,
	};

	enum class
	MeasurementRate : uint8_t
	{
		Hz100 = 0x00,
		Hz400 = int(Control1::DR),
	};

	enum class
	Scale : uint8_t
	{
		G2 = 0x00,
		G8 = int(Control1::FS),
	};

	enum class
	Interrupt : uint8_t
	{
		One = 0,
		Two = 0x04,
	};

	enum class
	Axis : uint8_t
	{
		X = 0b00,
		Y = 0b10,
		Z = 0b01,
	};

public:
	struct xpcc_packed
	Data
	{
		template < class Transport >
		friend class Lis302dl;

		Data() :
			data{0,0,0}, meta(0)
		{}
		// DATA ACCESS
		/// @{
		/// returns the scaled acceleration in g
		float inline
		getX() { return data[0] * (meta ? 9.2f/128 : 2.3f/128); }

		float inline
		getY() { return data[1] * (meta ? 9.2f/128 : 2.3f/128); }

		float inline
		getZ() { return data[2] * (meta ? 9.2f/128 : 2.3f/128); }
		/// @}

		inline int8_t
		operator [](uint8_t index)
		{ return (index < 3) ? data[index] : 0; }

	private:
		// data 0-2 = x,y,z
		int8_t data[3];
		// meta = false if G2, true if G8
		uint8_t meta;
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(InterruptSource source) { return static_cast<uint8_t>(source); }
	static constexpr Control3
	r(InterruptSource source) { return static_cast<Control3>(source); }
	static constexpr uint8_t
	i(Register reg) { return static_cast<uint8_t>(reg); }
	static constexpr uint8_t
	i(MeasurementRate rate) { return static_cast<uint8_t>(rate); }
	static constexpr Control1
	r(MeasurementRate rate) { return static_cast<Control1>(rate); }
	static constexpr uint8_t
	i(Scale scale) { return static_cast<uint8_t>(scale); }
	static constexpr Control1
	r(Scale scale) { return static_cast<Control1>(scale); }
	static constexpr uint8_t
	i(Interrupt interrupt) { return static_cast<uint8_t>(interrupt); }
	/// @endcond
}; // struct lis302

/**
 * LIS302DL 3-Axis accelerometer.
 *
 * The LIS302DL is an ultra compact low-power three axes linear accelerometer.
 * It includes a sensing element and an IC interface able to provide the measured
 * acceleration to the external world through an I2C/SPI serial interface.
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Lis3TransportI2c
 * @see Lis3TransportSpi
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class Transport >
class Lis302dl : public lis302dl, public Transport
{
public:
	/// Constructor, requires a lis302dl::Data object.
	/// For I2c this also sets the address to 0x1D (alternative: 0x1C).
	Lis302dl(Data &data, uint8_t address=0x1D);

	bool inline
	configureBlocking(Scale scale, MeasurementRate rate = MeasurementRate::Hz100)
	{
		return RF_CALL_BLOCKING(configure(scale, rate));
	}

	xpcc::ResumableResult<bool>
	configure(Scale scale, MeasurementRate rate = MeasurementRate::Hz100);

	// MARK: Control Registers
	xpcc::ResumableResult<bool> inline
	updateControlRegister(Control1_t setMask, Control1_t clearMask = Control1_t(0xff))
	{
		return updateControlRegister(0, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControlRegister(Control2_t setMask, Control2_t clearMask = Control2_t(0xff))
	{
		return updateControlRegister(1, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControlRegister(Control3_t setMask, Control3_t clearMask = Control3_t(0xff))
	{
		return updateControlRegister(2, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	writeInterruptSource(Interrupt interrupt, InterruptSource source)
	{
		if (interrupt == Interrupt::One)
			return updateControlRegister(r(source), Control3(0b111));

		return updateControlRegister(Control3(i(source) << 3), Control3(0b111000));
	}

	// MARK: Free Fall Registers
	xpcc::ResumableResult<bool> inline
	updateFreeFallConfiguration(Interrupt interrupt, FreeFallConfig_t setMask, FreeFallConfig_t clearMask = FreeFallConfig_t(0xff))
	{
		return updateRegister(i(Register::FfWuCfg1) | i(interrupt), setMask.value, clearMask.value);
	}

	xpcc::ResumableResult<bool> inline
	readFreeFallSource(Interrupt interrupt, FreeFallSource_t &source)
	{
		return this->read(i(Register::FfWuSrc1) | i(interrupt), source.value);
	}

	xpcc::ResumableResult<bool> inline
	setFreeFallThreshold(Interrupt interrupt, uint8_t threshold)
	{
		return this->write(i(Register::FfWuThs1) | i(interrupt), threshold);
	}

	xpcc::ResumableResult<bool> inline
	setFreeFallDuration(Interrupt interrupt, uint8_t duration)
	{
		return this->write(i(Register::FfWuDuration1) | i(interrupt), duration);
	}

	// MARK: Clock Registers
	xpcc::ResumableResult<bool> inline
	updateClickConfiguration(ClickConfig_t setMask, ClickConfig_t clearMask)
	{
		return updateRegister(i(Register::ClickCfg), setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	readClickSource(ClickSource_t &source)
	{
		return this->read(i(Register::ClickSrc), source);
	}

	xpcc::ResumableResult<bool> inline
	setClickThreshold(Axis axis, uint8_t threshold);

	xpcc::ResumableResult<bool> inline
	setClickTimeLimit(uint8_t limit)
	{
		return this->write(i(Register::ClickTimeLimit), limit);
	}

	xpcc::ResumableResult<bool> inline
	setClickLatency(uint8_t latency)
	{
		return this->write(i(Register::ClickLatency), latency);
	}

	xpcc::ResumableResult<bool> inline
	setClickWindow(uint8_t window)
	{
		return this->write(i(Register::ClickWindow), window);
	}

	xpcc::ResumableResult<bool>
	readAcceleration();

	Status_t
	getStatus()
	{ return Status_t(rawBuffer[3]); }

	Control1_t
	getControl1()
	{ return Control1_t(rawBuffer[0]); }

	Control2_t
	getControl2()
	{ return Control2_t(rawBuffer[1]); }

	Control3_t
	getControl3()
	{ return Control3_t(rawBuffer[3]); }

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	xpcc::ResumableResult<bool>
	updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask = Control_t(0xff));

	xpcc::ResumableResult<bool>
	updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask = 0xff);

	Data &data;
	// 0-2: control 0-2
	// 3: status
	// 4: -- buffer for updateRegister, but overridden in readAcceleration --
	// 5: out x
	// 6: -- unused, but overridden in readAcceleration --
	// 7: out y
	// 8: -- unused, but overridden in readAcceleration --
	// 9: out z
	uint8_t rawBuffer[10];
};

} // namespace xpcc

#include "lis302dl_impl.hpp"

#endif	// XPCC_LIS302DL_HPP
