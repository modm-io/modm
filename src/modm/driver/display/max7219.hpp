/*
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MAX7219_HPP
#define MODM_MAX7219_HPP

#include <stdint.h>

namespace modm
{
/**
 * Some light hardware abstractions of MAX7219
 *
 * @tparam	SPI		SPI Interface
 * @tapram	CS		Chip Select of MAX7219
 * @tparam	DRIVERS	Number of chips in daisy chain
 *
 * An arbitrary number of these chips can be daisy-chained
 *
 * @author	Sascha Schade
 * @ingroup	modm_driver_max7219
 */
template <typename SPI, typename CS, uint8_t MODULES = 1>
class Max7219
{
public:
    enum class Register : uint8_t
    {
        noop = 0x0,
        digit0 = 0x1,
        digit1 = 0x2,
        digit2 = 0x3,
        digit3 = 0x4,
        digit4 = 0x5,
        digit5 = 0x6,
        digit6 = 0x7,
        digit7 = 0x8,
        decodeMode = 0x9,
        intensity = 0xA,
        scanLimit = 0xB,
        shutdown = 0xC,
        displayTest = 0xF,
    };

    /**
     * Initialize as an 8x8 led matrix.
     */
    static void
    initializeMatrix()
    {
        // show all 8 digits
        setRegister(Register::scanLimit, 7);

        // using a LED matrix (not digits)
        setRegister(Register::decodeMode, 0);

        // no display test
        setRegister(Register::displayTest, 0);

        clear();

        // character intensity: range: 0..15
        setBrightness(15);

        // not in shutdown mode (i.e start it up)
        setRegister(Register::shutdown, 1);
    }

    static void
    clear()
    {
        // Iterate column 0 to 7 which is addressed 1 to 8
        for (uint8_t col = 0; col < 8; ++col)
        {
            CS::reset();
            for (uint8_t i = 0; i < MODULES; ++i)
            {
                SPI::transferBlocking(col + 1);
                SPI::transferBlocking(0);
            }
            CS::set();
        };
    }

    /**
     * Set a row of the MAX7219 chips with data
     *
     * @param	data	A pointer to the data, at least MODULES bytes long
     */
    static void
    setRow(uint8_t col, uint8_t* data)
    {
        CS::reset();
        for (uint8_t i = 0; i < MODULES; ++i)
        {
            SPI::transferBlocking(col + 1);
            SPI::transferBlocking(*data++);
        }
        CS::set();
    }

    /**
     * Set the brightness of the whole matrix.
     *
     * \param intensity range 0 to 15.
     */
    static void
    setBrightness(uint8_t intensity)
    {
        setRegister(Register::intensity, intensity % 16);
    }

    static void
    setRegister(Register reg, uint8_t data)
    {
        CS::reset();

        // Write the command multiple times, for each MODULES
        for (uint8_t i = 0; i < MODULES; ++i)
        {
            SPI::transferBlocking(uint8_t(reg));
            SPI::transferBlocking(data);
        }
        CS::set();
    }
};
}  // namespace modm

#endif
