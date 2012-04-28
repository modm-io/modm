// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/**
 * \ingroup		connectivity
 * \defgroup	sab		(SAB) Sensor Actuator Bus
 * 
 * \section sab_intro	Introduction
 * 
 * The SAB (<b>S</b>ensor <b>A</b>ctuator <b>B</b>us) is a simple
 * master-slave bus system. It is primarily used to query simple sensors and
 * control actuators inside our robots.
 * 
 * One master can communicate with up to 32 slaves. The slaves are only allowed to
 * transmit after a direct request by the master. They may signal an event by an
 * extra IO line, but this depends on the slave.
 * 
 * A complete example, explaining how to use the classes, is available in the
 * \c example/sab folder.
 * 
 * \section sab_protocol Protocol
 * 
 * Features:
 * - Baudrate is 115200 Baud.
 * - Maximum payload length is 32 byte.
 * - CRC8 (1-Wire)
 * 
 * \subsection structure	Structure
 * 
\verbatim
+------+--------+--------+---------+--------------+-----+
| SYNC | LENGTH | HEADER | COMMAND | ... DATA ... | CRC |
+------+--------+--------+---------+--------------+-----+
\endverbatim
 * 
 * - \b SYNC - Synchronization byte (always 0x54)
 * - \b LENGTH - Length of the payload (without header, command and CRC byte) 
 * - \b HEADER - Address of the slave and two flag bits
 * - \b COMMAND - Command code
 * - \b DATA - Up to 32 byte of payload
 * - \b CRC - CRC-8 checksum (iButton)
 * 
 * \subsubsection header Header
 * 
\verbatim
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
\endverbatim
 * 
 * The \c second bit is always \c false when the master is transmitting. In the other
 * direction, when the slaves are responding, the \c second bit has to following
 * meaning:
 * 
 * - \c true - Message is an positive response and may contain a payload
 * - \c false - Message signals an error condition and carries only one byte of
 *    payload. This byte is an error code.
 * 
 * \section sab_electric	Electrical characteristics
 * 
 * Between different boards CAN transceivers are used. Compared to RS485 the
 * CAN transceivers have the advantage to work without a separate direction input.
 * You can just connected the transceiver directly to the UART of your
 * microcontroller. 
 * 
 * \image html sab_external.png
 * 
 * Within a single PCB, standard digital levels are used (either 0-3,3V or 0-5V)
 * in a multi-drop configuration. Meaning it does not allow multiple drivers but
 * multiple receivers.
 * The idle state of a UART transmission line is high, so standard TTL-AND gates
 * have to be used for bundling transmission lines from multiple slaves.
 * 
 * \image html sab_internal.png
 * 
 * Both approaches can be combined to reduce the number of needed CAN
 * transceivers on a single board. Between two boards you should always use
 * transceivers and therefore differential signaling to improve noise immunity.
 * 
 * The signal lines to indicate events by the slave are strict optional, you may
 * or may not use them (if the slave provides them).
 * 
 * \author	Fabian Greif
 */

#ifndef XPCC__SAB_HPP
#define XPCC__SAB_HPP

#include "sab/interface.hpp"
#include "sab/slave.hpp"
#include "sab/master.hpp"

#endif	// XPCC__SAB_HPP
