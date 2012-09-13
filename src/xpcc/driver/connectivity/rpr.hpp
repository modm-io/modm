// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * \defgroup	rpr		Resilient Packet Ring
 *
 * This is a simplified version of the [Resilient Packet Ring]
 * ( http://en.wikipedia.org/wiki/Resilient_Packet_Ring ).
 *
 * Action callbacks to query requests can be defined as well as universal
 * callbacks to any transmitted messaged (Listener callbacks).
 * As an optional advanced feature, error handling callbacks can also be defined,
 * which fire if messages have not been able to be sent, or requests timed out
 * or misbehaved in other manners, or other nodes query unavailable information.
 *
 *
 * Frame Format
 * ------------
\verbatim
	   |<------ FCS Coverage ----->|
+------+-------+-------+-----------+-------+------+
|  SD  |  DA   |  SA   |  Payload  |  FCS  |  ED  |
+------+-------+-------+-----------+-------+------+
| 8bit | 16bit | 16bit | >=0 bytes | 16bit | 8bit |
+------+-------+-------+-----------+-------+------+
\endverbatim
 *
 *
 * Frame Fields
 * ------------
 *
 * The Start Frame Delimiter (SD) and End Frame Delimiter (ED) are
 * adapted from the [HDLC asynchronous framing]
 * ( http://en.wikipedia.org/wiki/High-Level_Data_Link_Control#Asynchronous_framing ).
 *
 *
 * The Desination Address (DA) and Source Address (DA) format are both 15 bit.
 * This uses the official MAC scheme for 2 Byte addressing.
 *
 * The Destination Address (DA) byte format:
\verbatim
  15   14  13  12  11  10   9   8      7    6   5   4   3   2   1   0
+-----+---+---+---+---+---+---+---+ +-----+---+---+---+---+---+---+---+
| I/G |   Upper (Group) Address   | |      Lower (Group) Address      |
+-----+---+---+---+---+---+---+---+ +-----+---+---+---+---+---+---+---+
      |<----- (Ring Number) ----->| |<---- (Station Subaddress) ----->|
\endverbatim
 * - I/G bit: address is an individual (0) or a group address (1).
 *
 * Individual Addresses identify a perticular station on the LAN and shall be
 * distinct from all other individual stations addresses on the same LAN.
 *
 * Group Addresses are used to address a frame to multiple destination stations
 * and may be associated with zero or more stations on a given LAN.
 *
 * Broadcast Address (0xffff) is used to address every station on the LAN.
 *
 * Null Address (0x0000) means the frame is not addressed to any particular station.
 *
 * It is recommended to use the 7bit Upper Address to identify the ring number,
 * and the 8bit Lower Address as a station subaddress.
 *
 * The Source Address (SA) byte format:
\verbatim
   15   14  13  12  11  10   9   8     7   6   5   4   3   2   1   0
+------+---+---+---+---+---+---+---+ +---+---+---+---+---+---+---+---+
| Zero |        Upper Address      | |         Lower Address         |
+------+---+---+---+---+---+---+---+ +---+---+---+---+---+---+---+---+
\endverbatim
 *
 * The Frame Check Sequence (FCS) uses CRC-16-IBM (0xa001) polynomial with the
 * initial value of 0xffff.
 *
 * It is strongly recommended to keep the maximum payload length lower than 64 bytes,
 * and break down longer payload into multiple frames.
 * The longer the payload the more buffer each station has to allocate.
 *
 * \author	Niklas Hauser
 */

#ifndef XPCC__RPR_HPP
#define XPCC__RPR_HPP

#include "rpr/node.hpp"

#endif	// XPCC__RESILIENT_PACKET_RING_HPP