/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_LAN8720A_HPP
#define MODM_LAN8720A_HPP

namespace modm
{

struct Lan8720a
{
	static constexpr uint32_t Address = 0x00;

	struct Register {
		static constexpr uint16_t BCR = 0x0000;
		static constexpr uint16_t BSR = 0x0001;
		static constexpr uint16_t AN  = 0x0004;
		static constexpr uint16_t SR  = 0x001f;
		static constexpr uint16_t ISFR      = 0x001d;
		static constexpr uint16_t ISFR_INT4 = 0x000B;
	};

	static constexpr uint32_t ResetDelay = 0x000000FF;
	static constexpr uint32_t ConfigDelay = 0x00000FFF;
	static constexpr int ReadTimeout = 0xffff;
	static constexpr int WriteTimeout = 0xffff;

	// TODO: use modm::Register for these bits
	static constexpr uint16_t Reset = 0x8000;
	static constexpr uint16_t LoopBack = 0x4000;
	static constexpr uint16_t FullDuplex100M = 0x2100;
	static constexpr uint16_t HalfDuplex100M = 0x2000;
	static constexpr uint16_t FullDuplex10M = 0x0100;
	static constexpr uint16_t HalfDuplex10M = 0x0000;
	static constexpr uint16_t AutoNegotiation = 0x1000;
	static constexpr uint16_t RestartAutoNegotiation = 0x0200;

	static constexpr uint16_t LinkedStatus = 0x0004;
	static constexpr uint16_t DuplexStatus = 0x0010;
	static constexpr uint16_t SpeedStatus = 0x0004;
	static constexpr uint16_t AutoNegotiationComplete = 0x0020;
	static constexpr uint16_t JabberDetection = 0x0002;
};

}

#endif // MODM_LAN8720A_HPP

