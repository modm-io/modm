/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2011-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HMC58X3_HPP
#define MODM_HMC58X3_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// forward declaration for friending with hmc58x3::Data
template < class I2cMaster >
class Hmc58x3;
template < class I2cMaster >
class Hmc5843;
template < class I2cMaster >
class Hmc5883;

struct hmc58x3
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		ConfigA = 0x00,
		ConfigB = 0x01,
		Mode = 0x02,
		DataX_Lsb = 0x03,
		DataX_Msb = 0x04,
		DataY_Lsb = 0x05,
		DataY_Msb = 0x06,
		DataZ_Lsb = 0x07,
		DataZ_Msb = 0x08,
		Status = 0x09,
		IdA = 0x10,
		IdB = 0x11,
		IdC = 0x12,
	};
	/// @endcond

public:
	/// Configuration Register A
	enum class
	ConfigA : uint8_t
	{
		MA1 = Bit6,				///< *HMC5883L only*
		MA0 = Bit5,				///< *HMC5883L only*
		MA_Mask = Bit6 | Bit5,	///< *HMC5883L only*

		DO2 = Bit4,
		DO1 = Bit3,
		DO0 = Bit2,
		DO_Mask = Bit4 | Bit3 | Bit2,

		MS1 = Bit1,
		MS0 = Bit0,
		MS_Mask = Bit1 | Bit0,
	};
	MODM_FLAGS8(ConfigA);

	/// Configuration Register B
	enum class
	ConfigB : uint8_t
	{
		GN2 = Bit7,
		GN1 = Bit6,
		GN0 = Bit5,
		GN_Mask = Bit7 | Bit6 | Bit5,
	};
	MODM_FLAGS8(ConfigB);

	/// Mode Register
	enum class
	Mode : uint8_t
	{
		MD1 = Bit1,
		MD0 = Bit0,
		MD_Mask = Bit1 | Bit0,
	};
	MODM_FLAGS8(Mode);

	/// Status Register
	enum class
	Status : uint8_t
	{
		REN = Bit2,		///< Internal voltage regulator enabled, *HMC5843 only*
		LOCK = Bit1,	///< This bit is set when some but not all for of the six data output registers have been read.
		RDY = Bit0,		///< Set when data is written to all six data registers
	};
	MODM_FLAGS8(Status);

public:
	/// Operation modes
	enum class
	OperationMode : uint8_t
	{
		ContinousConversion = 0,
		SingleConversion = int(Mode::MD0),
		Idle = int(Mode::MD1),
		Sleep = int(Mode::MD1) | int(Mode::MD0),	///< *HMC5843 only*
	};

public:
	struct modm_packed
	Data
	{
		template < class I2cMaster >
		friend class Hmc58x3;
		template < class I2cMaster >
		friend class Hmc5843;
		template < class I2cMaster >
		friend class Hmc5883;

		/// returns the magnetic field in Gauss
		///@{
		inline float
		getMagneticFieldX() { return getField(0); }

		inline float
		getMagneticFieldY() { return getField(1); }

		inline float
		getMagneticFieldZ() { return getField(2); }
		///@}

		/// returns `true` if the readings under- or overflowed
		///@{
		inline bool
		isOverflowX() { return isOverflow(0); }

		inline bool
		isOverflowY() { return isOverflow(1); }

		inline bool
		isOverflowZ() { return isOverflow(2); }
		///@}

		/// returns the current gain in Gauss
		inline float
		getGain() { return gain / 30.f; };

		inline float
		operator [](uint8_t index)
		{ return (index < 3) ? getField(index) : 0; }

	private:
		uint8_t data[6];
		uint8_t gain;

		inline bool
		isOverflow(uint8_t index)
		{
			int16_t* rawData = reinterpret_cast<int16_t*>(data);
			int16_t fieldValue = modm::fromBigEndian(rawData[index]);
			return (fieldValue == -4096);
		}

		float inline
		getField(uint8_t index)
		{
			int16_t* rawData = reinterpret_cast<int16_t*>(data);
			int16_t fieldValue = modm::fromBigEndian(rawData[index]);
			return (fieldValue / 2048.f) * getGain();
		}
	};
}; // struct hmc58x3

/**
 * HMC58x3 3-axis digital magnetometer family driver.
 *
 * The HMC58x3 is a surface-mount, multi-chip module designed for low-field
 * magnetic sensing with a digital interface for applications such as
 * low-cost compassing and magnetometry. The HMC58x3 includes high-resolution
 * magneto-resistive sensors plus an ASIC containing amplification,
 * automatic degaussing strap drivers, offset cancellation, and a 12-bit
 * ADC that enables 1-2 degrees compass heading accuracy.
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class Hmc58x3 : public hmc58x3, public modm::I2cDevice< I2cMaster, 2 >
{
protected:
	/// Constructor, requires a hmc58x3::Data object, sets address to default of 0x1e
	Hmc58x3(Data &data, uint8_t address=0x1e);

public:
	modm::ResumableResult<bool> inline
	setOperationMode(OperationMode mode)
	{ return updateMode(Mode_t(uint8_t(mode)), Mode::MD_Mask); }


	// MARK: Read access
	modm::ResumableResult<bool>
	readMagneticField();


protected:
	/// @cond
	modm::ResumableResult<bool>
	configureRaw(uint8_t rate, uint8_t gain, const uint8_t* gainValues, uint8_t average=0);

	modm::ResumableResult<bool>
	setGainRaw(uint8_t gain, const uint8_t* gainValues);
	/// @endcond


protected:
	// MARK: Control Registers
	modm::ResumableResult<bool> inline
	updateConfigA(ConfigA_t setMask, ConfigA_t clearMask = ConfigA_t(0x7f))
	{ return updateRegister(0, setMask.value, clearMask.value); }

	modm::ResumableResult<bool> inline
	updateConfigB(ConfigB_t setMask, ConfigB_t clearMask = ConfigB_t(0xe0))
	{ return updateRegister(1, setMask.value, clearMask.value); }

	modm::ResumableResult<bool> inline
	updateMode(Mode_t setMask, Mode_t clearMask = Mode::MD_Mask)
	{ return updateRegister(2, setMask.value, clearMask.value); }


	// MARK: Registers with instant access
	ConfigA_t getConfigA()
	{ return ConfigA_t(rawBuffer[0]); }

	ConfigB_t getConfigB()
	{ return ConfigB_t(rawBuffer[1]); }

	Mode_t getMode()
	{ return Mode_t(rawBuffer[2]); }

public:
	// MARK: Registers with buffered access
	Status_t getStatus()
	{ return Status_t(rawBuffer[9]); }

	modm::ResumableResult<bool>
	readStatus();

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

protected:
	/// @cond
	/// write a 8bit value a register
	modm::ResumableResult<bool> modm_always_inline
	write(Register reg, uint8_t &value)
	{ return write(reg, &value, 1); }

	/// write multiple 8bit values from a start register
	modm::ResumableResult<bool>
	write(Register reg, uint8_t *buffer, uint8_t length);

	/// read a 8bit value from a register
	modm::ResumableResult<bool> modm_always_inline
	read(Register reg, uint8_t &value)
	{ return read(reg, &value, 1); }

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
	read(Register reg, uint8_t *buffer, uint8_t length);

private:
	modm::ResumableResult<bool>
	updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask = 0xff);
	/// @endcond

protected:
	/// @cond
	Data &data;

	// the read buffer is for a continous read from address 0x00 -> 0x09
	// 0: config A
	// 1: config B
	// 2: mode
	// 3: out x low  -- also used for write address of register
	// 4: out x high -- also used for write buffer[0]
	// 5: out y low  -- also used for write buffer[1]
	// 6: out y high -- also used for write buffer[2]
	// 7: out z low
	// 8: out z high
	// 9: status
	uint8_t rawBuffer[10];
	/// @endcond
};

}

#include "hmc58x3_impl.hpp"

#endif // MODM_HMC58X3_HPP
