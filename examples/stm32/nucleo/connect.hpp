/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_EXAMPLE_NUCLEO_CONNECT_HPP
#define MODM_EXAMPLE_NUCLEO_CONNECT_HPP

namespace Nucleo
{

#ifdef NUCLEO_EXAMPLE_I2C
namespace I2c
{

#if defined(MODM_BOARD_NUCLEO_144) \
 or defined(MODM_BOARD_NUCLEO_64)
using Scl = Board::D15;
using Sda = Board::D14;
#else
using Scl = Board::A5;
using Sda = Board::A4;
#endif

#if defined(MODM_BOARD_NUCLEO_F031K6) \
 or defined(MODM_BOARD_NUCLEO_F042K6) \
 or defined(MODM_BOARD_NUCLEO_F303K8)
#define BITBANG
// These devices don't have HW I2C implemented yet
using Master = BitBangI2cMaster<Scl, Sda>;
#else
using Master = I2cMaster1;
#endif

void
connect()
{
#ifdef BITBANG
	Master::connect<Scl::BitBang, Sda::BitBang>();
#else
	Master::connect<Scl::Scl, Sda::Sda>();
#endif
	Master::initialize<Board::systemClock, Master::Baudrate::Standard>();
}

}
#endif // NUCLEO_EXAMPLE_I2C

#ifdef NUCLEO_EXAMPLE_SPI
namespace Spi
{

using Cs = GpioA4;
using Mosi = GpioB5;
using Miso = GpioB4;
using Sck = GpioB3;

using Master = SpiMaster1;

void
connect()
{
	Master::connect<Mosi::Mosi, Miso::Miso, Sck::Sck>();
	Master::initialize<Board::systemClock, MHz8, 500>();
}

}
#endif // NUCLEO_EXAMPLE_SPI

}

#endif // MODM_EXAMPLE_NUCLEO_CONNECT_HPP