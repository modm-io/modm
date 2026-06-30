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

#include <array>
#include <cstdint>
#include <cassert>
#include <cstddef>

namespace modm::platform::pio
{
	namespace implementation
	{
		struct Instruction {
			uint16_t value;
			constexpr Instruction(uint16_t val) : value(val) {}
			constexpr uint16_t store(size_t offset) const {
				return ((value & 0xe000) == 0x0000) ? (value + offset) : value;
			}
			constexpr Instruction operator | (uint16_t params) const {
				return Instruction(value | params);
			}
			constexpr bool operator == (uint16_t val) const {
				return value == val;
			}
		};

		enum class Opcode : uint16_t {
			JMP 	= 0x0000 << 13,
			WAIT 	= 0x0001 << 13,
			IN 		= 0x0002 << 13,
			OUT 	= 0x0003 << 13,
			PUSH 	= 0x0004 << 13,
			PULL 	= (0x0004 << 13) | (0x0001 << 7),
			MOV 	= 0x0005 << 13,
			IRQ 	= 0x0006 << 13,
			SET 	= 0x0007 << 13,
		};

		struct OptionalValue {
			uint16_t value{0};
			bool is_set{false};
		};

		static constexpr uint16_t NO_SIDE_VALUE = 0xffff;

		template <uint16_t side = NO_SIDE_VALUE,uint16_t delay = 0>
		struct SideDelay {
			static constexpr const uint16_t side_value = (side == NO_SIDE_VALUE) ? 0 : side;
			static constexpr const bool side_value_present = side != NO_SIDE_VALUE;
			static constexpr const uint16_t delay_value = delay;
		};

		template <typename Res>
		struct InstructionEncoding {
			static constexpr const Opcode opcode = Res::opcode;
			static constexpr const uint16_t DELAY_SIDESET_SHIFT = 8;
			static constexpr const uint16_t DELAY_SIDESET_MASK = 0x1f << DELAY_SIDESET_SHIFT;
			static constexpr const uint16_t ARG1_SHIFT = 5;

			using SideDelayConfig = SideDelay<>;

			template <uint16_t side = 0>
			struct SideValue : Res {
				using SideDelayConfig = SideDelay<side,0>;
				constexpr SideValue() {}
				template <uint16_t delay = 0>
				struct DelayValue : Res {
					constexpr DelayValue() {}
					using SideDelayConfig = SideDelay<side,delay>;
					template <typename Encoder>
					static constexpr Instruction encode(const Encoder& coder) {
						return Res::encode(coder) | (coder.template encodeSideDelay<SideDelayConfig>() << DELAY_SIDESET_SHIFT);
					}
				};
				template <uint16_t val>
				static constexpr auto delay = DelayValue<val>();

				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder)  {
					return Res::encode(coder) | (coder.template encodeSideDelay<SideDelayConfig>() << DELAY_SIDESET_SHIFT);
				}
			};
			template <uint16_t delay = 0>
			struct DelayValue : Res {
				constexpr DelayValue() {}
				using SideDelayConfig = SideDelay<NO_SIDE_VALUE,delay>;
				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder)  {
					return Res::encode(coder) | (coder.template encodeSideDelay<SideDelayConfig>() << DELAY_SIDESET_SHIFT);
				}
			};

			constexpr InstructionEncoding()  {}
			template <uint16_t val>
			static constexpr auto side = SideValue<val>();
			template <uint16_t val>
			static constexpr auto delay = DelayValue<val>();

			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& ) {
				return Instruction(uint16_t(opcode));
			}
		};


		enum class JmpCondition : uint16_t {
			ALWAYS 	= 0x00,
			NOT_X 	= 0x01,
			X_DEC 	= 0x02,
			NOT_Y	= 0x03,
			Y_DEC 	= 0x04,
			X_NE_Y	= 0x05,
			PIN 	= 0x06,
			NOT_ORSE = 0x07,
		};
		template <typename Label,typename Encoder>
		static constexpr uint16_t getJmpAddr(const Encoder& coder,uint16_t addr_value) {
			if constexpr (std::is_same_v<Label,void>) {
				return addr_value;
			} else {
				return coder.template getOffset<Label>();
			}
		}
		template <typename Label = void,uint16_t addr = 0x0000>
		struct Jmp : InstructionEncoding<Jmp<Label,addr>> {
			static constexpr const Opcode opcode = Opcode::JMP;
			static constexpr const JmpCondition condition_value = JmpCondition::ALWAYS;
			static constexpr const uint16_t addr_value = addr;
			static_assert(addr_value < 0x0f, "Invalid addr");

			using LabelName = Label;
			using Base = InstructionEncoding<Jmp<Label,addr>>;

			template <JmpCondition condition,typename LabelImpl = Label>
			struct JmpImpl : InstructionEncoding<JmpImpl<condition,LabelImpl>>{
				static constexpr const Opcode opcode = Opcode::JMP;
				static constexpr const JmpCondition condition_value = condition;
				static constexpr const uint16_t addr_value = addr;
				using LabelName = LabelImpl;
				using Base = InstructionEncoding<JmpImpl<condition,LabelImpl>>;
				constexpr JmpImpl() {}
				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder) {
					return Base::encode(coder) | (uint16_t(condition_value) << Base::ARG1_SHIFT) | getJmpAddr<LabelName>(coder,addr_value);
				}
				template <typename ToLabel>
				static constexpr auto to = JmpImpl<condition_value,ToLabel>();
			};

			explicit constexpr Jmp() {}
			static constexpr auto not_x = JmpImpl<JmpCondition::NOT_X>();
			static constexpr auto x_dec = JmpImpl<JmpCondition::X_DEC>();
			static constexpr auto not_y = JmpImpl<JmpCondition::NOT_Y>();
			static constexpr auto y_dec = JmpImpl<JmpCondition::Y_DEC>();
			static constexpr auto x_ne_y = JmpImpl<JmpCondition::X_NE_Y>();
			static constexpr auto pin = JmpImpl<JmpCondition::PIN>();
			static constexpr auto not_osre = JmpImpl<JmpCondition::NOT_ORSE>();

			template <typename ToLabel>
			static constexpr auto to = Jmp<ToLabel,addr_value>();

			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return Base::encode(coder) | (uint16_t(condition_value) << Base::ARG1_SHIFT) | getJmpAddr<LabelName>(coder,addr_value);
			}

		};



		enum class WaitSource : uint16_t {
			GPIO 		= 0x00,
			PIN 		= 0x01,
			IRQ 		= 0x02
		};
		template <int polarity>
		struct Wait  {
			static_assert(polarity == 0 || polarity == 1, "Invalid polarity");

			// @todo relative syntax
			template <WaitSource source,uint16_t index>
			struct WaitImpl : InstructionEncoding<WaitImpl<source,index>> {
				static constexpr const Opcode opcode = Opcode::WAIT;
				using Base = InstructionEncoding<WaitImpl<source,index>>;
				static constexpr WaitSource source_value = source;
				static constexpr uint16_t polarity_value = polarity;
				static constexpr uint16_t index_value = index;
				explicit constexpr WaitImpl() {}
				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder) {
					return Base::encode(coder) |
						(polarity_value << 7) |
						(uint16_t(source_value) << Base::ARG1_SHIFT) |
						index_value;
				}
			};

			explicit constexpr Wait() {}
			template <uint16_t gpio_num>
			static constexpr auto gpio = WaitImpl<WaitSource::GPIO,gpio_num>();
			template <uint16_t pin_num>
			static constexpr auto pin = WaitImpl<WaitSource::PIN,pin_num>();
			template <uint16_t irq_num>
			static constexpr auto irq = WaitImpl<WaitSource::IRQ,irq_num>();
		};

		enum class InSource : uint16_t {
			PINS 		= 0x00,
			X 			= 0x01,
			Y 			= 0x02,
			NULL_T 		= 0x03,
			ISR 		= 0x06,
			OSR 		= 0x07,
		};
		struct In {
			// @todo
			template <InSource source,uint16_t bits>
			struct InImpl : InstructionEncoding<InImpl<source,bits>> {
				static_assert(bits >= 1 && bits <= 32,"Invalid bits count");
				static constexpr const Opcode opcode = Opcode::IN;
				using Base = InstructionEncoding<InImpl<source,bits>>;
				static constexpr InSource source_value = source;
				static constexpr uint16_t bits_value = bits == 32 ? 0 : bits;
				explicit constexpr InImpl() {}
				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder) {
					return Base::encode(coder) |
						(uint16_t(source_value) << Base::ARG1_SHIFT) |
						bits_value;
				}
			};
			explicit constexpr In() {}
			template <uint16_t bits>
			static constexpr auto pins = InImpl<InSource::PINS,bits>();
			template <uint16_t bits>
			static constexpr auto x = InImpl<InSource::X,bits>();
			template <uint16_t bits>
			static constexpr auto y = InImpl<InSource::Y,bits>();
			template <uint16_t bits>
			static constexpr auto null = InImpl<InSource::NULL_T,bits>();
			template <uint16_t bits>
			static constexpr auto isr = InImpl<InSource::ISR,bits>();
			template <uint16_t bits>
			static constexpr auto osr = InImpl<InSource::OSR,bits>();
		};

		enum class OutDestination : uint16_t {
			PINS 	= 0x00,
			X 		= 0x01,
			Y 		= 0x02,
			NULL_T 	= 0x03,
			PINDIRS = 0x04,
			PC 		= 0x05,
			ISR 	= 0x06,
			EXEC 	= 0x07,
		};

		struct Out {
			template <OutDestination destination,uint16_t bits>
			struct OutImpl : InstructionEncoding<OutImpl<destination,bits>> {
				using Base = InstructionEncoding<OutImpl<destination,bits>>;
				static_assert(bits >= 1 && bits <= 32,"Invalid bits count");
				static constexpr const Opcode opcode = Opcode::OUT;
				static constexpr OutDestination destination_value = destination;
				static constexpr uint16_t bits_value = bits == 32 ? 0 : bits;
				constexpr OutImpl() {}
				template <typename Encoder>
				static constexpr Instruction encode(const Encoder& coder) {
					return Base::encode(coder) |
						(uint16_t(destination_value) << Base::ARG1_SHIFT) |
						bits_value;
				}
			};

			template <uint16_t nbits>
			static constexpr auto pins = OutImpl<OutDestination::PINS,nbits>();
			template <uint16_t nbits>
			static constexpr auto x = OutImpl<OutDestination::X,nbits>();
			template <uint16_t nbits>
			static constexpr auto y = OutImpl<OutDestination::Y,nbits>();
			template <uint16_t nbits>
			static constexpr auto null = OutImpl<OutDestination::NULL_T,nbits>();
			template <uint16_t nbits>
			static constexpr auto pindirs = OutImpl<OutDestination::PINDIRS,nbits>();
			template <uint16_t nbits>
			static constexpr auto pc = OutImpl<OutDestination::PC,nbits>();
			template <uint16_t nbits>
			static constexpr auto isr = OutImpl<OutDestination::ISR,nbits>();
			template <uint16_t nbits>
			static constexpr auto exec = OutImpl<OutDestination::EXEC,nbits>();

		};


		template <bool IfFull,bool Block>
		struct PushImpl : InstructionEncoding<PushImpl<IfFull,Block>> {
			using Base = InstructionEncoding<PushImpl<IfFull,Block>>;
			static constexpr const Opcode opcode = Opcode::PUSH;
			static constexpr const bool iffull_value = IfFull;
			static constexpr const bool block_value = Block;
			constexpr PushImpl() {}
			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return Base::encode(coder) | (iffull_value ? (1<<6) : 0) | (block_value ? (1<<5) : 0);
			}
		};
		struct PushImplIF : PushImpl<true,true> {
			constexpr PushImplIF() {}

			static constexpr auto block = PushImpl<true,true>();
			static constexpr auto noblock = PushImpl<true,false>();
		};
		struct Push : PushImpl<false,true> {
			constexpr Push() {}

			static constexpr auto iffull = PushImplIF();
			static constexpr auto block = PushImpl<false,true>();
			static constexpr auto noblock = PushImpl<false,false>();
		};

		template <bool IfEmpty, bool Block>
		struct PullImpl : InstructionEncoding<PullImpl<IfEmpty,Block>> {
			static constexpr const Opcode opcode = Opcode::PULL;
			using Base = InstructionEncoding<PullImpl<IfEmpty,Block>>;
			static constexpr const bool ifempty_value = IfEmpty;
			static constexpr const bool block_value = Block;
			constexpr PullImpl()  {}
			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return Base::encode(coder) | (ifempty_value ? (1<<6) : 0) | (block_value ? (1<<5) : 0);
			}
		};
		struct PullImplIE : PullImpl<true,true> {
			constexpr PullImplIE() {}
			static constexpr auto block = PullImpl<true,true>();
			static constexpr auto noblock = PullImpl<true,false>();
		};
		struct Pull : PullImpl<false,true> {
			constexpr Pull() {}
			static constexpr auto ifempty = PullImplIE();
			static constexpr auto block = PullImpl<false,true>();
			static constexpr auto noblock = PullImpl<false,false>();
		};

		enum class MovDestination : uint16_t {
			PINS 	= 0x00,
			X 		= 0x01,
			Y 		= 0x02,
			EXEC 	= 0x04,
			PC 		= 0x05,
			ISR 	= 0x06,
			OSR 	= 0x07,
		};

		enum class MovSource : uint16_t {
			PINS 	= 0x00,
			X 		= 0x01,
			Y 		= 0x02,
			NULL_T 	= 0x03,
			STATUS 	= 0x05,
			ISR 	= 0x06,
			OSR 	= 0x07,
		};

		enum class MovOperation : uint16_t {
			NONE 	= 0x00,
			INVERT 	= 0x01,
			REVERSE = 0x02,
		};

		template <MovDestination destination,MovOperation operation,MovSource source>
		struct MovImpl : InstructionEncoding<MovImpl<destination,operation,source>> {
			using Base = InstructionEncoding<MovImpl<destination,operation,source>>;
			static constexpr const Opcode opcode = Opcode::MOV;
			static constexpr MovSource source_value = source;
			static constexpr MovDestination destination_value = destination;
			static constexpr MovOperation operation_value = operation;
			static constexpr size_t OP_SHIFT = 3;
			static constexpr size_t DESTINATION_SHIFT = 5;
			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return Base::encode(coder) | uint16_t(source_value) |
						(uint16_t(operation_value)<<OP_SHIFT) |
						(uint16_t(destination_value) << DESTINATION_SHIFT);
			}
		};
		template <MovDestination destination,MovOperation operation>
		struct MovDstOp {
			static  constexpr auto pins = MovImpl<destination,operation,MovSource::PINS>();
			static  constexpr auto x = MovImpl<destination,operation,MovSource::X>();
			static  constexpr auto y = MovImpl<destination,operation,MovSource::Y>();
			static  constexpr auto null = MovImpl<destination,operation,MovSource::NULL_T>();
			static  constexpr auto status = MovImpl<destination,operation,MovSource::STATUS>();
			static  constexpr auto isr = MovImpl<destination,operation,MovSource::ISR>();
			static  constexpr auto osr = MovImpl<destination,operation,MovSource::OSR>();
		};
		template <MovDestination destination>
		struct MovDst {
			static  constexpr auto pins = MovImpl<destination,MovOperation::NONE,MovSource::PINS>();
			static  constexpr auto x = MovImpl<destination,MovOperation::NONE,MovSource::X>();
			static  constexpr auto y = MovImpl<destination,MovOperation::NONE,MovSource::Y>();
			static  constexpr auto null = MovImpl<destination,MovOperation::NONE,MovSource::NULL_T>();
			static  constexpr auto status = MovImpl<destination,MovOperation::NONE,MovSource::STATUS>();
			static  constexpr auto isr = MovImpl<destination,MovOperation::NONE,MovSource::ISR>();
			static  constexpr auto osr = MovImpl<destination,MovOperation::NONE,MovSource::OSR>();

			static constexpr auto invert = MovDstOp<destination,MovOperation::INVERT>();
			static constexpr auto reverse = MovDstOp<destination,MovOperation::REVERSE>();
		};
		struct Mov {
			static constexpr auto pins = MovDst<MovDestination::PINS>();
			static constexpr auto x = MovDst<MovDestination::X>();
			static constexpr auto y = MovDst<MovDestination::Y>();
			static constexpr auto exec = MovDst<MovDestination::EXEC>();
			static constexpr auto pc = MovDst<MovDestination::PC>();
			static constexpr auto isr = MovDst<MovDestination::ISR>();
			static constexpr auto osr = MovDst<MovDestination::OSR>();
		};

		// @todo relative syntax
		template <bool clear,bool wait,uint16_t index>
		struct IrqImpl : InstructionEncoding<IrqImpl<clear,wait,index>> {
			static constexpr const Opcode opcode = Opcode::IRQ;
			using Base = InstructionEncoding<IrqImpl<clear,wait,index>>;
			static constexpr bool clear_value = clear;
			static constexpr bool wait_value = wait;
			static constexpr uint16_t index_value = index;
			explicit constexpr IrqImpl() {}
			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return Base::encode(coder) |
					(clear_value ? (1<<6) : 0) |
					(wait_value ? (1<<5) : 0) |
					index_value;
			}
		};

		struct Irq {
			constexpr Irq() {}
			template <uint16_t index>
			static constexpr auto set = IrqImpl<false,false,index>();
			template <uint16_t index>
			static constexpr auto nowait = IrqImpl<false,false,index>();
			template <uint16_t index>
			static constexpr auto wait = IrqImpl<false,true,index>();
			template <uint16_t index>
			static constexpr auto clear = IrqImpl<true,false,index>();
		};

		enum class SetDestination : uint16_t{
			PINS 	= 0x00,
			X 		= 0x01,
			Y 		= 0x02,
			PINDIRS = 0x04,
		};
		template <SetDestination destination,uint16_t value>
		struct SetImpl : InstructionEncoding<SetImpl<destination,value>> {
			static_assert(value <= 31,"invalid data value");
			static constexpr const Opcode opcode = Opcode::SET;
			static constexpr SetDestination destination_value = destination;
			static constexpr uint16_t data_value = value;
			static constexpr size_t DESTINATION_SHIFT = 5;
			constexpr SetImpl() {}
			template <typename Encoder>
			static constexpr Instruction encode(const Encoder& coder) {
				return InstructionEncoding<SetImpl<destination,value>>::encode(coder) | data_value |
					(uint16_t(destination_value) << DESTINATION_SHIFT);
			}
		};
		struct Set {
			constexpr Set()  {}
			template <size_t data>
			static constexpr auto pins = SetImpl<SetDestination::PINS,data>();
			template <size_t data>
			static constexpr auto x = SetImpl<SetDestination::X,data>();
			template <size_t data>
			static constexpr auto y = SetImpl<SetDestination::Y,data>();
			template <size_t data>
			static constexpr auto pindirs = SetImpl<SetDestination::PINDIRS,data>();
		};


		template <typename ...Labels>
		struct LabelsList;

		template <typename Settings,typename Labels,typename Instructions,typename WrapSettings>
		struct ProgramBuilder;

		template <typename Name,size_t Offset>
		struct Label {
			using name = Name;
			static constexpr size_t offset = Offset;
		};

		template <>
		struct LabelsList<> {
			template <typename Name,size_t offset>
			struct add_s {
				using result = LabelsList<Label<Name,offset> >;
			};
			template <typename Name,size_t offset>
			using add = typename add_s<Name,offset>::result;

			template <typename Name>
			static constexpr bool exist() {
				return false;
			}
		};

		template <typename FirstLabel,typename ...Labels>
		struct LabelsList<FirstLabel,Labels...> {
			template <typename Name>
			static constexpr size_t find(){
				if constexpr (std::is_same_v<Name,typename FirstLabel::name>) {
					return FirstLabel::offset;
				} else {
					static_assert(sizeof ...(Labels)!=0,"Not found label");
					return LabelsList<Labels...>::template find<Name>();
				}
			}
			template <typename Name>
			static constexpr bool exist() {
				if constexpr(std::is_same_v<Name,typename FirstLabel::name>) {
					return true;
				} else {
					return LabelsList<Labels...>::template exist<Name>();
				}
			}
			template <typename Name,size_t offset>
			struct add_s {
				static_assert(!exist<Name>(),"Already added label");
				using result = LabelsList<FirstLabel,Labels...,Label<Name,offset> >;
			};
			template <typename Name,size_t offset>
			using add = typename add_s<Name,offset>::result;
		};


		template <typename ...Instructions>
		struct InstructionsList;

		template <>
		struct InstructionsList<> {
			static constexpr size_t length = 0;
			explicit constexpr InstructionsList() {}
			template <typename Instruction>
			struct add_s {
				using result = InstructionsList<Instruction>;
			};
			template <typename Instruction>
			using add = typename add_s<Instruction>::result;
		};


		template <typename FirstInstruction,typename ...Instructions>
		struct InstructionsList<FirstInstruction,Instructions...> {
			static constexpr size_t length = 1 + InstructionsList<Instructions...>::length;
			using head = FirstInstruction ;
			using tail = InstructionsList<Instructions...>;

			template <typename Instruction>
			struct add_s {
				using result = InstructionsList<Instruction,FirstInstruction,Instructions...>;
			};
			template <typename Instruction>
			using add = typename add_s<Instruction>::result;

			template <size_t I,typename ProgramBuilder>
			static constexpr Instruction encode_i(const ProgramBuilder& builder)  {
				if constexpr (I == length-1) {
					return head::encode(builder);
				} else {
					return tail::template encode_i<I>(builder);
				}
			}

			template <typename ProgramBuilder,std::size_t... I>
			static constexpr std::array<Instruction,length> build_i(const ProgramBuilder& builder,std::index_sequence<I...>)  {
				return {{ encode_i<I>(builder)..., }};
			};
			template <typename ProgramBuilder>
			static constexpr std::array<Instruction,length> build(const ProgramBuilder& builder) {
				using indexes = std::make_index_sequence<length>;
				return build_i(builder,indexes{});
			};
		};


		template <size_t Length,typename Labels,typename ProgramSettings,uint16_t wrap_target_t,uint16_t wrap_t>
		struct ProgramCode  {
			using settings = ProgramSettings;
			static constexpr const size_t length = Length;
			static constexpr const uint16_t wrap_target = wrap_target_t;
			static constexpr const uint16_t wrap = wrap_t;
			explicit constexpr ProgramCode( std::array<Instruction,Length>&& code) : code(std::move(code)) {

			}
			const std::array<Instruction,Length> code;
			template <typename Label>
			static constexpr size_t getOffset() {
				return Labels::template find<Label>();
			}
		};


		template <uint32_t count_t = 0,bool sideset_is_opt_t = true, uint16_t delay_max_t = 31, uint16_t sideset_max_t = 0>
		struct ProgramSettings {
			static constexpr uint32_t count = count_t;
			static constexpr uint32_t sideset_count = sideset_is_opt_t ? (count_t+1) : count_t;
			static constexpr bool sideset_is_opt = sideset_is_opt_t;
			static constexpr uint16_t delay_max = delay_max_t;
			static constexpr uint16_t sideset_max = sideset_max_t;
			static constexpr bool sideset_pindirs = false;
		};
		template <size_t count>
		struct ProgramSettingsSideset {
			static_assert(count <= 5,"maximum number of side set bits is 5");
			using result = ProgramSettings<count,false,(1u << (5 - count)) - 1,(1u << count) - 1>;
		};
		template <size_t count>
		struct ProgramSettingsOptSideset {
			static_assert(count <= 4,"maximum number of side set bits is 4");
			using result = ProgramSettings<count,true,(1u << (5 - (count+1))) - 1,(1u << count) - 1>;
		};

		template <OptionalValue target = OptionalValue{},OptionalValue instr = OptionalValue{}>
		struct WrapSettings {
			static constexpr OptionalValue wrap_target = target;
			static constexpr OptionalValue wrap_instr = instr;
		};

		template <typename SettingsConfig, typename Labels,typename Instructions,typename WrapSettingsT>
		struct ProgramBuilder {
			using labels = Labels;
			using settings = SettingsConfig;
			using instructions = Instructions;
			using wrap_settings = WrapSettingsT;

			explicit constexpr ProgramBuilder() {}

			template <size_t count>
			static constexpr auto sideset = ProgramBuilder<typename ProgramSettingsSideset<count>::result,labels,instructions,wrap_settings>();

			template <size_t count>
			static constexpr auto sideset_opt = ProgramBuilder<typename ProgramSettingsOptSideset<count>::result,labels,instructions,wrap_settings>();

			template <typename SideDelayConfig>
			static constexpr uint16_t encodeSideDelay() {
				auto res = SideDelayConfig::delay_value;
				static_assert(SideDelayConfig::delay_value <= settings::delay_max,"delay too long");

				if constexpr (SideDelayConfig::side_value_present) {
					static_assert(SideDelayConfig::side_value<=settings::sideset_max, "side value overflow");
					res |= (SideDelayConfig::side_value << (5-settings::sideset_count));
					if constexpr (settings::sideset_is_opt) {
						res |= 1 << 4;
					}
				} else {
					static_assert(settings::sideset_is_opt, "sideset is not optional");
				}
				//static_assert(side < (1<<sideset_count),"side bits overflow");
				return res;
			}
			template <typename Label>
			static constexpr size_t getOffset() {
				return Labels::template find<Label>();
			}

			template <typename dummy=void>
			static constexpr auto wrap = ProgramBuilder<settings,labels,instructions,WrapSettings<wrap_settings::wrap_target,{ instructions::length-1, true }>>();

			template <typename dummy=void>
			static constexpr auto wrapTarget = ProgramBuilder<settings,labels,instructions,WrapSettings<{ instructions::length, true },wrap_settings::wrap_instr>>();

			template <typename LabelName>
			static constexpr auto label = ProgramBuilder<settings,typename Labels::template add<LabelName,instructions::length>,instructions,wrap_settings>();

			template <typename Instr>
			static  constexpr auto instr( const Instr& )  {
				using AddedInstructions = typename instructions::add<Instr>;
				return ProgramBuilder<settings,labels,AddedInstructions,wrap_settings>();
			}
			static constexpr auto end() {
				ProgramBuilder pb;
				return ProgramCode<instructions::length,Labels,settings,
					wrap_settings::wrap_target.is_set?wrap_settings::wrap_target.value:0,
					wrap_settings::wrap_instr.is_set?wrap_settings::wrap_instr.value:(Instructions::length-1)
					>(instructions::build(pb));
			}
		};


		using EmptyProgram = ProgramBuilder<ProgramSettings<>,LabelsList<>,InstructionsList<>,WrapSettings<>>;
		static inline constexpr EmptyProgram  ProgramBegin() {
			return EmptyProgram();
		};
	}

	// public
	static constexpr auto Jmp = implementation::Jmp();
	template <int polarity>
	static constexpr auto Wait = implementation::Wait<polarity>();
	static constexpr auto In = implementation::In();
	static constexpr auto Out = implementation::Out();
	static constexpr auto Push = implementation::Push();
	static constexpr auto Pull = implementation::Pull();
	static constexpr auto Mov = implementation::Mov();
	static constexpr auto Irq = implementation::Irq();
	static constexpr auto Set = implementation::Set();
	static constexpr auto Nop = implementation::Mov().y.y;

	

}
