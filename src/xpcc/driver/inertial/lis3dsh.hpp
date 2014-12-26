// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS3DSH_HPP
#define XPCC_LIS3DSH_HPP

#include <xpcc/architecture/peripheral/register.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/math/utils/endianness.hpp>
#include "lis3_transport.hpp"

namespace xpcc
{

// forward declaration for friending with lis3dsh::Data
template < class Transport >
class Lis3dsh;

struct lis3dsh
{
	enum class
	Register : uint8_t
	{
		INFO1 = 0x0D,			///< Information register 1: r
		INFO2 = 0x0E,			///< Information register 2: r
		WHO_AM_I = 0x0F,		///< Device identification (0x3F): r

		CTRL_REG3 = 0x23,		///< Control register 3: r/w
		CTRL_REG4 = 0x20,		///< Control register 4: r/w
		CTRL_REG5 = 0x24,		///< Control register 5: r/w
		CTRL_REG6 = 0x25,		///< Control register 6: r/w

		STATUS = 0x27,			///< Status Data Register
		OUT_T = 0x0C,			///< Temperature Output

		OFF_X = 0x10,			///< Offset correction x-axis
		OFF_Y = 0x11,			///< Offset correction y-axis
		OFF_Z = 0x12,			///< Offset correction z-axis

		CS_X = 0x13,			///< Constant shift X
		CS_Y = 0x14,			///< Constant shift Y
		CS_Z = 0x15,			///< Constant shift Z

		LC_L = 0x16,			///< Long Counter register low
		LC_H = 0x17,			///< Long Counter register high

		STAT = 0x18,			///< Interrupt synchronization

		VFC_1 = 0x1B,			///< Vector filter coefficient 1
		VFC_2 = 0x1C,			///< Vector filter coefficient 2
		VFC_3 = 0x1D,			///< Vector filter coefficient 3
		VFC_4 = 0x1E,			///< Vector filter coefficient 4

		THRS3 = 0x1F,			///< Threshold value 3

		OUT_X_L = 0x28,			///< Output x-axis register low
		OUT_X_H = 0x29,			///< Output x-axis register high
		OUT_Y_L = 0x2A,			///< Output x-axis register low
		OUT_Y_H = 0x2B,			///< Output x-axis register high
		OUT_Z_L = 0x2C,			///< Output x-axis register low
		OUT_Z_H = 0x2D,			///< Output x-axis register high

		FIFO_CTRL = 0x2E,		///< FIFO control register
		FIFO_SRC = 0x2F,		///< FIFO source register

		// State Machine 1
		CTRL_REG1 = 0x21,		///< SM1 control register

		ST1_0 = 0x40,			///< SM1 code register 0 (additional 15 registers)

		TIM4_1 = 0x50,			///< SM1 general timer 4
		TIM3_1 = 0x51,			///< SM1 general timer 3
		TIM2_1_L = 0x52,		///< SM1 general timer 2 low
		TIM2_1_H = 0x53,		///< SM1 general timer 2 high
		TIM1_1_L = 0x54,		///< SM1 general timer 1 low
		TIM1_1_H = 0x55,		///< SM1 general timer 1 high

		THRS2_1 = 0x56,			///< SM1 threshold value 1
		THRS1_1 = 0x57,			///< SM1 threshold value 2

		MASK1_B = 0x59,			///< SM1 axis and sign mask
		MASK1_A = 0x5A,			///< SM1 axis and sign mask

		SETT1 = 0x5B,			///< SM1 detection settings
		PR1 = 0x5C,				///< SM1 program-reset pointer
		TC1_L = 0x5D,			///< SM1 timer counter low
		TC1_H = 0x5E,			///< SM1 timer counter high
		OUTS1 = 0x5F,			///< SM1 main set flag
		PEAK1 = 0x19,			///< SM1 peak value

		// State Machine 2
		CTRL_REG2 = 0x22,		///< SM2 control register

		ST2_0 = 0x60,			///< SM2 code register 0 (additional 15 registers)

		TIM4_2 = 0x70,			///< SM2 general timer 4
		TIM3_2 = 0x71,			///< SM2 general timer 3
		TIM2_2_L = 0x72,		///< SM2 general timer 2 low
		TIM2_2_H = 0x73,		///< SM2 general timer 2 high
		TIM1_2_L = 0x74,		///< SM2 general timer 1 low
		TIM1_2_H = 0x75,		///< SM2 general timer 1 high

		THRS2_2 = 0x76,			///< SM2 threshold value 1
		THRS1_2 = 0x77,			///< SM2 threshold value 2

		MASK2_B = 0x79,			///< SM2 axis and sign mask
		MASK2_A = 0x7A,			///< SM2 axis and sign mask

		SETT2 = 0x7B,			///< SM2 detection settings
		PR2 = 0x7C,				///< SM2 program-reset pointer
		TC2_L = 0x7D,			///< SM2 timer counter low
		TC2_H = 0x7E,			///< SM2 timer counter high
		OUTS2 = 0x7F,			///< SM2 main set flag
		PEAK2 = 0x1A,			///< SM2 peak value

		DES2 = 0x78,			///< Decimation factor
	};

public:
	/// CTRL_REG3 default value is 0x00
	enum class
	Control3 : uint8_t
	{
		DR_EN = Bit7,	///< DRDY signal enable to INT1.
		IEA = Bit6,		///< Interrupt signal polarity.
		IEL = Bit5,		///< Interrupt signal latching.
		INT2_EN = Bit4,	///< Interrupt 2 enable/disable.
		INT1_EN = Bit3,	///< Interrupt 1 enable/disable.
		VFILT = Bit2,	///< Vector filter enable/disable.
		STRT = Bit0,	///< Soft reset bit.
	};
	REGISTER8(Control3);

	/// CTRL_REG4 default value is 0x07
	enum class
	Control4 : uint8_t
	{
		ODR3 = Bit7,
		ODR2 = Bit6,
		ODR1 = Bit5,
		ODR0 = Bit4,
		ODR_Mask = Bit7 | Bit6 | Bit5 | Bit4,

		BDU = Bit3,		///< Block data update.
		ZEN = Bit2,		///< Z axis enable
		YEN = Bit1,		///< Y axis enable
		XEN = Bit0,		///< X axis enable
	};
	REGISTER8(Control4);


	/// CTRL_REG5 default value is 0x00
	enum class
	Control5 : uint8_t
	{
		BW2 = Bit7,
		BW1 = Bit6,

		FSCALE2 = Bit5,
		FSCALE1 = Bit4,
		FSCALE0 = Bit3,
		FSCALE_Mask = Bit5 | Bit4 | Bit3,

		ST2 = Bit2,
		ST1 = Bit1,

		SIM = Bit0,		///< PI serial interface mode selection.
	};
	REGISTER8(Control5);

	/// CTRL_REG6 default value is 0x00
	enum class
	Control6 : uint8_t
	{
		BOOT = Bit7,		///< Force reboot, cleared as soon as the reboot is finished. Active high.
		FIFO_EN = Bit6,		///< FIFO enable.
		WTM_EN = Bit5,		///< Enable FIFO Watermark level use.
		ADD_INC = Bit4,		///< Register address automatically incremented during a multiple byte access with a serial interface (I2C or SPI).
		P1_EMPTY = Bit3,	///< Enable FIFO Empty indication on int1.
		P1_WTM = Bit2,		///< FIFO Watermark interrupt on int1.
		P1_OVERRUN = Bit1,	///< FIFO overrun interrupt on int1.
		P2_BOOT = Bit0,		///< PI serial interface mode selection.
	};
	REGISTER8(Control6);

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
	REGISTER8(Status);

	/// STAT is read-only
	enum class
	IntStatus : uint8_t
	{
		LONG = Bit7,	///< 0=no interrupt, 1=long counter (LC) interrupt flag common for both SM
		SYNCW = Bit6,	///< Synchronization for external Host Controller interrupt based on output data
		SYNC1 = Bit5,	///< 0=SM1 running normally, 1=SM1 stopped and await restart request from SM2
		SYNC2 = Bit4,	///< 0=SM2 running normally, 1=SM2 stopped and await restart request from SM1
		INT_SM1 = Bit3,	///< SM1 - Interrupt Selection
		INT_SM2 = Bit2,	///< SM2 - Interrupt Selection
		DOR = Bit1,		///< Data overrun indicates not read data from output register when next data samples measure start;
		DRDY = Bit0,	///< data ready from output register
	};
	REGISTER8(IntStatus);

	/// FIFO_CTRL default value is 0x00
	enum class
	FifoControl : uint8_t
	{
		FMODE2 = Bit7,
		FMODE1 = Bit6,
		FMODE0 = Bit5,
		FMODE_Mask = Bit7 | Bit6 | Bit5,
		WTMP4 = Bit4,
		WTMP3 = Bit3,
		WTMP2 = Bit2,
		WTMP1 = Bit1,
		WTMP0 = Bit0,
		WTMP_Mask = Bit4 | Bit3 | Bit2 | Bit1 | Bit0,
	};
	REGISTER8(FifoControl);

	/// FIFO_SRC is read-only
	enum class
	FifoSource : uint8_t
	{
		WTM = Bit7,
		OVRN_FIFO = Bit6,
		EMPTY = Bit5,

		FSS4 = Bit4,
		FSS3 = Bit3,
		FSS2 = Bit2,
		FSS1 = Bit1,
		FSS0 = Bit0,
		FSS_Mask = Bit4 | Bit3 | Bit2 | Bit1 | Bit0,
	};
	REGISTER8(FifoSource);

public:
	/// MASK1_A, MASK1_B, MASK2_A, MASK2_B, OUTS1, OUTS2
	enum class
	AxisSign : uint8_t
	{
		P_X = Bit7,
		N_X = Bit6,

		P_Y = Bit5,
		N_Y = Bit4,

		P_Z = Bit3,
		N_Z = Bit2,

		P_V = Bit1,
		N_V = Bit0,
	};
	REGISTER8(AxisSign);

	/// CTRL_REG1, CTRL_REG2
	enum class
	SmControl : uint8_t
	{
		HYST2 = Bit7,
		HYST1 = Bit6,
		HYST0 = Bit5,
		HYST_Mask = Bit7 | Bit6 | Bit5,

		SM_PIN = Bit3,

		SM_EN = Bit0,
	};
	REGISTER8(SmControl);

	/// SETT1, SETT2
	enum class
	Sett : uint8_t
	{
		P_DET = Bit7,
		THR3_SA = Bit6,
		ABS = Bit5,

		THR3_MA = Bit2,
		R_TAM = Bit1,
		SITR = Bit0,
	};
	REGISTER8(Sett);

	REGISTER8_GROUP(Control,
			SmControl, Control3, Control4, Control5, Control6);

public:

	enum
	NR : uint8_t
	{
		// Next/reset conditions
		NOP = 0x0,		///< No operation

		TI1 = 0x1,		///< Timer 1 (16-bit value) valid
		TI2 = 0x2,		///< Timer 2 (16-bit value) valid
		TI3 = 0x3,		///< Timer 3 (8-bit value) valid
		TI4 = 0x4,		///< Timer 4 (8-bit value) valid

		GNTH1 = 0x5,	///< Any/triggered axis greater than THRS1
		GNTH2 = 0x6,	///< Any/triggered axis greater than THRS2

		LNTH1 = 0x7,	///< Any/triggered axis less than or equal to THRS1
		LNTH2 = 0x8,	///< Any/triggered axis less than or equal to THRS2

		GRTH1 = 0xB,	///< Any/triggered axis greater than reversed THRS1
		GRTH2 = 0xD,	///< Any/triggered axis greater than reversed THRS2

		LRTH1 = 0xC,	///< Any/triggered axis less than or equal to reversed THRS1
		LRTH2 = 0xE,	///< Any/triggered axis less than or equal to reversed THRS2

		GTTH1 = 0x9,	///< Any/triggered axis greater than THRS1 but using always standard axis mask (MASK1)
		LLTH2 = 0xA,	///< All axis less than or equal to THRS2
		NZERO = 0xF,	///< Any axis zero crossed
	};

	static constexpr NR
	next(NR cond) { return cond; }
	static constexpr NR
	reset(NR cond) { return NR(cond << 4); }

	enum
	CMD : uint8_t
	{
		// Commands main set
		STOP = 0x00,	///< Stops execution, and resets RESET- POINT to start
		CONT = 0x11,	///< Continues execution from RESET- POINT
		JMP = 0x22,		///< Conditional jump.
		SRP = 0x33,		///< Sets RESET-POINT to next step address
		CRP = 0x44,		///< Sets RESET-POINT to address 0 (start position)
		SETP = 0x55,	///< Sets parameter in register memory.
		SETS1 = 0x66,	///< Sets new value (1st parameter) to SETTy register
		STHR1 = 0x77,	///< Sets new value (1st parameter) to THRS1y register
		OUTC = 0x88,	///< Sets outputs to output registers
		OUTW = 0x99,	///< Sets outputs to output registers and waits for host actions for output latch release
		STHR2 = 0xAA,	///< Sets new value (1st parameter) to THRS2y register
		DEC = 0xBB,		///< Decreases long counter (LC) value and validate counter
		SISW = 0xCC,	///< Swaps temporary axis mask sign to opposite sign
		REL = 0xDD,		///< Releases temporary axis mask information
		STHR3 = 0xEE,	///< Sets new value (1st parameter) to THRS3y register
		SSYNC = 0xFF,	///< Toggles execution from one state machine to the other one

		// Commands extended set
		SABS0 = 0x12,	///< Sets ABS=0 in SETTy register (unsigned thrs)
		SABS1 = 0x13,	///< Sets ABS=1 in SETTy register (signed thrs)
		SELMA = 0x14,	///< Sets mask pointer to MASKy_A
		SRADI0 = 0x21,	///< Sets RADI=0 in SETT2 register (raw data mode)
		SRADI1 = 0x23,	///< Sets RADI=1 in SETT2 register (difference data mode)
		SELSA = 0x24,	///< Sets mask pointer to MASKy_B
		SCS0 = 0x31,	///< Sets D_CS=0 in SETT2 register (DIFF data form OFF for State Machine 2)
		SCS1 = 0x32,	///< Sets D_CS=1 in SETT2 register (Constant Shift data form ON for State Machine 2)
		SRTAM0 = 0x34,	///< Sets R_TAM=0 in SETTy register (Temporary Axis mask is kept intact)
		STIM3 = 0x41,	///< Sets a new value (1st parameter) to TIM3y register
		STIM4 = 0x42,	///< Sets a new value (1st parameter) to TIM4y register
		SRTAM1 = 0x43,	///< Sets R_TAM=1 in SETTy register (Temporary Axis mask is released to default after every valid NEXT condition)

		// Commands forbidden in SM1
		// SRADI0
		// SRADI1
		// SCS0
		// SCS1
	};

public:
	enum class
	FifoMode : uint8_t
	{
		Bypass = 0,
		Fifo = FifoControl::FMODE0,
		Stream = FifoControl::FMODE1,
		StreamTriggerFifo = int(FifoControl::FMODE1) | int(FifoControl::FMODE0),
		BypassTriggerStream = FifoControl::FMODE2,
		BypassTriggerFifo = int(FifoControl::FMODE2) | int(FifoControl::FMODE1) | int(FifoControl::FMODE0),
	};

	enum class
	MeasurementRate : uint8_t
	{
		Off = 0,
		Hz3_125 = Control4::ODR0,
		Hz6_25 = Control4::ODR1,
		Hz12_5 = int(Control4::ODR1) | int(Control4::ODR0),
		Hz25 = Control4::ODR2,
		Hz50 = int(Control4::ODR2) | int(Control4::ODR0),
		Hz100 = int(Control4::ODR2) | int(Control4::ODR1),
		Hz400 = int(Control4::ODR2) | int(Control4::ODR1) | int(Control4::ODR0),
		Hz800 = Control4::ODR3,
		Hz1600 = int(Control4::ODR3) | int(Control4::ODR0),
	};

	enum class
	Scale : uint8_t
	{
		G2 = 0,
		G4 = Control5::FSCALE0,
		G6 = Control5::FSCALE1,
		G8 = int(Control5::FSCALE1) | int(Control5::FSCALE0),
		G16 = Control5::FSCALE2,
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
		X = 2,
		Y = 1,
		Z = 0,
	};

public:
	struct ATTRIBUTE_PACKED
	Data
	{
		template < class Transport >
		friend class Lis3dsh;

		Data()
		:	data{0,0,0}, meta(2)
		{}
		// DATA ACCESS
		///@{
		/// returns the acceleration in g
		float inline
		getX() { return getData(0) * float(meta)/0x7FFF; }

		float inline
		getY() { return getData(1) * float(meta)/0x7FFF; }

		float inline
		getZ() { return getData(2) * float(meta)/0x7FFF; }
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
			return xpcc::fromLittleEndian(rData[index]);
		}

		// data 0-5 = xl,xh,yl,yh,zl,zh
		uint8_t data[6];
		// contains scale in g (5bit LSB)
		uint8_t meta;
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	static constexpr uint8_t
	i(FifoMode mode) { return uint8_t(mode); }
	static constexpr FifoControl
	r(FifoMode mode) { return FifoControl(mode); }
	static constexpr uint8_t
	i(MeasurementRate rate) { return uint8_t(rate); }
	static constexpr Control4
	r(MeasurementRate rate) { return Control4(rate); }
	static constexpr uint8_t
	i(Scale scale) { return uint8_t(scale); }
	static constexpr Control5
	r(Scale scale) { return Control5(scale); }
	static constexpr uint8_t
	i(Interrupt interrupt) { return uint8_t(interrupt); }
	/// @endcond
}; // struct lis3dsh

/**
 * LIS3DSH 3-Axis accelerometer.
 *
 * The LIS3DSH is an ultra low-power high performance three-axis linear accelerometer belonging to the
 * "nano" family with embedded state machine that can be programmed to implement autonomous applications.
 * The LIS3DSH has dynamically selectable full scales of ±2g/±4g/±6g/±8g/±16g and it is capable
 * of measuring accelerations with output data rates from 3.125 Hz to 1.6 kHz.
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 * @see Lis3TransportI2c
 * @see Lis3TransportSpi
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class Transport >
class Lis3dsh : public lis3dsh, public Transport
{
public:
	/// Constructor, requires a lis3dsh::Data object.
	/// For I2c this also sets the address to 0x1D (alternative: 0x1C).
	Lis3dsh(Data &data, uint8_t address=0x1D);

	bool inline
	initialize(Scale scale, MeasurementRate rate = MeasurementRate::Hz100)
	{
		return CO_CALL_BLOCKING(initialize(this, scale, rate));
	}

	xpcc::co::Result<bool>
	initialize(void *ctx, Scale scale, MeasurementRate rate = MeasurementRate::Hz100);

	// MARK: Control Registers
	xpcc::co::Result<bool> inline
	updateSmControl1(void *ctx, SmControl_t setMask, SmControl_t clearMask = SmControl_t(0xff))
	{
		return updateControlRegister(ctx, 1, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateSmControl2(void *ctx, SmControl_t setMask, SmControl_t clearMask = SmControl_t(0xff))
	{
		return updateControlRegister(ctx, 2, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControl(void *ctx, Control3_t setMask, Control3_t clearMask = Control3_t(0xff))
	{
		return updateControlRegister(ctx, 3, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControl(void *ctx, Control4_t setMask, Control4_t clearMask = Control4_t(0xff))
	{
		return updateControlRegister(ctx, 0, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControl(void *ctx, Control5_t setMask, Control5_t clearMask = Control5_t(0xff))
	{
		return updateControlRegister(ctx, 4, setMask, clearMask);
	}

	xpcc::co::Result<bool> inline
	updateControl(void *ctx, Control6_t setMask, Control6_t clearMask = Control6_t(0xff))
	{
		return updateControlRegister(ctx, 5, setMask, clearMask);
	}


	// MARK: Read access
	xpcc::co::Result<bool>
	readAcceleration(void *ctx);

	// instant access
	SmControl_t getControl1()
	{ return SmControl_t(rawBuffer[1]); }

	SmControl_t getControl2()
	{ return SmControl_t(rawBuffer[2]); }

	Control3_t getControl3()
	{ return Control3_t(rawBuffer[3]); }

	Control4_t getControl4()
	{ return Control4_t(rawBuffer[0]); }

	Control5_t getControl5()
	{ return Control5_t(rawBuffer[4]); }

	Control6_t getControl6()
	{ return Control6_t(rawBuffer[5]); }

	FifoControl_t getFifoControl()
	{ return FifoControl_t(rawBuffer[13]); }

	// buffered access
	Status_t getStatus()
	{ return Status_t(rawBuffer[6]); }

	FifoSource_t getFifoSource()
	{ return FifoSource_t(rawBuffer[14]); }

public:
	/// the data object for this sensor.
	Data &data;

private:
	xpcc::co::Result<bool>
	updateControlRegister(void *ctx, uint8_t index, Control_t setMask, Control_t clearMask = static_cast<Control_t>(0xff));

	xpcc::co::Result<bool>
	updateRegister(void *ctx, uint8_t reg, uint8_t setMask, uint8_t clearMask = 0xff);

	// the read buffer is for a continous read from address 0x20 -> 0x2F
	// 0: control 4
	// 1-3: control 1-3
	// 4-5: control 5-6
	// 6: status (read-only)
	// 7: out x low -- also use for readBuffer in updateRegister!
	// 8: out x high
	// 9: out y low
	// 10: out y high
	// 11: out z low
	// 12: out z high
	// 13: fifo control
	// 14: fifo source (read-only)
	uint8_t rawBuffer[15];
};

} // namespace xpcc

#include "lis3dsh_impl.hpp"

#endif	// XPCC_LIS3DSH_HPP
