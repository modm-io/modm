// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC6343_HPP
#define XPCC_HMC6343_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>

namespace xpcc
{

// forward declaration for friending with hmc6343::Data
template < class I2cMaster >
class Hmc6343;

struct hmc6343
{
	enum class
	Register : uint8_t
	{
		SlaveAddress = 0x00,		///< I2C Slave Address
		SoftwareVersion = 0x02,		///< Software Version Number
		OperationMode1 = 0x04,		///< Operation Mode Register 1
		OperationMode2 = 0x05,		///< Operation Mode Register 2
		/// @cond
		DeviceSerialLsb = 0x06,		///< Device Serial Number
		DeviceSerialMsb = 0x07,		///< Device Serial Number
		/// @endcond
		DateCodeYear = 0x08,		///< Package Date Code: Last two digits of the year
		DateCodeWeek = 0x09,		///< Package Date Code: Fiscal Week
		/// @cond
		DeviationAngleLsb = 0x0A,	///< Deviation Angle (+-1800) in tenth of a degree
		DeviationAngleMsb = 0x0B,	///< Deviation Angle (+-1800) in tenth of a degree
		VariationAngleLsb = 0x0C,	///< Variation Angle (+-1800) in tenth of a degree
		VariationAngleMsb = 0x0D,	///< Variation Angle (+-1800) in tenth of a degree
		X_OffsetLsb = 0x0E,			///< Hard-Iron Calibration Offset for the X-axis
		X_OffsetMsb = 0x0F,			///< Hard-Iron Calibration Offset for the X-axis
		Y_OffsetLsb = 0x10,			///< Hard-Iron Calibration Offset for the Y-axis
		Y_OffsetMsb = 0x11,			///< Hard-Iron Calibration Offset for the Y-axis
		Z_OffsetLsb = 0x12,			///< Hard-Iron Calibration Offset for the Z-axis
		Z_OffsetMsb = 0x13,			///< Hard-Iron Calibration Offset for the Z-axis
		/// @endcond
		Filter = 0x14,				///< Heading IIR Filter (0x00 to 0x0F typical)
	};

	enum class
	Register16 : uint8_t
	{
		DeviceSerial = Register::DeviceSerialLsb,		///< Device Serial Number
		DeviationAngle = Register::DeviationAngleLsb,	///< Deviation Angle (+-1800) in tenth of a degree
		VariationAngle = Register::VariationAngleLsb,	///< Variation Angle (+-1800) in tenth of a degree
		X_Offset = Register::X_OffsetLsb,				///< Hard-Iron Calibration Offset for the X-axis
		Y_Offset = Register::Y_OffsetLsb,				///< Hard-Iron Calibration Offset for the Y-axis
		Z_Offset = Register::Z_OffsetLsb,				///< Hard-Iron Calibration Offset for the Z-axis
	};

protected:
	/// @cond
	enum class
	Command : uint8_t
	{
		PostAccelData = 0x40,		///< Post Accel Data. AxMSB, AxLSB, AyMSB, AyLSB, AzMSB, AzLSB
		PostMagData = 0x45,			///< Post Mag Data. MxMSB, MxLSB, MyMSB, MyLSB, MzMSB, MzLSB
		PostHeadingData = 0x50,		///< Post Heading Data. HeadMSB, HeadLSB, PitchMSB, PitchLSB, RollMSB, RollLSB
		PostTiltData = 0x55,		///< Post Tilt Data. PitchMSB, PitchLSB, RollMSB, RollLSB, TempMSB, TempLSB
		PostOperationMode = 0x65,
		EnterUserCalibrationMode = 0x71,
		LevelOrientation = 0x72,
		UprightSidewaysOrientation = 0x73,
		UprightFlatFrontOrientation = 0x74,
		EnterRunMode = 0x75,
		EnterStandbyMode = 0x76,
		ExitUserCalibrationMode = 0x7E,
		ResetProcessor = 0x82,
		EnterSleepMode = 0x83,
		ExitSleepMode = 0x84,
		ReadEeprom = 0xE1,
		WriteEeprom = 0xF1,
	};
	/// @endcond

public:
	enum class
	Orientation : uint8_t
	{
		Level = Command::LevelOrientation,
		UprightSideways = Command::UprightSidewaysOrientation,
		UprightFlatFront = Command::UprightFlatFrontOrientation
	};

	enum class
	OperationMode : uint8_t
	{
		Comp = Bit7,					///< Calculating compass data if set. (read only)
		Cal = Bit6,						///< Calculating calibration offsets if set. (read only)
		Filter = Bit5,					///< IIR Heading Filter used if set.
		Run = Bit4,						///< Run Mode if set.
		Standby = Bit3,					///< Standby Mode if set.
		UprightFrontOrientation = Bit2,	///< Upright Front Orientation if set.
		UprightEdgeOrientation = Bit1,	///< Upright Edge Orientation if set.
		LevelOrientation = Bit0,		///< Level Orientation if set
	};
	XPCC_FLAGS8(OperationMode);

	enum class
	MeasurementRate : uint8_t
	{
		Hz1 = 0x00,
		Hz5 = 0x01,
		Hz10 = 0x02,
	};

	struct ATTRIBUTE_PACKED
	Data
	{
		template < class I2cMaster >
		friend class Hmc6343;

		// DATA ACCESS
		/// returns the acceleration in unknown units
		///@{
		int16_t ALWAYS_INLINE
		getAccelerationX() { return swapData(0); }

		int16_t ALWAYS_INLINE
		getAccelerationY() { return swapData(1); }

		int16_t ALWAYS_INLINE
		getAccelerationZ() { return swapData(2); }
		///@}

		/// returns the magnetic field in unknown units
		///@{
		int16_t ALWAYS_INLINE
		getMagneticFieldX() { return swapData(3); }

		int16_t ALWAYS_INLINE
		getMagneticFieldY() { return swapData(4); }

		int16_t ALWAYS_INLINE
		getMagneticFieldZ() { return swapData(5); }
		///@}

		/// returns the heading in tenth of degrees (0 -> 3600)
		int16_t ALWAYS_INLINE
		getHeading() { return swapData(6); }

		/// returns the Pitch in tenth of degrees (-900 -> 0 -> 900)
		int16_t ALWAYS_INLINE
		getPitch() { return swapData(7); }

		/// returns the Roll in tenth of degrees (-900 -> 0 -> 900)
		int16_t ALWAYS_INLINE
		getRoll() { return swapData(8); }

		/// returns the temperature in unknown format (was not specified in datasheet)
		int16_t ALWAYS_INLINE
		getTemperature() { return swapData(9); }

		/// returns the value of the operation mode register
		OperationMode_t ALWAYS_INLINE
		getOperationMode() { return OperationMode_t(data[20]); }


		inline int16_t
		operator [](uint8_t index)
		{ return (index < 10) ? swapData(index) : 0; }

	private:
		uint8_t data[21];

		int16_t inline
		swapData(uint8_t index)
		{
			int16_t* rawData = reinterpret_cast<int16_t*>(data);
			return xpcc::fromBigEndian(rawData[index]);
		}
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Command command) { return uint8_t(command); }
	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	static constexpr uint8_t
	i(Register16 reg) { return uint8_t(reg); }
	static constexpr uint8_t
	i(Orientation orientation) { return uint8_t(orientation); }
	static constexpr uint8_t
	i(MeasurementRate rate) { return uint8_t(rate); }
	/// @endcond
}; // struct hmc6343

/**
 * HMC6343 3-Axis Compass with algorithms driver.
 *
 * The Honeywell HMC6343 is a fully integrated compass module that
 * includes firmware for heading computation and calibration for
 * magnetic distortions. The module combines 3-axis magneto-resistive
 * sensors and 3-axis MEMS accelerometers, analog and digital
 * support circuits, microprocessor and algorithms required for
 * heading computation.
 *
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Hmc6343 : public hmc6343, public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	/// Constructor, requires a hmc6343::Data object, sets address to default of 0x19
	Hmc6343(Data &data, uint8_t address=0x19);


	// READING RAM
	/// read operation mode register 2
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	readOperationMode()
	{ return readPostData(Command::PostOperationMode, 20, 1); }



	// WRITING EEPROM
	/// Configures the sensor to normal orientation mode with 10Hz data rate.
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	setMeasurementRate(MeasurementRate measurementRate=MeasurementRate::Hz10)
	{ return writeRegister(Register::OperationMode2, i(measurementRate)); }

	/// sets a new deviation angle in eeprom
	xpcc::ResumableResult<bool> inline
	setDeviationAngle(int16_t angle)
	{ return writeRegister(Register16::DeviationAngle, static_cast<uint16_t>(angle)); }

	/// sets a new variation angle in eeprom
	xpcc::ResumableResult<bool> inline
	setVariationAngle(int16_t angle)
	{ return writeRegister(Register16::VariationAngle, static_cast<uint16_t>(angle)); }

	/// sets a new IIR filter in eeprom
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	setIIR_Filter(uint8_t filter)
	{ return writeRegister(Register::Filter, filter & 0x0f); }



	// READING EEPROM
	/// reads the device id from eeprom
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	getDeviceId(uint16_t &value)
	{ return readRegister(Register16::DeviceSerial, value); }

	/// sets a new IIR filter in eeprom
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	getIIR_Filter(uint8_t &value)
	{ return readRegister(Register::Filter, value); }



	// COMMANDS
	/// Sets the specified orientation
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	setOrientation(Orientation orientation)
	{ return writeCommand(static_cast<Command>(orientation)); }

	/// enters run mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	enterRunMode()
	{ return writeCommand(Command::EnterRunMode); }

	/// enters standby mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	enterStandbyMode()
	{ return writeCommand(Command::EnterStandbyMode); }

	/// enters sleep mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	enterSleepMode()
	{ return writeCommand(Command::EnterSleepMode); }

	/// exit sleep mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	exitSleepMode()
	{ return writeCommand(Command::ExitSleepMode, 20); }

	/// enters user calibration mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	enterUserCalibrationMode()
	{ return writeCommand(Command::EnterUserCalibrationMode); }

	/// exit user calibration mode
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	exitUserCalibrationMode()
	{ return writeCommand(Command::ExitUserCalibrationMode, 50); }

	/// resets the processor, any new command is delayed by 500ms
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	resetProcessor()
	{ return writeCommand(Command::ResetProcessor, 500); }



	// DATA REQUESTS
	/// reads the Acceleration registers and buffer the results
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	readAcceleration()
	{ return readPostData(Command::PostAccelData, 0, 6); }

	/// reads the Magnetometer registers and buffer the results
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	readMagneticField()
	{ return readPostData(Command::PostMagData, 6, 6); }

	/// reads the Heading registers and buffer the results
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	readHeading()
	{ return readPostData(Command::PostHeadingData, 12, 6); }

	/// reads the Tilt registers and buffer the results
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	readTilt()
	{ return readPostData(Command::PostTiltData, 14, 6); }


protected:
	/// Use these methods with caution!
	/// @{

	// RAW REGISTER ACCESS
	/// write a 8bit value into the eeprom
	xpcc::ResumableResult<bool>
	writeRegister(Register reg, uint8_t value);

	/// write a 16bit value into the eeprom
	xpcc::ResumableResult<bool>
	writeRegister(Register16 reg, uint16_t value);

	/// read a 8bit value from the eeprom
	xpcc::ResumableResult<bool>
	readRegister(Register reg, uint8_t &value);

	/// read a 16bit value from the eeprom
	xpcc::ResumableResult<bool>
	readRegister(Register16 reg, uint16_t &value);

	/// @}

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	xpcc::ResumableResult<bool>
	writeCommand(Command command, uint16_t timeout = 1);

	xpcc::ResumableResult<bool>
	readPostData(Command command, uint8_t offset, uint8_t readSize);

private:
	Data &data;
	uint8_t buffer[3];
	xpcc::ShortTimeout timeout;
};

} // namespace xpcc

#include "hmc6343_impl.hpp"

#endif	// XPCC_HMC6343_HPP
