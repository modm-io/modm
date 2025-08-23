// coding: utf-8
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VL53L0_HPP
#	error "Don't include this file directly, use 'vl53l0.hpp' instead!"
#endif

#include <modm/debug/logger.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Vl53l0<I2cMaster>::Vl53l0(Data &data, uint8_t address)
:	I2cDevice<I2cMaster>{address}, data{data},
	i2cBuffer{0,0,0,0,0,0,0}, index{0}, measurementTimeUs{DefaultMeasurementTime}
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::ping()
{
	if (not checkModelID()) return false;

	return checkRevisionID();
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::reset()
{
	// Set Reset
	if (not write(Register::SOFT_RESET__GO2_SOFT_RESET_N, 0x00)) return false;

	// Wait until the device is in reset
	if (not poll(Register::IDENTIFICATION__MODEL_ID, [](uint8_t value) {
		return value == 0x00;
	})) return false;

	// Release Reset
	if (not write(Register::SOFT_RESET__GO2_SOFT_RESET_N, 0x01)) return false;

	// Wait until the device responds again
	// After releasing reset it does not accept any I2C transactions
	// for some time and will respond with NACKs
	timeout.restart(500ms);
	modm::this_fiber::poll([&]{ return ping() or timeout.isExpired(); });

	return not timeout.isExpired();
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::initialize()
{
	using namespace vl53l0_private;

	// "Disable MSRC and TCC by default"
	// MSRC = Minimum Signal Rate Check
	// TCC = Target Center Check
	constexpr MeasurementSequenceStep_t standardSequence = MeasurementSequenceStep::DSS
		| MeasurementSequenceStep::PreRange
		| MeasurementSequenceStep::FinalRange;

	constexpr MSRCConfig_t msrcConfig = MSRCConfig::DisableRateCheck
		| MSRCConfig::DisablePreRangeCheck;

	// read the measurement sequence configuration into this->sequenceInfo
	if (not readSequenceInfo()) return false;

	// "Set I2C standard mode"
	if (not write(Register::UNDOCUMENTED__I2C_MODE, 0x00)) return false;

	// magic is happening here ...
	if (not write(Register(0x80), 0x01)) return false;
	if (not write(Register(0xFF), 0x01)) return false;
	if (not write(Register(0x00), 0x00)) return false;
	if (not read(Register(0x91), stopMode)) return false;
	if (not write(Register(0x00), 0x01)) return false;
	if (not write(Register(0xFF), 0x00)) return false;
	if (not write(Register(0x80), 0x00)) return false;

	if (not updateControlRegister(Register::MSRC__CONFIG_CONTROL, msrcConfig, Control_t(0))) return false;

	if (not writeUInt16(Register::FINAL_RANGE__CONFIG_MIN_COUNT_RATE_RTN_LIMIT, DefaultSignalRateLimit)) return false;

	// finish "data init" phase
	if (not write(Register::SYSTEM__SEQUENCE_CONFIG, 0xFF)) return false;

	// load SPAD calibration data from NVM and initialize dynamic SPAD configuration
	if (not initializeSpadConfig()) return false;

	if (not loadTuningSettings()) return false;

	// enable "new sample ready" interrupt flag
	if (not write(Register::SYSTEM__INTERRUPT_CONFIG_GPIO, InterruptConfig::NewSampleReady)) return false;
	if (not updateControlRegister(Register::GPIO__HV_MUX_ACTIVE_HIGH,
										 Control_t(0), GpioConfig::InterruptPolarityHigh)) return false;
	if (not write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range)) return false;

	// set default measurement sequence (TCC and MSRC disabled)
	if (not write(Register::SYSTEM__SEQUENCE_CONFIG, standardSequence)) return false;

	// recalculate measurement timings
	if (not setMaxMeasurementTime(this->measurementTimeUs)) return false;

	// VHV calibration
	if (not write(Register::SYSTEM__SEQUENCE_CONFIG, MeasurementSequenceStep::VhvCalibration)) return false;
	if(not performReferenceCalibration(Start::VhvCalibrationMode)) {
		MODM_LOG_ERROR << "VHV calibration failed." << modm::endl;
		return false;
	}

	// phase calibration
	if (not write(Register::SYSTEM__SEQUENCE_CONFIG, MeasurementSequenceStep::PhaseCalibration)) return false;
	if(not performReferenceCalibration()) {
		MODM_LOG_ERROR << "Phase calibration failed." << modm::endl;
		return false;
	}

	// restore measurement sequence settings
	if (not write(Register::SYSTEM__SEQUENCE_CONFIG, standardSequence)) return false;

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::loadTuningSettings()
{
	using namespace vl53l0_private;

	// write the configuration
	for(index = 0; index < 80; index++)
	{
		i2cBuffer[1] = configuration[index].value;
		if (not write(Register(configuration[index].reg), i2cBuffer[1])) return false;
	}

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::readSensor()
{
	static_assert(MaxMeasurementTimeUs / 1000 * 2 <= std::numeric_limits<uint16_t>::max(),
				"MaxMeasurementTimeUs out of range");

	data.reset();

	// undocumented black magic
	if (not write(Register(0x80), 0x01)) return false;
	if (not write(Register(0xFF), 0x01)) return false;
	if (not write(Register(0x00), 0x00)) return false;
	if (not write(Register(0x91), stopMode)) return false;
	if (not write(Register(0x00), 0x01)) return false;
	if (not write(Register(0xFF), 0x00)) return false;
	if (not write(Register(0x80), 0x00)) return false;

	// start range measurement
	if (not write(Register::SYSRANGE__START, Start::StartStop)) return false;

	// wait for the start flag to be cleared, use default timeout
	if (not poll(Register::SYSRANGE__START, [](uint8_t value) {
		return not (value & uint8_t(Start::StartStop));
	})) return false;

	// wait for the measurement to finish, timeout of 2 * this->measurementTimeUs
	if (not poll(Register::RESULT__INTERRUPT_STATUS, [](uint8_t value) {
		return value & (InterruptStatus::NewSampleReady | InterruptStatus::OutOfWindow).value;
	}, measurementTimeUs / 1000 * 2)) return false;

	// read 16 bit range value
	if (not read(Register::RESULT__RANGE_VALUE0, &i2cBuffer[1], 2)) return false;
	data.distanceBuffer[0] = i2cBuffer[1];
	data.distanceBuffer[1] = i2cBuffer[2];

	// clear interrupt flag
	if (not write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range)) return false;

	// read error code
	if (not read(Register::RESULT__RANGE_STATUS, i2cBuffer[1])) return false;
	data.error = RangeErrorCode_t::get(RangeStatus_t(i2cBuffer[1]));

	return true;
}

// MARK: setMaxMeasurementTime
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::setMaxMeasurementTime(uint32_t timeUs)
{
	if(timeUs > MaxMeasurementTimeUs) {
		MODM_LOG_ERROR << "Measurement time out of range." << modm::endl;
		return false;
	}

	// read the measurement sequence configuration into this->sequenceInfo
	if (not readSequenceInfo()) return false;

	if(not calculateFinalRangeTimeout(timeUs, sequenceInfo.finalRangeTimeout)) {
		MODM_LOG_ERROR << "Invalid timing requested, aborting." << modm::endl;
		return false;
	}

	if (not writeUInt16(Register::FINAL_RANGE__CONFIG_TIMEOUT_MACROP_HI, sequenceInfo.finalRangeTimeout)) return false;

	this->measurementTimeUs = timeUs;

	return true;
}

// MARK: checkModelID
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::checkModelID()
{
	if (not read(Register::IDENTIFICATION__MODEL_ID, &i2cBuffer[1], 2)) return false;

	return (i2cBuffer[1] == ModelID[0]) and (i2cBuffer[2] == ModelID[1]);
}

// MARK: checkRevisionID
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::checkRevisionID()
{
	if (not read(Register::IDENTIFICATION__REVISION_ID, i2cBuffer[1])) return false;

	return i2cBuffer[1] == RevisionID;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::setDeviceAddress(uint8_t address)
{
	if(write(Register::I2C_SLAVE__DEVICE_ADDRESS, (address & 0x7F)))
	{
		this->setAddress(address);
		return true;
	}

	return false;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::initializeSpadConfig()
{
	// read number and type of SPADs to be used for calibration
	if (not write(Register(0x80), 0x01)) return false;
	if (not write(Register(0xFF), 0x01)) return false;
	if (not write(Register(0x00), 0x00)) return false;
	if (not write(Register(0xFF), 0x06)) return false;
	if (not updateControlRegister(Register(0x83), Control_t(4), Control_t(0))) return false;
	if (not write(Register(0xFF), 0x07)) return false;
	if (not write(Register(0x81), 0x01)) return false;
	if (not write(Register(0x80), 0x01)) return false;

	if (not write(Register(0x94), 0x6b)) return false;
	if (not write(Register(0x83), 0x00)) return false;

	if (not poll(Register(0x83), [](uint8_t value) {
		return value != 0x00;
	})) return false;

	if (not write(Register(0x83), 0x01)) return false;
	if (not read(Register(0x92), i2cBuffer[1])) return false;

	spadInfo.referenceSpadCount = i2cBuffer[1] & 0x7F;
	spadInfo.useApertureSpads = (i2cBuffer[1] & (1 << 7)) != 0;

	if (not write(Register(0x81), 0x00)) return false;
	if (not write(Register(0xFF), 0x06)) return false;
	if (not updateControlRegister(Register(0x83), Control_t(0), Control_t(4))) return false;
	if (not write(Register(0xFF), 0x01)) return false;
	if (not write(Register(0x00), 0x01)) return false;
	if (not write(Register(0xFF), 0x00)) return false;
	if (not write(Register(0x80), 0x00)) return false;

	// read map of SPADs available for reference calibration
	if (not read(Register::GLOBAL__CONFIG_SPAD_ENABLES_REF_0, spadInfo.map, 6)) return false;

	// prepare setting SPAD config
	if (not write(Register(0xFF), 0x01)) return false;
	if (not write(Register::DYNAMIC_SPAD__REF_EN_START_OFFSET, 0x00)) return false;
	if (not write(Register::DYNAMIC_SPAD__NUM_REQUESTED_REF_SPAD, 0x2C)) return false;
	if (not write(Register(0xFF), 0x00)) return false;
	if (not write(Register::GLOBAL__CONFIG_REF_EN_START_SELECT, 0xB4)) return false;

	if(not setupReferenceSpadMap(spadInfo.map, &i2cBuffer[1]))
	{
		MODM_LOG_ERROR << "Invalid SPAD data in non-volatile memory.\n";
		MODM_LOG_ERROR << "A full 'SPAD management' recalibration has to be performed.\n";
		MODM_LOG_ERROR << "This procedure is not implemented in this driver.\n";
		MODM_LOG_ERROR << "Please use the VL53L0X API provided by ST." << modm::endl;

		return false;
	}

	// Write SPAD config to the device. This will not be written to NVM.
	if (not writeI2CBuffer(Register::GLOBAL__CONFIG_SPAD_ENABLES_REF_0, 6)) return false;

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::performReferenceCalibration(Start_t mode)
{
	static_assert(MaxMeasurementTimeUs / 1000 * 2 <= std::numeric_limits<uint16_t>::max(),
				"MaxMeasurementTimeUs out of range");

	// start measurement
	if (not write(Register::SYSRANGE__START, Start::StartStop | mode)) return false;

	// wait for the measurement to finish
	if (not poll(Register::RESULT__INTERRUPT_STATUS, [](uint8_t value) {
		return value & (InterruptStatus::NewSampleReady | InterruptStatus::OutOfWindow).value;
	}, measurementTimeUs / 1000 * 2)) return false;

	// clear interrupt flags
	if (not write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range)) return false;
	if (not write(Register::SYSRANGE__START, Start::SingleShotMode)) return false;

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::setupReferenceSpadMap(const uint8_t availableSpadMap[6], uint8_t calibrationSpadMap[6])
{
	uint8_t spadCount = spadInfo.referenceSpadCount;

	// SPAD index 0-11: non-aperture SPADs, 12-43: aperture SPADs
	const uint8_t offset = spadInfo.useApertureSpads ? 12 : 0;

	// reject invalid settings in non-volatile memory
	if(spadInfo.useApertureSpads and (spadInfo.referenceSpadCount > 32)) {
		return false;
	} else if((not spadInfo.useApertureSpads) and (spadInfo.referenceSpadCount > 12)) {
		return false;
	}

	for(int8_t spadIndex = 0; spadIndex < 48; ++spadIndex)
	{
		const uint8_t byteIndex = spadIndex / 8;
		const uint8_t bitIndex = spadIndex % 8;

		if(spadIndex < offset || spadCount == 0) {
			calibrationSpadMap[byteIndex] &= ~(1 << bitIndex);
		} else if(availableSpadMap[byteIndex] & (1 << bitIndex)) {
			calibrationSpadMap[byteIndex] |= (1 << bitIndex);
			--spadCount;
		}
	}

	// error: less SPADs available than requested
	if(spadCount > 0) {
		return false;
	}

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::readSequenceInfo()
{
	// read vcsel periods
	if (not read(Register::PRE_RANGE__CONFIG_VCSEL_PERIOD, sequenceInfo.vcselPeriodPreRange)) return false;
	if (not read(Register::FINAL_RANGE__CONFIG_VCSEL_PERIOD, sequenceInfo.vcselPeriodFinalRange)) return false;

	// decode vcsel period values
	sequenceInfo.vcselPeriodPreRange += 1;
	sequenceInfo.vcselPeriodPreRange <<= 1;
	sequenceInfo.vcselPeriodFinalRange += 1;
	sequenceInfo.vcselPeriodFinalRange <<= 1;

	if (not read(Register::SYSTEM__SEQUENCE_CONFIG, i2cBuffer[1])) return false;
	sequenceInfo.enabledSteps = static_cast<MeasurementSequenceStep>(i2cBuffer[1]);

	if (not read(Register::MSRC__CONFIG_TIMEOUT_MACROP, sequenceInfo.msrcDssTccTimeout)) return false;
	sequenceInfo.msrcDssTccTimeout += 1;

	if (not read(Register::PRE_RANGE__CONFIG_TIMEOUT_MACROP_HI, &i2cBuffer[1], 2)) return false;
	sequenceInfo.preRangeTimeout = decodeTimeout((i2cBuffer[1] << 8) + i2cBuffer[2]);

	if (not read(Register::FINAL_RANGE__CONFIG_TIMEOUT_MACROP_HI, &i2cBuffer[1], 2)) return false;
	sequenceInfo.finalRangeTimeout = decodeTimeout((i2cBuffer[1] << 8) + i2cBuffer[2]);

	return true;
}

template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::calculateFinalRangeTimeout(uint32_t measurementTime, uint16_t& registerValue)
{
	if(measurementTime < TimeOverhead::Start + TimeOverhead::End) {
		return false;
	}

	// available time for "final range" step in microseconds
	uint32_t finalRangeTime = measurementTime - TimeOverhead::Start - TimeOverhead::End;
	const uint16_t msrcDssTccTimeout = timeoutToMicroseconds(sequenceInfo.msrcDssTccTimeout,
															 sequenceInfo.vcselPeriodPreRange);

	if(sequenceInfo.enabledSteps & MeasurementSequenceStep::TCC)
	{
		const uint16_t stepTime = TimeOverhead::TCC + msrcDssTccTimeout;
		if(finalRangeTime <= stepTime) {
			return false;
		}
		finalRangeTime -= stepTime;
	}

	// "dynamic spad selection" is enabled
	if(sequenceInfo.enabledSteps & MeasurementSequenceStep::DSS)
	{
		const uint16_t stepTime = 2 * (TimeOverhead::DSS + msrcDssTccTimeout);
		if(finalRangeTime <= stepTime) {
			return false;
		}

		finalRangeTime -= stepTime;

	} // additional time for msrc is only needed if dss is disabled
	else if(sequenceInfo.enabledSteps & MeasurementSequenceStep::MSRC)
	{
		const uint16_t stepTime = TimeOverhead::MSRC + msrcDssTccTimeout;
		if(finalRangeTime <= stepTime) {
			return false;
		}

		finalRangeTime -= stepTime;
	}

	if(sequenceInfo.enabledSteps & MeasurementSequenceStep::PreRange)
	{
		const uint16_t stepTime = TimeOverhead::PreRange
			+ timeoutToMicroseconds(sequenceInfo.preRangeTimeout, sequenceInfo.vcselPeriodPreRange);

		if(finalRangeTime <= stepTime) {
			return false;
		}

		finalRangeTime -= stepTime;
	}

	if(finalRangeTime <= TimeOverhead::FinalRange) {
		return false;
	}

	finalRangeTime -= TimeOverhead::FinalRange;

	// convert value from microseconds to mclks
	// from now on finalRangeTime is a value in mclks
	finalRangeTime = timeoutToMclks(finalRangeTime, sequenceInfo.vcselPeriodFinalRange);

	if(sequenceInfo.enabledSteps & MeasurementSequenceStep::PreRange) {
		finalRangeTime += sequenceInfo.preRangeTimeout;
	}

	registerValue = encodeTimeout(finalRangeTime);
	return true;
}

template < typename I2cMaster >
uint32_t
modm::Vl53l0<I2cMaster>::timeoutToMicroseconds(uint16_t mclks, uint8_t vcselPeriodPclks)
{
	// in nanoseconds
	const uint32_t macroPeriodNs = ((UINT32_C(2304) * vcselPeriodPclks * 1655) + 500) / 1000;

	return ((mclks * macroPeriodNs) + 500) / 1000;
}

template < typename I2cMaster >
uint32_t
modm::Vl53l0<I2cMaster>::timeoutToMclks(uint32_t microseconds, uint8_t vcselPeriodPclks)
{
	const uint32_t macroPeriodNs = ((UINT32_C(2304) * vcselPeriodPclks * 1655) + 500) / 1000;

	return (((microseconds * 1000) + (macroPeriodNs / 2)) / macroPeriodNs);
}

template < typename I2cMaster >
uint16_t
modm::Vl53l0<I2cMaster>::encodeTimeout(uint32_t timeoutMclks)
{
	// Decompose timeoutMclks into the following expression:
	// coefficient*(2^exponent) + 1

	uint16_t encodedValue = 0;
	uint32_t coefficient = 0;
	uint16_t exponent = 0;

	if(timeoutMclks > 0)
	{
		coefficient = timeoutMclks - 1;

		while((coefficient & 0xFFFFFF00) > 0) {
			coefficient >>= 1;
			exponent++;
		}

		encodedValue = (exponent << 8) + static_cast<uint16_t>(coefficient & 0x00FF);
	}

	return encodedValue;
}

template < typename I2cMaster >
uint32_t
modm::Vl53l0<I2cMaster>::decodeTimeout(uint16_t registerValue)
{
	const uint8_t coefficient = registerValue & 0xFF;
	const uint8_t exponent = (registerValue & 0xFF00) >> 8;

	// format: coefficient*(2^exponent) + 1
	return (static_cast<uint32_t>(coefficient) << exponent) + 1;
}


// ----------------------------------------------------------------------------
// MARK: update register
template < typename I2cMaster >
bool
modm::Vl53l0<I2cMaster>::updateControlRegister(Register reg, Control_t setMask, Control_t clearMask)
{
	if(clearMask.value != 0xFF) {
		if(!read(reg, i2cBuffer[1])) {
			return false;
		}
	}
	i2cBuffer[1] = (i2cBuffer[1] & ~clearMask.value) | setMask.value;

	return write(reg, i2cBuffer[1]);
}

// MARK: write multilength register
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::write(Register reg, uint8_t value)
{
	i2cBuffer[1] = value;

	return writeI2CBuffer(reg, 1);
}

// MARK: write multilength register
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::writeI2CBuffer(Register reg, uint8_t dataLength)
{
	if(dataLength >= sizeof(i2cBuffer)) {
		return false;
	}

	i2cBuffer[0] = uint8_t(reg);
	this->transaction.configureWrite(i2cBuffer, dataLength + 1);

	return this->runTransaction();
}

template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::writeUInt16(Register reg, uint16_t value)
{
	i2cBuffer[0] = uint8_t(reg);
	i2cBuffer[1] = (value & 0xFF00) >> 8;
	i2cBuffer[2] = value & 0xFF;

	this->transaction.configureWrite(i2cBuffer, 3);

	return this->runTransaction();
}

// MARK: read multilength register
template < class I2cMaster >
bool
modm::Vl53l0<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	i2cBuffer[0] = uint8_t(reg);

	this->transaction.configureWriteRead(i2cBuffer, 1, buffer, length);

	return this->runTransaction();
}

template < class I2cMaster >
template < typename Predicate >
bool
modm::Vl53l0<I2cMaster>::poll(Register reg, Predicate pred, const uint16_t timeoutMs, const uint16_t stepMs)
{
	index = timeoutMs;

	while(true)
	{
		if(not read(reg, i2cBuffer[1])) {
			break;
		}

		if(pred(i2cBuffer[1])) {
			return true;
		}

		if(index == 0) {
			MODM_LOG_ERROR << "Timeout after " << timeoutMs << " msec" << modm::endl;
			break;
		}

		timeout.restart(std::chrono::milliseconds(stepMs));
		modm::this_fiber::poll([&]{ return timeout.isExpired(); });

		if(index >= stepMs) {
			index -= stepMs;
		} else {
			index = 0;
		}
	}

	return false;
}
