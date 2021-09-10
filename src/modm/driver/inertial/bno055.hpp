/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/math/geometry/vector3.hpp>
#include <modm/math/geometry/quaternion.hpp>

namespace modm
{

// forward declaration for friending with bno055::Data
template < class Transport >
class Bno055;

/// @ingroup modm_driver_bno055
struct bno055
{
	/// Available I2C addresses
	static constexpr uint8_t
	addr(uint8_t offset = 1)
	{
		return 0x28 + (offset & 0b1);
	};

	enum class
	Identifier : uint8_t
	{
		Chip = 0xA0,
		Acc = 0xFB,
		Mag = 0x32,
		Gyr = 0x0F,
	};

	enum class
	Register : uint8_t
	{
		PAGE_ID = 0x07,

		// PAGE0 REGISTER DEFINITION START
		CHIP_ID = 0x00,
		ACCEL_REV_ID = 0x01,
		MAG_REV_ID = 0x02,
		GYRO_REV_ID = 0x03,
		SW_REV_ID_LSB = 0x04,
		SW_REV_ID_MSB = 0x05,
		BL_REV_ID = 0x06,

		// Accel data register
		ACCEL_DATA_X_LSB = 0x08,
		ACCEL_DATA_X_MSB = 0x09,
		ACCEL_DATA_Y_LSB = 0x0A,
		ACCEL_DATA_Y_MSB = 0x0B,
		ACCEL_DATA_Z_LSB = 0x0C,
		ACCEL_DATA_Z_MSB = 0x0D,

		// Mag data register
		MAG_DATA_X_LSB = 0x0E,
		MAG_DATA_X_MSB = 0x0F,
		MAG_DATA_Y_LSB = 0x10,
		MAG_DATA_Y_MSB = 0x11,
		MAG_DATA_Z_LSB = 0x12,
		MAG_DATA_Z_MSB = 0x13,

		// Gyro data registers
		GYRO_DATA_X_LSB = 0x14,
		GYRO_DATA_X_MSB = 0x15,
		GYRO_DATA_Y_LSB = 0x16,
		GYRO_DATA_Y_MSB = 0x17,
		GYRO_DATA_Z_LSB = 0x18,
		GYRO_DATA_Z_MSB = 0x19,

		// Euler data registers
		EULER_H_LSB = 0x1A,
		EULER_H_MSB = 0x1B,

		EULER_R_LSB = 0x1C,
		EULER_R_MSB = 0x1D,

		EULER_P_LSB = 0x1E,
		EULER_P_MSB = 0x1F,

		// Quaternion data registers
		QUATERNION_DATA_W_LSB = 0x20,
		QUATERNION_DATA_W_MSB = 0x21,
		QUATERNION_DATA_X_LSB = 0x22,
		QUATERNION_DATA_X_MSB = 0x23,
		QUATERNION_DATA_Y_LSB = 0x24,
		QUATERNION_DATA_Y_MSB = 0x25,
		QUATERNION_DATA_Z_LSB = 0x26,
		QUATERNION_DATA_Z_MSB = 0x27,

		// Linear acceleration data registers
		LINEAR_ACCEL_DATA_X_LSB = 0x28,
		LINEAR_ACCEL_DATA_X_MSB = 0x29,
		LINEAR_ACCEL_DATA_Y_LSB = 0x2A,
		LINEAR_ACCEL_DATA_Y_MSB = 0x2B,
		LINEAR_ACCEL_DATA_Z_LSB = 0x2C,
		LINEAR_ACCEL_DATA_Z_MSB = 0x2D,

		// Gravity data registers
		GRAVITY_DATA_X_LSB = 0x2E,
		GRAVITY_DATA_X_MSB = 0x2F,
		GRAVITY_DATA_Y_LSB = 0x30,
		GRAVITY_DATA_Y_MSB = 0x31,
		GRAVITY_DATA_Z_LSB = 0x32,
		GRAVITY_DATA_Z_MSB = 0x33,

		// Temperature data register
		TEMP = 0x34,

		// Status registers
		CALIB_STAT = 0x35,
		SELFTEST_RESULT = 0x36,
		INTR_STAT = 0x37,
		SYS_CLK_STAT = 0x38,
		SYS_STAT = 0x39,
		SYS_ERR = 0x3A,

		// Unit selection register
		UNIT_SEL = 0x3B,
		DATA_SELECT = 0x3C,

		// Mode registers
		OPR_MODE = 0x3D,
		PWR_MODE = 0x3E,

		SYS_TRIGGER = 0x3F,
		TEMP_SOURCE = 0x40,
		// Axis remap registers
		AXIS_MAP_CONFIG = 0x41,
		AXIS_MAP_SIGN = 0x42,

		// SIC registers
		SIC_MATRIX_0_LSB = 0x43,
		SIC_MATRIX_0_MSB = 0x44,
		SIC_MATRIX_1_LSB = 0x45,
		SIC_MATRIX_1_MSB = 0x46,
		SIC_MATRIX_2_LSB = 0x47,
		SIC_MATRIX_2_MSB = 0x48,
		SIC_MATRIX_3_LSB = 0x49,
		SIC_MATRIX_3_MSB = 0x4A,
		SIC_MATRIX_4_LSB = 0x4B,
		SIC_MATRIX_4_MSB = 0x4C,
		SIC_MATRIX_5_LSB = 0x4D,
		SIC_MATRIX_5_MSB = 0x4E,
		SIC_MATRIX_6_LSB = 0x4F,
		SIC_MATRIX_6_MSB = 0x50,
		SIC_MATRIX_7_LSB = 0x51,
		SIC_MATRIX_7_MSB = 0x52,
		SIC_MATRIX_8_LSB = 0x53,
		SIC_MATRIX_8_MSB = 0x54,

		// Accelerometer Offset registers
		ACCEL_OFFSET_X_LSB = 0x55,
		ACCEL_OFFSET_X_MSB = 0x56,
		ACCEL_OFFSET_Y_LSB = 0x57,
		ACCEL_OFFSET_Y_MSB = 0x58,
		ACCEL_OFFSET_Z_LSB = 0x59,
		ACCEL_OFFSET_Z_MSB = 0x5A,

		// Magnetometer Offset registers
		MAG_OFFSET_X_LSB = 0x5B,
		MAG_OFFSET_X_MSB = 0x5C,
		MAG_OFFSET_Y_LSB = 0x5D,
		MAG_OFFSET_Y_MSB = 0x5E,
		MAG_OFFSET_Z_LSB = 0x5F,
		MAG_OFFSET_Z_MSB = 0x60,

		// Gyroscope Offset registers
		GYRO_OFFSET_X_LSB = 0x61,
		GYRO_OFFSET_X_MSB = 0x62,
		GYRO_OFFSET_Y_LSB = 0x63,
		GYRO_OFFSET_Y_MSB = 0x64,
		GYRO_OFFSET_Z_LSB = 0x65,
		GYRO_OFFSET_Z_MSB = 0x66,

		// Radius registers
		ACCEL_RADIUS_LSB = 0x67,
		ACCEL_RADIUS_MSB = 0x68,
		MAG_RADIUS_LSB = 0x69,
		MAG_RADIUS_MSB = 0x6A,
		// PAGE0 REGISTERS DEFINITION END

		// PAGE1 REGISTERS DEFINITION START
		// *** ORed with 0x80 to indicate page 1 ***
		// Configuration registers
		ACCEL_CONFIG = 0x88,
		MAG_CONFIG = 0x89,
		GYRO_CONFIG = 0x8A,
		GYRO_MODE_CONFIG = 0x8B,
		ACCEL_SLEEP_CONFIG = 0x8C,
		GYRO_SLEEP_CONFIG = 0x8D,
		MAG_SLEEP_CONFIG = 0x8E,

		// Interrupt registers
		INT_MASK = 0x8F,
		INT = 0x90,
		ACCEL_ANY_MOTION_THRES = 0x91,
		ACCEL_INTR_SETTINGS = 0x92,
		ACCEL_HIGH_G_DURN = 0x93,
		ACCEL_HIGH_G_THRES = 0x94,
		ACCEL_NO_MOTION_THRES = 0x95,
		ACCEL_NO_MOTION_SET = 0x96,
		GYRO_INTR_SETING = 0x97,
		GYRO_HIGHRATE_X_SET = 0x98,
		GYRO_DURN_X = 0x99,
		GYRO_HIGHRATE_Y_SET = 0x9A,
		GYRO_DURN_Y = 0x9B,
		GYRO_HIGHRATE_Z_SET = 0x9C,
		GYRO_DURN_Z = 0x9D,
		GYRO_ANY_MOTION_THRES = 0x9E,
		GYRO_ANY_MOTION_SET = 0x9F,

		// Unique ID
		UNIQUE_ID_0 = 0xD0,
		UNIQUE_ID_F = 0xDF,
		// PAGE1 REGISTERS DEFINITION END
	};

public:
	/// ST_RESULT reset value is 0x00
	/// Read: 1 indicated test passed; 0 indicates test failed
	enum class
	SelfTestResult : uint8_t
	{
		ST_MCU = Bit3,		///< Microcontroller self test result.
		ST_GYR = Bit2,		///< Gyroscope self test result.
		ST_MAG = Bit1,		///< Magnetometer self test result.
		ST_ACC = Bit0,		///< Accelerometer self test result.
	};
	MODM_FLAGS8(SelfTestResult);

	/// INT_STA reset value is 0x00
	/// Read: 1 indicates interrupt triggered; 0 indicates no interrupt triggered
	enum class
	InterruptStatus : uint8_t
	{
		ACC_NM = Bit7,			///< Status of Accelerometer no motion or slow motion interrupt
		ACC_AM = Bit6,			///< Status of Accelerometer any motion interrupt
		ACC_HIGH_G = Bit5,		///< Status of Accelerometer high-g interrupt

		GYR_HIGH_RATE = Bit3,	///< Status of gyroscope high rate interrupt
		GYRO_AM = Bit2,			///< Status of gyroscope any motion interrupt
	};
	MODM_FLAGS8(InterruptStatus);

	/// SYS_CLK_STATUS reset value is 0x00
	enum class
	SystemClockStatus : uint8_t
	{
		ST_MAIN_CLK = Bit0,	///< 1: in Configuration state, 0: Free to configure the CLK SRC
	};
	MODM_FLAGS8(SystemClockStatus);

	/// UNIT_SEL reset value is 0x00
	enum class
	UnitSelection : uint8_t
	{
		ORI_Android_Windows = Bit7,	///< 0: Windows orientation, 1: Android orientation

		TEMP_Unit = Bit5,			///< 0: Celsius, 1: Fahrenheit

		EUL_Unit = Bit2,			///< 0: Degrees, 1: Radians
		GYR_Unit = Bit1,			///< 0: dps, 1: rps
		ACC_Unit = Bit0,			///< 0: m/s^2, 1: mg
	};
	MODM_FLAGS8(UnitSelection);

	/// SYS_TRIGGER reset value is 0x00
	enum class
	SystemTrigger : uint8_t
	{
		CLK_SEL = Bit7,		///< 0: Use internal oscillator, 1: Use external oscillator.
		RST_INT = Bit6,		///< Set to reset all interrupt status bits, and INT output
		RST_SYS = Bit5,		///< Set to reset system

		Self_Test = Bit0,	///< Set to trigger self test
	};
	MODM_FLAGS8(SystemTrigger);


	/// ACC_Config reset value is 0x0D
	enum class
	AccConfig : uint8_t
	{
		AccPowerMode_Mask = Bit7 | Bit6 | Bit5,
		AccBandwidth_Mask = Bit4 | Bit3 | Bit2,
		AccRange_Mask = Bit1 | Bit0,
	};
	MODM_FLAGS8(AccConfig);

	/// ACC_PWR_Mode reset value is 0x00
	enum class
	AccPowerMode : uint8_t
	{
		Normal		= (0 << 5),
		Suspend		= (1 << 5),
		LowPower1	= (2 << 5),
		Standby		= (3 << 5),
		LowPower2	= (4 << 5),
		DeepSuspend	= (5 << 5),
	};
	MODM_FLAGS_CONFIG(AccConfig, AccPowerMode);

	/// ACC_BW reset value is 0x03
	enum class
	AccBandwidth : uint8_t
	{
		Hz7_81	= (0 << 2),
		Hz15_63	= (1 << 2),
		Hz31_25	= (2 << 2),
		Hz62_5	= (3 << 2),
		Hz125	= (4 << 2),
		Hz250	= (5 << 2),
		Hz500	= (6 << 2),
		Hz1000	= (7 << 2),
	};
	MODM_FLAGS_CONFIG(AccConfig, AccBandwidth);

	/// ACC_Range reset value is 0x01
	enum class
	AccRange : uint8_t
	{
		G2 = 0,
		G4 = 1,
		G8 = 2,
		G16 = 3,
	};
	MODM_FLAGS_CONFIG(AccConfig, AccRange);


	/// MAG_Config reset value is 0x0B
	enum class
	MagConfig : uint8_t
	{
		MagPowerMode_Mask = Bit6 | Bit5,
		MagOperationMode_Mask = Bit4 | Bit3,
		MagDataOutputRate_Mask = Bit2 | Bit1 | Bit0,
	};
	MODM_FLAGS8(MagConfig);

	/// MAG_Power_mode reset value is 0x00
	enum class
	MagPowerMode : uint8_t
	{
		Normal		= (0 << 5),
		Sleep		= (1 << 5),
		Suspend		= (2 << 5),
		ForceMode	= (3 << 5),
	};
	MODM_FLAGS_CONFIG(MagConfig, MagPowerMode);

	/// MAG_OPR_Mode reset value is 0x01
	enum class
	MagOperationMode : uint8_t
	{
		LowPower		= (0 << 3),
		Regular			= (1 << 3),
		EnhancedRegular	= (2 << 3),
		HighAccuracy	= (3 << 3),
	};
	MODM_FLAGS_CONFIG(MagConfig, MagOperationMode);

	/// MAG_Data_output _rate reset value is 0x03
	enum class
	MagDataOutputRate : uint8_t
	{
		Hz2 = 0,
		Hz6 = 1,
		Hz8 = 2,
		Hz10 = 3,
		Hz15 = 4,
		Hz20 = 5,
		Hz25 = 6,
		Hz30 = 7,
	};
	MODM_FLAGS_CONFIG(MagConfig, MagDataOutputRate);


	/// GYR_Config_0 reset value is 0x38
	enum class
	GyrConfig0 : uint8_t
	{
		GyrBandwidth_Mask = Bit5 | Bit4 | Bit3,
		GyrRange_Mask = Bit2 | Bit1 | Bit0,
	};
	MODM_FLAGS8(GyrConfig0);

	/// GYR_Bandwidth _rate reset value is 0x07
	enum class
	GyrBandwidth : uint8_t
	{
		Hz523	= (0 << 3),
		Hz230	= (1 << 3),
		Hz116	= (2 << 3),
		Hz47	= (3 << 3),
		Hz23	= (4 << 3),
		Hz12	= (5 << 3),
		Hz64	= (6 << 3),
		Hz32	= (7 << 3),
	};
	MODM_FLAGS_CONFIG(GyrConfig0, GyrBandwidth);

	/// GYR_Range reset value is 0x00
	enum class
	GyrRange : uint8_t
	{
		Dps2000	= 0,
		Dps1000	= 1,
		Dps500	= 2,
		Dps250	= 3,
		Dps125	= 4,
	};
	MODM_FLAGS_CONFIG(GyrConfig0, GyrRange);

	/// GYR_Config_1 reset value is 0x00
	enum class
	GyrConfig1 : uint8_t
	{
		GyrPowerMode_Mask = Bit2 | Bit1 | Bit0,
	};
	MODM_FLAGS8(GyrConfig1);

	/// GYR_Range reset value is 0x00
	enum class
	GyrPowerMode : uint8_t
	{
		Normal				= 0,
		FastPowerUp			= 1,
		DeepSuspend			= 2,
		Suspend				= 3,
		AdvancePowerSave	= 4,
	};
	MODM_FLAGS_CONFIG(GyrConfig1, GyrPowerMode);

	/// ACC_Sleep_Config reset value is 0x00
	/// GYR_Sleep_Config reset value is 0x00

	/// INT_MSK reset value is 0x00
	/// INT_EN reset value is 0x00
	/// Masking of interrupts, when enabled the interrupt will
	/// update the INT_STA register and trigger a change on the INT pin,
	/// when disabled only the INT_STA register will be updated.
	enum class
	Interrupt : uint8_t
	{
		ACC_NM = Bit7,			///< Masking of accelerometer no motion or slow motion interrupt
		ACC_AM = Bit6,			///< Masking of accelerometer any motion interrupt
		ACC_HIGH_G = Bit5,		///< Masking of accelerometer high-g interrupt

		GYR_HIGH_RATE = Bit3,	///< Masking of gyroscope high rate interrupt
		GYRO_AM = Bit2,			///< Masking of gyroscope any motion interrupt
	};
	MODM_FLAGS8(Interrupt);

public:
	enum class
	TemperatureSource : uint8_t
	{
		Accelerometer = 0,
		Gyroscope = 1,
	};

	enum class
	OperationMode : uint8_t
	{
		ConfigMode = 0b0000,
		Acc = 0b0001,
		Mag = 0b0010,
		Gyro = 0b0011,
		AccMag = 0b0100,
		AccGyro = 0b0101,
		MagGyro = 0b0110,
		AMG = 0b0111,
		IMU = 0b1000,
		Compass = 0b1001,
		M4G = 0b1010,
		NDOF_FMC_Off = 0b1011,
		NDOF = 0b1100,
	};

	enum class
	PowerMode : uint8_t
	{
		Normal = 0,
		LowPower = 1,
		Suspend = 2,
	};

	enum class
	StatusCode : uint8_t
	{
		SystemIdle = 0,
		SystemError = 1,
		InitializingPeripherals = 2,
		SystemInitialization = 3,
		ExecutingSelftest = 4,
		SensorFusionAlgorithmRunning = 5,
		SystemRunningWithoutFusionAlgorithm = 6,
	};

	enum class
	StatusError : uint8_t
	{
		NoError = 0,
		PeripheralInitializationError = 1,
		SystemInitializationError = 2,
		SelfTestResultFailed = 3,
		RegisterMapValueOutOfRange = 4,
		RegisterMapAddressOutOfRange = 5,
		RegisterMapWriteError = 6,
		BNOLowPowerModeNotAvailableForSelectedOperationMode = 7,
		AccelerometerPowerModeNotAvailable = 8,
		FusionAlgorithmConfigurationError = 9,
		SensorConfigurationError = 0xA,
	};

public:
	using Registers_t = FlagsGroup<
		SelfTestResult_t,
		InterruptStatus_t,
		SystemClockStatus_t,
		UnitSelection_t,
		SystemTrigger_t,
		AccConfig_t,
		MagConfig_t,
		GyrConfig0_t,
		GyrConfig1_t,
		Interrupt_t,
		modm::Flags8<TemperatureSource>,
		modm::Flags8<OperationMode>,
		modm::Flags8<PowerMode>,
		modm::Flags8<StatusCode>,
		modm::Flags8<StatusError>
	>;

public:
	struct modm_packed
	Data
	{
		template < class I2cMaster >
		friend class Bno055;

		inline Vector3f acceleration()        const { return Vector3f(r.acceleration).scaled(1.f/16); }
		inline Vector3f magnetometer()        const { return Vector3f(r.magnetometer).scaled(1.f/16); }
		inline Vector3f gyroscope()           const { return Vector3f(r.gyroscope).scaled(1.f/16); }
		inline float heading()                const { return r.heading / 16.f; }
		inline float roll()                   const { return r.roll / 16.f; }
		inline float pitch()                  const { return r.pitch / 16.f; }
		inline Quaternion<float> quaternion() const { return Quaternion<float>(r.quaternion).scale(1l << 14); }
		inline Vector3f linear_acceleration() const { return Vector3f(r.linear_acceleration).scaled(1.f/100); }
		inline Vector3f gravity()             const { return Vector3f(r.gravity).scaled(1.f/100); }
		inline int8_t temperature()           const { return r.temperature; }

	private:
		struct Raw {
			int16_t acceleration[3];
			int16_t magnetometer[3];
			int16_t gyroscope[3];
			int16_t heading;
			int16_t roll;
			int16_t pitch;
			int16_t quaternion[4];
			int16_t linear_acceleration[3];
			int16_t gravity[3];
			int8_t temperature;
		} r;
		static constexpr uint8_t size = sizeof(Raw);
	};
}; // struct bno055

/**
 * @ingroup modm_driver_bno055
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Bno055 : public bno055, public modm::I2cDevice<I2cMaster, 4>
{
public:
	/// Constructor, requires a bno055::Data object.
	inline Bno055(Data &data, uint8_t address=addr()):
		I2cDevice<I2cMaster,4>(address), data(data) {}

	inline modm::ResumableResult<bool>
	configure(OperationMode mode=OperationMode::NDOF)
	{
		return updateRegister(Register::OPR_MODE, mode);
	}

	inline modm::ResumableResult<bool>
	readData()
	{
		return readRegister(Register::ACCEL_DATA_X_LSB, (uint8_t*)&data.r, Data::size);
	}

	inline modm::ResumableResult<bool>
	enableExternalClock()
	{
		return updateRegister(Register::SYS_TRIGGER, SystemTrigger::CLK_SEL, Registers_t(0));
	}

public:
	inline modm::ResumableResult<bool>
	updateRegister(Register reg, Registers_t setMask, Registers_t clearMask = Registers_t(0xff))
	{
		RF_BEGIN();
		RF_CALL(setPageId(reg));

		buffer[0] = uint8_t(reg);
		this->transaction.configureWriteRead(buffer, 1, buffer, 1);
		RF_CALL( this->runTransaction() );

		buffer[1] = (buffer[0] & ~clearMask.value) | setMask.value;
		buffer[0] = uint8_t(reg);
		this->transaction.configureWrite(buffer, 2);
		RF_END_RETURN_CALL( this->runTransaction() );
	}

	inline modm::ResumableResult<bool>
	readRegister(Register reg, uint8_t *output, size_t length=1)
	{
		RF_BEGIN();
		RF_CALL(setPageId(reg));

		buffer[0] = uint8_t(reg);
		this->transaction.configureWriteRead(buffer, 1, output, length);
		RF_END_RETURN_CALL( this->runTransaction() );
	}

public:
	inline Data&
	getData()
	{ return data; }

protected:
	inline modm::ResumableResult<bool>
	setPageId(Register regi)
	{
		const uint8_t reg = uint8_t(regi);
		RF_BEGIN();

		if ((reg ^ prev_reg) & 0x80) {
			buffer[0] = uint8_t(Register::PAGE_ID);
			buffer[1] = reg >> 7;
			this->transaction.configureWrite(buffer, 2);
			buffer[2] = RF_CALL( this->runTransaction() );
			if (buffer[2]) prev_reg = reg;
			RF_RETURN(static_cast<bool>(buffer[2]));
		}

		RF_END_RETURN(true);
	}

private:
	Data &data;
	uint8_t buffer[3];
	uint8_t prev_reg;
};

} // namespace modm

#pragma GCC diagnostic pop
