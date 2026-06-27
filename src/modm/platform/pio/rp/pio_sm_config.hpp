/*
 * Copyright (c) 2026, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <hardware/structs/pio.h>

namespace modm::platform::pio
{

	class StateMachineConfig {
	public:
	
		uint32_t clkdiv{0};
		uint32_t execctrl{0};
		uint32_t shiftctrl{0};
		uint32_t pinctrl{0};

		template <  uint16_t div_int, uint8_t div_frac >
		constexpr StateMachineConfig& setClkDiv() {
			clkdiv = 	(uint32_t(div_frac) << PIO_SM0_CLKDIV_FRAC_LSB) |
								(uint32_t(div_int) << PIO_SM0_CLKDIV_INT_LSB);
			return *this;
		}
		constexpr StateMachineConfig& setWrap(uint32_t wrap_target,uint32_t wrap) {
			execctrl = 	(execctrl & ~(PIO_SM0_EXECCTRL_WRAP_TOP_BITS | PIO_SM0_EXECCTRL_WRAP_BOTTOM_BITS)) |
									(wrap_target << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB) |
									(wrap << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);
			return *this;
		}
		template <bool shiftRight,bool autoPush,size_t pushThreshold>
		constexpr StateMachineConfig& setInShift() {
			shiftctrl = (shiftctrl &
										~(PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_BITS |
											PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS |
											PIO_SM0_SHIFTCTRL_PUSH_THRESH_BITS)) |
									((shiftRight?1:0) << PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_LSB) |
									((autoPush?1:0) << PIO_SM0_SHIFTCTRL_AUTOPUSH_LSB) |
									((pushThreshold & 0x1fu) << PIO_SM0_SHIFTCTRL_PUSH_THRESH_LSB);
			return *this;
		}
		template <bool shiftRight,bool autoPull,size_t pullThreshold>
		constexpr StateMachineConfig& setOutShift() {
			shiftctrl = (shiftctrl &
									~(PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_BITS |
										PIO_SM0_SHIFTCTRL_AUTOPULL_BITS |
										PIO_SM0_SHIFTCTRL_PULL_THRESH_BITS)) |
									((shiftRight?1:0) << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB) |
									((autoPull?1:0) << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) |
									((pullThreshold & 0x1fu) << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
			return *this;
		}
		template <typename settings>
		constexpr StateMachineConfig& setSideset() {
			constexpr auto bit_count = settings::sideset_count;
			constexpr auto optional = settings::sideset_is_opt;
			constexpr auto pindirs = settings::sideset_pindirs;
			static_assert(bit_count <= 5,"too many sideset pins");
			static_assert(!optional || bit_count >= 1,"invalid config");

			pinctrl = (pinctrl & ~PIO_SM0_PINCTRL_SIDESET_COUNT_BITS) |
								(bit_count << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB);
			execctrl = (execctrl & ~(PIO_SM0_EXECCTRL_SIDE_EN_BITS | PIO_SM0_EXECCTRL_SIDE_PINDIR_BITS)) |
									((optional?1:0) << PIO_SM0_EXECCTRL_SIDE_EN_LSB) |
									((pindirs?1:0) << PIO_SM0_EXECCTRL_SIDE_PINDIR_LSB);
      		return *this;
		}
		template < class StartPin>
		constexpr StateMachineConfig& setSidesetPins() {
			pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_SIDESET_BASE_BITS)) |
							(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
			return *this;
		}
		template < class StartPin , size_t count>
		constexpr StateMachineConfig& setOutPins() {
			pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_OUT_BASE_BITS | PIO_SM0_PINCTRL_OUT_COUNT_BITS)) |
								(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_OUT_BASE_LSB) |
								(uint32_t(count) << PIO_SM0_PINCTRL_OUT_COUNT_LSB);
			return *this;
		}
		template < class StartPin , size_t count>
		constexpr StateMachineConfig& setSetPins() {
			pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_SET_BASE_BITS | PIO_SM0_PINCTRL_SET_COUNT_BITS)) |
								(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_SET_BASE_LSB) |
								(uint32_t(count) << PIO_SM0_PINCTRL_SET_COUNT_LSB);
			return *this;
		}
		template < class StartPin>
		constexpr StateMachineConfig& setInPins() {
			pinctrl = (pinctrl & ~PIO_SM0_PINCTRL_IN_BASE_BITS) |
								(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_IN_BASE_LSB);
			return *this;
		}
		constexpr StateMachineConfig& setFifoJoinTx() {
			shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS)) |
									PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS;
			return *this;
		}
		constexpr StateMachineConfig& setFifoJoinRx() {
			shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS)) |
									PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS;
			return *this;
		}
		constexpr StateMachineConfig& setFifoJoinNone() {
			shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS));
			return *this;
		}

		template< class SystemClock, frequency_t freq, percent_t tolerance=pct(1) >
		constexpr StateMachineConfig& setFrequency() {
			static_assert(freq <= SystemClock::PeriFrequency and
						  SystemClock::PeriFrequency <= freq  * 65535ull,
					"SystemClock::PeriFrequency must be in the range [freq, 65535 x freq].");
			// 16.8 fractional freq generator
			constexpr uint64_t min = (1ul << 8);
			constexpr uint64_t max = (1ul << 24) - 1ul;

			constexpr uint64_t prescaler = std::max(min,std::min(max,(uint64_t(SystemClock::SysFrequency)<<8) / freq));
			constexpr uint64_t res_freq = (uint64_t(SystemClock::SysFrequency)<<8) / prescaler;

			//constexpr auto result = Prescaler::from_linear(SystemClock::SysFrequency<<8, freq, min, max);
			modm::PeripheralDriver::assertBaudrateInTolerance< res_freq, uint64_t(freq), tolerance >();

			clkdiv = 	(uint32_t(prescaler & 0xff) << PIO_SM0_CLKDIV_FRAC_LSB) |
								(uint32_t(prescaler>>8) << PIO_SM0_CLKDIV_INT_LSB);

			return *this;
		}
	};

	template <typename Program>
	static inline constexpr StateMachineConfig sm_config(size_t offset,const Program& ) {
		return StateMachineConfig()
			.template setSideset<typename Program::settings>()
			.template setClkDiv<1,0>()
			.setWrap(Program::wrap_target+offset,Program::wrap+offset)
			.template setInShift<true,false,32>()
			.template setOutShift<true,false,32>();
	}
}
