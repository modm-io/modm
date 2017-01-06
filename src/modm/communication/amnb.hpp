/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2011-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * @ingroup		communication
 * @defgroup	amnb		Asynchronous Multi-Node Bus (AMNB)
 *
 * @section amnb_intro	Introduction
 *
 * The AMNB (**A**synchronous **M**ulti-**N**ode **B**us) is a
 * multi-master bus system, using p-persitent CSMA to send messages.
 *
 * One bus can be populated with up to 64 nodes. The nodes can be queried for
 * data and they will respond like an SAB Slave, and can query data from other
 * nodes like an SAB Master, or they can just broadcast a message.
 * Each node can listen to all the responses and broadcasts and store that
 * information for its purpose.
 *
 * Action callbacks to query requests can be defined as well as universal
 * callbacks to any transmitted messaged (Listener callbacks).
 * As an optional advanced feature, error handling callbacks can also be defined,
 * which fire if messages have not been able to be sent, or requests timed out
 * or misbehaved in other manners, or other nodes query unavailable information.
 *
 * @section amnb_protocol Protocol
 *
 * Features:
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
  0   0 | data broadcast by a node
  0   1 | data request by a node
  1   0 | negative response from the node (NACK)
  1   1 | positive response from the node (ACK)
@endverbatim
 *
 * When transmitting, the *second bit* determines, whether or not to expect an
 * answer from the addressed node.
 * To just send information without any need for acknowledgment, use a broadcast.
 * When a node is responding, the *second bit* has to following meaning:
 *
 * - `true` - Message is an positive response and may contain a payload
 * - `false` - Message signals an error condition and carries only one byte of
 *    payload. This byte is an error code.
 *
 * @section amnb_electric	Electrical characteristics
 *
 * Between different boards CAN transceivers are used. Compared to RS485 the
 * CAN transceivers have the advantage to work without a separate direction input.
 * You can just connected the transceiver directly to the UART of your
 * microcontroller.
 * These are identical to the SAB CAN electrical characteristics.
 * You have to use the CAN transceivers, otherwise it cannot be determined, if
 * the bus is busy or available for transmission.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */

#ifndef MODM_AMNB_HPP
#define MODM_AMNB_HPP

#include "amnb/node.hpp"

#endif	// MODM_AMNB_HPP
