/* Copyright (c) 2014-2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

//
// Arduino UNO
//

#include "arduino_uno.hpp"

xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > serialDevice;
xpcc::IOStream serialStream(serialDevice);
