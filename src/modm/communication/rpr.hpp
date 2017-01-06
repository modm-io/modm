/*
 * Copyright (c) 2014-2015, Niklas Hauser
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
 * @defgroup	rpr		Resilient Packet Ring (RPR)
 *
 * This is a simplified version of the [Resilient Packet Ring]
 * ( http://en.wikipedia.org/wiki/Resilient_Packet_Ring ).
 *
 * Listener callbacks to received messaged can be defined. All messages are buffered.
 * As an optional advanced feature, error handling callbacks can also be defined.
 *
 *
 * Frame Format
 * ------------
@verbatim
	   |<------ FCS Coverage ----->|
+------+-------+-------+-----------+-------+------+
|  SD  |  DA   |  SA   |  Payload  |  FCS  |  ED  |
+------+-------+-------+-----------+-------+------+
| 8bit | 16bit | 16bit | >=0 bytes | 16bit | 8bit |
+------+-------+-------+-----------+-------+------+
@endverbatim
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
@verbatim
  15   14  13  12  11  10   9   8      7    6   5   4   3   2   1   0
+-----+---+---+---+---+---+---+---+ +-----+---+---+---+---+---+---+---+
| I/G |   Upper (Group) Address   | |      Lower (Group) Address      |
+-----+---+---+---+---+---+---+---+ +-----+---+---+---+---+---+---+---+
      |<----- (Ring Number) ----->| |<---- (Station Subaddress) ----->|
@endverbatim
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
@verbatim
   15   14  13  12  11  10   9   8     7   6   5   4   3   2   1   0
+------+---+---+---+---+---+---+---+ +---+---+---+---+---+---+---+---+
| Zero |        Upper Address      | |         Lower Address         |
+------+---+---+---+---+---+---+---+ +---+---+---+---+---+---+---+---+
@endverbatim
 *
 * The Frame Check Sequence (FCS) uses CRC-16-IBM (0xa001) polynomial with the
 * initial value of 0xffff.
 *
 * It is strongly recommended to keep the maximum payload length lower than 64 bytes,
 * and break down longer payload into multiple frames.
 * The longer the payload the more buffer each station has to allocate.
 *
 * @author	Niklas Hauser
 */

#include "rpr/node.hpp"
