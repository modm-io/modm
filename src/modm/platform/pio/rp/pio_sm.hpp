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
#include <modm/processing/fiber.hpp>
#include <hardware/structs/pio.h>

#include "pio_sm_config.hpp"

namespace modm::platform::pio::implementation
{


	namespace details {
		template <typename DataType>
		struct hw_data_type;
		template <>
		struct hw_data_type<uint32_t> {
			using rw_type = io_rw_32;
			using ro_type = io_ro_32;
		};
		template <>
		struct hw_data_type<uint16_t> {
			using rw_type = io_rw_16;
			using ro_type = io_ro_16;
		};
		template <>
		struct hw_data_type<uint8_t> {
			using rw_type = io_rw_8;
			using ro_type = io_ro_8;
		};
	}

	template <typename Pio,size_t SM, typename DataType>
	class FifoAdapter {
	private:
		using rw_type = typename details::hw_data_type<DataType>::rw_type;
		using ro_type = typename details::hw_data_type<DataType>::ro_type;
	public:
		static inline bool txFifoFull() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_TXFULL_LSB + SM))) != 0;
		}
		static inline bool txFifoEmpty() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_TXEMPTY_LSB + SM))) != 0;
		}

		static inline void writeBlocking(DataType val) {
			while(txFifoFull()) {__NOP();}
			writeUnsafe(val);
		}
		static inline void writeBlocking(const DataType* data,size_t length) {
			for (size_t i=0;i<length;++i) {
				writeBlocking(data[i]);
			}
		}
		static void write(DataType val) {
			while (txFifoFull()) modm::this_fiber::yield();
    		writeUnsafe(val);
		}
		static inline void writeUnsafe(DataType val) {
			*reinterpret_cast<rw_type*>(&Pio::pio().txf[SM]) = val;
		}

		static inline bool rxFifoFull() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_RXFULL_LSB + SM))) != 0;
		}
		static inline bool rxFifoEmpty() {
			return (Pio::pio().fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + SM))) != 0;
		}
		static inline DataType readBlocking() {
			while(rxFifoEmpty()) {__NOP();}
			return readUnsafe();
		}
		static DataType read() {
			while (rxFifoEmpty()) modm::this_fiber::yield();
		    return readUnsafe();
		}
		static inline DataType readUnsafe() {
			return *reinterpret_cast<ro_type*>(&Pio::pio().rxf[SM]);
		}
	};

	// default access to full dword
	template <typename Pio,size_t SM>
	class StateMachine : public FifoAdapter<Pio, SM, uint32_t> {
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
		template <class Pin>
		static inline void setPinDir(bool isOut) {
			sm().pinctrl = (1u << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
				(uint32_t(Pin::pin) << PIO_SM0_PINCTRL_SET_BASE_LSB);
			if (isOut) {
				exec(pio::Set.pindirs<1>.encode(Encoder()));
			} else {
				exec(pio::Set.pindirs<0>.encode(Encoder()));
			}
		}
		template <class Pin>
		static inline void setPinVal(bool value) {
			sm().pinctrl = (1u << PIO_SM0_PINCTRL_SET_COUNT_LSB) |
				(uint32_t(Pin::pin) << PIO_SM0_PINCTRL_SET_BASE_LSB);
			if (value) {
				exec(pio::Set.pins<1>.encode(Encoder()));
			} else {
				exec(pio::Set.pins<0>.encode(Encoder()));
			}
		}
		static inline void exec(const pio::implementation::Instruction& instr) {
			sm().instr = instr.value;
		}
		static constexpr uint32_t STALL_MASK = 1u << (PIO_FDEBUG_TXSTALL_LSB + SM);
		
		template <typename>
		struct ToBoolArg {
			using type = bool;
		};
	public:
		template < class... Pins >
		static void setPinsDirection(typename ToBoolArg<Pins>::type... value) {
			auto pinctrlSaved = sm().pinctrl;
			(setPinDir<Pins>(value),...);
			sm().pinctrl = pinctrlSaved;
		}

		template < class... Pins >
		static void setPinsValue(typename ToBoolArg<Pins>::type... value) {
			auto pinctrlSaved = sm().pinctrl;
			auto execctrSaved = sm().execctrl;
			(setPinVal<Pins>(value),...);
			sm().pinctrl = pinctrlSaved;
			sm().execctrl = execctrSaved;
		}

		static void init(const StateMachineConfig& config, uint16_t startPC) {
			setEnabled(false);

			sm().clkdiv = config.clkdiv;
			sm().execctrl = config.execctrl;
			sm().shiftctrl = config.shiftctrl;
			sm().pinctrl = config.pinctrl;

			clearFifos();
			// Clear FIFO debug flags
		  	constexpr uint32_t fdebug_sm_mask =
										(1u << PIO_FDEBUG_TXOVER_LSB) |
										(1u << PIO_FDEBUG_RXUNDER_LSB) |
										(1u << PIO_FDEBUG_TXSTALL_LSB) |
										(1u << PIO_FDEBUG_RXSTALL_LSB);
			Pio::pio().fdebug = fdebug_sm_mask << SM;
			restart();
			clkDivRestart();
			jump(startPC);
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

		static void resetStall() {
			Pio::pio().fdebug = STALL_MASK;
		}
		static void waitStall() {
			while ((Pio::pio().fdebug & STALL_MASK)==0) {__NOP();}
		}
		template <class SystemClock>
		static void setFrequency(frequency_t freq) {
			// 16.8 fractional freq generator
			constexpr uint64_t min = (1ul << 8);
			constexpr uint64_t max = (1ul << 24) - 1ul;

			uint64_t prescaler = std::max(min,std::min(max,(uint64_t(SystemClock::SysFrequency)<<8) / freq));

			StateMachine::sm().clkdiv = (uint32_t(prescaler & 0xff) << PIO_SM0_CLKDIV_FRAC_LSB) |
								(uint32_t(prescaler>>8) << PIO_SM0_CLKDIV_INT_LSB);

		}

		using FifoAccess8 = FifoAdapter<Pio,SM,uint8_t>;
		using FifoAccess16 = FifoAdapter<Pio,SM,uint16_t>;
		using FifoAccess32 = FifoAdapter<Pio,SM,uint32_t>;

	};
}
