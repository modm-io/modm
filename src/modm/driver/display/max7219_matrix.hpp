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

#ifndef MODM_MAX7219_MATRIX_HPP
#define MODM_MAX7219_MATRIX_HPP

#include "max7219.hpp"

#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>

namespace modm
{
/**
 * @brief	Driver for arrays of LED 8x8 Matrices driven by MAX7219
 *
 * Orientation of the module is with the connector to bottom.
 * Multiple displays are connected from right to left.
 *
 * Layout of the modules for a 24 pixel x 8 pixel display.
 *
 *           CCCC BBBB
 *     +----+----+----+
 *     |    |    |    |
 *     |    |    |    |
 *     +----+----+----+
 *     |MMMM|MMMM|MMMM|
 *     +----+----+----+
 *      CCCC BBBB AAAA
 *
 * MMMM is the Chip
 * AAAA is connected to the microcontroller
 * BBBB is connected to BBBB
 * CCCC is connected to CCCC
 *
 * @tparam	SPI		SPI Interface
 * @tapram	CS		Chip Select of MAX7219
 * @tparam	COLUMNS	Number of modules placed horizontally, from right to left.
 * @tparam	ROW		Number of modules placed vertically
 *
 * 8x8 LED modules with MAX7219 are easily and cheaply available.
 * @ingroup modm_driver_max7219
 */
template <typename SPI, typename CS, uint8_t COLUMNS = 1, uint8_t ROWS = 1>
class Max7219Matrix : public MonochromeGraphicDisplayVertical<8 * COLUMNS, 8 * ROWS>
{
public:
    virtual ~Max7219Matrix() = default;

    /**
     * \brief	Initialize the display
     */
    void
    initialize();

    /**
     * \brief	Update the display with the content of the RAM buffer
     */
    virtual void
    update() override;

protected:
    // Instance of a daisy chain of COLUMNS * ROWS modules.
    Max7219<SPI, CS, COLUMNS * ROWS> max;
};
}  // namespace modm

/* ------------------- Implementation --------------------- */

template <typename SPI, typename CS, uint8_t COLUMNS, uint8_t ROWS>
void
modm::Max7219Matrix<SPI, CS, COLUMNS, ROWS>::initialize()
{
    max.initializeMatrix();
    this->clear();
}

template <typename SPI, typename CS, uint8_t COLUMNS, uint8_t ROWS>
void
modm::Max7219Matrix<SPI, CS, COLUMNS, ROWS>::update()
{
    // Iterate column 0 to 7 of MAX LED driver
    for (uint8_t ledCol = 0; ledCol < 8; ++ledCol)
    {
        uint8_t buf[COLUMNS * ROWS];
        uint8_t idx = 0;

        // Iterate over display buffer
        // Luckily each LED Matrix has as many pixels vertical
        // as pixels are stored in one byte.

        // col is the column of LED modules and
        // a group of eight pixels horizontal
        for (uint8_t col = 0; col < COLUMNS; ++col)
        {
            // row is the row of LED modules and
            // a group of eight pixels vertical
            for (uint8_t row = 0; row < ROWS; ++row)
            {
                buf[idx] = this->buffer[col * 8 + ledCol][row];
                ++idx;
            }
        }

        max.setRow(ledCol, buf);
    }
}

#endif  // MODM_MAX7219_MATRIX_HPP
