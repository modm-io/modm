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
 * \defgroup	amnb		(AMNB) Asynchronous Multi-Node Bus
 * 
 * \section amnb_intro	Introduction
 * 
 * The AMNB (<b>A</b>synchronous <b>M</b>ulti-<b>N</b>ode <b>B</b>us) is a
 * multi-master bus system, using p-persitent CSMA to send messages.
 * 
 * One bus can be populated with up to 32 nodes. The nodes can be queried for
 * data and they will respond like an SAB Slave, and can query data from other
 * nodes like an SAB Master, or they can just broadcast a message.
 * Each node can listen to all the responses and broadcasts and store that
 * information for its purpose.
 *
 * Action callbacks to query requests can be defined as well as universal
 * callbacks to any transmitted messaged (Listener callbacks).
 * As an optional advanced feature, error handling callbacks can also be defined,
 * which fire if messaged have not been able to be sent, or requests timed out
 * or misbehaved in other manners, or other nodes query unavailable information.
 * 
 * \section amnb_protocol Protocol
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
  0   0 | data broadcast by a node
  0   1 | data request by a node
  1   0 | negative response from the node (NACK)
  1   1 | positive response from the node (ACK)
\endverbatim
 * 
 * When transmitting, the \c second bit determines, whether or not to expect an
 * answer from the addressed node.
 * To just send information without any need for acknowledgment, use a broadcast.
 * When a node is responding, the \c second bit has to following meaning:
 * 
 * - \c true - Message is an positive response and may contain a payload
 * - \c false - Message signals an error condition and carries only one byte of
 *    payload. This byte is an error code.
 * 
 * \section amnb_electric	Electrical characteristics
 * 
 * Between different boards CAN transceivers are used. Compared to RS485 the
 * CAN transceivers have the advantage to work without a separate direction input.
 * You can just connected the transceiver directly to the UART of your
 * microcontroller.
 * These are identical to the SAB CAN electrical characteristics.
 * You have to use the CAN transceivers, otherwise it cannot be determined, if
 * the bus is busy or available for transmission.
 * 
 * \author	Fabian Greif
 * \author	Niklas Hauser
 */

#ifndef XPCC__AMNB_HPP
#define XPCC__AMNB_HPP

#include "amnb/node.hpp"

#endif	// XPCC__AMNB_HPP
