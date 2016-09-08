/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

#include "al_avreb_can.hpp"

xpcc::IODeviceWrapper< Uart1, xpcc::IOBuffer::BlockIfFull > serialDevice;
xpcc::IOStream serialStream(serialDevice);
