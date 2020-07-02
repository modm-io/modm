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

#ifndef MODM_LSM6DS33_HPP
#define MODM_LSM6DS33_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/math/utils/endianness.hpp>
#include <modm/math/geometry/vector3.hpp>
#include "lis3_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_lsm6ds33
struct lsm6ds33
{
	enum class
	Register : uint8_t
	{
		FIFO_CTRL1 = 0x06,	///< FIFO control register 1
		FIFO_CTRL2 = 0x07,	///< FIFO control register 2
		FIFO_CTRL3 = 0x08,	///< FIFO control register 3
		FIFO_CTRL4 = 0x09,	///< FIFO control register 4
		FIFO_CTRL5 = 0x0A,	///< FIFO control register 5

		ORIENT_CFG_G = 0x0B, ///< Orientation config register

		INT1_CTRL = 0x0D,	///< Interrrupt 1 Control register
		INT2_CTRL = 0x0E,	///< Interrrupt 1 Control register

		WHO_AM_I = 0x0F,	///< Contains 0x69

		CTRL1 = 0x10,		///< Control register 1: rw
		CTRL2 = 0x11,		///< Control register 2: rw
		CTRL3 = 0x12,		///< Control register 3: rw
		CTRL4 = 0x13,		///< Control register 4: rw
		CTRL5 = 0x14,		///< Control register 5: rw
		CTRL6 = 0x15,		///< Control register 6: rw
		CTRL7 = 0x16,		///< Control register 7: rw
		CTRL8 = 0x17,		///< Control register 8: rw
		CTRL9 = 0x18,		///< Control register 9: rw
		CTRL10 = 0x19,		///< Control register 10: rw

		STATUS = 0x1E,		///< Status Data Register

		OUT_TEMP_L = 0x20,	///< Temperature output register low
		OUT_TEMP_H = 0x21,	///< Temperature output register high

		OUT_X_L_G = 0x22,		///< Angular rates output x-axis register low
		OUT_X_H_G = 0x23,		///< Angular rates output x-axis register high
		OUT_Y_L_G = 0x24,		///< Angular rates output y-axis register low
		OUT_Y_H_G = 0x25,		///< Angular rates output y-axis register high
		OUT_Z_L_G = 0x26,		///< Angular rates output z-axis register low
		OUT_Z_H_G = 0x27,		///< Angular rates output z-axis register high

		OUT_X_L_XL = 0x28,		///< Acceleration output x-axis register low
		OUT_X_H_XL = 0x29,		///< Acceleration output x-axis register high
		OUT_Y_L_XL = 0x2A,		///< Acceleration output y-axis register low
		OUT_Y_H_XL = 0x2B,		///< Acceleration output y-axis register high
		OUT_Z_L_XL = 0x2C,		///< Acceleration output z-axis register low
		OUT_Z_H_XL = 0x2D,		///< Acceleration output z-axis register high

		FIFO_STATUS1 = 0x3A,	///< Fifo status register 1
		FIFO_STATUS2 = 0x3B,	///< Fifo status register 2
		FIFO_STATUS3 = 0x3C,	///< Fifo status register 3
		FIFO_STATUS4 = 0x3D,	///< Fifo status register 4

		FIFO_DATA_OUT_L = 0x3E,	///< Fifo data output register low
		FIFO_DATA_OUT_H = 0x3F,	///< Fifo data output register high

	};

public:
	/// Control register for the acceleration sensor
	enum class
	Control1 : uint8_t
	{
		ODR3_XL = Bit7,		///< Output data rate bit 3
		ODR2_XL = Bit6,		///< Output data rate bit 2
		ODR1_XL = Bit5,		///< Output data rate bit 1
		ODR0_XL = Bit4,		///< Output data rate bit 0

		FS1_XL = Bit3,		///< Full scale selection bit 1
		FS0_XL = Bit2,		///< Full scale selection bit 0

		BW1_XL = Bit1,		///< Anti-aliasing bandwidth bit 1
		BW0_XL = Bit0,		///< Anti-aliasing bandwidth bit 0

	};
	MODM_FLAGS8(Control1);

	/// Output Data Rates for the Acceleration sensor
	enum class
	AccDataRate : uint8_t
	{
		Off = 0x00,				///< Turn off the acceleration sensor
		Rate_13_Hz = 0x10,		///< 13 Hz
		Rate_26_Hz = 0x20,		///< 26 Hz
		Rate_52_Hz = 0x30,		///< 52 Hz
		Rate_104_Hz = 0x40,		///< 104 Hz
		Rate_208_Hz = 0x50,		///< 208 Hz
		Rate_416_Hz = 0x60,		///< 416 Hz
		Rate_833_Hz = 0x70,		///< 833 Hz
		Rate_1666_Hz = 0x80,	///< 1666 Hz
		Rate_3332_Hz = 0x90,	///< 3332 Hz
		Rate_6664_Hz = 0xA0,	///< 6664 Hz
	};
	typedef modm::Configuration<Control1_t, AccDataRate, Bit7 | Bit6 | Bit5 | Bit4> AccDataRate_t;

	/// Full scale of the acceleration sensor data output
	enum class
	AccScale : uint8_t
	{
		Scale_2_G = 0x00,		///< +- 2g
		Scale_4_G = 0x08,		///< +- 4g
		Scale_8_G = 0x0C,		///< +- 8g
		Scale_16_G = 0x04,		///< +- 16g
	};
	typedef modm::Configuration<Control1_t, AccScale, Bit3 | Bit2 > AccScale_t;

	/// Anti aliasing filter bandwith for the acceleration sensor (only used when Control Register 4, Bit 7 is 1)
	enum class
	AccAABandwith : uint8_t
	{
		Bw_400_Hz = 0x00,		///< 400 Hz
		Bw_200_Hz = 0x01,		///< 200 Hz
		BW_100_Hz = 0x02,		///< 100 Hz
		Bw_50_Hz = 0x03,		///< 50 Hz
	};
	typedef modm::Configuration<Control1_t, AccAABandwith, Bit3 | Bit2 > AccAABandwith_t;

// ----------------------------------------------------------------------------

	/// Control Register 2 for the Gyroscope
	enum class
	Control2 : uint8_t
	{
		ODR3_G = Bit7,		///< Output data rate bit 3
		ODR2_G = Bit6,		///< Output data rate bit 2
		ODR1_G = Bit5,		///< Output data rate bit 1
		ODR0_G = Bit4,		///< Output data rate bit 0

		FS2_G = Bit3,		///< Full scale selection bit 2
		FS1_G = Bit2,		///< Full scale selection bit 1
		FS0_G = Bit1,		///< Full scale selection bit 0
	};
	MODM_FLAGS8(Control2);

	/// Output Data Rates for the gyroscope
	enum class
	GyroDataRate : uint8_t
	{
		Off = 0x00,				///< Turn off the gyroscope
		Rate_13_Hz = 0x10,		///< 13 Hz
		Rate_26_Hz = 0x20,		///< 26 Hz
		Rate_52_Hz = 0x30,		///< 52 Hz
		Rate_104_Hz = 0x40,		///< 104 Hz
		Rate_208_Hz = 0x50,		///< 208 Hz
		Rate_416_Hz = 0x60,		///< 416 Hz
		Rate_833_Hz = 0x70,		///< 833 Hz
		Rate_1666_Hz = 0x80,	///< 1666 Hz
	};
	typedef modm::Configuration<Control2_t, GyroDataRate, Bit7 | Bit6 | Bit5 | Bit4> GyroDataRate_t;

	/// Full scale of the gyroscope data output
	enum class
	GyroScale : uint8_t
	{
		Scale_125_dps = 0x02,	///< 125 degrees per second
		Scale_245_dps = 0x00,	///< 245 degrees per second
		Scale_500_dps = 0x04,	///< 500 degrees per second
		Scale_1000_dps = 0x08,	///< 1000 degrees per second
		Scale_2000_dps = 0x0C,	///< 2000 degrees per secons
	};
	typedef modm::Configuration<Control2_t, GyroScale, Bit3 | Bit2 | Bit1 > GyroScale_t;

// ----------------------------------------------------------------------------

	/// Control Register 3
	enum class
	Control3 : uint8_t
	{
		BOOT = Bit7,		///< Reboot memory content
		BDU = Bit6,			///< Block data update (0 = continous (default), 1 = wait until registers read)
		H_LACTIVE = Bit5,	///< Interrupt activation level (0 = high active(default), 1= low active)
		PP_OD = Bit4,		///< Interrupt pin mode (0 = Push-Pull(default), 1 = Open-Drain)
		SIM = Bit3,			///< SPI Interface Mode (0 = 4 Wire(default), 1 = 3 Wire)
		IF_INC = Bit2,		///< I2C increment register on access (0 = disabled, 1= enabled(default))
		BLE = Bit1,			///< Endianess selection (0 = Little endian(default), 1 = Big endian.
		SW_RESET = Bit0		///< Software Reset
	};
	MODM_FLAGS8(Control3);

// ----------------------------------------------------------------------------

	/// Control Register 4
	enum class
	Control4 : uint8_t
	{
		XL_BW_SCAL_ODR = Bit7,	///< Manual selection of bandwidth for the acceleration sensor (0 = automatic, 1 = manual)
		SLEEP_G = Bit6,			///< Gyroscope sleep mode enable (default = 0)
		INT2_on_INT1 = Bit5,	///< Interrupt 2 output on Interrupt 1 pad (default = 0)
		FIFO_TEMP_EN = Bit4		///< Enable Temperature output as 4th fifo set
	};
	MODM_FLAGS8(Control4);

// ----------------------------------------------------------------------------

	/// Control Register 6
	enum class
	Control6 : uint8_t
	{
		TRIG_EN = Bit7,		///< Gyroscope data edge-sensitive trigger enable (default = 0)
		LVL_EN = Bit6,		///< Gyroscope data level-sensitive trigger enable (default = 0)
		LVL2_EN = Bit5,		///< Gyroscope level-sensitive latched enable.
		XL_HM_MODE = Bit4	///< Disable high performance mode for acceleration sensor (default = 0)
	};
	MODM_FLAGS8(Control6);

// ----------------------------------------------------------------------------

	/// Control Register 7
	enum class
	Control7 : uint8_t
	{
		G_HM_MODE = Bit7,	///< Disable high performance mode for gyroscope (default = 0)s
		HP_G_EN = Bit6,		///< Enable high-pass filter for gyroscope (default = 0)
		HP_CF_G1 = Bit5,	///< Gyroscope high-pass filter cutoff frequency bit 1
		HP_CF_G0 = Bit4,	///< Gyroscope high-pass filter cutoff frequuency bit 0
		HP_G_RST = Bit3		///< Reset high-pass filter for gyroscope.
	};
	MODM_FLAGS8(Control7);

	/// High-pass cutoff frequency selection for the gyroscope
	enum class
	GyroHPCutoff : uint8_t
	{
		Cutoff_0_0081_Hz = 0x00,	///< 0.0081 Hz
		Cutoff_0_0324_Hz = 0x10,	///< 0.0324 Hz
		Cutoff_2_07_Hz = 0x20,		///< 2.07 Hz
		Cutoff_16_32_Hz = 0x30		///< 16.32 Hz
	};
	typedef modm::Configuration<Control7_t, GyroHPCutoff, Bit5 | Bit4 > GyroHPCutoff_t;

// ----------------------------------------------------------------------------

	/// Control Register 8
	enum class
	Control8: uint8_t
	{
		LPF_XL_EN = Bit7,		///< Enable the low-pass filter for the acceleration sensor
		HP_CF_XL1 = Bit6,		///< Acceleration sensor high-pass filter cutoff frequency bit 1
		HP_CF_XL0 = Bit5,		///< Acceleration sensor high-pass filter cutoff frequency bit 0
		HP_SLOPE_XL_EN = Bit3	///< High-pass or slope filter selection
	};
	MODM_FLAGS8(Control8);

	/// High-pass cutoff frequency selection for the gyroscope
	enum class
	AccHPCutoff : uint8_t
	{
		Cutoff_DIV_50 = 0x00,		///< Acceleration Data Rate / 50
		Cutoff_DIV_100 = 0x10,		///< Acceleration Data Rate / 100
		Cutoff_DIV_9 = 0x20,		///< Acceleration Data Rate / 9
		Cutoff_DIV_400 = 0x30		///< Acceleration Data Rate / 400
	};
	typedef modm::Configuration<Control8_t, AccHPCutoff, Bit5 | Bit4 > AccHPCutoff_t;

// ----------------------------------------------------------------------------
	/// Control Register 9
	enum class
	Control9: uint8_t
	{
		Z_EN_XL = Bit5,		///< Enable Z-Axis for the acceleration sensor (default 1)
		X_EN_XL = Bit4,		///< Enable X-Axis for the acceleration sensor (default 1)
		Y_EN_XL = Bit3		///< Enable Y-Axis for the acceleration sensor (default 1)
	};
	MODM_FLAGS8(Control9);

// ----------------------------------------------------------------------------

	/// Control Register 10
	enum class
	Control10: uint8_t
	{
		Z_EN_G = Bit5,		///< Enable Z-Axis for the gyroscope (default 1)
		X_EN_G = Bit4,		///< Enable X-Axis for the gyroscope (default 1)
		Y_EN_G = Bit3		///< Enable Y-Axis for the gyroscope (default 1)
	};
	MODM_FLAGS8(Control10);

// ----------------------------------------------------------------------------

	/// STATUS is read-only
	enum class
	Status : uint8_t
	{
		EV_BOOT = Bit3,	///< Bootup is running
		TDA = Bit2,		///< New temperature data available
		GDA = Bit1,		///< New gyroscope data available
		XLDA = Bit0		///< New acceleration sensor data available
	};
	MODM_FLAGS8(Status);

protected:
	// Conversion table to convert raw acceleration values to physical
	static constexpr float accConvTable [4] =	{
												0.000061035f, // Conversion for 2 g scale
												0.000488281f, // Conversion for 16 g scale
												0.00012207f,  // Conversion for 4 g scale
												0.000244141f  // Conversion for 8 g scale
												};

	static constexpr float gyroConvTable [4] =	{
												0.007476807f, // Conversion for 245 dps scale
												0.015258789f, // Conversion for 500 dps scale
												0.030517578f,  // Conversion for 1000 dps scale
												0.061035156f  // Conversion for 2000 scale
												};

	/// @endcond
}; // struct lsm6ds33

/**
 * \tparam	I2cMaster	I2cMaster interface
 *
 * \author	Benjamin Carrick
 * \ingroup modm_driver_lsm6ds33
 */
template < class I2cMaster >
class Lsm6ds33 : public lsm6ds33, public Lis3TransportI2c<I2cMaster>
{
public:
	/**
	 * \brief Construct a driver object for the LSM6DS33 chip
	 *
	 * \param address The I2C device address of the sensor
	 */
	Lsm6ds33(uint8_t address = 0x6A);

	/**
	 * \brief Configures the acceleration sensor
	 * This method will setup the data rate  and the scale with which the sensor will sample the acceleration data.
	 * A sample rate has to be set in order to turn on the sensor.
	 *
	 * \param accRate The sample rate for the acceleration sensor
	 * \param accScale The full scale of the acceleration data
	 * \return Whether the configuration was successful
	 */
	modm::ResumableResult<bool>
	configureAccelerationSensor(AccDataRate accRate, AccScale accScale);

	/**
	 * \brief Configures the gyroscope
	 * This method will setup the data rate and the scale with which the sensor will sample the spin rates.
	 * A sample rate has to be set in order to turn on the sensor.
	 *
	 * \param accRate The sample rate for the gyroscope
	 * \param accScale The full scale of the spin rate data
	 * \return Whether the configuration was successful
	 */
	modm::ResumableResult<bool>
	configureGyroscope(GyroDataRate gyroRate, GyroScale gyroScale);

	/**
	 * @brief Reads out the raw acceleration data into a given vector object.
	 *
	 * @param acceleration A reference to a Vector3i object the data will be written to
	 * @return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readAccelerationRaw(Vector3i& acceleration);

	/**
	 * @brief Reads out the raw gyroscope data into a given vector object.
	 *
	 * @param spinRates A reference to a Vector3i object the data will be written to
	 * @return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readGyroscopeRaw(Vector3i& spinRates);

	/**
	 * @brief Get the currently configured scale for the acceleration sensor
	 *
	 * @return AccScale The currently config acceleration data scale
	 */
	AccScale
	getAccelerationScale();

	/**
	 * @brief Get the currently configured scale for the gyroscope
	 *
	 * @return AccScale The currently config gyroscope data scale
	 */
	GyroScale
	getGyroscopeScale();


	/**
	 * \brief Reads out the scaled acceleration data in g
	 *
	 * \param acceleration A reference to a Vector3f object the data will be written to
	 * \return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readAcceleration(Vector3f& acceleration);

	/**
	 * \brief Reads out the scaled spin rates from the gyroscope in degrees per second
	 *
	 * \param spinRates A reference to a Vector3f object the data will be written to
	 * \return Whether the sensor data have been read
	 */
	modm::ResumableResult<bool>
	readGyroscope(Vector3f& spinRates);

private:
	/// The shadow for the control register 1 (accelerometer)
	Control1_t control1Shadow;
	/// The shadow for the control register 2 (gyro)
	Control2_t control2Shadow;
	/// The shadow for the control register 3
	Control3_t control3Shadow;

	/// The buffer for reading out the data registers
	int16_t readBuffer[3];

	/// Variable to hold the success of a transaction to the sensor
	bool success;
};

} // namespace modm

#include "lsm6ds33_impl.hpp"

#endif	// MODM_LSM6DS33_HPP
