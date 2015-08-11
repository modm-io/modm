// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LSM303A_HPP
#define XPCC_LSM303A_HPP

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/processing/resumable.hpp>
#include <xpcc/math/utils/endianness.hpp>
#include "lis3_transport.hpp"

namespace xpcc
{

// forward declaration for friending with lsm303a::Data
template < class I2cMaster >
class Lsm303a;

struct lsm303a
{
	enum class
	Register : uint8_t
	{
		WHO_AM_I = 0x0F,	///< Contains 0x33

		CTRL1 = 0x20,		///< Control register 1: rw, 0x07
		CTRL2 = 0x21,		///< Control register 2: rw
		CTRL3 = 0x22,		///< Control register 3: rw
		CTRL4 = 0x23,		///< Control register 4: rw
		CTRL5 = 0x24,		///< Control register 5: rw
		CTRL6 = 0x25,		///< Control register 6: rw

		REFERENCE = 0x26,	///< Reference register: r/w
		STATUS = 0x27,		///< Status Data Register

		OUT_X_L = 0x28,		///< Output x-axis register low
		OUT_X_H = 0x29,		///< Output x-axis register high
		OUT_Y_L = 0x2A,		///< Output x-axis register low
		OUT_Y_H = 0x2B,		///< Output x-axis register high
		OUT_Z_L = 0x2C,		///< Output x-axis register low
		OUT_Z_H = 0x2D,		///< Output x-axis register high

		FIFO_CTRL = 0x2E,	///< FIFO control register
		FIFO_SRC = 0x2F,	///< FIFO source register

		INT1_CFG = 0x30,		///< Interrupt1 config register
		INT1_SRC = 0x31,		///< Interrupt1 source register
		INT1_THS = 0x32,		///< Interrupt1 threshold register
		INT1_DURATION = 0x33,	///< Interrupt1 duration register

		INT2_CFG = 0x34,		///< Interrupt2 config register
		INT2_SRC = 0x35,		///< Interrupt2 source register
		INT2_THS = 0x36,		///< Interrupt2 threshold register
		INT2_DURATION = 0x37,	///< Interrupt2 duration register

		CLICK_CFG = 0x38,		///< Click config register
		CLICK_SRC = 0x39,		///< Click source register
		CLICK_THS = 0x3A,		///< Click threshold register

		TIME_LIMIT = 0x3B,		///< Click config register
		TIME_LATENCY = 0x3C,	///< Click source register
		TIME_WINDOW = 0x3D,		///< Click threshold register
	};

public:
	/// CTRL_REG1 default value is 0x07
	enum class
	Control1 : uint8_t
	{
		ODR3 = Bit7,
		ODR2 = Bit6,
		ODR1 = Bit5,
		ODR0 = Bit4,

		LPen = Bit3,	///< Power-down mode enable
		Zen = Bit2,		///< Z axis enable
		Yen = Bit1,		///< Y axis enable
		Xen = Bit0,		///< X axis enable
	};
	XPCC_FLAGS8(Control1);

	/// CTRL_REG2 default value is 0x00
	enum class
	Control2 : uint8_t
	{
		HPM1 = Bit7,	///< High-pass filter mode selection
		HPM0 = Bit6,	///< High-pass filter mode selection

		HPCF2 = Bit5,	///< High-pass filter cutoff frequency
		HPCF1 = Bit4,	///< High-pass filter cutoff frequency

		FDS = Bit3,		///< Filtered data selection

		HPCLICK = Bit2,	///< High-pass filter enabled for click function

		HPIS2 = Bit1,	///< High-pass filter enabled for AOI function on Interrupt 2
		HPIS1 = Bit0	///< High-pass filter enabled for AOI function on Interrupt 1
	};
	XPCC_FLAGS8(Control2);

	/// CTRL_REG3 default value is 0x00
	enum class
	Control3 : uint8_t
	{
		I1_CLICK = Bit7,	///< CLICK interrupt on INT1.
		I1_AOI1 = Bit6,		///< AOI1 interrupt on INT1.
		I1_AOI2 = Bit5,		///< AOI2 interrupt on INT1.
		I1_DRDY1 = Bit4,	///< Data-ready on DRDY/INT1.
		I1_DRDY2 = Bit3,	///< Data-ready on DRDY/INT2.
		I1_WTM = Bit2,		///< FIFO watermark interrupt on DRDY/INT1.
		I1_OVERRUN = Bit1,	///< FIFO overrun interrupt on DRDY/INT1.
	};
	XPCC_FLAGS8(Control3);

	/// CTRL_REG4 default value is 0x00
	enum class
	Control4 : uint8_t
	{
		BDU = Bit7,		///< Block data update. (0: continuos update; 1: output registers not updated until MSb and LSb reading)
		BLE = Bit6,		///< Big/little endian data selection. (1: Data LSb @ lower address; 0: Data MSb @ lower address)

		FS1 = Bit5,
		FS0 = Bit4,

		HR = Bit3,		///< High-resolution output mode
		SIM = Bit0,		///< SPI serial interface mode selection. (0: 4-wire interface; 1: 3-wire interface).
	};
	XPCC_FLAGS8(Control4);

	/// CTRL_REG5 default value is 0x00
	enum class
	Control5 : uint8_t
	{
		BOOT = Bit7,		///< Force reboot, cleared as soon as the reboot is finished. Active high.
		FIFO_EN = Bit6,		///< FIFO enable.

		LIR_INT1 = Bit3,	///< Latch interrupt request on INT1_SRC register, with INT1_SRC register cleared by reading INT1_SRC itself.
		D4D_INT1 = Bit2,	///< 4D enable: 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
		LIR_INT2 = Bit1,	///< Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by reading INT2_SRC itself.
		D4D_INT2 = Bit0,	///< 4D enable: 4D detection is enabled on INT2 when 6D bit on INT2_CFG is set to 1.
	};
	XPCC_FLAGS8(Control5);

		/// CTRL_REG6 default value is 0x00
	enum class
	Control6 : uint8_t
	{
		I2_CLICKen = Bit7,	///< CLICK interrupt on PAD2.
		I2_INT1 = Bit6,		///< Interrupt 1 on PAD2.
		I2_INT2 = Bit5,		///< Interrupt 2 on PAD2.
		BOOT_I1 = Bit4,		///< Reboot memory content on PAD2.
		P2_ACT = Bit3,		///< Active function status on PAD2.

		H_LACTIVE = Bit1,	///< Interrupt active high, low.
	};
	XPCC_FLAGS8(Control6);

	/// STATUS is read-only
	enum class
	Status : uint8_t
	{
		ZYXOR = Bit7,	///< X, Y, and Z axis data overrun.
		ZOR = Bit6,		///< Z axis data overrun.
		YOR = Bit5,		///< Y axis data overrun.
		XOR = Bit4,		///< X axis data overrun.
		ZYXDA = Bit3,	///< X, Y, and Z axis new data available.
		ZDA = Bit2,		///< Z axis new data available.
		YDA = Bit1,		///< Y axis new data available.
		XDA = Bit0,		///< X axis new data available.
	};
	XPCC_FLAGS8(Status);

	/// FIFO_CTRL default value is 0x00
	enum class
	FifoControl : uint8_t
	{
		FM1 = Bit7,
		FM0 = Bit6,

		TR = Bit5,

		FTH4 = Bit4,
		FTH3 = Bit3,
		FTH2 = Bit2,
		FTH1 = Bit1,
		FTH0 = Bit0,
	};
	XPCC_FLAGS8(FifoControl);

	/// FIFO_SRC is read-only
	enum class
	FifoSource : uint8_t
	{
		WTM = Bit7,
		OVRN = Bit6,
		EMPTY = Bit5,

		FSS4 = Bit4,
		FSS3 = Bit3,
		FSS2 = Bit2,
		FSS1 = Bit1,
		FSS0 = Bit0,
	};
	XPCC_FLAGS8(FifoSource);

		/// INT_CFG default value is 0x00
	enum class
	IntConfig : uint8_t
	{
		AOI = Bit7,		///< And/or combination of Interrupt events. (0: OR combination of interrupt events; 1: AND combination of interrupt events)
		D6 = Bit6,		///<6-direction detection function enabled.
		ZHIE = Bit5,	///< Enable interrupt generation on Z high event.
		ZLIE = Bit4,	///< Enable interrupt generation on Z low event.
		YHIE = Bit3,	///< Enable interrupt generation on Y high event.
		YLIE = Bit2,	///< Enable interrupt generation on Y low event.
		XHIE = Bit1,	///< Enable interrupt generation on X high event.
		XLIE = Bit0,	///< Enable interrupt generation on X low event.
	};
	XPCC_FLAGS8(IntConfig);

	/// INT1_SRC default value is 0x00
	enum class
	IntSource : uint8_t
	{
		IA = Bit6,	///< Interrupt Active.
		ZH = Bit5,	///< Z high.
		ZL = Bit4,	///< Z low.
		YH = Bit3,	///< Y high.
		YL = Bit2,	///< Y low.
		XH = Bit1,	///< X high.
		XL = Bit0,	///< X low.
	};
	XPCC_FLAGS8(IntSource);

	/// CLOCK_CFG default value is 0x00
	enum class
	ClickConfig : uint8_t
	{
		ZD = Bit5,
		ZS = Bit4,
		YD = Bit3,
		YS = Bit2,
		XD = Bit1,
		XS = Bit0,
	};
	XPCC_FLAGS8(ClickConfig);

	/// CLOCK_SRC default value is 0x00
	enum class
	ClickSource : uint8_t
	{
		IA = Bit6,		///< Interrupt Active.
		DCLICK = Bit5,	///< Double-click enable.
		SCLICK = Bit4,	///< Single-click enable.
		Sign = Bit3,	///< Click sign. 0: positive detection, 1: negative detection
		Z = Bit2,
		Y = Bit1,
		X = Bit0,
	};
	XPCC_FLAGS8(ClickSource);

public:
	typedef FlagsGroup<
			Control1_t, Control2_t, Control3_t, Control4_t, Control5_t, Control6_t,
			FifoControl_t, IntConfig_t, ClickConfig_t
	> Control_t;

public:
	enum class
	FifoMode : uint8_t
	{
		Bypass = 0,
		Fifo = FifoControl::FM0,
		Stream = FifoControl::FM1,
		Trigger = int(FifoControl::FM1) | int(FifoControl::FM0)
	};

	enum class
	MeasurementRate : uint8_t
	{
		Off = 0,
		Hz1 = Control1::ODR0,
		Hz10 = Control1::ODR1,
		Hz25 = int(Control1::ODR1) | int(Control1::ODR0),
		Hz50 = Control1::ODR2,
		Hz100 = int(Control1::ODR2) | int(Control1::ODR0),
		Hz200 = int(Control1::ODR2) | int(Control1::ODR1),
		Hz400 = int(Control1::ODR2) | int(Control1::ODR1) | int(Control1::ODR0),

		Hz1620 = Control1::ODR3,
		Hz5376 = int(Control1::ODR3) | int(Control1::ODR0),
	};

	enum class
	Scale : uint8_t
	{
		G2 = 0,
		G4 = Control4::FS0,
		G8 = Control4::FS1,
		G16 = int(Control4::FS1) | int(Control4::FS0),
	};

public:
	struct ATTRIBUTE_PACKED
	Data
	{
		template < class Transport >
		friend class Lsm303a;

		Data()
		:	data{0,0,0,0,0,0}, scale(1)
		{}
		// DATA ACCESS
		///@{
		/// returns the acceleration in g
		float inline
		getX() { return getData(0) * scale * 0.001f; }

		float inline
		getY() { return getData(1) * scale * 0.001f; }

		float inline
		getZ() { return getData(2) * scale * 0.001f; }
		///@}

		inline int16_t
		operator [](uint8_t index)
		{ return (index < 3) ? getData(index) : 0; }

		inline uint8_t*
		getPointer()
		{ return data; }

	private:
		int16_t inline
		getData(uint8_t index)
		{
			int16_t *rData = reinterpret_cast<int16_t*>(data);
			return (xpcc::fromLittleEndian(rData[index]) >> 4);
		}

		// data 0-5 = xl,xh,yl,yh,zl,zh
		uint8_t data[6];
		// contains scale in g (5bit LSB)
		uint8_t scale;
		//  1,  2,  4,  12 mg per LSB for
		// 2g, 4g, 8g, 16g range
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	static constexpr uint8_t
	i(MeasurementRate rate) { return uint8_t(rate); }
	static constexpr uint8_t
	i(Scale scale) { return uint8_t(scale); }
	static constexpr Control4
	r(Scale scale) { return Control4(i(scale)); }
	/// @endcond
}; // struct lsm303a

/**
 * LSM303DLHC 3-Axis accelerometer.
 *
 * The LSM303DLHC is a system-in-package featuring a 3D digital linear
 * acceleration sensor and a 3D digital magnetic sensor.
 * The sensor has linear acceleration full scales of ±2g/±4g/±8g/±16g and a
 * magnetic field full scale of ±1.3/±1.9/±2.5/±4.0/±4.7/±5.6/±8.1 gauss.
 * The LSM303DLHC includes an I2C serial bus interface that supports standard and fast mode 100 kHz and 400 kHz.
 *
 * Since the device actually features two I2C devices which are just placed in the same
 * package, this class only implements the acceleration sensor.
 * For the magnetic field sensor see the `Lsm303m` class!
 *
 * @see	Lsm303m
 *
 * @tparam	I2cMaster	A class conforming to the I2cMaster interface
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Lsm303a : public lsm303a, public Lis3TransportI2c<I2cMaster>
{
public:
	/// Constructor, requires a lsm303a::Data object.
	Lsm303a(Data &data, uint8_t address = 0b0011001);

	bool inline
	configureBlocking(Scale scale, MeasurementRate rate = MeasurementRate::Hz100)
	{
		return RF_CALL_BLOCKING(configure(scale, rate));
	}

	xpcc::ResumableResult<bool>
	configure(Scale scale, MeasurementRate rate = MeasurementRate::Hz100);

	// MARK: Control Registers
	xpcc::ResumableResult<bool> inline
	updateControl(Control1_t setMask, Control1_t clearMask = Control1_t(0xff))
	{
		return updateControlRegister(1, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControl(Control2_t setMask, Control2_t clearMask = Control2_t(0xff))
	{
		return updateControlRegister(2, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControl(Control3_t setMask, Control3_t clearMask = Control3_t(0xff))
	{
		return updateControlRegister(3, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControl(Control4_t setMask, Control4_t clearMask = Control4_t(0xff))
	{
		return updateControlRegister(0, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControl(Control5_t setMask, Control5_t clearMask = Control5_t(0xff))
	{
		return updateControlRegister(4, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateControl(Control6_t setMask, Control6_t clearMask = Control6_t(0xff))
	{
		return updateControlRegister(5, setMask, clearMask);
	}

	xpcc::ResumableResult<bool> inline
	updateFifoControl(FifoControl_t setMask, FifoControl_t clearMask = FifoControl_t(0xff))
	{
		return updateControlRegister(14, setMask, clearMask);
	}


	// MARK: Read access
	xpcc::ResumableResult<bool>
	readAcceleration();

	// MARK: Registers with instant access
	Control1_t getControl1()
	{ return Control1_t(rawBuffer[0]); }

	Control2_t getControl2()
	{ return Control2_t(rawBuffer[1]); }

	Control3_t getControl3()
	{ return Control3_t(rawBuffer[2]); }

	Control4_t getControl4()
	{ return Control4_t(rawBuffer[3]); }

	Control5_t getControl5()
	{ return Control5_t(rawBuffer[4]); }

	Control6_t getControl6()
	{ return Control6_t(rawBuffer[5]); }

	uint8_t getReference()
	{ return rawBuffer[5]; }

	FifoControl_t getFifoControl()
	{ return FifoControl_t(rawBuffer[14]); }

	// MARK: Registers with buffered access
	Status_t getStatus()
	{ return Status_t(rawBuffer[7]); }

	FifoSource_t getFifoSource()
	{ return FifoSource_t(rawBuffer[15]); }

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	xpcc::ResumableResult<bool>
	updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask = static_cast<Control_t>(0xff));

	xpcc::ResumableResult<bool>
	updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask = 0xff);

	Data &data;

	// the read buffer is for a continous read from address 0x27 -> 0x2D
	// 0-5: control 1-6 (rw)
	// 6: reference (rw)
	// 7: status (ro)
	// 8: out x low
	// 9: out x high
	// 10: out y low
	// 11: out y high
	// 12: out z low
	// 13: out z high
	// 14: fifo control (rw)
	// 15: fifo source (ro)

	// Nope!
	// 16: int1 control (rw)
	// 17: int1 source (ro)
	// ... two registers not buffered
	// 18: int2 control (rw)
	// 19: int2 source (ro)
	// ... two registers not buffered
	// 20: click config (rw)
	// 21: click source (ro)
	// Nope!

	uint8_t rawBuffer[16];
};

} // namespace xpcc

#include "lsm303a_impl.hpp"

#endif	// XPCC_LSM303A_HPP
