// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP2515_CAN_BIT_TIMINGS_HPP
#define MODM_MCP2515_CAN_BIT_TIMINGS_HPP

#include <modm/architecture/interface/can.hpp>
#include <modm/architecture/interface/clock.hpp>

namespace modm
{

template<int32_t Clk, int32_t Bitrate>
class CanBitTimingMcp2515
{
private:
	struct CanBitTimingConfiguration {
		uint8_t sjw;
		uint8_t prop;
		uint8_t ps1;
		uint8_t ps2;
		uint8_t prescaler;
	};

	static constexpr uint8_t calcSJW() {
		return (Clk ==  modm::clock::MHz8)? 1 :
			   (Clk == modm::clock::MHz16)? 1 :
			   (Clk == modm::clock::MHz20)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 1 : 3 ) : 0;
	}

	static constexpr uint8_t calcProp() {
		return (Clk ==  modm::clock::MHz8)? 1 :
			   (Clk == modm::clock::MHz16)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 1 : 3 ) :
			   (Clk == modm::clock::MHz20)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 3 : 5 ) : 0;
	}

	static constexpr uint8_t calcPS1() {
		return (Clk ==  modm::clock::MHz8)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 1 : 4 ) :
			   (Clk == modm::clock::MHz16)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 4 : 8 ) :
			   (Clk == modm::clock::MHz20)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 4 : 8 ) : 0;
	}

	static constexpr uint8_t calcPS2() {
		return (Clk ==  modm::clock::MHz8)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 1 : 2) :
			   (Clk == modm::clock::MHz16)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 2 : 4 ) :
			   (Clk == modm::clock::MHz20)? ( (Bitrate == modm::Can::Bitrate::MBps1) ? 2 : 4 ) : 0;
	}

	static constexpr uint8_t calcPrescaler(uint8_t sjw, uint8_t prop, uint8_t ps1, uint8_t ps2) {
		return Clk / (Bitrate * (sjw + prop + ps1 + ps2));
	}

	static constexpr CanBitTimingConfiguration calculateBestConfig() {
		return { calcSJW(), calcProp(), calcPS1(), calcPS2(),
			calcPrescaler(calcSJW(), calcProp(), calcPS1(), calcPS2()) };
	}

	static constexpr CanBitTimingConfiguration BestConfig = calculateBestConfig();

public:
	static constexpr uint8_t getSJW()  { return BestConfig.sjw;  }
	static constexpr uint8_t getProp() { return BestConfig.prop; }
	static constexpr uint8_t getPS1()  { return BestConfig.ps1;  }
	static constexpr uint8_t getPS2()  { return BestConfig.ps2;  }
	static constexpr uint8_t getPrescaler() { return BestConfig.prescaler; }

private:
	static_assert(getSJW()  >= 1 and getSJW()  <= 4, "SJW only allowed between 1 and 4.");
	static_assert(getProp() >= 1 and getProp() <= 8, "Prop only allowed between 1 and 8.");
	static_assert(getPS1()  >= 1 and getPS1()  <= 8, "PS1 only allowed between 1 and 8.");
	static_assert(getPS2()  >= 1 and getPS2()  <= 8, "PS2 only allowed between 1 and 8.");
	static_assert(getPrescaler() >= 2 and getPrescaler() <= 128, "Prescaler only allowed between 2 and 128.");
	static_assert(getProp() + getPS1() >= getPS2(), "Condition Prop + PS1 >= PS2 not fulfilled.");

	// (Clk / getPrescaler()) / Sum TQ == Baudrate
	static_assert((Clk / getPrescaler()) / (getSJW() + getProp() + getPS1() + getPS2()) == Bitrate, "Desired baud rate not achived.");

	// Sampling point >= 75%, <= 80% of Nominal Bit Time (NBT)
	static_assert((double(getSJW() + getProp() + getPS1()) / double(getSJW() + getProp() + getPS1() + getPS2())) >= 0.75 ,"Sampling point is earlier than 75% of nominal bit time.");
	static_assert((double(getSJW() + getProp() + getPS1()) / double(getSJW() + getProp() + getPS1() + getPS2())) <= 0.80 ,"Sampling point is later than 80% of nominal bit time.");
};

}

#endif
