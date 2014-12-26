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
 * @defgroup	sab2		(SAB2) Sensor Actuator Bus v2
 *
 * @section sab_intro	Introduction
 *
 * The SAB (**S**ensor **A**ctuator **B**us) is a simple
 * master-slave bus system. It is primarily used to query simple sensors and
 * control actuators inside our robots.
 *
 * One master can communicate with up to 32 slaves. The slaves are only allowed to
 * transmit after a direct request by the master. They may signal an event by an
 * extra IO line, but this depends on the slave.
 *
 * A complete example, explaining how to use the classes, is available in the
 * `example/sab` folder.
 *
 * @section sab_protocol Protocol
 *
 * Features:
 * - Baudrate is 115200 Baud.
 * - Maximum payload length is 32 byte.
 * - CRC8 (1-Wire)
 *
 * @subsection structure	Structure
 *
 @verbatim
 +------+--------+--------+---------+--------------+-----+
 | SYNC | LENGTH | HEADER | COMMAND | ... DATA ... | CRC |
 +------+--------+--------+---------+--------------+-----+
 @endverbatim
 *
 * - `SYNC` - Synchronization byte (always 0x54)
 * - `LENGTH` - Length of the payload (without header, command and CRC byte)
 * - `HEADER` - Address of the slave and two flag bits
 * - `COMMAND` - Command code
 * - `DATA` - Up to 32 byte of payload
 * - `CRC` - CRC-8 checksum (iButton)
 *
 * @subsubsection header Header
 *
 @verbatim
 7   6   5   4   3   2   1   0
 +---+---+---+---+---+---+---+---+
 | Flags |      ADDRESS          |
 +---+---+---+---+---+---+---+---+

 Flags | Meaning
 --------+---------
 0   0 | request by the master
 0   1 | reserved
 1   0 | negative response from the slave (NACK)
 1   1 | positive response from the slave (ACK)
 @endverbatim
 *
 * The *second bit* is always `false` when the master is transmitting. In the other
 * direction, when the slaves are responding, the second bit has to following
 * meaning:
 *
 * - `true` - Message is an positive response and may contain a payload
 * - `false` - Message signals an error condition and carries only one byte of
 *    payload. This byte is an error code.
 *
 * @section sab_electric	Electrical characteristics
 *
 * Between different boards CAN transceivers are used. Compared to RS485 the
 * CAN transceivers have the advantage to work without a separate direction input.
 * You can just connected the transceiver directly to the UART of your
 * microcontroller.
 *
 * @image html sab_external.png
 *
 * Within a single PCB, standard digital levels are used (either 0-3,3V or 0-5V)
 * in a multi-drop configuration. Meaning it does not allow multiple drivers but
 * multiple receivers.
 * The idle state of a UART transmission line is high, so standard TTL-AND gates
 * have to be used for bundling transmission lines from multiple slaves.
 *
 * @image html sab_internal.png
 *
 * Both approaches can be combined to reduce the number of needed CAN
 * transceivers on a single board. Between two boards you should always use
 * transceivers and therefore differential signaling to improve noise immunity.
 *
 * The signal lines to indicate events by the slave are strict optional, you may
 * or may not use them (if the slave provides them).
 *
 * @author	Fabian Greif
 */

#ifndef XPCC__SAB2_HPP
#define XPCC__SAB2_HPP

#include "sab2/interface.hpp"

#endif	// XPCC__SAB2_HPP
