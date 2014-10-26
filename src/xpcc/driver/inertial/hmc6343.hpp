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

#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>

namespace xpcc
{

struct hmc6343
{
	enum class
	Register : uint8_t
	{
		SlaveAddress = 0x00,		//!< I2C Slave Address
		SoftwareVersion = 0x02,		//!< Software Version Number
		OperationMode1 = 0x04,		//!< Operation Mode Register 1
		OperationMode2 = 0x05,		//!< Operation Mode Register 2
		DeviceSerialLsb = 0x06,		//!< Device Serial Number
		DeviceSerialMsb = 0x07,		//!< Device Serial Number
		DateCodeYear = 0x08,		//!< Package Date Code: Last two digits of the year
		DateCodeWeek = 0x09,		//!< Package Date Code: Fiscal Week
		DeviationAngleLsb = 0x0A,	//!< Deviation Angle (+-1800) in tenth of a degree
		DeviationAngleMsb = 0x0B,	//!< Deviation Angle (+-1800) in tenth of a degree
		VariationAngleLsb = 0x0C,	//!< Variation Angle (+-1800) in tenth of a degree
		VariationAngleMsb = 0x0D,	//!< Variation Angle (+-1800) in tenth of a degree
		X_OffsetLsb = 0x0E,			//!< Hard-Iron Calibration Offset for the X-axis
		X_OffsetMsb = 0x0F,			//!< Hard-Iron Calibration Offset for the X-axis
		Y_OffsetLsb = 0x10,			//!< Hard-Iron Calibration Offset for the Y-axis
		Y_OffsetMsb = 0x11,			//!< Hard-Iron Calibration Offset for the Y-axis
		Z_OffsetLsb = 0x12,			//!< Hard-Iron Calibration Offset for the Z-axis
		Z_OffsetMsb = 0x13,			//!< Hard-Iron Calibration Offset for the Z-axis
		FilterLsb = 0x14,			//!< Heading IIR Filter (0x00 to 0x0F typical)
		FilterMsb = 0x15,			//!< Heading IIR Filter (set at zero)
	};

	enum class
	Register16 : uint8_t
	{
		DeviceSerial = Register::DeviceSerialLsb,		//!< Device Serial Number
		DeviationAngle = Register::DeviationAngleLsb,	//!< Deviation Angle (+-1800) in tenth of a degree
		VariationAngle = Register::VariationAngleLsb,	//!< Variation Angle (+-1800) in tenth of a degree
		X_Offset = Register::X_OffsetLsb,				//!< Hard-Iron Calibration Offset for the X-axis
		Y_Offset = Register::Y_OffsetLsb,				//!< Hard-Iron Calibration Offset for the Y-axis
		Z_Offset = Register::Z_OffsetLsb,				//!< Hard-Iron Calibration Offset for the Z-axis
	};

protected:
	enum class
	Command : uint8_t
	{
		PostAccelData = 0x40,		//!< Post Accel Data. AxMSB, AxLSB, AyMSB, AyLSB, AzMSB, AzLSB
		PostMagData = 0x45,			//!< Post Mag Data. MxMSB, MxLSB, MyMSB, MyLSB, MzMSB, MzLSB
		PostHeadingData = 0x50,		//!< Post Heading Data. HeadMSB, HeadLSB, PitchMSB, PitchLSB, RollMSB, RollLSB
		PostTiltData = 0x55,		//!< Post Tilt Data. PitchMSB, PitchLSB, RollMSB, RollLSB, TempMSB, TempLSB
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

public:
	enum class
	Orientation : uint8_t
	{
		Level = Command::LevelOrientation,
		UprightSideways = Command::UprightSidewaysOrientation,
		UprightFlatFront = Command::UprightFlatFrontOrientation
	};

	struct OperationMode
	{
		enum
		{
			Comp = 0x80,					//!< Calculating compass data if set. (read only)
			Cal = 0x40,						//!< Calculating calibration offsets if set. (read only)
			Filter = 0x20,					//!< IIR Heading Filter used if set.
			Run = 0x10,						//!< Run Mode if set.
			Standby = 0x08,					//!< Standby Mode if set.
			UprightFrontOrientation = 0x04,	//!< Upright Front Orientation if set.
			UprightEdgeOrientation = 0x02,	//!< Upright Edge Orientation if set.
			LevelOrientation = 0x01,		//!< Level Orientation if set
		};
	};

	enum class
	MeasurementRate : uint8_t
	{
		Hz1 = 0x00,
		Hz5 = 0x01,
		Hz10 = 0x02,
	};

	struct Data
	{
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
		uint8_t ALWAYS_INLINE
		getOperationMode() { return data[20]; }


		ALWAYS_INLINE uint8_t
		operator [](size_t index)
		{ return (index < 21) ? data[index] : 0; }

		ALWAYS_INLINE uint8_t*
		getPointer()
		{ return data; }

	private:
		uint8_t data[21];

		int16_t inline
		swapData(uint8_t index)
		{
			uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
			return static_cast<int16_t>(xpcc::math::bigEndianToHost(rawData[index]));
		}
	};

protected:
	/// @{
	/// @private enum class to integer helper functions.
	static constexpr uint8_t
	i(Command command) { return static_cast<uint8_t>(command); }
	static constexpr uint8_t
	i(Register reg) { return static_cast<uint8_t>(reg); }
	static constexpr uint8_t
	i(Register16 reg) { return static_cast<uint8_t>(reg); }
	static constexpr uint8_t
	i(Orientation orientation) { return static_cast<uint8_t>(orientation); }
	static constexpr uint8_t
	i(MeasurementRate rate) { return static_cast<uint8_t>(rate); }
	/// @}
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
 * @ingroup inertial
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Hmc6343 : public hmc6343, public xpcc::I2cDevice< I2cMaster >, public xpcc::co::NestedCoroutine<1>
{
public:
	/// \brief	Constructor, requires pointer to 21 byte array, sets address to default of 0x19
	Hmc6343(Data &data, uint8_t address=0x19);

	/// pings the sensor
	xpcc::co::Result<bool>
	ping(void *ctx);



	// READING RAM
	/// read operation mode register 2
	xpcc::co::Result<bool> ALWAYS_INLINE
	readOperationMode(void *ctx)
	{ return readPostData(ctx, Command::PostOperationMode, 20, 1); }



	// WRITING EEPROM
	/// Configures the sensor to normal orientation mode with 10Hz data rate.
	xpcc::co::Result<bool> ALWAYS_INLINE
	setMeasurmentRate(void *ctx, MeasurementRate measurementRate=MeasurementRate::Hz10)
	{ return writeRegister(ctx, Register::OperationMode2, i(measurementRate)); }

	/// sets a new deviation angle in eeprom
	xpcc::co::Result<bool> inline
	setDeviationAngle(void *ctx, int16_t angle)
	{ return writeRegister(ctx, Register16::DeviationAngle, static_cast<uint16_t>(angle)); }

	/// sets a new variation angle in eeprom
	xpcc::co::Result<bool> inline
	setVariationAngle(void *ctx, int16_t angle)
	{ return writeRegister(ctx, Register16::VariationAngle, static_cast<uint16_t>(angle)); }

	/// sets a new IIR filter in eeprom
	xpcc::co::Result<bool> ALWAYS_INLINE
	setIIR_Filter(void *ctx, uint8_t filter)
	{ return writeRegister(ctx, Register::FilterLsb, filter & 0x0f); }



	// READING EEPROM
	/// reads the device id from eeprom
	xpcc::co::Result<bool> ALWAYS_INLINE
	readDeviceId(void *ctx, uint16_t &value)
	{ return readRegister(ctx, Register16::DeviceSerial, value); }



	// COMMANDS
	/// Sets the specified orientation
	xpcc::co::Result<bool> ALWAYS_INLINE
	setOrientation(void *ctx, Orientation orientation)
	{ return writeCommand(ctx, static_cast<Command>(orientation)); }

	/// enters run mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	enterRunMode(void *ctx)
	{ return writeCommand(ctx, Command::EnterRunMode); }

	/// enters standby mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	enterStandbyMode(void *ctx)
	{ return writeCommand(ctx, Command::EnterStandbyMode); }

	/// enters sleep mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	enterSleepMode(void *ctx)
	{ return writeCommand(ctx, Command::EnterSleepMode); }

	/// exit sleep mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	exitSleepMode(void *ctx)
	{ return writeCommand(ctx, Command::ExitSleepMode); }

	/// enters user calibration mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	enterUserCalibrationMode(void *ctx)
	{ return writeCommand(ctx, Command::EnterUserCalibrationMode); }

	/// exit user calibration mode
	xpcc::co::Result<bool> ALWAYS_INLINE
	exitUserCalibrationMode(void *ctx)
	{ return writeCommand(ctx, Command::ExitUserCalibrationMode, 50); }

	/// resets the processor, any new command is delayed by 500ms
	xpcc::co::Result<bool> ALWAYS_INLINE
	resetProcessor(void *ctx)
	{ return writeCommand(ctx, Command::ResetProcessor, 500); }



	// DATA REQUESTS
	/// reads the Acceleration registers and buffer the results
	xpcc::co::Result<bool> ALWAYS_INLINE
	readAcceleration(void *ctx)
	{ return readPostData(ctx, Command::PostAccelData, 0, 6); }

	/// reads the Magnetometer registers and buffer the results
	xpcc::co::Result<bool> ALWAYS_INLINE
	readMagneticField(void *ctx)
	{ return readPostData(ctx, Command::PostMagData, 6, 6); }

	/// reads the Heading registers and buffer the results
	xpcc::co::Result<bool> ALWAYS_INLINE
	readHeading(void *ctx)
	{ return readPostData(ctx, Command::PostHeadingData, 12, 6); }

	/// reads the Tilt registers and buffer the results
	xpcc::co::Result<bool> ALWAYS_INLINE
	readTilt(void *ctx)
	{ return readPostData(ctx, Command::PostTiltData, 14, 6); }



	// RAW REGISTER ACCESS
	/// write a 8bit value into the eeprom
	xpcc::co::Result<bool>
	writeRegister(void *ctx, Register reg, uint8_t value);

	/// write a 16bit value into the eeprom
	xpcc::co::Result<bool>
	writeRegister(void *ctx, Register16 reg, uint16_t value);

	/// read a 8bit value from the eeprom
	xpcc::co::Result<bool>
	readRegister(void *ctx, Register reg, uint8_t &value);

	/// read a 16bit value from the eeprom
	xpcc::co::Result<bool>
	readRegister(void *ctx, Register16 reg, uint16_t &value);


public:
	Data &data;

private:
	xpcc::co::Result<bool>
	writeCommand(void *ctx, Command command, uint16_t timeout = 1);

	xpcc::co::Result<bool>
	readPostData(void *ctx, Command command, uint8_t offset, uint8_t readSize);

	struct I2cTask
	{
		enum
		{
			Idle = 0,

			// insert all registers from 0x00 to 0x15 for writing (+0x01)
			WriteEepromBase = 0x01,
			// insert all registers from 0x00 to 0x15 for posting (+0x17)
			PostEepromBase = 0x17,
			// insert all registers from 0x00 to 0x15 for reading (+0x2C)
			ReadEepromBase = 0x2C,

			// insert all post commands from 0x40 to 0x65 for writing (+0x02)
			PostCommandBase = 0x02,
			// insert all post commands from 0x40 to 0x65 for reading (+0x03)
			ReadCommandBase = 0x03,
			// insert all remaining commands from 0x71 to 0xF1 (+0x02)
			WriteCommandBase = PostCommandBase,

			// ping \o/
			Ping = 0xFF
		};
	};

private:
	uint8_t buffer[3];
	xpcc::Timeout<> timeout;

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter<xpcc::I2cWriteReadAdapter> adapter;
};

} // namespace xpcc

#include "hmc6343_impl.hpp"

#endif	// XPCC_HMC6343_HPP
