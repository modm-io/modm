// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2022, Jónas Holm Wentzlau
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

#ifndef MODM_ADS7828_HPP
#error "Don't include this file directly, use 'ads7828.hpp' instead!"
#endif

namespace modm
{

template <typename I2cMaster>
Ads7828<I2cMaster>::Ads7828(Data &data, uint8_t address) : data(data), modm::I2cDevice<I2cMaster, 1>(address)
{
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads7828<I2cMaster>::startMeasurement(InputChannel channel)
{
    RF_BEGIN();
    InputChannel_t::set(commandByte, channel);
    this->transaction.configureWrite(&commandByte.value, 1);
    RF_END_RETURN_CALL(this->runTransaction());
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads7828<I2cMaster>::setPowerDownSelection(PowerDown powerDownSelection)
{
    RF_BEGIN();
    PowerDown_t::set(commandByte, powerDownSelection);
    this->transaction.configureWrite(&commandByte.value, 1);
    RF_END_RETURN_CALL(this->runTransaction());
}

// ----------------------------------------------------------------------------

template <typename I2cMaster>
modm::ResumableResult<bool>
Ads7828<I2cMaster>::readConversionResult()
{
    RF_BEGIN();
    this->transaction.configureRead(data.data, 2);
    RF_END_RETURN_CALL(this->runTransaction());
}

} // modm namespace