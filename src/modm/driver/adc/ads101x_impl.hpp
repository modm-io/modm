/*
 * Copyright (c) 2022, Jonas Kazem Andersen
 * Copyright (c) 2022, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS101X_HPP
#   error "Don't include this file directly, use 'ads101x.hpp' instead!"
#endif

namespace modm
{

template <typename I2cMaster>
Ads101x<I2cMaster>::Ads101x(Data &data, uint8_t address) : I2cDevice<I2cMaster, 2>(address), data(data)
{
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::initialize()
{
    RF_BEGIN();

    InputMultiplexer_t::set(config, InputMultiplexer::Input0);
    FullScaleRange_t::set(config, FullScaleRange::V2_048);
    DeviceOperatingMode_t::set(config, DeviceOperatingMode::SingleShot);
    DataRate_t::set(config, DataRate::Sps1600);
    ComparatorQueue_t::set(config, ComparatorQueue::Disable);
    data.lsbSizeIndex = i(FullScaleRange::V2_048) >> 9;

    RF_END_RETURN_CALL(writeRegister(Register::Config, config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::isBusy()
{
    RF_BEGIN();

    buffer[0] = i(Register::Config);
    this->transaction.configureWriteRead(buffer, 1, buffer, 2);

    if (RF_CALL(this->runTransaction()))
    {
        RF_RETURN((static_cast<uint16_t>(buffer[0] << 8) & i(ConfigRegister::OS)) == 0);
    }
    RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::startSingleShotConversion()
{
    RF_BEGIN();

    DeviceOperatingMode_t::set(config, DeviceOperatingMode::SingleShot);

    RF_END_RETURN_CALL(writeRegister(Register::Config, i(ConfigRegister::OS) | config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::startContinuousConversion(DataRate dataRate)
{
    RF_BEGIN();

    DataRate_t::set(config, dataRate);
    DeviceOperatingMode_t::set(config, DeviceOperatingMode::Continuous);

    RF_END_RETURN_CALL(writeRegister(Register::Config, config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::startSingleShotConversion(InputMultiplexer input)
{
    RF_BEGIN();

    InputMultiplexer_t::set(config, input);
    DeviceOperatingMode_t::set(config, DeviceOperatingMode::SingleShot);

    RF_END_RETURN_CALL(writeRegister(Register::Config, i(ConfigRegister::OS) | config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::startContinuousConversion(InputMultiplexer input, DataRate dataRate)
{
    RF_BEGIN();

    DataRate_t::set(config, dataRate);
    InputMultiplexer_t::set(config, input);
    DeviceOperatingMode_t::set(config, DeviceOperatingMode::Continuous);

    RF_END_RETURN_CALL(writeRegister(Register::Config, config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::readConversionResult()
{
    RF_BEGIN();

    buffer[0] = i(Register::Conversion);
    this->transaction.configureWriteRead(buffer, 1, data.data, 2);

    RF_END_RETURN_CALL(this->runTransaction());
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::enableConversionReadyFunction()
{
    RF_BEGIN();

    if (not RF_CALL(writeRegister(Register::LowThreshold, 0x0000)))
    {
        RF_RETURN(false);
    }
    if (not RF_CALL(writeRegister(Register::HighThreshold, 0xFFFF)))
    {
        RF_RETURN(false);
    }

    ComparatorQueue_t::set(config, ComparatorQueue::OneConversion);
    if (not RF_CALL(writeRegister(Register::Config, config.value)))
    {
        RF_RETURN(false);
    }

    RF_END_RETURN(true);
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::enableComparator(ComparatorMode mode, ComparatorPolarity polarity, ComparatorLatch latch, ComparatorQueue queue)
{
    RF_BEGIN();

    ComparatorMode_t::set(config, mode);
    ComparatorPolarity_t::set(config, polarity);
    ComparatorLatch_t::set(config, latch);
    ComparatorQueue_t::set(config, queue);

    RF_END_RETURN_CALL(writeRegister(Register::Config, config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::setFullScaleRange(FullScaleRange fullScaleRange)
{
    RF_BEGIN();

    FullScaleRange_t::set(config, fullScaleRange);
    data.lsbSizeIndex = i(fullScaleRange) >> 9;

    RF_END_RETURN_CALL(writeRegister(Register::Config, config.value));
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads101x<I2cMaster>::writeRegister(Register reg, uint16_t data)
{
    RF_BEGIN();

    buffer[0] = i(reg);
    buffer[1] = (data >> 8) & 0xFF;
    buffer[2] = data & 0xFF;

    this->transaction.configureWrite(buffer, 3);

    RF_END_RETURN_CALL(this->runTransaction());
}

} // modm namespace