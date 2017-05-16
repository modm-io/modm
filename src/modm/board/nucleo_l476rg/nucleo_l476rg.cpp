/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */
#include "nucleo_l476rg.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Board::stlink::Uart, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);
