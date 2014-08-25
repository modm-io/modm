// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HMC6343_HPP
#define XPCC__HMC6343_HPP

#include <xpcc/architecture/peripheral/i2c_adapter.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#import "i2c_transaction.hpp"
#import "i2c.hpp"

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
		VariationAngle = Register::VariationAngleLsb ,	//!< Variation Angle (+-1800) in tenth of a degree
		X_Offset = Register::X_OffsetLsb,				//!< Hard-Iron Calibration Offset for the X-axis
		Y_Offset = Register::Y_OffsetLsb,				//!< Hard-Iron Calibration Offset for the Y-axis
		Z_Offset = Register::Z_OffsetLsb,				//!< Hard-Iron Calibration Offset for the Z-axis
	};

	enum class
	Command : uint8_t
	{
		PostAccelData = 0x40,		//!< Post Accel Data. AxMSB, AxLSB, AyMSB, AyLSB, AzMSB, AzLSB
		PostMagData = 0x45,			//!< Post Mag Data. MxMSB, MxLSB, MyMSB, MyLSB, MzMSB, MzLSB
		PostHeadingData = 0x50,		//!< Post Heading Data. HeadMSB, HeadLSB, PitchMSB, PitchLSB, RollMSB, RollLSB
		PostTiltData = 0x55,		//!< Post Tilt Data. PitchMSB, PitchLSB, RollMSB, RollLSB, TempMSB, TempLSB
		PostOperationMode = 0x65,
		EnterUserCalibration = 0x71,
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

	enum class
	Orientation : uint8_t
	{
		Level = 0x72,
		UprightSideways = 0x73,
		UprightFlatFront = 0x74
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
} // namespace hmc6343

/**
 * \brief	HMC6343 3-Axis Compass with algorithms driver.
 *
 * The Honeywell HMC6343 is a fully integrated compass module that
 * includes firmware for heading computation and calibration for
 * magnetic distortions. The module combines 3-axis magneto-resistive
 * sensors and 3-axis MEMS accelerometers, analog and digital
 * support circuits, microprocessor and algorithms required for
 * heading computation.
 *
 * \ingroup inertial
 * \author	Niklas Hauser
 *
 * \tparam I2cMaster Asynchronous Two Wire interface
 */
template < typename I2cMaster >
class Hmc6343 : public xpcc::I2cDevice< I2cMaster >, public hmc6343, private xpcc::pt::NestedProtoface<1>
{
public:
	/// \brief	Constructor, requires pointer to 21 byte array, sets address to default of 0x19
	Hmc6343(uint8_t* data, uint8_t address=0x19);

	/**
	 * @return pointer to 8bit array containing Axyz Mxyz Heading Pitch Roll Temperature.
	 * Be aware that the array is in BIG ENDIAN format, so you cannot
	 * simply reinterpret the result as int16_t!!
	 */
	ALWAYS_INLINE uint8_t*
	getData() { return data; }

	// MARK: Tasks
	/// pings the sensor
	bool
	startPing();

	bool ALWAYS_INLINE const
	runPing();

	bool ALWAYS_INLINE const
	isPingSuccessful();



	// READING RAM
	bool ALWAYS_INLINE
	startReadOperationMode()
	{ return startReadPostData(Command::PostOperationMode); }

	bool ALWAYS_INLINE
	runReadOperationMode()
	{ return runReadPostData(Command::PostOperationMode, 20, 1); }

	bool ALWAYS_INLINE
	isReadOperationModeSuccessful()
	{ return isReadPostDataSuccessful(Command::PostOperationMode); }

	uint8_t ALWAYS_INLINE
	getOperationMode() { return data[20]; }



	// WRITING EEPROM
	/// Configures the sensor to normal orientation mode with 10Hz data rate.
	bool ALWAYS_INLINE
	startSetMeasurmentRate(MeasurementRate measurementRate=MeasurementRate::Hz10)
	{ return startWriteRegister(Register::OperationMode2, static_cast<uint8_t>(measurementRate)); }

	bool ALWAYS_INLINE const
	runSetMeasurmentRate()
	{ return runWriteRegister(Register::OperationMode2); }

	bool ALWAYS_INLINE const
	isSetMeasurmentRateSuccessful()
	{ return isWriteRegisterSuccessful(Register::OperationMode2); }


	/// sets a new deviation angle in eeprom
	bool inline
	startSetDeviationAngle(int16_t angle)
	{ return startWrite16BitRegister(Register::DeviationAngle, static_cast<uint16_t>(angle)); }

	bool ALWAYS_INLINE
	runSetDeviationAngle()
	{ return runWrite16BitRegister(Register::DeviationAngle); }

	bool ALWAYS_INLINE const
	isSetDeviationAngleSuccessful()
	{ return isWrite16BitRegisterSuccessful(Register::DeviationAngle); }


	/// sets a new variation angle in eeprom
	bool inline
	startSetVariationAngle(int16_t angle)
	{ return startWrite16BitRegister(Register::VariationAngle, static_cast<uint16_t>(angle)); }

	bool ALWAYS_INLINE
	runSetVariationAngle()
	{ return runWrite16BitRegister(Register::VariationAngle); }

	bool ALWAYS_INLINE const
	isSetVariationAngleSuccessful()
	{ return isWrite16BitRegisterSuccessful(Register::VariationAngle); }


	/// sets a new IIR filter in eeprom
	bool ALWAYS_INLINE
	startSetIIR_Filter(uint8_t filter)
	{ return startWriteRegister(Register::FilterLsb, filter & 0x0f); }

	bool ALWAYS_INLINE const
	runSetIIR_Filter()
	{ return runWriteRegister(Register::FilterLsb); }

	bool ALWAYS_INLINE const
	isSetIIR_FilterSuccessful()
	{ return isWriteRegisterSuccessful(Register::FilterLsb); }



	// READING EEPROM
	/// reads the device id from eeprom
	bool ALWAYS_INLINE
	startReadDeviceId()
	{ return startRead16BitRegister(Register16::DeviceSerial); }

	bool ALWAYS_INLINE
	runReadDeviceId()
	{ return runRead16BitRegister(Register16::DeviceSerial); }

	bool ALWAYS_INLINE const
	isReadDeviceIdSuccessful()
	{ return isRead16BitRegisterSuccessful(Register16::DeviceSerial); }

	uint16_t ALWAYS_INLINE
	getReadDeviceId() { return getRead16BitRegister(); }



	// COMMANDS
	/// Sets the specified orientation
	bool ALWAYS_INLINE
	startSetOrientation(Orientation orientation=Orientation::Level);

	bool ALWAYS_INLINE const
	runSetOrientation();

	bool ALWAYS_INLINE const
	isSetOrientationSuccessful();

	/// enters run mode
	bool ALWAYS_INLINE
	startEnterRunMode()
	{ return startWriteCommand(Command::EnterRunMode); }

	bool ALWAYS_INLINE const
	runEnterRunMode()
	{ return runWriteCommand(Command::EnterRunMode); }

	bool ALWAYS_INLINE const
	isEnterRunModeSuccessful()
	{ return isWriteCommandSuccessful(Command::EnterRunMode); }

	/// enters standby mode
	bool ALWAYS_INLINE
	startEnterStandByMode()
	{ return startWriteCommand(Command:EnterStandbyMode); }

	bool ALWAYS_INLINE const
	runEnterStandbyMode()
	{ return runWriteCommand(Command::EnterStandbyMode); }

	bool ALWAYS_INLINE const
	isEnterStandbyModeSuccessful()
	{ return isWriteCommandSuccessful(Command::EnterStandbyMode); }

	/// enters sleep mode
	bool ALWAYS_INLINE
	startEnterSleepMode()
	{ return startWriteCommand(Command::EnterSleepMode); }

	bool ALWAYS_INLINE const
	runEnterSleepMode()
	{ return runWriteCommand(Command::EnterSleepMode); }

	bool ALWAYS_INLINE const
	isEnterSleepModeSuccessful()
	{ return isWriteCommandSuccessful(Command::EnterSleepMode); }

	/// exit sleep mode
	bool ALWAYS_INLINE
	startExitSleepMode()
	{ return startWriteCommand(Command::ExitSleepMode, 20); }

	bool ALWAYS_INLINE const
	runExitSleepMode()
	{ return runWriteCommand(Command::ExitSleepMode); }

	bool ALWAYS_INLINE const
	isExitSleepModeSuccessful()
	{ return isWriteCommandSuccessful(Command::ExitSleepMode); }

	/// enters user calibration mode
	bool ALWAYS_INLINE
	startEnterUserCalibrationMode()
	{ return startWriteCommand(Command::EnterUserCalibrationMode); }

	bool ALWAYS_INLINE const
	runEnterUserCalibrationMode()
	{ return runWriteCommand(Command::EnterUserCalibrationMode); }

	bool ALWAYS_INLINE const
	isEnterUserCalibrationSuccessful()
	{ return isWriteCommandSuccessful(Command::EnterUserCalibrationMode); }

	/// exit user calibration mode
	bool ALWAYS_INLINE
	startExitUserCalibrationMode()
	{ return startWriteCommand(Command::ExitUserCalibrationMode, 50); }

	bool ALWAYS_INLINE const
	runExitUserCalibrationMode()
	{ return runWriteCommand(Command::ExitUserCalibrationMode); }

	bool ALWAYS_INLINE const
	isExitUserCalibrationModeSuccessful()
	{ return isWriteCommandSuccessful(Command::ExitUserCalibrationMode); }

	/// resets the processor, any new command is delayed by 500ms
	bool ALWAYS_INLINE
	startResetProcessor()
	{ return startWriteCommand(Command::ResetProcessor, 500); }

	bool ALWAYS_INLINE const
	runResetProcessor()
	{ return runWriteCommand(Command::ResetProcessor); }

	bool ALWAYS_INLINE const
	isResetProcessorSuccessful()
	{ return isWriteCommandSuccessful(Command::ResetProcessor); }



	// DATA REQUESTS
	/**
	 * reads the Acceleration registers and buffer the results
	 * sets `isNewAccelerationDataAvailable()` to `true`
	 */
	bool ALWAYS_INLINE
	startReadAcceleration()
	{ return startReadPostData(Command::PostAccelData); }

	bool ALWAYS_INLINE
	runReadAcceleration()
	{ return runReadPostData(Command::PostAccelData, 0, 6); }

	bool ALWAYS_INLINE const
	isReadAccelerationSuccessful()
	{ return isReadPostDataSuccessful(Command::PostAccelData); }

	/// returns the x-acceleration in unknown units
	inline int16_t
	getAccelerationX() { return swapData(0); }

	/// returns the y-acceleration in unknown units
	inline int16_t
	getAccelerationY() { return swapData(1); }

	/// returns the z-acceleration in unknown units
	inline int16_t
	getAccelerationZ() { return swapData(2); }

	/**
	 * reads the Magnetometer registers and buffer the results
	 * sets `isNewMagnetometerDataAvailable()` to `true`
	 */
	bool ALWAYS_INLINE
	startReadMagneticField()
	{ return startReadPostData(Command::PostMagData); }

	bool ALWAYS_INLINE
	runReadMagneticField()
	{ return runReadPostData(Command::PostMagData, 6, 6); }

	bool ALWAYS_INLINE const
	isReadMagneticFieldSuccessful()
	{ return isReadPostDataSuccessful(Command::PostMagData); }

	/// returns the x-acceleration in unknown units
	inline int16_t
	getMagneticFieldX() { return swapData(3); }

	/// returns the y-acceleration in unknown units
	inline int16_t
	getMagneticFieldY() { return swapData(4); }

	/// returns the z-acceleration in unknown units
	inline int16_t
	getMagneticFieldZ() { return swapData(5); }

	/**
	* reads the Heading registers and buffer the results
	* sets `isNewHeadingDataAvailable()` to `true`
	*/
	bool ALWAYS_INLINE
	startReadHeading()
	{ return startReadPostData(Command::PostHeadingData); }

	bool ALWAYS_INLINE
	runReadHeading()
	{ return runReadPostData(Command::PostHeadingData, 12, 6); }

	bool ALWAYS_INLINE const
	isReadHeadingSuccessful()
	{ return isReadPostDataSuccessful(Command::PostHeadingData); }

	/// returns the heading in tenth of degrees (0 -> 3600)
	inline int16_t
	getHeading() { return swapData(6); }

	/**
	 * reads the Tilt registers and buffer the results
	 * sets `isNewTiltDataAvailable()` to `true`
	 */
	bool ALWAYS_INLINE
	startReadTilt()
	{ return startReadPostData(Command::PostTiltData); }

	bool ALWAYS_INLINE
	runReadTilt()
	{ return runReadPostData(Command::PostTiltData, 14, 6); }

	bool ALWAYS_INLINE const
	isReadTiltSuccessful()
	{ return isReadPostDataSuccessful(Command::PostTiltData); }

	/// returns the Pitch in tenth of degrees (-900 -> 0 -> 900)
	inline int16_t
	getPitch() { return swapData(7); }

	/// returns the Roll in tenth of degrees (-900 -> 0 -> 900)
	inline int16_t
	getRoll() { return swapData(8); }

	/// returns the temperature in unknown format (was not specified in datasheet)
	inline int16_t
	getTemperature() { return swapData(9); }

private:
	uint16_t ALWAYS_INLINE
	swapData(uint8_t index);


protected:
	bool
	startWriteCommand(Command command, uint16_t timeout = 1);

	bool ALWAYS_INLINE const
	runWriteCommand(Command command);

	bool ALWAYS_INLINE const
	isWriteCommandSuccessful(Command command);

	/**
	 * writes 8bit data to a register
	 * @param	reg		register address
	 * @param	data	8bit data to write
	 */
	bool inline
	startWriteRegister(Register reg, uint8_t value);

private:
	bool
	startWriteRegisterIgnoreTaskCheck(Register reg, uint8_t value);

protected:
	bool ALWAYS_INLINE const
	runWriteRegister(Register reg);

	bool ALWAYS_INLINE const
	isWriteRegisterSuccessful(Register reg);

	/**
	 * writes 16bit data to a register
	 * @param	reg		register address
	 * @param	data	16bit data to write
	 */
	bool
	startWrite16BitRegister(Register16 reg, uint16_t value);

	bool ALWAYS_INLINE
	runWrite16BitRegister(Register16 reg);

	bool ALWAYS_INLINE const
	isWrite16BitRegisterSuccessful(Register16 reg);

	/**
	 * reads a 8bit register
	 * @param	reg		register address
	 * @return `true` if successful
	 */
	bool inline
	startReadRegister(Register reg);

private:
	bool
	startReadRegisterIgnoreTaskCheck(Register reg);

protected:
	bool ALWAYS_INLINE const
	runReadRegister(Register reg);

	bool ALWAYS_INLINE const
	isReadRegisterSuccessful(Register reg);

	uint8_t ALWAYS_INLINE const
	getReadRegister();

	/**
	 * reads a 16bit register
	 * @param	reg		register address
	 * @return `true` if successful
	 */
	bool
	startRead16BitRegister(Register16 reg);

	bool ALWAYS_INLINE
	runRead16BitRegister(Register16 reg);

	bool ALWAYS_INLINE const
	isRead16BitRegisterSuccessful(Register16 reg);

	uint16_t ALWAYS_INLINE const
	getRead16BitRegister();

	/**
	 * reads
	 * @param	reg		post command
	 * @return `true` if successful
	 */
	bool
	startReadPostData(Command command);

	bool ALWAYS_INLINE
	runReadPostData(Command command, uint8_t offset);

	bool ALWAYS_INLINE const
	isReadPostDataSuccessful(Command command);

private:
	xpcc::Timeout<> timeout;

	struct Task
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

	struct NPtTask
	{
		enum
		{
			ReadEeprom,
			ReadPostData,
			Read16BitRegister,
			Write16BitRegister,
		};
	};

	volatile uint8_t task;
	volatile uint8_t success;
	uint8_t* data;
	uint8_t buffer[3];
	uint8_t registerBufferLSB;
	uint8_t registerBufferMSB;

	xpcc::I2cTagAdapter<xpcc::I2cWriteReadAdapter> adapter;
};

} // namespace xpcc

#include "hmc6343_impl.hpp"

#endif	// XPCC__HMC6343_HPP
