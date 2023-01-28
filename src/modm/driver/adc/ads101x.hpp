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
#define MODM_ADS101X_HPP

#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{
/// @ingroup modm_driver_ads101x
struct ads101x
{
    enum class
    Register : uint8_t
    {
        Conversion = 0b00,
        Config = 0b01,
        LowThreshold = 0b10,
        HighThreshold = 0b11
    };

public:
    /// The Config register is used to control the operating mode, input selection, data rate, full-scale range, and comparator modes.
    enum class
    ConfigRegister : uint16_t
    {
        OS = Bit15,
        MUX2 = Bit14,
        MUX1 = Bit13,
        MUX0 = Bit12,
        PGA2 = Bit11,
        PGA1 = Bit10,
        PGA0 = Bit9,
        MODE = Bit8,
        DR2 = Bit7,
        DR1 = Bit6,
        DR0 = Bit5,
        COMP_MODE = Bit4,
        COMP_POL = Bit3,
        COMP_LAT = Bit2,
        COMP_QUE1 = Bit1,
        COMP_QUE0 = Bit0,
    };
    MODM_FLAGS16(ConfigRegister);

    enum class
    FullScaleRange : uint16_t
    {
        V6_144 = 0,
        V4_096 = int(ConfigRegister::PGA0),
        V2_048 = int(ConfigRegister::PGA1),
        V1_024 = int(ConfigRegister::PGA1) | int(ConfigRegister::PGA0),
        V0_512 = int(ConfigRegister::PGA2),
        V0_256 = int(ConfigRegister::PGA2) | int(ConfigRegister::PGA0),
    };
    typedef Configuration<ConfigRegister_t, FullScaleRange, (Bit11 | Bit10 | Bit9)> FullScaleRange_t;

    enum class
    DeviceOperatingMode : uint16_t
    {
        Continuous = 0,
        SingleShot = int(ConfigRegister::MODE), // default
    };
    typedef Configuration<ConfigRegister_t, DeviceOperatingMode, Bit8> DeviceOperatingMode_t;

    enum class
    InputMultiplexer : uint16_t
    {
        Input0 = 0,
        Input1 = int(ConfigRegister::MUX0),
        Input2 = int(ConfigRegister::MUX1),
        Input3 = int(ConfigRegister::MUX1) | int(ConfigRegister::MUX0),
        Input4 = int(ConfigRegister::MUX2),
        Input5 = int(ConfigRegister::MUX2) | int(ConfigRegister::MUX0),
        Input6 = int(ConfigRegister::MUX2) | int(ConfigRegister::MUX1),
        Input7 = int(ConfigRegister::MUX2) | int(ConfigRegister::MUX1) | int(ConfigRegister::MUX0),
    };
    typedef Configuration<ConfigRegister_t, InputMultiplexer, (Bit14 | Bit13 | Bit12)> InputMultiplexer_t;

    enum class
    DataRate : uint16_t
    {
        Sps128 = 0,
        Sps250 = int(ConfigRegister::DR0),
        Sps490 = int(ConfigRegister::DR1),
        Sps920 = int(ConfigRegister::DR1) | int(ConfigRegister::DR0),
        Sps1600 = int(ConfigRegister::DR2),
        Sps2400 = int(ConfigRegister::DR2) | int(ConfigRegister::DR0),
        Sps3300 = int(ConfigRegister::DR2) | int(ConfigRegister::DR1)
    };
    typedef Configuration<ConfigRegister_t, DataRate, (Bit7 | Bit6 | Bit5)> DataRate_t;

    enum class
    ComparatorMode : uint16_t
    {
        Traditional = 0,
        Window = int(ConfigRegister::COMP_MODE),
    };
    typedef Configuration<ConfigRegister_t, ComparatorMode, Bit4> ComparatorMode_t;

    enum class
    ComparatorPolarity : uint16_t
    {
        ActiveLow = 0,
        ActiveHigh = int(ConfigRegister::COMP_POL),
    };
    typedef Configuration<ConfigRegister_t, ComparatorPolarity, Bit3> ComparatorPolarity_t;

    enum class
    ComparatorLatch : uint16_t
    {
        Nonlatching = 0,
        Latching = int(ConfigRegister::COMP_LAT),
    };
    typedef Configuration<ConfigRegister_t, ComparatorLatch, Bit2> ComparatorLatch_t;

    enum class
    ComparatorQueue : uint16_t
    {
        // Enable the comparator function
        // Assert the ALERT/RDY pin after the specified number of successive conversions exceeding the upper or lower threshold
        OneConversion = 0,
        TwoConversions = int(ConfigRegister::COMP_QUE0),
        FourConversions = int(ConfigRegister::COMP_QUE1),

        // Disable comparator queue and set the ALERT/RDY pin to a high-impedance state
        Disable = int(ConfigRegister::COMP_QUE1) | int(ConfigRegister::COMP_QUE0)
    };
    typedef Configuration<ConfigRegister_t, ComparatorQueue, Bit4> ComparatorQueue_t;

    struct modm_packed
    Data
    {
        inline int16_t
        getValue()
        {
            return static_cast<int16_t>((data[0] << 8) | data[1]) / 16;
        }

        inline float
        getVoltage()
        {
            return getValue() * lsbSizeTable[lsbSizeIndex];
        }

        uint8_t data[2];
        uint8_t lsbSizeIndex = 0;
    };

protected:
    // Conversion table for converting the raw voltage reading to the correct scale
    static constexpr float lsbSizeTable[] = {
        0.003f,   // LSB for +- 6.144V FSR
        0.002f,   // LSB for +- 4.096V FSR
        0.001f,   // LSB for +- 2.048V FSR
        0.0005f,  // LSB for +- 1.024V FSR
        0.00025f, // LSB for +- 0.512V FSR
        0.000125f // LSB for +- 0.256V FSR
    };

protected:
    /// @cond
    static constexpr uint8_t
    i(Register reg) { return uint8_t(reg); }
    static constexpr uint16_t
    i(ConfigRegister config) { return uint16_t(config); }
    static constexpr uint16_t
    i(FullScaleRange fsr) { return uint16_t(fsr); }
    /// @endcond
};

/**
 * @tparam	I2cMaster	I2cMaster interface
 *
 * @author	Jonas Kazem Andersen
 * @author  Rasmus Kleist Hørlyck Sørensen
 * @ingroup modm_driver_ads101x
 */
template <typename I2cMaster>
class Ads101x : public ads101x, public modm::I2cDevice<I2cMaster, 2>
{
public:
    /**
     * @param data
     * @param address
     */
    Ads101x(Data &data, uint8_t address = 0x49);

    /// Call this function before using the device
    modm::ResumableResult<bool>
    initialize();

    /// Determine if the device is currently performing a conversion
    modm::ResumableResult<bool>
    isBusy();

    /// Start a single conversion with the specified input
    modm::ResumableResult<bool>
    startSingleShotConversion();

    /// Start continuous conversions with the specified datarate and input
    modm::ResumableResult<bool>
    startContinuousConversion(DataRate dataRate = DataRate::Sps1600);

    /// Start a single conversion with the specified input
    /// @warning ADS1015 only
    modm::ResumableResult<bool>
    startSingleShotConversion(InputMultiplexer input = InputMultiplexer::Input0);

    /// Start continuous conversions with the specified datarate and input
    /// @warning ADS1015 only
    modm::ResumableResult<bool>
    startContinuousConversion(InputMultiplexer input = InputMultiplexer::Input0, DataRate dataRate = DataRate::Sps1600);

    /// Read the last conversion result
    /// @attention Following power-up, the conversion result remains zero until the first conversion is completed
    modm::ResumableResult<bool>
    readConversionResult();

    /// Enable the conversion-ready function of the ALERT/RDY pin
    /// @attention enabling the conversion-ready function disables the comparator and sets the ComparatorQue value to one conversion
    modm::ResumableResult<bool>
    enableConversionReadyFunction();

    /// Enable the comparator
    /// @warning ADS1014 and ADS1015 only
    /// @warning To use the comparator-function the high threshold must be greater than the low threshold
    modm::ResumableResult<bool>
    enableComparator(ComparatorMode mode, ComparatorPolarity polarity, ComparatorLatch latch, ComparatorQueue queue);

    /// Set the low threshold used by the comparator queue
    /// @warning ADS1014 and ADS1015 only
    /// @warning The low threshold value must be smaller than the high threshold value
    /// @attention The high threshold value must be updated whenever the PGA settings are changed
    modm::ResumableResult<bool>
    setLowThreshold(uint16_t threshold)
    {
        return writeRegister(Register::LowThreshold, (threshold << 4) & 0xFFF0);
    }

    /// Set the high threshold used by the comparator queue
    /// @warning ADS1014 and ADS1015 only
    /// @warning The high threshold value must be greater than the low threshold value
    /// @attention The high threshold value must be updated whenever the PGA settings are changed
    modm::ResumableResult<bool>
    setHighThreshold(uint16_t threshold)
    {
        return writeRegister(Register::HighThreshold, (threshold << 4) | 0x0F);
    }

    /// Set the full scale range by programming the PGA and corresponding LSB size
    modm::ResumableResult<bool>
    setFullScaleRange(FullScaleRange fullScaleRange);

private:
    modm::ResumableResult<bool>
    writeRegister(Register reg, uint16_t data);

    Data &data;

    /// Command buffer for writing to device
    uint8_t buffer[3];

    /// Configuration variable for writing to the configuration register
    ConfigRegister_t config;
};

}

#include "ads101x_impl.hpp"

#endif // MODM_ADS101X_HPP
