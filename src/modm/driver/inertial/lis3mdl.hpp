/*
 * Copyright (c) 2020, Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LIS3MDL_HPP
#define MODM_LIS3MDL_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/math/utils/endianness.hpp>
#include <modm/math/geometry/vector3.hpp>
#include "lis3_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_lis3mdl
struct lis3mdl
{
	enum class
	Register : uint8_t
	{
		WHO_AM_I = 0x0F,	///< Contains 0x3D

		CTRL1 = 0x20,		///< Control register 1: rw
		CTRL2 = 0x21,		///< Control register 2: rw
		CTRL3 = 0x22,		///< Control register 3: rw
		CTRL4 = 0x23,		///< Control register 4: rw
		CTRL5 = 0x24,		///< Control register 5: rw

		STATUS = 0x27,		///< Status Data Register

		OUT_X_L = 0x28,		///< Angular rates output x-axis register low
		OUT_X_H = 0x29,		///< Angular rates output x-axis register high
		OUT_Y_L = 0x2A,		///< Angular rates output y-axis register low
		OUT_Y_H = 0x2B,		///< Angular rates output y-axis register high
		OUT_Z_L = 0x2C,		///< Angular rates output z-axis register low
		OUT_Z_H = 0x2D,		///< Angular rates output z-axis register high

		OUT_TEMP_L = 0x2E,	///< Temperature output register low
		OUT_TEMP_H = 0x2F,	///< Temperature output register high

		INT_CFG = 0x30,		///< Interrupt Configuration Register
		INT_SRC = 0x31,		///< Interrupt Source Register
		INT_THS_L = 0x33,	///< Interrupt Threshold Value Low
		INT_THS_H = 0x34	///< Interrupt Threshold Value High
	};

public:
	/// Control register 1
	enum class
	Control1 : uint8_t
	{
		TEMP_EN = Bit7,		///< Temperature sensor enable

		OM1 = Bit6,			///< Operation mode bit 1
		OM0 = Bit5,			///< Operation mode bit 0

		DO2 = Bit4,			///< Data output rate bit 2
		DO1 = Bit3,			///< Data output rate bit 1
		DO0 = Bit2,			///< Data output rate bit 0

		FAST_ODR = Bit1,	///< Fast data acquisition (for rates higher than 80Hz)
		ST = Bit0,			///< Self test enable

	};
	MODM_FLAGS8(Control1);

	/// Output Data Rates for the Magnetometer sensor
	enum class
	DataRate : uint8_t
	{
		Rate_0_625_Hz = 0x00,	///< 0.625 Hz
		Rate_1_25_Hz = 0x04,	///< 1.25 Hz
		Rate_2_5_Hz = 0x80,		///< 2.5 Hz
		Rate_5_Hz = 0xC0,		///< 5 Hz
		Rate_10_Hz = 0x10,		///< 10 Hz
		Rate_20_Hz = 0x14,		///< 20 Hz
		Rate_40_Hz = 0x18,		///< 40 Hz
		Rate_80_Hz = 0x1C,		///< 80 Hz
		Rate_115_Hz = 0x62,		///< 115 Hz
		Rate_300_Hz = 0x42,		///< 300 Hz
		Rate_569_Hz = 0x22,		///< 560 Hz
		Rate_1000_Hz = 0x02,	///< 1000 Hz
	};
	typedef modm::Configuration<Control1_t, DataRate, Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1> DataRate_t;

	/// Operation mode to be used with high speed data rates (>80 Hz)
	enum class
	HighSpeedModes : uint8_t
	{
		LowPower = 0x00,			//< 1000 Hz
		MediumPerformance = 0x01,	//< 560 Hz
		HighPerformance = 0x02,		//< 300 Hz
		UltraHighPerformance = 0x03 //< 115 Hz
	};
	//This is the definition of the Highspeed modes for the Y and X axes. Z axis is in register 4
	typedef modm::Configuration<Control1_t, HighSpeedModes, Bit2 | Bit1, 4> HighSpeedModes_t;

// ----------------------------------------------------------------------------

	/// Control Register 2
	enum class
	Control2 : uint8_t
	{
		FS1 = Bit6,		///< Full-Scale configuration bit 1
		FS0 = Bit5,		///< Full-Scale configuration bit 0
		REBOOT = Bit3,	///< Reboot memory content
		SOFT_RST= Bit2,	///< Soft reset
	};
	MODM_FLAGS8(Control2);

	/// Full scale of the magnetometer data output
	enum class
	Scale : uint8_t
	{
		Scale_4_gauss = 0x00,	///< 4 gauss
		Scale_8_gauss = 0x40,	///< 8 gauss
		Scale_12_gauss = 0x80,	///< 12 gauss
		Scale_16_gauss = 0xC0,	///< 16 gauss
	};
	typedef modm::Configuration<Control2_t, Scale, Bit6 | Bit5 > Scale_t;

// ----------------------------------------------------------------------------

	/// Control Register 3
	enum class
	Control3 : uint8_t
	{
		LP = Bit5,		///< Low Power Mode Enable
		SIM = Bit2,		///< SPI Interface Mode (0 = 4 Wire(default), 1 = 3 Wire)
		MD_1 = Bit1,	///< Sensor Operation Mode bit 1
		MD_0 = Bit0		///< Sensor Operation Mode bit 0
	};
	MODM_FLAGS8(Control3);

	enum class
	OperationMode : uint8_t
	{
		Off = 0x02,			///< Power down mode
		Continous = 0x00,	///< Continous conversion mode
		Single = 0x01,		///< Single shot mode
	};
	typedef modm::Configuration<Control3_t, OperationMode, Bit1 | Bit0 > OperationMode_t;

// ----------------------------------------------------------------------------

	/// Control Register 4
	enum class
	Control4 : uint8_t
	{
		OMZ_1 = Bit3,	///< Operation Mode for z axis bit 1
		OMZ_0 = Bit2,	///< Operation Mode for z axis bit 1
		BLE = Bit1,		///< Bit order of the data output (0 = little endian, 1 = big endian)
	};
	MODM_FLAGS8(Control4);
	// The configuration for the high speed mode of the Z axis, see register 1 for Y and Z axes
	typedef modm::Configuration<Control4_t, HighSpeedModes, Bit2 | Bit1, 2> HighSpeedModesZ_t;

// ----------------------------------------------------------------------------

	/// Control Register 5
	enum class
	Control5 : uint8_t
	{
		FAST_READ = Bit7,	///< Fast read enable
		BDU		  = Bit6,	///< Blocking read
	};
	MODM_FLAGS8(Control5);

// ----------------------------------------------------------------------------

	/// STATUS is read-only
	enum class
	Status : uint8_t
	{
		ZYXOR = Bit7,	///< Overrun in Z,Y and X axis
		ZOR = Bit6,		///< Overrun in Z axis
		YOR = Bit5,		///< Overrun in Y axis
		XOR = Bit4,		///< Overrun in X axis
		ZYXDA = Bit3,	///< New data available in Z,Y and X axis
		ZDA = Bit2,		///< New data available in Z axis
		YDA = Bit1,		///< New data available in Y axis
		XDA = Bit0		///< New data available in X axis

	};
	MODM_FLAGS8(Status);

protected:
	// Conversion table to convert raw acceleration values to physical
	static constexpr float convTable [4] =	{
												0.00012207f,  // Conversion for 4 gauss scale
												0.000244141f, // Conversion for 8 gauss scale
												0.000366211f, // Conversion for 12 gauss scale
												0.000488281f  // Conversion for 16 gauss scale
												};
	/// @endcond
}; // struct lis3mdl

/**
 * \tparam	I2cMaster	I2cMaster interface
 *
 * \author	Benjamin Carrick
 * \ingroup modm_driver_lis3mdl
 */
template < class I2cMaster >
class Lis3mdl : public lis3mdl, public Lis3TransportI2c<I2cMaster>
{
public:
	/**
	 * \brief Construct a driver object for the LIS3MDL chip
	 *
	 * \param address The I2C device address of the sensor
	 */
	Lis3mdl(uint8_t address = 0x1C);

	/**
	 * \brief Configures the sensor
	 * This method will setup the data rate  and the scale with which the sensor will sample the data.
	 *
	 * \param rate The sample rate for the sensor
	 * \param scale The full scale of the output data
	 * \return Whether the configuration was successful
	 */
	modm::ResumableResult<bool>
	configure(DataRate rate, Scale scale);

	/**
	 * @brief Sets the mode of the sensor
	 *
	 * @param mode The mode on which the magnetometer operates
	 * @return Whether the mode has been set
	 */
	modm::ResumableResult<bool>
	setMode(OperationMode mode);


	/**
	 * @brief Get the currently configured scale for the magnetometer
	 *
	 * @return The currently configured data scale
	 */
	Scale
	getScale();

	/**
	 * @brief Reads out the raw magnetometer into a given vector object.
	 *
	 * @param data A reference to a Vector3i object the data will be written to
	 * @return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readMagnetometerRaw(Vector3i& data);

	/**
	 * \brief Reads out the scaled magnetometer data in gauss
	 *
	 * \param data A reference to a Vector3f object the data will be written to
	 * \return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readMagnetometer(Vector3f& data);

private:
	/// The shadow for the control register 1
	Control1_t control1Shadow;
	/// The shadow for the control register 2
	Control2_t control2Shadow;
	/// The shadow for the control register 3
	Control3_t control3Shadow;
	/// The shadow for the control register 4
	Control4_t control4Shadow;
	/// The shadow for the control register 5
	Control5_t control5Shadow;

	/// The buffer for reading out the data registers
	int16_t readBuffer[3];

	/// Variable to hold the success of a transaction to the sensor
	bool success;
};

} // namespace modm

#include "lis3mdl_impl.hpp"

#endif	// MODM_LIS3MDL_HPP
