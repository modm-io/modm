/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/math/utils.hpp>

namespace modm
{

// forward declaration for friending with mmc5603::Data
template < class I2cMaster >
class Mmc5603;

/// @ingroup modm_driver_mmc5603
struct mmc5603
{
	/// The MEMSIC device 7-bit device address is 0110000 where the three
	/// LSB’s are pre-programmed into the MMC5603NJ by the factory.
	static constexpr
	uint8_t addr(uint8_t factory=0)
	{ return 0b0110000 | (factory & 0b111); }

	static constexpr uint8_t ProductID{0x10};

	enum class
	Register : uint8_t
	{
		Xout0 = 0x00,				// Xout[19:12]
		Xout1 = 0x01,				// Xout[11:4]
		Yout0 = 0x02,				// Yout[19:12]
		Yout1 = 0x03,				// Yout[11:4]
		Zout0 = 0x04,				// Zout[19:12]
		Zout1 = 0x05,				// Zout[11:4]
		Xout2 = 0x06,				// Xout[3:0]
		Yout2 = 0x07,				// Yout[3:0]
		Zout2 = 0x08,				// Zout[3:0]
		Tout = 0x09,				// Temperature output
		Status1 = 0x18,				// Device status1
		ODR = 0x1A,					// Output data rate
		InternalControl0 = 0x1B,	// Control register 0
		InternalControl1 = 0x1C,	// Control register 1
		InternalControl2 = 0x1D,	// Control register 2
		ST_X_TH = 0x1E,				// X-axis selftest threshold
		ST_Y_TH = 0x1F,				// Y-axis selftest threshold
		ST_Z_TH = 0x20,				// Z-axis selftest threshold
		ST_X = 0x27,				// X-axis selftest set value
		ST_Y = 0x28,				// Y-axis selftest set value
		ST_Z = 0x29,				// Z-axis selftest set value
		ProductID = 0x39,			// Product ID
	};

public:
	enum class
	Status1 : uint8_t
	{
		/// This bit is an indicator of successfully reading its OTP memory
		/// either as part of its power up sequence, or after an I2C command
		/// that reloads the OTP memory, such as resetting the chip and
		/// refreshing the OTP registers.
		OTP_read_done = Bit4,
		/// This bit is an indicator of the selftest signal, it keeps low once
		/// the device PASS selftest.
		Sat_sensor = Bit5,
		/// This bit indicates that a measurement of magnetic field is done and
		/// the data is ready to be read. This bit is reset only when any of the
		/// magnetic data registers is read.
		Meas_m_done = Bit6,
		/// This bit indicates that a measurement of temperature is done and
		/// the data is ready to be read. This bit is reset only when the
		/// temperature register is read.
		Meas_t_done = Bit7,
	};
	MODM_FLAGS8(Status1);

	enum class
	Control0 : uint8_t
	{
		/// Take Measure of Magnetic field, or TM_M bit. Writing a 1 into this
		/// location causes the chip to perform a magnetic measurement.
		/// This bit is self-clearing at the end of each measurement.
		Take_meas_M = Bit0,
		/// Take Measure of Temperature, or TM_T bit. Writing a 1 into this
		/// location causes the chip to perform a temperature measurement.
		/// This bit is self-clearing at the end of each measurement.
		Take_meas_T = Bit1,
		/// Writing a 1 into this location will cause the chip to do the Set
		/// operation, which will allow large set current to flow through the
		/// sensor coils for 375ns. This bit is self-cleared at the end of Set operation.
		DoSet = Bit3,
		/// Writing a 1 into this location will cause the chip to do the Reset
		/// operation, which will allow large reset current to flow through the
		/// sensor coils for 375ns. This bit is self-cleared at the end of Reset operation.
		DoReset = Bit4,
		/// Writing a 1 into this location will enable the function of automatic
		/// set/reset. This function applies to both on-demand and continuous-time
		/// measurements. This bit must be set to 1 in order to activate the feature
		/// of periodic set. This bit is recommended to set to “1” in the application.
		Auto_SR_en = Bit5,
		/// Writing a 1 into this location will enable the function of automatic
		/// self-test. The threshold in register 1EH, 1FH, 20H should be set before
		/// this bit is set to 1. This bit clears itself after the operation is completed.
		Auto_st_en = Bit6,
		/// Writing a 1 into this location will start the calculation of the measurement
		/// period according to the ODR. This bit should be set before continuous-mode
		/// measurements are started. This bit is self- cleared after the measurement
		/// period is calculated by internal circuits.
		Cmm_freq_en = Bit7,
	};
	MODM_FLAGS8(Control0);

	enum class
	Control1 : uint8_t
	{
		Bandwidth_Mask = 0b11,
		/// Writing “1” will disable this channel, and reduce Measurement Time and
		/// total charge per measurement.When a channel is disabled it is simply
		/// skipped during Take Measure routine. Its output register is not reset
		/// and will maintain the last value written to it when this channel was
		/// active. Note: Y/Z needs to be inhibited the same time in case needed.
		X_inhibit = Bit2,
		Y_inhibit = Bit3,
		Z_inhibit = Bit4,
		/// Writing 1 into this location will bring a DC current through the
		/// self-test coil of the sensor. This current will cause an offset of
		/// the magnetic field. This function is used to check whether the sensor
		/// has been saturated.
		St_enp = Bit5,
		/// The function of this bit is similar to ST_ENP, but the offset of the
		/// magnetic field is of opposite polarity.
		St_enm = Bit6,
		/// Software Reset. Writing "1" will cause the part to reset, similar to
		/// power-up. It will clear all registers and also re-read OTP as part
		/// of its startup routine. The power on time is 20mS.
		Sw_reset = Bit7,
	};
	MODM_FLAGS8(Control1);

	/// The bandwidth adjusts the length of the decimation filter. It controls
	/// the duration of each measurement.
	/// @note X/Y/Z channel measurements are taken sequentially. Delay Time among
	///       those measurements is 1/3 of the Measurement Time defined in the table.
	enum class
	Bandwidth : uint8_t
	{
		Ms6_6 = 0x00, ///< 6.6ms measurement time
		Ms3_5 = 0x01, ///< 3.5ms measurement time
		Ms2_0 = 0x02, ///< 2.0ms measurement time
		Ms1_2 = 0x03, ///< 1.2ms measurement time
	};
	MODM_FLAGS_CONFIG(Control1, Bandwidth);

	enum class
	Control2 : uint8_t
	{
		PeriodicalSet_Mask = 0b111,
		/// Writing 1 into this location will enable the function of periodical set.
		En_prd_set = Bit3,
		/// The device will enter continuous mode, if ODR has been set to a
		/// non-zero value and a 1 has been written into Cmm_freq_en. The
		/// internal counter will start counting as well since this bit is set.
		Cmm_en = Bit4,
		/// Factory use only, reset value is 0.
		INT_mdt_en = Bit5,
		/// Factory use only, reset value is 0.
		INT_meas_done_en = Bit6,
		/// If this bit is set to 1 to achieve 1000Hz ODR.
		Hpower = Bit7,
	};
	MODM_FLAGS8(Control2);

	/// These bits determine how many measurements are done before a set is
	/// executed, when the part is in continuous mode and the automatic
	/// set/reset is enabled. From 000 to 111, the sensor will do one set
	/// for every 1, 25, 75, 100, 250, 500, 1000, and 2000 samples. In order
	/// to enable this feature, both En_prd_set and Auto_SR must be set to
	/// 1, and the part should work in continuous mode. Please note that
	/// during this operation, the sensor will not be reset.
	enum class
	PeriodicalSet : uint8_t
	{
		Samples1 = 0x00,
		Samples25 = 0x01,
		Samples75 = 0x02,
		Samples100 = 0x03,
		Samples250 = 0x04,
		Samples500 = 0x05,
		Samples1000 = 0x06,
		Samples2000 = 0x07,
	};
	MODM_FLAGS_CONFIG(Control2, PeriodicalSet);

	struct modm_packed
	Data
	{
		template < class I2cMaster >
		friend class Mmc5603;

		// DATA ACCESS
		/// returns the magnetic field as signed 32-bit value with 16384 counts/Gauss
		///@{
		int32_t inline
		getX() const { return swapData(0); }

		int32_t inline
		getY() const { return swapData(1); }

		int32_t inline
		getZ() const { return swapData(2); }
		///@}

		/// returns the magnetic field in Gauss as floating point
		///@{
		float inline
		getXf() const { return swapDataFloat(0); }

		float inline
		getYf() const { return swapDataFloat(1); }

		float inline
		getZf() const { return swapDataFloat(2); }
		///@}

		/// returns the temperature in Celcius
		int8_t inline
		getTemperature() const { return int16_t(data[9]) * 4/5 - 75; }

		int32_t inline
		operator [](uint8_t index) const
		{ return (index < 3) ? swapData(index) : 0; }

	protected:
		int32_t inline
		swapData(uint8_t index) const
		{ return int32_t(((data[2*index] << 16) | (data[2*index+1] << 8) | (data[6+index])) >> 4) - 524288l; }

		float inline
		swapDataFloat(uint8_t index) const
		{ return swapData(index) / 16384.f; }

		uint8_t data[10];
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	static constexpr uint8_t
	i(Bandwidth bw) { return uint8_t(bw); }
	static constexpr uint8_t
	i(PeriodicalSet ps) { return uint8_t(ps); }
	using Register_t = FlagsGroup<Control0_t, Control1_t, Control2_t>;
	/// @endcond
}; // struct mmc5603

/**
 * @ingroup modm_driver_mmc5603
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Mmc5603 : public mmc5603, public modm::I2cDevice< I2cMaster, 3 >
{
public:
	/// Constructor, requires a mmc5603::Data object
	Mmc5603(Data &data, uint8_t address=addr())
	:	I2cDevice<I2cMaster,3>(address), data(data)
	{}

	modm::ResumableResult<bool>
	ping()
	{
		RF_BEGIN();
		buffer[1] = 0;
		RF_CALL(read(Register::ProductID, buffer[1]));
		RF_END_RETURN(buffer[1] == ProductID);
	}

	modm::ResumableResult<bool>
	startTemperatureMeasurement()
	{ return update(Register::InternalControl0, Control0::Take_meas_T); }

	/// Continuous Mode up to 250Hz with Automatic Set/Reset, 2ms Measurement Time
	modm::ResumableResult<bool>
	configureContinuousMode(uint8_t frequency, Bandwidth bandwidth=Bandwidth::Ms2_0)
	{
		RF_BEGIN();
		if (not RF_CALL(write(Register::ODR, frequency)))
			RF_RETURN(false);
		if (not RF_CALL(update(Register::InternalControl1, Register_t(i(bandwidth)))))
		    RF_RETURN(false);
		if (not RF_CALL(update(Register::InternalControl0, Control0::Cmm_freq_en | Control0::Auto_SR_en)))
		    RF_RETURN(false);
		RF_END_RETURN_CALL(update(Register::InternalControl2, Control2::Cmm_en));
	}

public:
	Data& getData() { return data; }
	Status1_t getStatus1() { return Status1(rb(Register::Status1)); }
	uint8_t getOutputDataRate() { return rb(Register::ODR); }
	Control0_t getControl0() { return rb(Register::InternalControl0); }
	Control1_t getControl1() { return rb(Register::InternalControl1); }
	Control2_t getControl2() { return rb(Register::InternalControl2); }

public:
	modm::ResumableResult<bool>
	readProductId(uint8_t &value)
	{ return read(Register::ProductID, value); }

	modm::ResumableResult<bool>
	readMagneticField()
	{ return read(Register::Xout0, data.data, 10); }

public:
	modm::ResumableResult<bool>
	update(Register reg, Register_t setMask, Register_t clearMask = Register_t(0))
	{
		RF_BEGIN();
		rb(reg) = (rb(reg) & ~clearMask.value) | setMask.value;
		RF_END_RETURN_CALL(write(reg, rb(reg)));
	}

	modm::ResumableResult<bool>
	write(Register reg, uint8_t value)
	{
		RF_BEGIN();
		buffer[0] = i(reg);
		buffer[1] = value;
		this->transaction.configureWrite(buffer, 2);
		RF_END_RETURN_CALL(this->runTransaction());
	}

	modm::ResumableResult<bool>
	read(Register reg, uint8_t &value)
	{ return read(reg, &value, 1); }

	modm::ResumableResult<bool>
	read(Register reg, uint8_t *data, uint8_t size)
	{
		RF_BEGIN();
		buffer[0] = i(reg);
		this->transaction.configureWriteRead(buffer, 1, data, size);
		RF_END_RETURN_CALL(this->runTransaction());
	}

protected:
	Data &data;
	// Internal register are write-only, so a read-modify-write does not work
	// 0: 0x18 status1
	// 1: 0x19 [reserved]
	// 2: 0x1A ODR
	// 3: 0x1B Internal Control 0
	// 4: 0x1C Internal Control 1
	// 5: 0x1D Internal Control 2
	// 6: 0x1E X threshold
	// 7: 0x1F Y threshold
	// 8: 0x20 Z threshold
	uint8_t regBuffer[9] = {};
	uint8_t& rb(Register reg) { return regBuffer[i(reg) - 0x18]; }
	// I2C buffer
	uint8_t buffer[2];
};

} // namespace modm

