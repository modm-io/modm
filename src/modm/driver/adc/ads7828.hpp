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
#define MODM_ADS7828_HPP

#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @ingroup modm_driver_ads7828
struct ads7828
{
protected:
    enum class
    CommandByte : uint8_t
    {
        SD = Bit7,
        C2 = Bit6,
        C1 = Bit5,
        C0 = Bit4,
        PD1 = Bit3,
        PD0 = Bit2,
    };
    MODM_FLAGS8(CommandByte);

public:
    enum class
    InputChannel : uint8_t
    {
        // Differential Inputs
        Ch0Ch1 = 0u,
        Ch2Ch3 = int(CommandByte::C0),
        Ch4Ch5 = int(CommandByte::C1),
        Ch6Ch7 = int(CommandByte::C1) | int(CommandByte::C0),
        Ch1Ch0 = int(CommandByte::C2),
        Ch3Ch2 = int(CommandByte::C2) | int(CommandByte::C0),
        Ch5Ch4 = int(CommandByte::C2) | int(CommandByte::C1),
        Ch7Ch6 = int(CommandByte::C2) | int(CommandByte::C1) | int(CommandByte::C0),

        // Single-Ended Inputs
        Ch0 = int(CommandByte::SD),
        Ch1 = int(CommandByte::SD) | int(CommandByte::C2),
        Ch2 = int(CommandByte::SD) | int(CommandByte::C0),
        Ch3 = int(CommandByte::SD) | int(CommandByte::C2) | int(CommandByte::C0),
        Ch4 = int(CommandByte::SD) | int(CommandByte::C1),
        Ch5 = int(CommandByte::SD) | int(CommandByte::C2) | int(CommandByte::C1),
        Ch6 = int(CommandByte::SD) | int(CommandByte::C1) | int(CommandByte::C0),
        Ch7 = int(CommandByte::SD) | int(CommandByte::C2) | int(CommandByte::C1) | int(CommandByte::C0),
    };
    typedef Configuration<CommandByte_t, InputChannel, Bit7 | Bit6 | Bit5 | Bit4> InputChannel_t;

    enum class
    PowerDown : uint8_t
    {
        InternalReferenceOffAdcConverterOff = 0u,
        InternalReferenceOnAdcConverterOff  = int(CommandByte::PD1),
        InternalReferenceOffAdcConverterOn  = int(CommandByte::PD0),
        InternalReferenceOnAdcConverterOn   = int(CommandByte::PD1) | int(CommandByte::PD0),
    };
    typedef Configuration<CommandByte_t, PowerDown, Bit3 | Bit2> PowerDown_t;

    struct modm_packed
    Data
    {
        template <class I2cMaster>
        friend class Ads7828;

        constexpr uint16_t
        getValue() const
        {
            return static_cast<uint16_t>((data[0] << 8) | data[1]);
        }

        constexpr float
        getVoltage(float vref) const
        {
            return vref * getValue() / 4096;
        }

    protected:
        uint8_t data[2];
    };
};

/**
 * @tparam	I2cMaster	I2cMaster interface
 *
 * @author	Jonas Holm Wentzlau
 * @author	Jonas Kazem Andersen
 * @author  Rasmus Kleist Hørlyck Sørensen
 * @ingroup modm_driver_ads101x
 */
template <typename I2cMaster>
class Ads7828 : public ads7828, public modm::I2cDevice<I2cMaster, 1>
{
public:
    /**
     * @brief Construct a new Ads7828 object
     *
     * @param data
     * @param address
     *      The adress is the i2c adress
     */
    Ads7828(Data &data, uint8_t address = 0b1001000);

    /**
     * @brief Measures the voltage in a single channel
     *
     * @param channel
     *      Channelx where x is the channel to be read
     * @param settings
     *      The ads7828 has an internal reference which can be on or off, and the adc itself can be on or off.
     *		INTERNAL_REFERENCE_mode_ADC_mode, where mode is either ON or OFF. Does not have to be the same
        */
    modm::ResumableResult<bool>
    startMeasurement(InputChannel input);

    /**
     * @brief Sets the power down selection
     *
     * @param powerDownSelection
     *      PD1 = 1 to have internal reference ON
     *      PD0 = 1 to have A/D converter ON
     */
    modm::ResumableResult<bool>
    setPowerDownSelection(PowerDown powerDownSelection);

    /**
     * @brief Reads the latest measurement result
     **/
    modm::ResumableResult<bool>
    readConversionResult();

    inline Data &
    getData()
    {
        return data;
    }

private:
    Data &data;

    CommandByte_t commandByte;
};

} // modm namespace

#include "ads7828_impl.hpp"

#endif // MODM_ADS7828_HPP