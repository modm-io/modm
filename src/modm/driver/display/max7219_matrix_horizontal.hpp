/*
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MAX7219_MATRIX_HORIZONTAL_HPP
#define MODM_MAX7219_MATRIX_HORIZONTAL_HPP

#include <modm/driver/display/max7219.hpp>
#include <modm/ui/display/monochrome_graphic_display_horizontal.hpp>

namespace modm
{
/**
 * Driver for arrays of 8x8 LED matrices driven by MAX7219.
 *
 * This driver is intended for cheaply available 4x MAX7219 dot matrix
 * display ordered in a horizontal fashion.
 *
 * Layout of the modules for a 64 pixel x 16 pixel display (four strips
 * of 4 led modules):
 *
 *          (0,0)
 *            +----+----+----+----+    +----+----+----+----+
 *     Din => | 1  | 2  | 3  | 4  | -> | 5  | 6  | 7  | 8  | --.
 *            |    |    |    |    |    |    |    |    |    |   |
 *            +----+----+----+----+    +----+----+----+----+   |
 *        .-> | 9  | 10 | 11 | 12 | -> | 13 | 14 | 15 | 16 |   |
 *        |   |    |    |    |    |    |    |    |    |    |   |
 *        |   +----+----+----+----+    +----+----+----+----+   |
 *        |                                            (31,15) |
 *        `----------------------------------------------------´
 *
 * @tparam  SPI     SPI Interface
 * @tapram  CS      Chip Select of MAX7219
 * @tparam  COLUMNS Number of led modules placed horizontally, from left to right.
 * @tparam  ROW     Number of modules placed vertically
 *
 * @ingroup modm_driver_max7219
 */
template <typename SPI, typename CS, uint8_t COLUMNS = 1, uint8_t ROWS = 1>
class Max7219MatrixHorizontal
    : public MonochromeGraphicDisplayHorizontal<8 * COLUMNS, 8 * ROWS>
{
public:
    virtual ~Max7219MatrixHorizontal() = default;

    inline void
    initialize()
    {
        mDriver.initializeMatrix();
        this->clear();
    }

    /**
     * Set the brightness of the whole matrix.
     *
     * \param intensity range 0 to 15.
     */
    inline void
    setBrightness(uint8_t intensity)
    {
        mDriver.setBrightness(intensity);
    }

    virtual void
    update() override;

protected:
    Max7219<SPI, CS, COLUMNS * ROWS> mDriver;
};

// Implementation of the member functions

template <typename SPI, typename CS, uint8_t COLUMNS, uint8_t ROWS>
void
Max7219MatrixHorizontal<SPI, CS, COLUMNS, ROWS>::update()
{
    // Iterate column 0 to 7 of MAX LED driver
    for (uint8_t ledCol = 0; ledCol < 8; ++ledCol)
    {
        uint8_t buf[COLUMNS * ROWS];
        uint8_t idx = COLUMNS * ROWS;

        // row is the row of LED modules and
        // a group of eight pixels vertical
        for (uint8_t row = 0; row < ROWS; ++row)
        {
            // col is the column of LED modules and
            // a group of eight pixels horizontal
            for (uint8_t col = 0; col < COLUMNS; ++col)
            {
                buf[--idx] = this->buffer[col][row * 8 + ledCol];
            }
        }

        mDriver.setRow(7 - ledCol, buf);
    }
}
}  // namespace modm

#endif
