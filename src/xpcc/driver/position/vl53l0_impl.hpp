// coding: utf-8
/* Copyright (c) 2017, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_VL53L0_HPP
#	error "Don't include this file directly, use 'vl53l0.hpp' instead!"
#endif

#include <xpcc/debug/logger.hpp>

#define VL53L0_RF_CALL(rf) if(not RF_CALL(rf)) { RF_RETURN(false); }

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Vl53l0<I2cMaster>::Vl53l0(Data &data, uint8_t address)
:	I2cDevice<I2cMaster, 5>{address}, data{data},
	i2cBuffer{0,0,0,0,0,0,0}, index{0}, measurementTimeUs{DefaultMeasurementTime}
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::ping()
{
	RF_BEGIN();

	VL53L0_RF_CALL(checkModelID());

	RF_END_RETURN_CALL(checkRevisionID());
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::initialize()
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

	RF_BEGIN();

	// read the measurement sequence configuration into this->sequenceInfo
	VL53L0_RF_CALL(readSequenceInfo());

	// "Set I2C standard mode"
	VL53L0_RF_CALL(write(Register::UNDOCUMENTED__I2C_MODE, 0x00));

	// magic is happening here ...
	VL53L0_RF_CALL(write(Register(0x80), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x01));
	VL53L0_RF_CALL(write(Register(0x00), 0x00));
	VL53L0_RF_CALL(read(Register(0x91), stopMode));
	VL53L0_RF_CALL(write(Register(0x00), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x00));
	VL53L0_RF_CALL(write(Register(0x80), 0x00));

	VL53L0_RF_CALL(updateControlRegister(Register::MSRC__CONFIG_CONTROL, msrcConfig, Control_t(0)));

	VL53L0_RF_CALL(writeUInt16(Register::FINAL_RANGE__CONFIG_MIN_COUNT_RATE_RTN_LIMIT, DefaultSignalRateLimit));

	// finish "data init" phase
	VL53L0_RF_CALL(write(Register::SYSTEM__SEQUENCE_CONFIG, 0xFF));

	// load SPAD calibration data from NVM and initialize dynamic SPAD configuration
	VL53L0_RF_CALL(initializeSpadConfig());

	VL53L0_RF_CALL(loadTuningSettings());

	// enable "new sample ready" interrupt flag
	VL53L0_RF_CALL(write(Register::SYSTEM__INTERRUPT_CONFIG_GPIO, InterruptConfig::NewSampleReady));
	VL53L0_RF_CALL(updateControlRegister(Register::GPIO__HV_MUX_ACTIVE_HIGH,
										 Control_t(0), GpioConfig::InterruptPolarityHigh));
	VL53L0_RF_CALL(write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range));

	// set default measurement sequence (TCC and MSRC disabled)
	VL53L0_RF_CALL(write(Register::SYSTEM__SEQUENCE_CONFIG, standardSequence));

	// recalculate measurement timings
	VL53L0_RF_CALL(setMaxMeasurementTime(this->measurementTimeUs));

	// VHV calibration
	VL53L0_RF_CALL(write(Register::SYSTEM__SEQUENCE_CONFIG, MeasurementSequenceStep::VhvCalibration));
	if(not RF_CALL(performReferenceCalibration(Start::VhvCalibrationMode))) {
		XPCC_LOG_ERROR << "VHV calibration failed." << xpcc::endl;
		RF_RETURN(false);
	}

	// phase calibration
	VL53L0_RF_CALL(write(Register::SYSTEM__SEQUENCE_CONFIG, MeasurementSequenceStep::PhaseCalibration));
	if(not RF_CALL(performReferenceCalibration())) {
		XPCC_LOG_ERROR << "Phase calibration failed." << xpcc::endl;
		RF_RETURN(false);
	}

	// restore measurement sequence settings
	VL53L0_RF_CALL(write(Register::SYSTEM__SEQUENCE_CONFIG, standardSequence));

	RF_END_RETURN(true);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::loadTuningSettings()
{
	using namespace vl53l0_private;

	RF_BEGIN();

	// write the configuration
	for(index = 0; index < 80; index++)
	{
		i2cBuffer[1] = configuration[index].value;
		VL53L0_RF_CALL( write(Register(configuration[index].reg), i2cBuffer[1]) );
	}

	RF_END_RETURN(true);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::readSensor()
{
	static_assert(MaxMeasurementTimeUs / 1000 * 2 <= ArithmeticTraits<uint16_t>::max,
				"MaxMeasurementTimeUs out of range");

	RF_BEGIN();

	data.reset();

	// undocumented black magic
	VL53L0_RF_CALL(write(Register(0x80), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x01));
	VL53L0_RF_CALL(write(Register(0x00), 0x00));
	VL53L0_RF_CALL(write(Register(0x91), stopMode));
	VL53L0_RF_CALL(write(Register(0x00), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x00));
	VL53L0_RF_CALL(write(Register(0x80), 0x00));

	// start range measurement
	VL53L0_RF_CALL(write(Register::SYSRANGE__START, Start::StartStop));

	// wait for the start flag to be cleared, use default timeout
	VL53L0_RF_CALL(poll(Register::SYSRANGE__START, [](uint8_t value) {
		return not (value & uint8_t(Start::StartStop));
	}));

	// wait for the measurement to finish, timeout of 2 * this->measurementTimeUs
	VL53L0_RF_CALL(poll(Register::RESULT__INTERRUPT_STATUS, [](uint8_t value) {
		return value & (InterruptStatus::NewSampleReady | InterruptStatus::OutOfWindow).value;
	}, measurementTimeUs / 1000 * 2));

	// read 16 bit range value
	VL53L0_RF_CALL(read(Register::RESULT__RANGE_VALUE0, &i2cBuffer[1], 2));
	data.distanceBuffer[0] = i2cBuffer[1];
	data.distanceBuffer[1] = i2cBuffer[2];

	// clear interrupt flag
	VL53L0_RF_CALL(write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range));

	// read error code
	VL53L0_RF_CALL(read(Register::RESULT__RANGE_STATUS, i2cBuffer[1]));
	data.error = RangeErrorCode_t::get(RangeStatus_t(i2cBuffer[1]));

	RF_END_RETURN(true);
}

// MARK: setMaxMeasurementTime
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::setMaxMeasurementTime(uint32_t timeUs)
{
	RF_BEGIN();

	if(timeUs > MaxMeasurementTimeUs) {
		XPCC_LOG_ERROR << "Measurement time out of range." << xpcc::endl;
		RF_RETURN(false);
	}

	// read the measurement sequence configuration into this->sequenceInfo
	VL53L0_RF_CALL(readSequenceInfo());

	if(not calculateFinalRangeTimeout(timeUs, sequenceInfo.finalRangeTimeout)) {
		XPCC_LOG_ERROR << "Invalid timing requested, aborting." << xpcc::endl;
		RF_RETURN(false);
	}

	VL53L0_RF_CALL(writeUInt16(Register::FINAL_RANGE__CONFIG_TIMEOUT_MACROP_HI, sequenceInfo.finalRangeTimeout));

	this->measurementTimeUs = timeUs;

	RF_END_RETURN(true);
}

// MARK: checkModelID
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::checkModelID()
{
	RF_BEGIN();

	VL53L0_RF_CALL(read(Register::IDENTIFICATION__MODEL_ID, &i2cBuffer[1], 2));

	RF_END_RETURN(i2cBuffer[1] == ModelID[0] && i2cBuffer[2] == ModelID[1]);
}

// MARK: checkRevisionID
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::checkRevisionID()
{
	RF_BEGIN();

	VL53L0_RF_CALL(read(Register::IDENTIFICATION__REVISION_ID, i2cBuffer[1]));

	RF_END_RETURN(i2cBuffer[1] == RevisionID);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::setDeviceAddress(uint8_t address)
{
	RF_BEGIN();

	if(RF_CALL(write(Register::I2C_SLAVE__DEVICE_ADDRESS, (address & 0x7F))))
	{
		this->setAddress(address);
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::initializeSpadConfig()
{
	RF_BEGIN();

	// read number and type of SPADs to be used for calibration
	VL53L0_RF_CALL(write(Register(0x80), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x01));
	VL53L0_RF_CALL(write(Register(0x00), 0x00));
	VL53L0_RF_CALL(write(Register(0xFF), 0x06));
	VL53L0_RF_CALL(updateControlRegister(Register(0x83), Control_t(4), Control_t(0)));
	VL53L0_RF_CALL(write(Register(0xFF), 0x07));
	VL53L0_RF_CALL(write(Register(0x81), 0x01));
	VL53L0_RF_CALL(write(Register(0x80), 0x01));

	VL53L0_RF_CALL(write(Register(0x94), 0x6b));
	VL53L0_RF_CALL(write(Register(0x83), 0x00));

	VL53L0_RF_CALL(poll(Register(0x83), [](uint8_t value) {
		return value != 0x00;
	}));

	VL53L0_RF_CALL(write(Register(0x83), 0x01));
	VL53L0_RF_CALL(read(Register(0x92), i2cBuffer[1]));

	spadInfo.referenceSpadCount = i2cBuffer[1] & 0x7F;
	spadInfo.useApertureSpads = (i2cBuffer[1] & (1 << 7)) != 0;

	VL53L0_RF_CALL(write(Register(0x81), 0x00));
	VL53L0_RF_CALL(write(Register(0xFF), 0x06));
	VL53L0_RF_CALL(updateControlRegister(Register(0x83), Control_t(0), Control_t(4)));
	VL53L0_RF_CALL(write(Register(0xFF), 0x01));
	VL53L0_RF_CALL(write(Register(0x00), 0x01));
	VL53L0_RF_CALL(write(Register(0xFF), 0x00));
	VL53L0_RF_CALL(write(Register(0x80), 0x00));

	// read map of SPADs available for reference calibration
	VL53L0_RF_CALL(read(Register::GLOBAL__CONFIG_SPAD_ENABLES_REF_0, spadInfo.map, 6));

	// prepare setting SPAD config
	VL53L0_RF_CALL(write(Register(0xFF), 0x01));
	VL53L0_RF_CALL(write(Register::DYNAMIC_SPAD__REF_EN_START_OFFSET, 0x00));
	VL53L0_RF_CALL(write(Register::DYNAMIC_SPAD__NUM_REQUESTED_REF_SPAD, 0x2C));
	VL53L0_RF_CALL(write(Register(0xFF), 0x00));
	VL53L0_RF_CALL(write(Register::GLOBAL__CONFIG_REF_EN_START_SELECT, 0xB4));

	if(not setupReferenceSpadMap(spadInfo.map, &i2cBuffer[1]))
	{
		XPCC_LOG_ERROR << "Invalid SPAD data in non-volatile memory.\n";
		XPCC_LOG_ERROR << "A full 'SPAD management' recalibration has to be performed.\n";
		XPCC_LOG_ERROR << "This procedure is not implemented in this driver.\n";
		XPCC_LOG_ERROR << "Please use the VL53L0X API provided by ST." << xpcc::endl;
		
		RF_RETURN(false);
	}

	// Write SPAD config to the device. This will not be written to NVM.
	VL53L0_RF_CALL(writeI2CBuffer(Register::GLOBAL__CONFIG_SPAD_ENABLES_REF_0, 6));

	RF_END_RETURN(true);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::performReferenceCalibration(Start_t mode)
{
	static_assert(MaxMeasurementTimeUs / 1000 * 2 <= ArithmeticTraits<uint16_t>::max,
				"MaxMeasurementTimeUs out of range");

	RF_BEGIN();

	// start measurement
	VL53L0_RF_CALL(write(Register::SYSRANGE__START, Start::StartStop | mode));

	// wait for the measurement to finish
	VL53L0_RF_CALL(poll(Register::RESULT__INTERRUPT_STATUS, [](uint8_t value) {
		return value & (InterruptStatus::NewSampleReady | InterruptStatus::OutOfWindow).value;
	}, measurementTimeUs / 1000 * 2));
	
	// clear interrupt flags
	VL53L0_RF_CALL(write(Register::SYSTEM__INTERRUPT_CLEAR, InterruptClear::Range));
	VL53L0_RF_CALL(write(Register::SYSRANGE__START, Start::SingleShotMode));

	RF_END_RETURN(true);
}

template < typename I2cMaster >
bool
xpcc::Vl53l0<I2cMaster>::setupReferenceSpadMap(const uint8_t availableSpadMap[6], uint8_t calibrationSpadMap[6])
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
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::readSequenceInfo()
{
	RF_BEGIN();

	// read vcsel periods
	VL53L0_RF_CALL(read(Register::PRE_RANGE__CONFIG_VCSEL_PERIOD, sequenceInfo.vcselPeriodPreRange));
	VL53L0_RF_CALL(read(Register::FINAL_RANGE__CONFIG_VCSEL_PERIOD, sequenceInfo.vcselPeriodFinalRange));

	// decode vcsel period values
	sequenceInfo.vcselPeriodPreRange += 1;
	sequenceInfo.vcselPeriodPreRange <<= 1;
	sequenceInfo.vcselPeriodFinalRange += 1;
	sequenceInfo.vcselPeriodFinalRange <<= 1;

	VL53L0_RF_CALL(read(Register::SYSTEM__SEQUENCE_CONFIG, i2cBuffer[1]));
	sequenceInfo.enabledSteps = static_cast<MeasurementSequenceStep>(i2cBuffer[1]);

	VL53L0_RF_CALL(read(Register::MSRC__CONFIG_TIMEOUT_MACROP, sequenceInfo.msrcDssTccTimeout));
	sequenceInfo.msrcDssTccTimeout += 1;

	VL53L0_RF_CALL(read(Register::PRE_RANGE__CONFIG_TIMEOUT_MACROP_HI, &i2cBuffer[1], 2));
	sequenceInfo.preRangeTimeout = decodeTimeout((i2cBuffer[1] << 8) + i2cBuffer[2]);

	VL53L0_RF_CALL(read(Register::FINAL_RANGE__CONFIG_TIMEOUT_MACROP_HI, &i2cBuffer[1], 2));	
	sequenceInfo.finalRangeTimeout = decodeTimeout((i2cBuffer[1] << 8) + i2cBuffer[2]);

	RF_END_RETURN(true);
}

template < typename I2cMaster >
bool
xpcc::Vl53l0<I2cMaster>::calculateFinalRangeTimeout(uint32_t measurementTime, uint16_t& registerValue)
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
xpcc::Vl53l0<I2cMaster>::timeoutToMicroseconds(uint16_t mclks, uint8_t vcselPeriodPclks)
{
	// in nanoseconds
	const uint32_t macroPeriodNs = ((UINT32_C(2304) * vcselPeriodPclks * 1655) + 500) / 1000;

	return ((mclks * macroPeriodNs) + 500) / 1000;
}

template < typename I2cMaster >
uint32_t
xpcc::Vl53l0<I2cMaster>::timeoutToMclks(uint32_t microseconds, uint8_t vcselPeriodPclks)
{
	const uint32_t macroPeriodNs = ((UINT32_C(2304) * vcselPeriodPclks * 1655) + 500) / 1000;

	return (((microseconds * 1000) + (macroPeriodNs / 2)) / macroPeriodNs);
}

template < typename I2cMaster >
uint16_t
xpcc::Vl53l0<I2cMaster>::encodeTimeout(uint32_t timeoutMclks)
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
xpcc::Vl53l0<I2cMaster>::decodeTimeout(uint16_t registerValue)
{
	const uint8_t coefficient = registerValue & 0xFF;
	const uint8_t exponent = (registerValue & 0xFF00) >> 8;

	// format: coefficient*(2^exponent) + 1
	return (static_cast<uint32_t>(coefficient) << exponent) + 1;
}


// ----------------------------------------------------------------------------
// MARK: update register
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::updateControlRegister(Register reg, Control_t setMask, Control_t clearMask)
{
	RF_BEGIN();

	if(clearMask.value != 0xFF) {
		if(!RF_CALL(read(reg, i2cBuffer[1]))) {
			RF_RETURN(false);
		}
	}
	i2cBuffer[1] = (i2cBuffer[1] & ~clearMask.value) | setMask.value;

	RF_END_RETURN_CALL(write(reg, i2cBuffer[1]));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::write(Register reg, uint8_t value)
{
	RF_BEGIN();
	i2cBuffer[1] = value;
	
	RF_END_RETURN_CALL(writeI2CBuffer(reg, 1));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::writeI2CBuffer(Register reg, uint8_t dataLength)
{
	RF_BEGIN();

	if(dataLength >= sizeof(i2cBuffer)) {
		RF_RETURN(false);
	}

	i2cBuffer[0] = uint8_t(reg);
	this->transaction.configureWrite(i2cBuffer, dataLength + 1);

	RF_END_RETURN_CALL(this->runTransaction());
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::writeUInt16(Register reg, uint16_t value)
{
	RF_BEGIN();

	i2cBuffer[0] = uint8_t(reg);
	i2cBuffer[1] = (value & 0xFF00) >> 8;
	i2cBuffer[2] = value & 0xFF;

	this->transaction.configureWrite(i2cBuffer, 3);

	RF_END_RETURN_CALL(this->runTransaction());
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	RF_BEGIN();

	i2cBuffer[0] = uint8_t(reg);

	this->transaction.configureWriteRead(i2cBuffer, 1, buffer, length);

	RF_END_RETURN_CALL(this->runTransaction());
}

template < class I2cMaster >
template < typename Predicate >
xpcc::ResumableResult<bool>
xpcc::Vl53l0<I2cMaster>::poll(Register reg, Predicate pred, uint16_t timeoutMs, uint16_t waitMs)
{
	RF_BEGIN();

	index = timeoutMs;

	while(true)
	{
		if(not RF_CALL(read(reg, i2cBuffer[1]))) {
			break;
		}

		if(pred(i2cBuffer[1])) {
			RF_RETURN(true);
		}

		if(index == 0) {
			XPCC_LOG_ERROR << "Timeout" << xpcc::endl;
			break;
		}

		timeout.restart(waitMs);
		RF_WAIT_UNTIL(timeout.isExpired());

		if(index >= waitMs) {
			index -= waitMs;
		} else {
			index = 0;
		}
	}

	RF_END_RETURN(false);
}
