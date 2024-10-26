/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <modm/math/algorithm/prescaler.hpp>
#include <modm/processing/resumable/resumable.hpp>
#include <hardware/structs/pio.h>

namespace modm::platform::pio::implementation
{

	template <typename Pio,size_t SM>
	class StateMachine {
	private:
		static_assert(SM<NUM_PIO_STATE_MACHINES,"Invalid SM");
		static inline pio_sm_hw& sm() {
			return Pio::pio().sm[SM];
		}
		struct Encoder {
			constexpr uint16_t encodeSideDelay(const pio::implementation::OptionalValue& /*side*/,uint16_t /*delay*/) const {
				return 0;
			}
		};
		template <class Pin,bool isOut>
		static inline void addPin() {
			sm().pinctrl = (1u << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
				(uint32_t(Pin::pin) << PIO_SM0_PINCTRL_SET_BASE_LSB);
			exec(pio::Set.pindirs<isOut?1:0>.encode(Encoder()));
		}
		static inline void exec(const pio::implementation::Instruction& instr) {
			sm().instr = instr.value;
		}
		static constexpr uint32_t STALL_MASK = 1u << (PIO_FDEBUG_TXSTALL_LSB + SM);
		static inline void write_fifo(uint32_t val) {
			Pio::pio().txf[SM] = val;
		}
		static inline uint32_t read_fifo() {
			return Pio::pio().rxf[SM];
		}
	public:
		template < class... Pins >
		static void addOutput() {
			auto pinctrlSaved = sm().pinctrl;
			(addPin<Pins,true>(),...);
			sm().pinctrl = pinctrlSaved;
		}
		template < class... Pins >
		static void addInput() {
			auto pinctrlSaved = sm().pinctrl;
			(addPin<Pins,false>(),...);
			sm().pinctrl = pinctrlSaved;
		}
		template <typename Setting  >
		struct Config {
			using settings = Setting;
			uint32_t clkdiv{0};
			uint32_t execctrl{0};
			uint32_t shiftctrl{0};
			uint32_t pinctrl{0};

			template <  uint16_t div_int, uint8_t div_frac >
			constexpr Config& setClkDiv() {
				clkdiv = 	(uint32_t(div_frac) << PIO_SM0_CLKDIV_FRAC_LSB) |
									(uint32_t(div_int) << PIO_SM0_CLKDIV_INT_LSB);
				return *this;
			}
			constexpr Config& setWrap(uint32_t wrap_target,uint32_t wrap) {
				execctrl = 	(execctrl & ~(PIO_SM0_EXECCTRL_WRAP_TOP_BITS | PIO_SM0_EXECCTRL_WRAP_BOTTOM_BITS)) |
										(wrap_target << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB) |
										(wrap << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);
				return *this;
			}
			template <bool shiftRight,bool autoPush,size_t pushThreshold>
			constexpr Config& setInShift() {
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
			constexpr Config& setOutShift() {
				shiftctrl = (shiftctrl &
										~(PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_BITS |
											PIO_SM0_SHIFTCTRL_AUTOPULL_BITS |
											PIO_SM0_SHIFTCTRL_PULL_THRESH_BITS)) |
										((shiftRight?1:0) << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB) |
										((autoPull?1:0) << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) |
										((pullThreshold & 0x1fu) << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
				return *this;
			}
			constexpr Config& setSideset() {
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
			constexpr Config& setSidesetPins() {
				pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_SIDESET_BASE_BITS)) |
								(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
				return *this;
			}
			template < class StartPin , size_t count>
			constexpr Config& setOutPins() {
				pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_OUT_BASE_BITS | PIO_SM0_PINCTRL_OUT_COUNT_BITS)) |
									(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_OUT_BASE_LSB) |
									(count << PIO_SM0_PINCTRL_OUT_COUNT_LSB);
				return *this;
			}
			template < class StartPin , size_t count>
			constexpr Config& setSetPins() {
				pinctrl = (pinctrl & ~(PIO_SM0_PINCTRL_SET_BASE_BITS | PIO_SM0_PINCTRL_SET_COUNT_BITS)) |
									(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_SET_BASE_LSB) |
									(count << PIO_SM0_PINCTRL_SET_COUNT_LSB);
				return *this;
			}
			template < class StartPin>
			constexpr Config& setInPins() {
				pinctrl = (pinctrl & ~PIO_SM0_PINCTRL_IN_BASE_BITS) |
									(uint32_t(StartPin::pin) << PIO_SM0_PINCTRL_IN_BASE_LSB);
				return *this;
			}
			constexpr Config& setFifoJoinTx() {
				shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS)) |
										PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS;
				return *this;
			}
			constexpr Config& setFifoJoinRx() {
				shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS)) |
										PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS;
				return *this;
			}
			constexpr Config& setFifoJoinNone() {
				shiftctrl = (shiftctrl & ~(PIO_SM0_SHIFTCTRL_FJOIN_TX_BITS | PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS));
				return *this;
			}

			void init(uint16_t startPC) {
				StateMachine::setEnabled(false);

				StateMachine::sm().clkdiv = clkdiv;
				StateMachine::sm().execctrl = execctrl;
				StateMachine::sm().shiftctrl = shiftctrl;
				StateMachine::sm().pinctrl = pinctrl;

				StateMachine::clearFifos();
				// Clear FIFO debug flags
			  constexpr uint32_t fdebug_sm_mask =
											(1u << PIO_FDEBUG_TXOVER_LSB) |
											(1u << PIO_FDEBUG_RXUNDER_LSB) |
											(1u << PIO_FDEBUG_TXSTALL_LSB) |
											(1u << PIO_FDEBUG_RXSTALL_LSB);
				Pio::pio().fdebug = fdebug_sm_mask << SM;
				StateMachine::restart();
				StateMachine::clkDivRestart();
				StateMachine::jump(startPC);
			}

			template< class SystemClock, frequency_t freq, percent_t tolerance=pct(1) >
			constexpr Config&
			setFrequency()
			{
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
		static constexpr auto config(size_t offset,const Program& ) {
			return Config<typename Program::settings>()
				.setSideset()
				.template setClkDiv<1,0>()
				.setWrap(Program::wrap_target+offset,Program::wrap+offset)
				.template setInShift<true,false,32>()
				.template setOutShift<true,false,32>();
		}

		static void clearFifos() {
			// changing the FIFO join state clears the fifo
			hw_xor_bits(&sm().shiftctrl, PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS);
			hw_xor_bits(&sm().shiftctrl, PIO_SM0_SHIFTCTRL_FJOIN_RX_BITS);
		}
		static void setEnabled(bool enabled) {
			Pio::pio().ctrl = (Pio::pio().ctrl & ~(1u << SM)) | ((enabled?1:0) << SM);
		}
		static void restart() {
			Pio::pio().ctrl |= 1u << (PIO_CTRL_SM_RESTART_LSB + SM);
		}
		static void clkDivRestart() {
			Pio::pio().ctrl |= 1u << (PIO_CTRL_CLKDIV_RESTART_LSB + SM);
		}

		static void jump(uint16_t pc) {
			exec(pio::Jmp.encode(Encoder())|pc);
		}

		static inline bool txFifoFull() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_TXFULL_LSB + SM))) != 0;
		}
		static inline bool txFifoEmpty() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_TXEMPTY_LSB + SM))) != 0;
		}

		static inline void writeBlocking(uint32_t val) {
			while(txFifoFull()) {__NOP();}
			write_fifo(val);
		}
		static inline void writeBlocking(const uint32_t* data,size_t length) {
			for (size_t i=0;i<length;++i) {
				writeBlocking(data[i]);
			}
		}
		static ResumableResult<void> write(uint32_t val);

		static inline bool rxFifoFull() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_RXFULL_LSB + SM))) != 0;
		}
		static inline bool rxFifoEmpty() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + SM))) != 0;
		}
		static inline uint32_t readBlocking() {
			while(rxFifoEmpty()) {__NOP();}
			return read_fifo();
		}
		static ResumableResult<uint32_t> read();

		static void resetStall() {
			Pio::pio().fdebug = STALL_MASK;
		}
		static void waitStall() {
			while ((Pio::pio().fdebug & STALL_MASK)==0) {__NOP();}
		}

	};
}
#include "pio_sm_impl.hpp"
