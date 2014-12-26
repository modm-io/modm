// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

/**
 * @ingroup		communication
 * @defgroup 	can (CAN) Controller Area Network
 * @brief 		Interface to the CAN-Communication BUS.
 *
 * Controller–area network (CAN or CAN-bus) is a vehicle bus standard designed
 * to allow microcontrollers and devices to communicate with each other within
 * a vehicle without a host computer.
 *
 * CAN is a message based protocol, designed specifically for automotive
 * applications but now also used in other areas such as industrial automation
 * and medical equipment.
 */

#include "can/message.hpp"
#include "can/canusb.hpp"
