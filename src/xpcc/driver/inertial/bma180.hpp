/*
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMA180_HPP
#define MODM_BMA180_HPP

#include <modm/architecture/interface/i2c_adapter.hpp>

namespace modm
{
	namespace bma180
	{
		// MARK: Registers
		/// The addresses of the Configuration and Data Registers
		enum Register
		{
			REGISTER_CHIP_ID = 0x00,		///< metal fix default 0x03
			REGISTER_VERSION = 0x01,		///< metal fix default 0x12
			REGISTER_DATA_X0 = 0x02,		///< X-Axis Data LSB
			REGISTER_DATA_X1 = 0x03,		///< X-Axis Data MSB
			REGISTER_DATA_Y0 = 0x04,		///< Y-Axis Data LSB
			REGISTER_DATA_Y1 = 0x05,		///< Y-Axis Data MSB
			REGISTER_DATA_Z0 = 0x06,		///< Z-Axis Data LSB
			REGISTER_DATA_Z1 = 0x07,		///< Z-Axis Data MSB
			REGISTER_DATA_TEMP = 0x08,		///< Temperature Data

			REGISTER_STATUS1 = 0x09,
			REGISTER_STATUS2 = 0x0A,
			REGISTER_STATUS3 = 0x0B,
			REGISTER_STATUS4 = 0x0C,

			REGISTER_CTRL0 = 0x0D,
			REGISTER_CTRL1 = 0x0E,
			REGISTER_CTRL2 = 0x0F,
			REGISTER_RESET = 0x10,

			REGISTER_BW_TCS = 0x20,			///< TCS is factory calibrated
			REGISTER_CTRL3 = 0x21,
			REGISTER_CTRL4 = 0x22,
			REGISTER_HY = 0x23,
			REGISTER_SLOPE_TAPSENS_INFO = 0x24,
			REGISTER_HIGH_LOW_INFO = 0x25,
			REGISTER_LOW_DUR = 0x26,
			REGISTER_HIGH_DUR = 0x27,
			REGISTER_TAPSENS_TH = 0x28,
			REGISTER_LOW_TH = 0x29,
			REGISTER_HIGH_TH = 0x2A,
			REGISTER_SLOPE_TH = 0x2B,

			REGISTER_CD1 = 0x2C,
			REGISTER_CD2 = 0x2D,

			REGISTER_TRF_X = 0x2E,			///< factory calibrated
			REGISTER_TRF_Y = 0x2F,			///< factory calibrated
			REGISTER_TRF_Z = 0x30,			///< factory calibrated
			REGISTER_GAIN_T = 0x31,			///< factory calibrated
			REGISTER_GAIN_X = 0x32,			///< factory calibrated
			REGISTER_GAIN_Y = 0x33,			///< factory calibrated
			REGISTER_GAIN_Z = 0x34,			///< factory calibrated
			REGISTER_OFFSET_LSB1 = 0x35,
			REGISTER_OFFSET_LSB2 = 0x36,
			REGISTER_OFFSET_T = 0x37,		///< factory calibrated
			REGISTER_OFFSET_X = 0x38,		///< factory calibrated
			REGISTER_OFFSET_Y = 0x39,		///< factory calibrated
			REGISTER_OFFSET_Z = 0x3A		///< factory calibrated
		};

		// The memory layout of this sensor is scary crazy.
		// So scary crazy, only Germans could have thought this up.

		// MARK: Data Registers
		/// The options of REGISTER_CHIP_ID
		enum ChipId {
			CHIP_ID = 0x03
		};
		/// The options of REGISTER_VERSION
		enum Version {
			AL_VERSION = 0x10,
			ML_VERSION = 0x02
		};

		/// The options of REGISTER_DATA_X0
		enum DataX0 {
			ACC_X_LSB = (0x3F << 2),
			NEW_DATA_X = 0x01
		};
		// REGISTER_DATA_X1
		/// The options of REGISTER_DATA_Y0
		enum DataY0 {
			ACC_Y_LSB = (0x3F << 2),
			NEW_DATA_Y = 0x01
		};
		// REGISTER_DATA_Y1
		/// The options of REGISTER_DATA_Z0
		enum DataZ0 {
			ACC_Z_LSB = (0x3F << 2),
			NEW_DATA_Z = 0x01
		};
		// REGISTER_DATA_Z1
		// REGISTER_DATA_TEMP

		// MARK: Status Registers
		/// The options of REGISTER_STATUS1
		enum Status1 {
			S_FIRST_TAPSENS = 0x80,
			L_ALERT = 0x20,
			L_SLOPE_SIGN_X_INT = 0x10,
			L_SLOPE_SIGN_Y_INT = 0x08,
			L_SLOPE_SIGN_Z_INT = 0x04,
			S_OFFESET_ST = 0x02,
			L_EE_WRITE = 0x01
		};
		/// The options of REGISTER_STATUS2
		enum Status2 {
			S_HIGH_TH = 0x80,
			S_LOW_TH = 0x40,
			S_SLOPE = 0x20,
			S_TAPSENS = 0x10,
			L_LOW_SIGN_X_INT = 0x04,
			L_LOW_SIGN_Y_INT = 0x02,
			L_LOW_SIGN_Z_INT = 0x01
		};
		/// The options of REGISTER_STATUS3
		enum Status3 {
			L_HIGH_TH_INT = 0x80,
			L_LOW_TH_INT = 0x40,
			L_SLOPE_INT = 0x20,
			L_TAPSENS_INT = 0x10,
			L_X_FIRST_INT = 0x04,
			L_Y_FIRST_INT = 0x02,
			L_Z_FIRST_INT = 0x01
		};
		/// The options of REGISTER_STATUS4
		enum Status4 {
			L_HIGH_SIGN_X_INT = 0x80,
			L_HIGH_SIGN_Y_INT = 0x40,
			L_HIGH_SIGN_Z_INT = 0x20,
			L_TAPSENS_SIGN_X = 0x10,
			L_TAPSENS_SIGN_Y = 0x08,
			L_TAPSENS_SIGN_Z = 0x04
		};

		// MARK: Control Registers
		/// The options of REGISTER_CTRL0
		enum Ctrl0 {
			RESET_INT = 0x40,
			UPDATE_IMAGE = 0x20,
			EE_W = 0x10,
			ST1 = 0x08,
			ST0 = 0x04,
			SLEEP_M = 0x02,
			DIS_WAKE_UP = 0x01
		};
		/// The options of REGISTER_CTRL1
		enum Ctrl1 {
			EN_OFFSET_X = 0x80,
			EN_OFFSET_Y = 0x40,
			EN_OFFSET_Z = 0x20
		};
		/// The options of REGISTER_CTRL2
		enum Ctrl2 {
			UNLOCK_EE = 0xF0,
		};
		/// The options of REGISTER_RESET
		enum Reset {
			RESET = 0xB6
		};

		// MARK: Image Registers
		/// The options of REGISTER_BW_TCS
		enum Bandwidth {
			BW = 0xF0,
			BW_10HZ = 0x00,
			BW_20HZ = 0x10,
			BW_40HZ = 0x20,
			BW_75HZ = 0x30,
			BW_150HZ = 0x40,
			BW_300HZ = 0x50,
			BW_600HZ = 0x60,
			BW_1200HZ = 0x70,
			BW_HP = 0x80,
			BW_BP = 0x90
		};
		/// The options of REGISTER_BW_TCS
		enum Tcs {
			TCS = 0x0F,
			TCS_N4_0 = 0x00,
			TCS_N3_5 = 0x01,
			TCS_N3_0 = 0x02,
			TCS_N2_5 = 0x03,
			TCS_N2_0 = 0x04,
			TCS_N1_5 = 0x05,
			TCS_N1_0 = 0x06,
			TCS_N0_5 = 0x07,
			TCS_0_0  = 0x08,
			TCS_P0_5 = 0x09,
			TCS_P1_0 = 0x0A,
			TCS_P1_5 = 0x0B,
			TCS_P2_0 = 0x0C,
			TCS_P2_5 = 0x0D,
			TCS_P3_0 = 0x0E,
			TCS_P3_5 = 0x0F,
		};
		/// The options of REGISTER_CTRL3
		enum Control3 {
			SLOPE_ALERT = 0x80,
			SLOPE_INT = 0x40,
			HIGH_INT = 0x20,
			LOW_INT = 0x10,
			TAPSENS_INT = 0x08,
			ADV_INT = 0x04,
			NEW_DATA_INT = 0x02,
			LAT_INT = 0x01,
		};
		/// The options of REGISTER_CTRL4
		enum Control4 {
			LOW_HY_LSB = (0x03 << 6),

			MOT_CD = (0x03 << 4),
			MOT_CD_RESET = (0x00 << 4),
			MOT_CD_1N = (0x01 << 4),
			MOT_CD_2N = (0x02 << 4),
			MOT_CD_3N = (0x03 << 4),

			FF_CD = (0x03 << 2),
			FF_CD_RESET = (0x00 << 2),
			FF_CD_1N = (0x01 << 2),
			FF_CD_2N = (0x02 << 2),
			FF_CD_3N = (0x03 << 2),

			OFFSET_FINETUNING = 0x03,
			OFFSET_FINETUNING_NO_ACTION = 0x00,
			OFFSET_FINETUNING_FINE_CALIBRATION = 0x01,
			OFFSET_FINETUNING_COARSE_CALIBRATION = 0x02,
			OFFSET_FINETUNING_FULL_CALIBRATION = 0x03,
		};
		/// The options of REGISTER_HY
		enum Hy {
			HIGH_HY = (0x1F << 3),
			LOW_HY_MSB = 0x07
		};
		/// The options of REGISTER_SLOPE_TAPSENS_INFO
		enum SlopeTapsensInfo {
			SLOPE_INT_X = 0x80,
			SLOPE_INT_Y = 0x40,
			SLOPE_INT_Z = 0x20,
			SLOPE_FILTER = 0x10,
			TAPSENS_INT_X = 0x08,
			TAPSENS_INT_Y = 0x04,
			TAPSENS_INT_Z = 0x02,
			TAPSENS_FILTER = 0x01,
		};
		/// The options of REGISTER_HIGH_LOW_INFO
		enum HighLowInfo {
			HIGH_INT_X = 0x80,
			HIGH_INT_Y = 0x40,
			HIGH_INT_Z = 0x20,
			HIGH_FILTER = 0x10,
			LOW_INT_X = 0x08,
			LOW_INT_Y = 0x04,
			LOW_INT_Z = 0x02,
			LOW_FILTER = 0x01,
		};
		/// The options of REGISTER_LOW_DUR
		enum LowDur {
			LOW_DUR = (0x7F << 1),
			TRF_RANGE = 0x01,
		};
		/// The options of REGISTER_HIGH_DUR
		enum HighDur {
			HIGH_DUR = (0x7F << 1),
			DISABLE_I2C = 0x01,
		};

		// REGISTER_TAPSENS_TH
		// REGISTER_LOW_TH
		// REGISTER_HIGH_TH
		// REGISTER_SLOPE_TH
		// REGISTER_CD1
		// REGISTER_CD2

		// MARK: Image Registers, TCO
		/// The options of REGISTER_TRF_X
		enum SlopeDuration {
			TRF_X = (0x3F << 2),
			SLOPE_DURATION = 0x03,
			SLOPE_DURATION_1 = 0x00,
			SLOPE_DURATION_3 = 0x01,
			SLOPE_DURATION_5 = 0x02,
			SLOPE_DURATION_7 = 0x03
		};
		/// The options of REGISTER_TRF_Y
		enum WakeUpDuration {
			TRF_Y = (0x3F << 2),
			WAKE_UP_DURATION = 0x03,
			WAKE_UP_DURATION_20MS = 0x00,
			WAKE_UP_DURATION_80MS = 0x01,
			WAKE_UP_DURATION_320MS = 0x02,
			WAKE_UP_DURATION_2560MS = 0x03
		};
		/// The options of REGISTER_TRF_Z
		enum ModeConfig {
			TRF_Z = (0x3F << 2),
			MODE_CONFIG = 0x03,
			MODE_CONFIG_LOW_NOISE = 0x00,
			MODE_CONFIG_ULTRA_LOW_NOISE = 0x01,
			MODE_CONFIG_LOW_NOISE_REDUCED_POWER = 0x02,
			MODE_CONFIG_LOW_POWER = 0x03
		};

		// MARK: Image Registers, GAIN
		/// The options of REGISTER_GAIN_T
		enum TapSensDuration {
			GAIN_T = (0x1F << 3),
			TAPSENS_DURATION = 0x07,
			TAPSENS_DURATION_120T = 0x00,
			TAPSENS_DURATION_180T = 0x01,
			TAPSENS_DURATION_240T = 0x02,
			TAPSENS_DURATION_360T = 0x03,
			TAPSENS_DURATION_600T = 0x04,
			TAPSENS_DURATION_1200T = 0x05,
			TAPSENS_DURATION_1800T = 0x06,
			TAPSENS_DURATION_2400T = 0x07
		};
		/// The options of REGISTER_GAIN_{X,Y,Z}
		enum GainXYZ {
			GAIN_X = (0x7F << 1),
			GAIN_Y = (0x7F << 1),
			GAIN_Z = (0x7F << 1),
			DISABLE_REGULATORS = 0x01,
			SHADOW_DISABLE = 0x01,
			WAKE_UP = 0x01
		};

		// MARK: Image Registers, OFFSET
		/// The options of REGISTER_OFFSET_LSB1
		enum Range {
			OFFSET_X_LSB = 0xF0,
			RANGE = (0x07 << 1),
			RANGE_1G = (0x00 << 1),
			RANGE_1_5G = (0x01 << 1),
			RANGE_2G = (0x02 << 1),
			RANGE_3G = (0x03 << 1),
			RANGE_4G = (0x04 << 1),
			RANGE_8G = (0x05 << 1),
			RANGE_16G = (0x06 << 1),
			SAMPLE_SKIPPING = 0x01
		};
		/// The options of REGISTER_OFFSET_LSB2
		enum OffsetLsb2 {
			OFFSET_Z_LSB = 0xF0,
			OFFSET_Y_LSB = 0x0F
		};
		/// The options of REGISTER_OFFSET_T
		enum OffsetT {
			OFFSET_T_MSB = (0x7F << 1),
			READOUT_12BIT = 0x01
		};
		// REGISTER_OFFSET_X
		// REGISTER_OFFSET_Y
		// REGISTER_OFFSET_Z
	}

	/**
	 * \brief Basic BMA180 digital accelerometer sensor driver
	 *
	 * The BMA180 is a 3-axis accelerometer with high resolution (14-bit)
	 * measurement at up to +-16 g. Digital output data is formatted as 16-bit
	 * twos complement and is accessible through I2C digital interface.
	 *
	 * The following LSB-values are valid:
	 * 1 LSB{ADC} = 0.25mg in 2g range; it scales with range
	 * 1 LSB{TEMP} = 0.5 degrees Celsius
	 *
	 * For further information on the special sensing functions, consult the
	 * <a href="http://www.bosch-sensortec.com/content/language1/downloads/BST-BMA180-FL000-03.pdf">
	 * datasheet</a>.
	 *
	 * \author	Niklas Hauser
	 * \ingroup driver_inertial
	 *
	 * \tparam I2cMaster Asynchronous Two Wire interface
	 */
	template < typename I2cMaster >
	class Bma180 : protected modm::I2cWriteReadTransaction
	{
	public:
		/**
		 * \brief	Constructor
		 * \param	address		address is 0x40 with SDO pin low else 0x41
		 */
		Bma180(uint8_t* data, uint8_t address=0x40);

		/**
		 * Configures the sensor to full scale resolution with specified range,
		 * bandwidth, mode and interrupt enable.
		 * Be aware that the sensor is only calibrated for MODE_CONFIG_LOW_NOISE.
		 * By changing to other modes, the offset is changed too, thus
		 * subsequently an offset correction has to be performed.
		 */
		bool
		configure(bma180::Range range=bma180::RANGE_3G, bma180::Bandwidth bandwidth=bma180::BW_75HZ, bma180::ModeConfig mode=bma180::MODE_CONFIG_LOW_NOISE, bool interrupt=true);

		/**
		 * only read the X-ZDATA0-1 registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		modm_always_inline void
		readAccelerometer();

		/* \return pointer to 8bit array containing xyzt temperature and accelerations
		 */
		inline uint8_t*
		getData();

		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has been accessed, or data is being
		 * copied into the buffer.
		 */
		modm_always_inline bool
		isNewDataAvailable();

		void
		update();

		/// perform soft reset on sensor
		bool
		reset();

		/**
		 * General method for writing only part of a register.
		 *
		 * \param mask	use Group Masks and/or Bit Masks
		 * \param value	use Group Configurations and/or Bit Masks
		 */
		bool
		writeMaskedRegister(bma180::Register reg, uint8_t mask, uint8_t value);

	private:
		/**
		 * writes 8bit data to a register, blocking!
		 * \param reg register address
		 * \param data 8bit data to write
		 */
		bool
		writeRegister(bma180::Register reg, uint8_t value);

		/**
		 * reads a 8bit register, blocking!
		 * \param reg the 8bit register to read
		 * \return 8bit content
		 */
		uint8_t
		readRegister(bma180::Register reg);

		enum Status {
			READ_ACCELEROMETER_PENDING = 0x01,
			READ_ACCELEROMETER_RUNNING = 0x02,
			NEW_ACCELEROMETER_DATA = 0x04,
		};

		uint8_t status;
		uint8_t* data;
		uint8_t buffer[2];
	};

}

#include "bma180_impl.hpp"

#endif // MODM_BMA180_HPP
