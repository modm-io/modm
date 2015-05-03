// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ITG3200_HPP
#define XPCC_ITG3200_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/math/geometry/angle.hpp>

namespace xpcc
{

// forward declaration for friending with itg3200::Data
template < class I2cMaster >
class Itg3200;

struct itg3200
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		WHO_AM_I = 0x00,		///< I2C ID 0x68

		SMPLRT_DIV = 0x15,		///< Sample Rate Divider
		DLPF_FS = 0x16,			///< Digital Low Pass Filter and Full Scale
		INT_CFG = 0x17,			///< Interrupt Configuration

		INT_STATUS = 0x1A,		///< Interrupt Status

		TEMP_OUT_H = 0x1B,		///< Temperature Data MSB
		TEMP_OUT_L = 0x1C,		///< Temperature Data LSB

		GYRO_XOUT_H = 0x1D,		///< X-Axis Data MSB
		GYRO_XOUT_L = 0x1E,		///< X-Axis Data LSB
		GYRO_YOUT_H = 0x1F,		///< Y-Axis Data MSB
		GYRO_YOUT_L = 0x20,		///< Y-Axis Data LSB
		GYRO_ZOUT_H = 0x21,		///< Z-Axis Data MSB
		GYRO_ZOUT_L = 0x22,		///< Z-Axis Data LSB

		PWR_MGM = 0x3E,			///< Power Management
	};

	enum class
	Filter : uint8_t
	{
		FS_SEL1 = Bit4,
		FS_SEL0 = Bit3,
		FS_SEL_Mask = Bit4 | Bit3,
		FullScale = FS_SEL0,

		DLPF_CFG2 = Bit2,
		DLPF_CFG1 = Bit1,
		DLPF_CFG0 = Bit0,
		DLPF_CFG_Mask = Bit2 | Bit1 | Bit0,
	};
	/// @endcond

public:
	enum class
	LowPassFilter : uint8_t
	{
		Hz256 = 0,
		Hz188 = Filter::DLPF_CFG0,
		Hz98 = Filter::DLPF_CFG1,
		Hz42 = int(Filter::DLPF_CFG1) | int(Filter::DLPF_CFG0),
		Hz20 = Filter::DLPF_CFG2,
		Hz10 = int(Filter::DLPF_CFG2) | int(Filter::DLPF_CFG0),
		Hz5 = int(Filter::DLPF_CFG2) | int(Filter::DLPF_CFG1),
	};

protected:
	/// @cond
	XPCC_FLAGS8(Filter);
	typedef Configuration< Filter_t, LowPassFilter, (Bit2 | Bit1 | Bit0) > LowPassFilter_t;
	/// @endcond

public:
	/// The bit masks of the INT_CFG register
	enum class
	Interrupt : uint8_t
	{
		ACTL = Bit7,				///< Logic level for INT output pin: 1=active low, 0=active high
		OPEN = Bit6,				///< Drive type for INT output pin: 1=open drain, 0=push-pull
		LATCH_INT_EN = Bit5,		///< Latch mode: 1=latch until interrupt is cleared, 0=50us pulse
		INT_ANYRD_2CLEAR = Bit4,	///< Latch clear method: 1=any register read, 0=status register read only
		ITG_RDY_EN = Bit2,			///< Enable interrupt when device is ready (PLL ready after changing clock source)
		RAW_RDY_EN = Bit0,			///< Enable interrupt when data is available
	};
	XPCC_FLAGS8(Interrupt);

	/// The bit masks of the INT_STATUS register
	enum class
	Status : uint8_t
	{
		ITG_RDY = Bit2,			///< PLL ready
		RAW_DATA_RDY = Bit0,	///< Raw data is ready
	};
	XPCC_FLAGS8(Status);

	/// The bit masks of the PWR_MGM register
	enum class
	Power : uint8_t
	{
		H_RESET = Bit7,			///< Reset device and internal registers to the power-up-default settings
		SLEEP = Bit6,			///< Enable low power sleep mode

		STBY_XG = Bit5,			///< Put gyro X in standby mode (1=standby, 0=normal)
		STBY_YG = Bit4,			///< Put gyro Y in standby mode (1=standby, 0=normal)
		STBY_ZG = Bit3,			///< Put gyro Z in standby mode (1=standby, 0=normal)

		CLK_SEL2 = Bit2,
		CLK_SEL1 = Bit1,
		CLK_SEL0 = Bit0,
		CLK_SEL_Mask = Bit2 | Bit1 | Bit0,
	};
	XPCC_FLAGS8(Power);

	enum class
	ClockSource : uint8_t
	{
		Internal = 0,						///< Internal oscillator
		PllX = Power::CLK_SEL0,				///< PLL with X Gyro reference
		PllY = Power::CLK_SEL1,				///< PLL with Y Gyro reference
		PllZ = int(Power::CLK_SEL1) | int(Power::CLK_SEL0),		///< PLL with Z Gyro reference
		PllExternal32kHz = Power::CLK_SEL2,	///< PLL with external 32.768kHz reference
		PllExternal19MHz = int(Power::CLK_SEL2) | int(Power::CLK_SEL0),	///< PLL with external 19.2MHz reference
	};
	typedef Configuration< Power_t, ClockSource, (Bit2 | Bit1 | Bit0) > ClockSource_t;

public:
	struct ATTRIBUTE_PACKED
	Data
	{
		template< class I2cMaster >
		friend class Itg3200;

		/// returns the rotation rate in degrees per second
		///@{
		inline float
		getRotationRateX() { return getRate(1); }

		inline float
		getRotationRateY() { return getRate(2); }

		inline float
		getRotationRateZ() { return getRate(3); }
		///@}

		/// returns the temperature in degrees Celcius
		inline float
		getTemperature()
		{
			int16_t* rawData = reinterpret_cast<int16_t*>(data);
			int16_t rateValue = xpcc::fromBigEndian(rawData[0]) + 13200;
			return (rateValue / 280.f) + 35.f;
		}

		inline float
		operator [](uint8_t index)
		{ return (index < 3) ? getRate(index+1) : 0; }

	private:
		uint8_t data[8];

		/// index must be in range of [1, 3] !!!
		float inline
		getRate(uint8_t index)
		{
			int16_t* rawData = reinterpret_cast<int16_t*>(data);
			int16_t rateValue = xpcc::fromBigEndian(rawData[index]);
			return rateValue / 14.375f;
		}
	};
}; // struct itg3200

/**
 * Basic ITG3200 digital gyroscope sensor driver
 *
 * For further information on the special sensing functions, consult the
 * <a href="http://invensense.com/mems/gyro/documents/PS-ITG-3200-00-01.4.pdf">
 * datasheet</a>.
 *
 * @author	Niklas Hauser
 * @ingroup driver_inertial
 */
template < typename I2cMaster >
class Itg3200 : public itg3200, public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	/// Constructor, requires an itg3200::Data object, sets address to default of 0x68 (AD0 low: 0x69)
	Itg3200(Data &data, uint8_t address=0x68);


	xpcc::ResumableResult<bool>
	configure(LowPassFilter filter=LowPassFilter::Hz20, uint8_t divider=0);

	/// reads the temperature and gyro registers and buffer the results
	xpcc::ResumableResult<bool>
	readRotationRate();


	xpcc::ResumableResult<bool> inline
	setLowPassFilter(LowPassFilter filter)
	{ return updateFilter(filter, Filter::DLPF_CFG_Mask); }

	xpcc::ResumableResult<bool> inline
	setSampleRateDivider(uint8_t divider);


	xpcc::ResumableResult<bool> inline
	updateInterrupt(Interrupt_t setMask, Interrupt_t clearMask = Interrupt_t(0xf5))
	{ return updateRegister(1, setMask.value, clearMask.value); }

	xpcc::ResumableResult<bool> inline
	updatePower(Power_t setMask, Power_t clearMask = Power_t(0xff))
	{ return updateRegister(11, setMask.value, clearMask.value); }


	// MARK: Registers with instant access
	LowPassFilter getLowPassFilter()
	{ return LowPassFilter_t::get(getFilter()); }

	Interrupt_t getInterrupt()
	{ return Interrupt_t(rawBuffer[1]); }

	Power_t getPower()
	{ return Power_t(rawBuffer[11]); }

	// MARK: Registers with buffered access
	Status_t getStatus()
	{ return Status_t(rawBuffer[2]); }

	xpcc::ResumableResult<bool>
	readStatus();

protected:
	/// @cond
	xpcc::ResumableResult<bool> inline
	updateFilter(Filter_t setMask, Filter_t clearMask = Filter_t(0x1f))
	{ return updateRegister(0, setMask.value, clearMask.value); }

	Filter_t getFilter()
	{ return Filter_t(rawBuffer[0]); }
	/// @endcond

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }


protected:
	/// @cond
	/// write a 8bit value a register
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	write(Register reg, uint8_t &value)
	{ return write(reg, &value, 1); }

	/// write multiple 8bit values from a start register
	xpcc::ResumableResult<bool>
	write(Register reg, uint8_t *buffer, uint8_t length, bool copyBuffer=true);

	/// read a 8bit value from a register
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	read(Register reg, uint8_t &value)
	{ return read(reg, &value, 1); }

	/// read multiple 8bit values from a start register
	xpcc::ResumableResult<bool>
	read(Register reg, uint8_t *buffer, uint8_t length);

private:
	xpcc::ResumableResult<bool>
	updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask = 0xff);
	/// @endcond

protected:
	/// @cond
	Data &data;
	// the read buffer is for a continous read from address 0x00 -> 0x09
	// (x: SMPLRT_DIV is not buffered!)
	// 0: DLPF_FS
	// 1: INT_CFG
	//
	// 2: INT_STATUS
	// 3: TEMP_OUT_H  -- also used for write address of register
	// 4: TEMP_OUT_L -- also used for write buffer[0]
	// 5: GYRO_XOUT_H  -- also used for write buffer[1]
	// 6: GYRO_XOUT_L -- also used for write buffer[2]
	// 7: GYRO_YOUT_H
	// 8: GYRO_YOUT_L
	// 9: GYRO_ZOUT_H
	// 10: GYRO_ZOUT_L
	//
	// 11: PWR_MGM
	uint8_t rawBuffer[12];
	/// @endcond
};

}	// namespace xpcc

#include "itg3200_impl.hpp"

#endif // XPCC_ITG3200_HPP
