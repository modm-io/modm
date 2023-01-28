// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2022, Sarah Vilete
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX31855_HPP
#   error "Don't include this file directly, use 'max31855.hpp' instead!"
#endif

namespace modm
{

template <typename SpiMaster, typename Cs>
Max31855<SpiMaster, Cs>::Max31855(Data &data) : data(data)
{
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
void
Max31855<SpiMaster, Cs>::initialize()
{
    Cs::setOutput(modm::Gpio::High);
}

// -----------------------------------------------------------------------------

template <typename SpiMaster, typename Cs>
modm::ResumableResult<void>
Max31855<SpiMaster, Cs>::readout()
{
    RF_BEGIN();
    RF_WAIT_UNTIL(this->acquireMaster());

    Cs::reset();
    RF_CALL(SpiMaster::transfer(nullptr, data.data, 4));

    if (this->releaseMaster())
    {
        Cs::set();
    }

    RF_END();
}

} // namespace modm