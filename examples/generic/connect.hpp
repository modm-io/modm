/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_EXAMPLE_GENERIC_CONNECT_HPP
#define MODM_EXAMPLE_GENERIC_CONNECT_HPP

/* This is a helper file that maps the connectivity of the generic examples
 * to the specific board layout.
 */

namespace Generic
{

#ifdef GENERIC_EXAMPLE_UART

namespace Uart
{

using Tx = GpioOutputA9;
using Instance = Usart1;

void
connect()
{
	Instance::connect<Tx::Tx>();
    Instance::initialize<Board::systemClock, 115200>();
}

}

#endif // GENERIC_EXAMPLE_LOGGER

#ifdef GENERIC_EXAMPLE_CAN
namespace Can
{

#if defined(MODM_BOARD_NUCLEO_144)
using Tx = Board::D66;
using Rx = Board::D67;
#elif defined(MODM_BOARD_NUCLEO_64)
using Tx = Board::D14;
using Rx = Board::D15;
#else
using Tx = GpioOutputB9;
using Rx = GpioInputB8;
#endif

// using Tx2 = GpioOutputB13;
// using Rx2 = GpioInputB5;

/* Unfortunately, STM32 are a bit inconsistent in Peripheral enumerations,
 * Sometimes the CAN peripheral is called just CAN or CAN1 or CAN1 & CAN2. */
#if defined(MODM_BOARD_BLUE_PILL) \
 or defined(MODM_BOARD_BLACK_PILL) \
 or defined(MODM_BOARD_DISCO_F072RB) \
 or defined(MODM_BOARD_DISCO_F303VC) \
 or defined(MODM_BOARD_NUCLEO_F103RB)
using Instance = modm::platform::Can;
#else
using Instance = Can1;
#endif

void
connect()
{
	Instance::connect<Tx::Tx, Rx::Rx>();
	Instance::initialize<Board::systemClock, Instance::Bitrate::kBps125>(9);

	// CanFilter::setStartFilterBankForCan2(14);
	// Instance2::connect<Tx2::Tx, Rx2::Rx>();
	// Instance2::initialize<Board::systemClock, Instance::Bitrate::kBps125>(9);
}

}
#endif // GENERIC_EXAMPLE_CAN

}

#endif // MODM_EXAMPLE_GENERIC_CONNECT_HPP