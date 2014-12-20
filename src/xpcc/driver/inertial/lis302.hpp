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

#include <xpcc/architecture/peripheral/register.hpp>
#include <xpcc/processing/coroutine.hpp>
#include "lis3_transport.hpp"

namespace xpcc
{

struct lis302
{
protected:
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

public:
	/// CTRL_REG1 default value is 0x07
	enum class
	Control1 : uint8_t
	{
		DR = 0x80,		///< Data rate selection. (0: 100 Hz output data rate; 1: 400 Hz output data rate)
		PD = 0x40,		///< Power Down Control. (0: power down mode; 1: active mode)
		FS = 0x20,		///< Full Scale selection. (refer to Table 3 for typical full scale value)
		STP = 0x10,		///< Self Test Enable. (0: normal mode; 1: self test P enabled)
		STM = 0x08,		///< Self Test Enable. (0: normal mode; 1: self test M enabled)
		Zen = 0x04,		///< Z axis enable. (0: Z axis disabled; 1: Z axis enabled)
		Yen = 0x02,		///< Y axis enable. (0: Y axis disabled; 1: Y axis enabled)
		Xen = 0x01,		///< X axis enable. (0: X axis disabled; 1: X axis enabled)
	};
	REGISTER8(Control1);

	/// CTRL_REG2 default value is 0x00
	enum class
	Control2 : uint8_t
	{
		SIM = 0x80,			///< SPI Serial Interface Mode selection. (0: 4-wire interface; 1: 3-wire interface)
		BOOT = 0x40,		///< Reboot memory content. (0: normal mode; 1: reboot memory content)

		FDS = 0x10,			///< Filtered Data Selection. (0: internal filter bypassed; 1: data from internal filter sent to output register)
		HP_FF_WU2 = 0x08,	///< High Pass filter enabled for FreeFall/WakeUp # 2. (0: filter bypassed; 1: filter enabled)
		HP_FF_WU1 = 0x04,	///< High Pass filter enabled for Free-Fall/Wake-Up #1. (0: filter bypassed; 1: filter enabled)
		HP_COEFF2 = 0x02,	///< High pass filter cut-off frequency configuration.
		HP_COEFF1 = 0x01,	///< High pass filter cut-off frequency configuration.
	};
	REGISTER8(Control2);

	/// CTRL_REG3 default value is 0x00
	enum class
	Control3 : uint8_t
	{
		IHL = 0x80,		///< Interrupt active high, low. (0: active high; 1: active low)
		PP_OD = 0x40,	///< Push-pull/Open Drain selection on interrupt pad. (0: push-pull; 1: open drain)

		I2CFG2 = 0x20,	///< Data Signal on Int2 pad control bits.
		I2CFG1 = 0x10,	///< Data Signal on Int2 pad control bits.
		I2CFG0 = 0x08,	///< Data Signal on Int2 pad control bits.

		I1CFG2 = 0x04,	///< Data Signal on Int1 pad control bits.
		I1CFG1 = 0x02,	///< Data Signal on Int1 pad control bits.
		I1CFG0 = 0x01,	///< Data Signal on Int1 pad control bits.
	};
	REGISTER8(Control3);

	REGISTER8_GROUP(Control,
			Control1, Control2, Control3);

	/// STATUS_REG default value is 0x00
	enum class
	Status : uint8_t
	{
		ZYXOR = 0x80,	///< X, Y and Z axis data overrun.
		ZOR = 0x40,		///< Z axis data overrun.
		YOR = 0x20,		///< Y axis data overrun.
		XOR = 0x10,		///< X axis data overrun.

		ZYXDA = 0x08,	///< X, Y and Z axis new data available.
		ZDA = 0x04,		///< Z axis new data available.
		YDA = 0x02,		///< Y axis new data available.
		XDA = 0x01,		///< X axis new data available.
	};
	REGISTER8(Status);

	/// FF_WU_CFG default value is 0x00
	enum class
	FreeFallConfig : uint8_t
	{
		AOI = 0x80,		///< And/or combination of Interrupt events. (0: OR combination of interrupt events; 1: AND combination of interrupt events)
		LIR = 0x40,		///< Latch Interrupt request into FF_WU_SRC reg with the FF_WU_SRC reg cleared by reading FF_WU_SRC_1 reg.
		ZHIE = 0x20,	///< Enable interrupt generation on Z high event.
		ZLIE = 0x10,	///< Enable interrupt generation on Z low event.
		YHIE = 0x08,	///< Enable interrupt generation on Y high event.
		YLIE = 0x04,	///< Enable interrupt generation on Y low event.
		XHIE = 0x02,	///< Enable interrupt generation on X high event.
		XLIE = 0x01,	///< Enable interrupt generation on X low event.
	};
	REGISTER8(FreeFallConfig);

	/// FF_WU_SRC default value is 0x00
	enum class
	FreeFallSource : uint8_t
	{
		IA = 0x40,	///< Interrupt Active.
		ZH = 0x20,	///< Z high.
		ZL = 0x10,	///< Z low.
		YH = 0x08,	///< Y high.
		YL = 0x04,	///< Y low.
		XH = 0x02,	///< X high.
		XL = 0x01,	///< X low.
	};
	REGISTER8(FreeFallSource);

	/// FF_WU_THS default value is 0x00
	enum class
	FreeFallThreshold : uint8_t
	{
		DRCM = 0x80,	///< Resetting mode selection.
		THS_Mask = 0x7F,		///< Free-fall / wake-up Threshold.
	};
	REGISTER8(FreeFallThreshold);

	/// CLOCK_CFG default value is 0x00
	enum class
	ClickConfig : uint8_t
	{
		LIR = 0x40,		///< Latch interrupt request into CLICK_SRC reg with the CLICK_SRC reg refreshed by reading CLICK_SRC reg.
		DoubleZ = 0x20,
		SingleZ = 0x10,
		DoubleY = 0x08,
		SingleY = 0x04,
		DoubleX = 0x02,
		SingleX = 0x01,
	};
	REGISTER8(ClickConfig);

	/// CLOCK_SRC default value is 0x00
	enum class
	ClickSource : uint8_t
	{
		IA = 0x40,		///< Interrupt Active.
		DoubleZ = 0x20,
		SingleZ = 0x10,
		DoubleY = 0x08,
		SingleY = 0x04,
		DoubleX = 0x02,
		SingleX = 0x01,
	};
	REGISTER8(ClickSource);

	REGISTER8_GROUP(Register,
			Control1, Control2, Control3, Status,
			FreeFallConfig, FreeFallSource, FreeFallThreshold,
			ClickConfig, ClickSource);

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
		Hz400 = Control1::DR,
	};

	enum class
	Scale : uint8_t
	{
		G2 = 0x00,
		G8 = Control1::FS,
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
	struct __attribute__ ((packed))
	Data
	{
		Data()
		:	data{0,0,0,0}
		{}
		// DATA ACCESS
		/// @{
		/// returns the acceleration in g
		float ALWAYS_INLINE
		getX() { return data[0] * (static_cast<Control1>(data[3]) & Control1::FS ? 9.2f/128 : 2.3f/128); }

		float ALWAYS_INLINE
		getY() { return data[1] * (static_cast<Control1>(data[3]) & Control1::FS ? 9.2f/128 : 2.3f/128); }

		float ALWAYS_INLINE
		getZ() { return data[2] * (static_cast<Control1>(data[3]) & Control1::FS ? 9.2f/128 : 2.3f/128); }
		/// @}


		ALWAYS_INLINE uint8_t
		operator [](uint8_t index)
		{ return (index < 3) ? data[index] : 0; }

		ALWAYS_INLINE int8_t*
		getPointer()
		{ return data; }

	private:
		// data 0-2 = x,y,z
		// data 3 = bool
		int8_t data[4];
	};

protected:
	/// @{
	/// @private enum class to integer helper functions.
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
	/// @}
}; // struct lis302

/**
 * LIS302DL 3-Axis accelerometer.
 *
 * The LIS302DL is an ultra compact low-power three axes linear accelerometer.
 * It includes a sensing element and an IC interface able to provide the measured
 * acceleration to the external world through an I2C/SPI serial interface.
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Lis302I2cTransport
 * @see Lis302SpiTransport
 *
 * @ingroup inertial
 * @author	Niklas Hauser
 */
template < class Transport >
class Lis302 : public lis302, public Transport, protected xpcc::pt::Protothread
{
public:
	/// Constructor, requires a lis302::Data object.
	/// For I2c this also sets the address to 0x1D (alternative: 0x1C).
	Lis302(Data &data, uint8_t address=0x1D);

	bool inline
	initialize(Scale scale, MeasurementRate rate = MeasurementRate::Hz100)
	{
		return CO_CALL_BLOCKING(initialize(this, scale, rate));
	}

	xpcc::co::Result<bool>
	initialize(void *ctx, Scale scale, MeasurementRate rate = MeasurementRate::Hz100);

	// MARK: Control Registers
	xpcc::co::Result<bool> inline
	updateControlRegister(void *ctx, Control1_t setMask, Control1_t clearMask = static_cast<Control1_t>(0xff))
	{
		return updateControlRegister(ctx, 0, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControlRegister(void *ctx, Control2_t setMask, Control2_t clearMask = static_cast<Control2_t>(0xff))
	{
		return updateControlRegister(ctx, 1, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControlRegister(void *ctx, Control3_t setMask, Control3_t clearMask = static_cast<Control3_t>(0xff))
	{
		return updateControlRegister(ctx, 2, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	writeInterruptSource(void *ctx, Interrupt interrupt, InterruptSource source)
	{
		if (interrupt == Interrupt::One)
			return updateControlRegister(ctx, r(source), static_cast<Control3>(0b111));

		return updateControlRegister(ctx, static_cast<Control3>(i(source) << 3), static_cast<Control3>(0b111000));
	}

	// MARK: Free Fall Registers
	xpcc::co::Result<bool> inline
	updateFreeFallConfiguration(void *ctx, Interrupt interrupt, FreeFallConfig_t setMask, FreeFallConfig_t clearMask = static_cast<FreeFallConfig_t>(0xff))
	{
		return updateRegister(ctx, i(Register::FfWuCfg1) | i(interrupt), setMask.value, clearMask.value);
	}

	xpcc::co::Result<bool> inline
	readFreeFallSource(void *ctx, Interrupt interrupt, FreeFallSource_t &source)
	{
		return this->read(ctx, i(Register::FfWuSrc1) | i(interrupt), source.value);
	}

	xpcc::co::Result<bool> inline
	writeFreeFallThreshold(void *ctx, Interrupt interrupt, uint8_t threshold)
	{
		return this->write(ctx, i(Register::FfWuThs1) | i(interrupt), threshold);
	}

	xpcc::co::Result<bool> inline
	writeFreeFallDuration(void *ctx, Interrupt interrupt, uint8_t duration)
	{
		return this->write(ctx, i(Register::FfWuDuration1) | i(interrupt), duration);
	}

	// MARK: Clock Registers
	xpcc::co::Result<bool> inline
	updateClickConfiguration(void *ctx, ClickConfig_t setMask, ClickConfig_t clearMask)
	{
		return updateRegister(ctx, i(Register::ClickCfg), setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	readClickSource(void *ctx, ClickSource_t &source)
	{
		return this->read(ctx, i(Register::ClickSrc), source);
	}

	xpcc::co::Result<bool> inline
	writeClickThreshold(void *ctx, Axis axis, uint8_t threshold);

	xpcc::co::Result<bool> inline
	writeClickTimeLimit(void *ctx, uint8_t limit)
	{
		return this->write(ctx, i(Register::ClickTimeLimit), limit);
	}

	xpcc::co::Result<bool> inline
	writeClickLatency(void *ctx, uint8_t latency)
	{
		return this->write(ctx, i(Register::ClickLatency), latency);
	}

	xpcc::co::Result<bool> inline
	writeClickWindow(void *ctx, uint8_t window)
	{
		return this->write(ctx, i(Register::ClickWindow), window);
	}

	xpcc::co::Result<bool>
	readAcceleration(void *ctx);

	uint8_t
	getStatus()
	{ return rawBuffer[3]; }

	uint8_t
	getControl(uint8_t index)
	{ return index < 3 ? rawBuffer[index] : 0; }

public:
	/// the data object for this sensor.
	Data &data;

private:
	xpcc::co::Result<bool>
	updateControlRegister(void *ctx, uint8_t index, Control_t setMask, Control_t clearMask = static_cast<Control_t>(0xff));

	xpcc::co::Result<bool>
	updateRegister(void *ctx, uint8_t reg, uint8_t setMask, uint8_t clearMask = 0xff);

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

#include "lis302_impl.hpp"

#endif	// XPCC_LIS302DL_HPP
