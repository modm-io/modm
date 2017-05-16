// coding: utf-8
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VL53L0_HPP
#define MODM_VL53L0_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/driver/accessor.hpp>
#include <modm/processing/processing.hpp>
#include <modm/utils/arithmetic_traits.hpp>

namespace modm
{

/// @cond
namespace vl53l0_private
{
	struct
	BinaryConfiguration
	{
		uint8_t reg;
		uint8_t value;
	};

	extern modm::accessor::Flash<vl53l0_private::BinaryConfiguration> configuration;
}	// namespace vl53l0_private
/// @endcond

// forward declaration for friending with vl53l0::Data
template < class I2cMaster >
class Vl53l0;

struct vl53l0
{
public:
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		SYSRANG_TART = 0x00,

		SYSTE_HRESH_HIGH = 0x0C,
		SYSTE_HRESH_LOW = 0x0E,
		SYSTE_EQUENCE_CONFIG = 0x01,
		SYSTE_ANGE_CONFIG = 0x09,
		SYSTE_NTERMEASUREMENT_PERIOD = 0x04,
		SYSTE_NTERRUPT_CONFIG_GPIO = 0x0A,
		SYSTE_NTERRUPT_CLEAR = 0x0B,
		SYSTE_ISTOGRAM_BIN = 0x81,

		GPI_V_MUX_ACTIVE_HIGH = 0x84,

		RESUL_NTERRUPT_STATUS = 0x13,
		RESUL_ANGE_STATUS = 0x14,
		RESUL_ANGE_VALUE0 = (0x14+10),
		RESUL_ANGE_VALUE1 = (0x14+11),
		RESUL_ORE_PAGE = 0x01,
		RESUL_ORE_AMBIENT_WINDOW_EVENTS_RTN = 0xBC,
		RESUL_ORE_RANGING_TOTAL_EVENTS_RTN = 0xC0,
		RESUL_ORE_AMBIENT_WINDOW_EVENTS_REF = 0xD0,
		RESUL_ORE_RANGING_TOTAL_EVENTS_REF = 0xD4,
		RESUL_EAK_SIGNAL_RATE_REF = 0xB6,

		ALG_ART_TO_PART_RANGE_OFFSET_MM = 0x28,
		//ALG_HASECAL_LIM = 0x0030, /* 0x0130 */ 
		ALG_HASECAL_CONFIG_TIMEOUT = 0x30,

		I2C_SLAV_EVICE_ADDRESS = 0x8A,

		MSR_ONFIG_CONTROL = 0x60,
		MSR_ONFIG_TIMEOUT_MACROP = 0x46,

		PRE_RANG_ONFIG_MIN_SNR = 0x27,
		PRE_RANG_ONFIG_VALID_PHASE_LOW = 0x56,
		PRE_RANG_ONFIG_VALID_PHASE_HIGH = 0x57,
		PRE_RANG_IN_COUNT_RATE_RTN_LIMIT = 0x64,
		PRE_RANG_ONFIG_SIGMA_THRESH_HI = 0x61,
		PRE_RANG_ONFIG_SIGMA_THRESH_LO = 0x62,
		PRE_RANG_ONFIG_VCSEL_PERIOD = 0x50,
		PRE_RANG_ONFIG_TIMEOUT_MACROP_HI = 0x51,
		PRE_RANG_ONFIG_TIMEOUT_MACROP_LO = 0x52,

		FINAL_RANG_ONFIG_MIN_SNR = 0x67,
		FINAL_RANG_ONFIG_VALID_PHASE_LOW = 0x47,
		FINAL_RANG_ONFIG_VALID_PHASE_HIGH = 0x48,
		FINAL_RANG_ONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,
		FINAL_RANG_ONFIG_VCSEL_PERIOD = 0x70,
		FINAL_RANG_ONFIG_TIMEOUT_MACROP_HI = 0x71,
		FINAL_RANG_ONFIG_TIMEOUT_MACROP_LO = 0x72,

		CROSSTALK_COMPENSATIO_EAK_RATE_MCPS = 0x20,

		HISTOGRA_ONFIG_INITIAL_PHASE_SELECT = 0x33,
		HISTOGRA_ONFIG_READOUT_CTRL = 0x55,

		SOFT_RESE_O2_SOFT_RESET_N = 0xBF,

		IDENTIFICATIO_ODEL_ID = 0xC0,
		IDENTIFICATIO_EVISION_ID = 0xC2,

		OS_ALIBRATE_VAL = 0xF8,

		GLOBA_ONFIG_VCSEL_WIDTH = 0x32,
		GLOBA_ONFIG_SPAD_ENABLES_REF_0 = 0xB0,
		GLOBA_ONFIG_SPAD_ENABLES_REF_1 = 0xB1,
		GLOBA_ONFIG_SPAD_ENABLES_REF_2 = 0xB2,
		GLOBA_ONFIG_SPAD_ENABLES_REF_3 = 0xB3,
		GLOBA_ONFIG_SPAD_ENABLES_REF_4 = 0xB4,
		GLOBA_ONFIG_SPAD_ENABLES_REF_5 = 0xB5,
		GLOBA_ONFIG_REF_EN_START_SELECT = 0xB6,
		
		DYNAMIC_SPA_UM_REQUESTED_REF_SPAD = 0x4E, /* 0x14E */
		DYNAMIC_SPA_EF_EN_START_OFFSET = 0x4F, /* 0x14F */

		POWER_MANAGEMEN_O1_POWER_FORCE = 0x80,

		VH_ONFIG_PAD_SCL_SD_XTSUP_HV = 0x89,

		UNDOCUMENTE_2C_MODE = 0x88,
	};

public:
	/// SYSRANG_TART
	enum class
	Start : uint8_t
	{
		/// StartStop trigger based on current mode and system configuration of device_ready. FW clears register automatically.
		/// Setting this bit to 1 in single-shot mode starts a single measurement.
		/// Setting this bit to 1 in continuous mode will either start continuous operation (if stopped) or halt continuous operation (if started).
		/// This bit is auto-cleared in both modes of operation.
		StartStop = Bit0,

		// bits 1-4: mode flags
		SingleShotMode = 0,
		ContinuousMode = Bit1,
		BackToBackMode = Bit2,
		TimedMode = Bit3,
		HistogramMode = Bit4,

		// Set this flag for vhv calibration
		VhvCalibrationMode = Bit6
	};
	MODM_FLAGS8(Start);


	/// SYSTE_EQUENCE_CONFIG
	enum class
	MeasurementSequenceStep : uint8_t
	{
		TCC = Bit4,
 		DSS = Bit3 | Bit5, // "dynamic spad selection"
		MSRC = Bit2, // "minimum signal rate check"
		PreRange = Bit6,
		FinalRange = Bit7,

		VhvCalibration = Bit0,
		PhaseCalibration = Bit1
	};
	MODM_FLAGS8(MeasurementSequenceStep);

	/// SYSTE_NTERRUPT_CONFIG_GPIO
	enum class
	InterruptConfig : uint8_t
	{
		Disabled = 0,
		LevelLow = 1,		///< (value < thresh_low)
		LevelHigh = 2,		///< (value > thresh_high)
		OutOfWindow = 3,	///< (value < thresh_low  OR  value > thresh_high)
		NewSampleReady = 4,
	};
	MODM_FLAGS8(InterruptConfig);

	/// SYSTE_NTERRUPT_CLEAR
	enum class
	InterruptClear : uint8_t
	{
		Range = Bit0,
	};
	MODM_FLAGS8(InterruptClear);

	// GPI_V_MUX_ACTIVE_HIGH
	enum class
	GpioConfig : uint8_t
	{
		InterruptPolarityHigh = Bit4
	};
	MODM_FLAGS8(GpioConfig);

	/// RESUL_NTERRUPT_STATUS_GPIO
	enum class
	InterruptStatus : uint8_t
	{
		Disabled = 0,
		LevelLow = 1,		///< (value < thresh_low)
		LevelHigh = 2,		///< (value > thresh_high)
		OutOfWindow = 3,	///< (value < thresh_low  OR  value > thresh_high)
		NewSampleReady = 4,
	};
	MODM_FLAGS8(InterruptStatus);
	
	/// RESUL_ANGE_STATUS
	enum class
	RangeStatus : uint8_t
	{
		// When set to 0, the device is busy
		DeviceReady = Bit0,
	};
	MODM_FLAGS8(RangeStatus);

	enum class
	RangeErrorCode : uint8_t
	{
		NoError = 0x0,
		VCSEL_ContinuityTest = 0x1,
		VCSEL_WatchdogTest = 0x2,
		VHV_NoValueFound = 0x3,
		MSRC_NoTarget = 0x4, 
		SNR_Check = 0x5,
		RangePhaseCheck = 0x6,
		SigmaThresholdCheck = 0x7,
		TCC = 0x8,
		PhaseConsistency = 0x9,
		MinClip = 0xA,
		RangeComplete = 0xB,
		RangingAlgoUnderflow = 0xC,
		RangingAlgoOverflow = 0xD,
		RangeIgnoreThreshold = 0xE,
	};
	typedef modm::Configuration<RangeStatus_t, RangeErrorCode, 0x0F, 3> RangeErrorCode_t;

	/// MSR_ONFIG_CONTROL
	/// MSRC = "minimum signal rate check"
	enum class
	MSRCConfig : uint8_t
	{
		DisableRateCheck = Bit1,
 		DisablePreRangeCheck = Bit4,
	};
	MODM_FLAGS8(MSRCConfig);

public:
	using Control_t = modm::FlagsGroup<
			Start_t,
			MeasurementSequenceStep_t,
			InterruptConfig_t,
			InterruptClear_t,
			GpioConfig_t,
			InterruptStatus_t,
			RangeStatus_t,
			MSRCConfig_t>;

	static constexpr uint8_t ModelID[2] = { 0xEE, 0xAA };
	static constexpr uint8_t RevisionID = 0x10;

	// 30000 us = 30 ms
	static constexpr uint32_t DefaultMeasurementTime = 30000;

	// required signal rate per time period (unknown unit)
	// this is the default value used by ST and will probably work fine
	static constexpr uint16_t DefaultSignalRateLimit = 32;

	struct TimeOverhead
	{
		// in microseconds
		static constexpr uint16_t Start = 1910;
		static constexpr uint16_t End = 960;
		static constexpr uint16_t MSRC = 660;
		static constexpr uint16_t TCC = 590;
		static constexpr uint16_t DSS = 690;
		static constexpr uint16_t PreRange = 660;
		static constexpr uint16_t FinalRange = 550; 
	};

public:
	struct modm_packed
	Data
	{
		template< class I2cMaster >
		friend class Vl53l0;

		static constexpr uint16_t InvalidDistance = 0x1FFE;

		Data() { reset(); }

		/// @return the distance in millimeters
		inline uint16_t
		getDistance()
		{
			return (static_cast<uint16_t>(distanceBuffer[0]) << 8) + distanceBuffer[1];
		}

		inline bool
		isValid()
		{
			if((getDistance() & InvalidDistance) == InvalidDistance) {
				return false;
			}

			return (error == RangeErrorCode::RangeComplete
				|| error == RangeErrorCode::NoError);
		}
		
		inline RangeErrorCode
		getRangeError() { return error; }

		inline void
		reset()
		{
			distanceBuffer[0] = (InvalidDistance & 0xFF00) >> 8;
			distanceBuffer[1] = InvalidDistance & 0xFF;
			error = RangeErrorCode::NoError;
		}
	private:
		uint8_t distanceBuffer[2];
		RangeErrorCode error;
	};
}; // struct vl53l0

/**
 * VL53L0X: Proximity sensor module
 *
 * The class is build for single-shot measurements.
 *
 * @author	Christopher Durand
 * @ingroup driver_position
 */
template < typename I2cMaster >
class Vl53l0 : public vl53l0, public modm::I2cDevice< I2cMaster, 5 >
{
public:
	/// Constructor, requires an vl53l0::Data object, sets address to default of 0x29
	Vl53l0(Data &data, uint8_t address=0x29);

	/// Ping the I2C device.
	/// This overwrites the build-in ping method of I2cDevice with a read of the model number.
	modm::ResumableResult<bool>
	ping();

	modm::ResumableResult<bool>
	initialize();

	/// Set a new I2C address (< 128) for this device.
	/// The address is not permanent and must be set again after every device boot.
	modm::ResumableResult<bool>
	setDeviceAddress(uint8_t address);

	/// Reads the distance and buffers the result
	inline modm::ResumableResult<bool>
	readDistance()
	{ return readSensor(); }

	inline RangeErrorCode
	getRangeError()
	{ return data.error; }

	template <typename T>
	modm::ResumableResult<bool>
	updateRegister(Register reg, T setMask, T clearMask = T(0xff))
	{
		return updateControlRegister(reg, Control_t(setMask), Control_t(clearMask));
	}

	/// the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

	/// Set the maximum measurement time.
	///
	/// Increasing this value will improve the accuracy.
	/// The default is ~30ms. ST recommends 2000000us = 200ms for high precision.
	///
	/// useful range of values: ~20000us - 2000000us (20ms - 2s)
	modm::ResumableResult<bool>
	setMaxMeasurementTime(uint32_t timeUs);

	/// Get the configured maximum measurement time.
	inline uint32_t
	getMaxMeasurementTime()
	{ return measurementTimeUs; }
protected:
	/// @cond
	/// read multiple 8bit values from subsequent registers
	modm::ResumableResult<bool>
	read(Register reg, uint8_t *buffer, uint8_t length);
	/// @endcond

	/// @cond
	/// read an 8bit value from a register
	modm::ResumableResult<bool> modm_always_inline
	read(Register reg, uint8_t& value)
	{ return read(reg, &value, 1); }
	/// @endcond

	/// @cond
	/// write to a control register
	modm::ResumableResult<bool> modm_always_inline
	write(Register reg, Control_t value)
	{ return write(reg, value.value); }
	/// @endcond

	/// @cond
	/// write an 8bit value to a register
	modm::ResumableResult<bool>
	write(Register reg, uint8_t value);
	/// @endcond

	/// @cond
	/// write a 16bit value to 2 subsequent registers in big endian format
	modm::ResumableResult<bool>
	writeUInt16(Register reg, uint16_t value);
	/// @endcond

	/// @cond
	modm::ResumableResult<bool>
	updateControlRegister(Register reg, Control_t setMask, Control_t clearMask = Control_t(0xff));
	/// @endcond

	/// @cond
	/// periodically read an 8 bit value from a register until the supplied
	/// predicate returns true
	///
	/// the predicate must take a uint8_t argument and return a bool
	template < typename Predicate >
	modm::ResumableResult<bool>
	poll(Register reg, Predicate pred, uint16_t timeoutMs = 20, uint16_t stepMs = 4);
	/// @endcond
private:
	/// @cond
	/// write multiple 8bit values from i2cBuffer[1..6] to
	/// a sequence of registers
	///
	/// The maximum data length is 6 since i2cBuffer[0]
	/// will contain the register address.
	modm::ResumableResult<bool>
	writeI2CBuffer(Register reg, uint8_t dataLength);
	/// @endcond

	modm::ResumableResult<bool>
	readSensor();

	modm::ResumableResult<bool>
	checkModelID();

	modm::ResumableResult<bool>
	checkRevisionID();

	modm::ResumableResult<bool>
	loadTuningSettings();

	// SPAD = "single photon avalanche diode"
	modm::ResumableResult<bool>
	initializeSpadConfig();

	modm::ResumableResult<bool>
	performReferenceCalibration(Start_t modeFlags = Start_t(0));

	modm::ResumableResult<bool>
	readSequenceInfo();

private:
	bool
	setupReferenceSpadMap(const uint8_t spadMapIn[6], uint8_t spadMapOut[6]);

	bool
	calculateFinalRangeTimeout(uint32_t maxMeasurementTime, uint16_t& registerValue);

	uint32_t
	timeoutToMicroseconds(uint16_t mclks, uint8_t vcselPeriodPclks);

	uint32_t
	timeoutToMclks(uint32_t microseconds, uint8_t vcselPeriodPclks);

	static uint16_t
	encodeTimeout(uint32_t timeoutMclks);

	static uint32_t
	decodeTimeout(uint16_t registerValue);
private:
	// Maximum argument value for setMaxMeasurementTime()
	// Its only purpose is to prevent int overflows.
	// The maximum value the sensor accepts is probably lower (~ 2*10^6 us = 2s).
	static constexpr uint32_t MaxMeasurementTimeUs = 4000000;

	Data &data;

	modm::ShortTimeout timeout;

	// Internal I2C write buffer
	// 0: Index[7:0]
	// 1: Data[0]
	// 2: Data[1]
	// ...
	// 7: Data[6]
	uint8_t i2cBuffer[7];

	uint16_t index;

	// This value is read during initialization and is needed to
	// start a measurement.
	uint8_t stopMode;

	uint32_t measurementTimeUs;

	// Use a union to save memory
	// spadInfo will only be used by initializeSpadConfig() during initialization
	union
	{
		struct
		{
			uint16_t preRangeTimeout;
			uint16_t finalRangeTimeout;
			uint8_t msrcDssTccTimeout;

			uint8_t vcselPeriodPreRange;
			uint8_t vcselPeriodFinalRange;

			MeasurementSequenceStep enabledSteps;
		} sequenceInfo;

		// SPAD = "single photon avalanche diode"
		// This data is needed initialize the spad map registers
		struct
		{
			uint8_t map[6];

			uint8_t referenceSpadCount : 7;
			uint8_t useApertureSpads : 1;
		} spadInfo;
	};
};

}	// namespace modm

#include "vl53l0_impl.hpp"

#endif // MODM_VL53L0_HPP
