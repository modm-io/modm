// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_VL6180_HPP
#define XPCC_VL6180_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/architecture/driver/accessor.hpp>
#include <xpcc/processing.hpp>

namespace xpcc
{

/// @cond
namespace vl6180_private
{
	struct
	BinaryConfiguration
	{
		uint16_t reg;
		uint8_t value;
	};
	// see Application Note AN4545 page 24: SR03 settings
	extern xpcc::accessor::Flash<vl6180_private::BinaryConfiguration> configuration;

	// gain settings
	extern xpcc::accessor::Flash<float> gain;
}	// namespace vl6180_private
/// @endcond

// forward declaration for friending with vl6180::Data
template < class I2cMaster >
class Vl6180;

struct vl6180
{
public:
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint16_t
	{
		IDENTIFICATION__MODEL_ID = 0x00,
		IDENTIFICATION__MODEL_REV_MAJOR = 0x01,
		IDENTIFICATION__MODEL_REV_MINOR = 0x02,
		IDENTIFICATION__MODULE_REV_MAJOR = 0x03,
		IDENTIFICATION__MODULE_REV_MINOR = 0x04,
		IDENTIFICATION__DATE_HI = 0x06,
		IDENTIFICATION__DATE_LO = 0x07,
		IDENTIFICATION__TIME_HI = 0x08,
		IDENTIFICATION__TIME_LO = 0x09,

		SYSTEM__MODE_GPIO0 = 0x10,
		SYSTEM__MODE_GPIO1 = 0x11,
		SYSTEM__HISTORY_CTRL = 0x12,
		SYSTEM__INTERRUPT_CONFIG_GPIO = 0x14,
		SYSTEM__INTERRUPT_CLEAR = 0x15,
		SYSTEM__FRESH_OUT_OF_RESET = 0x16,
		SYSTEM__GROUPED_PARAMETER_HOLD = 0x17,

		SYSRANGE__START = 0x18,
		SYSRANGE__THRESH_HIGH = 0x19,
		SYSRANGE__THRESH_LOW = 0x1A,
		SYSRANGE__INTERMEASUREMENT_PERIOD = 0x1B,
		SYSRANGE__MAX_CONVERGENCE_TIME = 0x1C,
		SYSRANGE__CROSSTALK_COMPENSATION_RATE = 0x1E,
		SYSRANGE__CROSSTALK_VALID_HEIGHT = 0x21,
		SYSRANGE__EARLY_CONVERGENCE_ESTIMATE = 0x22,
		SYSRANGE__PART_TO_PART_RANGE_OFFSET = 0x24,
		SYSRANGE__RANGE_IGNORE_VALID_HEIGHT = 0x25,
		SYSRANGE__RANGE_IGNORE_THRESHOLD = 0x26,
		SYSRANGE__MAX_AMBIENT_LEVEL_MULT = 0x2C,
		SYSRANGE__RANGE_CHECK_ENABLES = 0x2D,
		SYSRANGE__VHV_RECALIBRATE = 0x2E,
		SYSRANGE__VHV_REPEAT_RATE = 0x31,

		SYSALS__START = 0x38,
		SYSALS__THRESH_HIGH = 0x3A,
		SYSALS__THRESH_LOW = 0x3C,
		SYSALS__INTERMEASUREMENT_PERIOD = 0x3E,
		SYSALS__ANALOGUE_GAIN = 0x3F,
		SYSALS__INTEGRATION_PERIOD = 0x40,

		RESULT__RANGE_STATUS = 0x4D,
		RESULT__ALS_STATUS = 0x4E,
		RESULT__INTERRUPT_STATUS_GPIO = 0x4F,
		RESULT__ALS_VAL = 0x50,
		RESULT__HISTORY_BUFFER_0 = 0x52,
		RESULT__HISTORY_BUFFER_1 = 0x53,
		RESULT__HISTORY_BUFFER_2 = 0x54,
		RESULT__HISTORY_BUFFER_3 = 0x55,
		RESULT__HISTORY_BUFFER_4 = 0x56,
		RESULT__HISTORY_BUFFER_5 = 0x57,
		RESULT__HISTORY_BUFFER_6 = 0x58,
		RESULT__HISTORY_BUFFER_7 = 0x59,
		RESULT__HISTORY_BUFFER_8 = 0x5A,
		RESULT__HISTORY_BUFFER_9 = 0x5B,
		RESULT__HISTORY_BUFFER_10 = 0x5C,
		RESULT__HISTORY_BUFFER_11 = 0x5D,
		RESULT__HISTORY_BUFFER_12 = 0x5E,
		RESULT__HISTORY_BUFFER_13 = 0x5F,
		RESULT__HISTORY_BUFFER_14 = 0x60,
		RESULT__RANGE_VAL = 0x62,
		RESULT__RANGE_RAW = 0x64,
		RESULT__RANGE_RETURN_RATE = 0x66,
		RESULT__RANGE_REFERENCE_RATE = 0x68,
		RESULT__RANGE_RETURN_SIGNAL_COUNT = 0x6C,
		RESULT__RANGE_REFERENCE_SIGNAL_COUNT = 0x70,
		RESULT__RANGE_RETURN_AMB_COUNT = 0x74,
		RESULT__RANGE_REFERENCE_AMB_COUNT = 0x78,
		RESULT__RANGE_RETURN_CONV_TIME = 0x7C,
		RESULT__RANGE_REFERENCE_CONV_TIME = 0x80,

		READOUT__AVERAGING_SAMPLE_PERIOD = 0x10A,
		FIRMWARE__BOOTUP = 0x119,
		FIRMWARE__RESULT_SCALER = 0x120,
		I2C_SLAVE__DEVICE_ADDRESS = 0x212,
		INTERLEAVED_MODE__ENABLE = 0x2A3,
	};

public:
	/// SYSTEM__MODE_GPIO0 and SYSTEM__MODE_GPIO1
	enum class
	GpioMode : uint8_t
	{
		IsMainShutdown = Bit6,	///< Priority mode - when enabled, other bits of the register are ignored. GPIO0 is main XSHUTDOWN input.
		Polarity = Bit5,		///< Signal Polarity Selection. 0: Active-low, 1: Active-high
		Select = Bit4,			///< Functional configuration options. 0: OFF (Hi-Z), 1: GPIO Interrupt output
	};
	XPCC_FLAGS8(GpioMode);

	/// SYSTEM__HISTORY_CTRL
	enum class
	HistoryControl : uint8_t
	{
		/// User-command to clear history (FW will auto-clear this bit when clear has completed). 0: Disabled, 1: Clear all history buffers
		BufferClear = Bit2,
		/// Select mode buffer results for: 0: Ranging (stores the last 16 ranging values (8-bit), 1: ALS (stores the last 8 ALS values (16-bit)
		BufferMode = Bit1,
		BufferEnable = Bit0,	///< Enable History buffering. 0: Disabled, 1: Enabled
	};
	XPCC_FLAGS8(HistoryControl);

	/// SYSTEM__INTERRUPT_CONFIG_GPIO
	enum class InterruptConfig {};
	typedef xpcc::Flags8<InterruptConfig> InterruptConfig_t;

	enum class
	InterruptSource : uint8_t
	{
		Disabled = 0,
		LevelLow = 1,		///< (value < thresh_low)
		LevelHigh = 2,		///< (value > thresh_high)
		OutOfWindow = 3,	///< (value < thresh_low  OR  value > thresh_high)
		NewSampleReady = 4,
	};
	typedef xpcc::Configuration<InterruptConfig_t, InterruptSource, 0x7> RangeInterruptSource_t;
	typedef xpcc::Configuration<InterruptConfig_t, InterruptSource, 0x7, 3> ALS_InterruptSource_t;

	/// SYSTEM__INTERRUPT_CLEAR
	enum class
	InterruptClear : uint8_t
	{
		Error = Bit2,
		ALS = Bit1,
		Range = Bit0,
	};
	XPCC_FLAGS8(InterruptClear);

	/// SYSRANGE__START
	/// SYSALS__START
	enum class
	Start : uint8_t
	{
		ModeSelect = Bit1,	///< Device Mode select. 0: Ranging Mode Single-Shot, 1: Ranging Mode Continuous
		/// StartStop trigger based on current mode and system configuration of device_ready. FW clears register automatically.
		/// Setting this bit to 1 in single-shot mode starts a single measurement.
		/// Setting this bit to 1 in continuous mode will either start continuous operation (if stopped) or halt continuous operation (if started).
		/// This bit is auto-cleared in both modes of operation.
		StartStop = Bit0,
	};
	XPCC_FLAGS8(Start);

	/// SYSRANGE__RANGE_CHECK_ENABLES
	enum class
	RangeCheck : uint8_t
	{
		SignalToNoise = Bit4,		///< 0: Disabled, 1: Enabled
		RangeIgnore = Bit1,			///< 0: Disabled, 1: Enabled
		EarlyConvergence = Bit0,	///< 0: Disabled, 1: Enabled
	};
	XPCC_FLAGS8(RangeCheck);

	/// SYSRANGE__VHV_RECALIBRATE
	enum class
	VhvRecalibrate : uint8_t
	{
		/// FW controlled status bit showing when FW has completed auto-vhv process.
		/// 0: FW has finished autoVHV operation, 1: During autoVHV operation
		Status = Bit1,
		/// User-Controlled enable bit to force FW to carry out recalibration of the VHV setting for sensor array.
		/// FW clears bit after operation carried out.
		/// 0: Disabled, 1: Manual trigger for VHV recalibration. Can only be called when ALS and ranging are in STOP mode
		Recalibrate = Bit0,
	};
	XPCC_FLAGS8(VhvRecalibrate);

	/// SYSALS__ANALOGUE_GAIN
	enum class
	AnalogGain : uint8_t
	{
		Gain1_0  = 0x46,
		Gain1_25 = 0x45,
		Gain1_67 = 0x44,
		Gain2_5  = 0x43,
		Gain5_0  = 0x42,
		Gain10   = 0x41,
		Gain20   = 0x40,
		Gain40   = 0x47,
	};

	/// RESULT__RANGE_STATUS
	enum class
	RangeStatus : uint8_t
	{
		/// When set to 1, indicates the device mode and configuration can be changed and a
		/// new start command will be accepted. When 0, indicates the device is busy.
		DeviceReady = Bit0,
	};
	XPCC_FLAGS8(RangeStatus);

	enum class
	RangeErrorCode : uint8_t
	{
		NoError = 0x00,
		VCSEL_ContinuityTest = 0x10,
		VCSEL_WatchdogTest = 0x20,
		VCSEL_Watchdog = 0x30,
		PLL1_Lock = 0x40,
		PLL2_Lock = 0x50,
		EarlyConvergenceEstimate = 0x60,
		MaxConvergence = 0x70,
		NoTargetIgnore = 0x80,
		MaxSignalToNoiseRatio = 0xB0,
		RawRangingAlgoUnderflow = 0xC0,
		RawRangingAlgoOverflow = 0xD0,
		RangingAlgoUnderflow = 0xE0,
		RangingAlgoOverflow = 0xF0,
	};
	typedef xpcc::Configuration<RangeStatus_t, RangeErrorCode, 0xF0> RangeErrorCode_t;

	/// RESULT__ALS_STATUS
	enum class
	ALS_Status : uint8_t
	{
		/// When set to 1, indicates the device mode and configuration can be changed and a
		/// new start command will be accepted. When 0, indicates the device is busy.
		DeviceReady = Bit0,
	};
	XPCC_FLAGS8(ALS_Status);

	enum class
	ALS_ErrorCode : uint8_t
	{
		NoError = 0x00,
		OverflowError = 0x10,
		UnderflowError = 0x20,
	};
	typedef xpcc::Configuration<ALS_Status_t, ALS_ErrorCode, 0xF0> ALS_ErrorCode_t;

	/// RESULT__INTERRUPT_STATUS_GPIO
	enum class InterruptStatus {};
	typedef xpcc::Flags8<InterruptStatus> InterruptStatus_t;

	typedef xpcc::Configuration<InterruptStatus_t, InterruptSource, 0x7> RangeInterruptStatus_t;
	typedef xpcc::Configuration<InterruptStatus_t, InterruptSource, 0x7, 3> ALS_InterruptStatus_t;

	enum class
	InterruptError : uint8_t
	{
		NoError = 0,
		LaserSafetyError = (1 << 6),	///< Run for your lives! (GET TO THE CHOPPER!)
		PLL_Error = (2 << 6),			///< either PLL1 or PLL2
	};
	typedef xpcc::Configuration<InterruptStatus_t, InterruptError, (3 << 6)> InterruptError_t;

public:
	using Control_t = xpcc::FlagsGroup<
			GpioMode_t,
			HistoryControl_t,
			InterruptConfig_t,
			InterruptClear_t,
			Start_t,
			RangeCheck_t,
			VhvRecalibrate_t,
			RangeStatus_t,
			ALS_Status_t,
			InterruptStatus_t>;

public:
	struct ATTRIBUTE_PACKED
	Data
	{
		template< class I2cMaster >
		friend class Vl6180;

		Data()
		:	data{255,0,0}, gain(6), time(100) {}

		/// @return the distance in millimeters, 255 if error
		inline uint8_t
		getDistance() { return data[0]; }

		/// @return the ambient light in calibrated LUX, values depend on chosen analog gain!
		inline float
		getAmbientLight()
		{
			uint16_t* rawData = reinterpret_cast<uint16_t*>(data+1);
			uint16_t value = xpcc::fromBigEndian(*rawData);
			float lux = (32.f * value);
			lux /= (vl6180_private::gain[gain & 0x7] * time);
			return lux;
		}

	private:
		uint8_t data[3];
		uint8_t gain;
		uint16_t time;
	};
}; // struct vl6180

/**
 * VL6180X: Proximity and ambient light sensing (ALS) module
 *
 * This implementation loads the configuration as recommended by the application note AN2424.
 * The class is build for single-shot measurements.
 *
 * For custom settings use the `updateRegister()` function.
 *
 * @author	Niklas Hauser
 * @ingroup driver_position
 */
template < typename I2cMaster >
class Vl6180 : public vl6180, public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	/// Constructor, requires an vl6180::Data object, sets address to default of 0x29
	Vl6180(Data &data, uint8_t address=0x29);

	xpcc::co::ResumableResult<bool>
	ping();

	xpcc::co::ResumableResult<bool>
	initialize();

	/// Set a new I2C address (< 128) for this device.
	/// The address is not permanent and must be set again after every device boot.
	xpcc::co::ResumableResult<bool>
	setDeviceAddress(uint8_t address);

	/// Sets a new analog gain for ALS.
	xpcc::co::ResumableResult<bool>
	setGain(AnalogGain gain);

	/// Sets a new integration time for ALS.
	/// @param	time	integration time in ms, max ~500ms.
	xpcc::co::ResumableResult<bool>
	setIntegrationTime(uint16_t time);

	/// Reads the distance and buffer the results (can take 10-55ms).
	xpcc::co::ResumableResult<bool>
	readDistance()
	{ return readSensor(true); }

	/// Reads the ambient light and buffer the results.
	/// This takes as long as the chosen integration time (100ms default).
	xpcc::co::ResumableResult<bool>
	readAmbientLight()
	{ return readSensor(false); }

	inline RangeErrorCode
	getRangeError()
	{ return rangeError; }

	inline ALS_ErrorCode
	getALS_Error()
	{ return alsError; }


	template <typename T>
	xpcc::co::ResumableResult<bool>
	updateRegister(Register reg, T setMask, T clearMask = T(0xff))
	{
		return updateControlRegister(reg, Control_t(setMask), Control_t(clearMask));
	}

public:
	/// write a 8bit value a register
	xpcc::co::ResumableResult<bool> ALWAYS_INLINE
	write(Register reg, uint8_t value)
	{ return write(reg, value, 1); }

protected:
	/// @cond
	/// read a 8bit value from a register
	xpcc::co::ResumableResult<bool> ALWAYS_INLINE
	read(Register reg, uint8_t &value)
	{ return read(reg, &value, 1); }
	/// @endcond

public:
	/// the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

protected:
	/// @cond
	/// write multiple 8bit values from a start register
	xpcc::co::ResumableResult<bool>
	write(Register reg, uint8_t value, uint8_t length);
	/// @endcond

public:
	/// read multiple 8bit values from a start register
	xpcc::co::ResumableResult<bool>
	read(Register reg, uint8_t *buffer, uint8_t length);

protected:
	/// @cond
	xpcc::co::ResumableResult<bool>
	updateControlRegister(Register reg, Control_t setMask, Control_t clearMask = Control_t(0xff));
	/// @endcond

private:
	xpcc::co::ResumableResult<bool>
	readSensor(bool isDistance = true);

private:
	Data &data;

	xpcc::ShortTimeout timeout;
	RangeErrorCode rangeError;
	ALS_ErrorCode alsError;

	// Internal write buffer
	// 0: Index[15:8]
	// 1: Index[7:0]
	// 2: Data[0]
	// 3: Data[1]
	uint8_t i2cBuffer[4];
	union
	{
		Register reg;
		uint8_t byte[2];
	} logicBuffer;
};

}	// namespace xpcc

#include "vl6180_impl.hpp"

#endif // XPCC_VL6180_HPP
