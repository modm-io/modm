/*
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

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
template < typename SPI, typename CS, uint8_t MODULES = 1 >
class Max7219
{
public:
    static void
    initialize()
    {
        // show all 8 digits
        sendByte(MAX7219_REG_SCANLIMIT, 7);

        // using a LED matrix (not digits)
        sendByte(MAX7219_REG_DECODEMODE, 0);    

        // no display test
        sendByte(MAX7219_REG_DISPLAYTEST, 0);

        clear();

        // character intensity: range: 0..15
        setBrightness(15);

        // not in shutdown mode (i.e start it up)
        sendByte(MAX7219_REG_SHUTDOWN, 1);
    }

    static void
    clear()
    {
    	// Iterate column 0 to 7 which is addressed 1 to 8
        for (uint8_t col = 1; col <= 8; ++col) {
            sendByte(col, 0);
        };
    }

    /**
     * Set a row of the MAX7219 chips with data
     *
     * @param	data	A pointer to the data, at least MODULES bytes long
     */
    static void
    setRow(uint8_t col, uint8_t * data)
    {
        CS::reset();
        for (uint8_t ii = 0; ii < MODULES; ++ii)
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
        sendByte(MAX7219_REG_INTENSITY, intensity % 16);
    }

private:
    static constexpr uint8_t MAX7219_REG_NOOP        = 0x0;
    static constexpr uint8_t MAX7219_REG_DIGIT0      = 0x1;
    static constexpr uint8_t MAX7219_REG_DIGIT1      = 0x2;
    static constexpr uint8_t MAX7219_REG_DIGIT2      = 0x3;
    static constexpr uint8_t MAX7219_REG_DIGIT3      = 0x4;
    static constexpr uint8_t MAX7219_REG_DIGIT4      = 0x5;
    static constexpr uint8_t MAX7219_REG_DIGIT5      = 0x6;
    static constexpr uint8_t MAX7219_REG_DIGIT6      = 0x7;
    static constexpr uint8_t MAX7219_REG_DIGIT7      = 0x8;
    static constexpr uint8_t MAX7219_REG_DECODEMODE  = 0x9;
    static constexpr uint8_t MAX7219_REG_INTENSITY   = 0xA;
    static constexpr uint8_t MAX7219_REG_SCANLIMIT   = 0xB;
    static constexpr uint8_t MAX7219_REG_SHUTDOWN    = 0xC;
    static constexpr uint8_t MAX7219_REG_DISPLAYTEST = 0xF;

    static void
    sendByte(uint8_t cmd, uint8_t data)
    {
        CS::reset();

        // Write the command multiple times, for each MODULES
        for (uint8_t ii = 0; ii < MODULES; ++ii)
        {
			SPI::transferBlocking(cmd);
			SPI::transferBlocking(data);
        }
        CS::set();
    }
};
}

#endif
