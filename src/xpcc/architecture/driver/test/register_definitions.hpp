// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UNITTEST_REGISTER_DEFINITIONS_HPP
#define XPCC_UNITTEST_REGISTER_DEFINITIONS_HPP

#include <xpcc/architecture/driver/register.hpp>

typedef xpcc::Register8 Register8;
REGISTER_GROUP(ControlRegister, Register8);

/*
struct
Control3 : public ControlRegister
{
	using ControlRegister::ControlRegister;

	// operators
	friend constexpr bool
	operator==(const Control3& lhs, const Control3& rhs)
	{ return (lhs.value == rhs.value); }
	friend constexpr bool
	operator!=(const Control3& lhs, const Control3& rhs)
	{ return !(lhs == rhs); }

	template<typename O> bool
	operator==(const O&) const = delete;

	friend constexpr Control3 operator | (Control3 const &lhs, Control3 const &rhs)
	{ return Control3(Control3::Bit(lhs.value | rhs.value)); }
	friend constexpr Control3 operator & (Control3 const &lhs, Control3 const &rhs)
	{ return Control3(Control3::Bit(lhs.value & rhs.value)); }
	friend constexpr Control3 operator ^ (Control3 const &lhs, Control3 const &rhs)
	{ return Control3(Control3::Bit(lhs.value ^ rhs.value)); }

	inline Control3& operator |= (Control3 const &rhs)
	{ *this = *this | rhs; return *this; }
	inline Control3& operator &= (Control3 const &rhs)
	{ *this = *this & rhs; return *this; }
	inline Control3& operator ^= (Control3 const &rhs)
	{ *this = *this ^ rhs; return *this; }


	struct Bit : public xpcc::RegisterBit<Type>
	{
		using RegisterBit::RegisterBit;

		operator ControlRegister() const
		{ return ControlRegister(Control3(Bit(value))); }
	};
	constexpr Control3(const Bit &bit)
	:	ControlRegister(bit.value) {}


	struct Group : public xpcc::RegisterBitGroup<Type>
	{
		using RegisterBitGroup::RegisterBitGroup;

		operator ControlRegister() const
		{ return ControlRegister(Control3(Group(value))); }

		struct Config : public xpcc::RegisterBitGroupConfig<Type>
		{
			using RegisterBitGroupConfig::RegisterBitGroupConfig;

			operator ControlRegister() const
			{ return ControlRegister(Control3(Group(value))); }
		};

		friend constexpr Group operator | (Config const &lhs, Config const &rhs)
		{ return Group(lhs.value | rhs.value); }
		friend constexpr Group operator & (Config const &lhs, Config const &rhs)
		{ return Group(lhs.value & rhs.value); }
		friend constexpr Group operator ^ (Config const &lhs, Config const &rhs)
		{ return Group(lhs.value ^ rhs.value); }
		friend constexpr Group operator ~(Config const &rhs)
		{ return Group(~rhs.value); }

		constexpr Group(const Config &config)
		:	Group(config.value) {}
	};
	constexpr Control3(const Group &group)
	:	ControlRegister(group.value) {}
	constexpr Control3(const Group::Config &config)
	:	ControlRegister(config.value) {}

	friend constexpr bool
	operator==(const Control3& lhs, const Bit& rhs)
	{ return (lhs.value == rhs.value); }
	friend constexpr bool
	operator==(const Control3& lhs, const Group& rhs)
	{ return (lhs.value == rhs.value); }
	friend constexpr bool
	operator==(const Control3& lhs, const Group::Config& rhs)
	{ return (lhs.value == rhs.value); }


	static constexpr Bit IHL    = xpcc::Bit7;
	static constexpr Bit PP_OD  = xpcc::Bit6;
	static constexpr Bit I2CFG2 = xpcc::Bit5;
	static constexpr Bit I2CFG1 = xpcc::Bit4;
	static constexpr Bit I2CFG0 = xpcc::Bit3;
	static constexpr Bit I1CFG2 = xpcc::Bit2;
	static constexpr Bit I1CFG1 = xpcc::Bit1;
	static constexpr Bit I1CFG0 = xpcc::Bit0;

	struct Interrupt : public Group
	{
		using Group::Group;

		static constexpr Config Mask = 0b111;

		static constexpr Config GND = 0;
		static constexpr Config FF_WU_1 = 0x01;
		static constexpr Config FF_WU_2 = 0x02;
		static constexpr Config FF_WU_1_OR_2 = 0x03;
		static constexpr Config DataReady = 0x04;
		static constexpr Config Click = 0x07;
	};

	struct Source2 : public Group
	{
		using Group::Group;

		static constexpr Type BitPosition = 3;
		static constexpr Config Mask = 0b111 << BitPosition;

		static constexpr Config GND = 0 << BitPosition;
		static constexpr Config FF_WU_1 = 0x01 << BitPosition;
		static constexpr Config FF_WU_2 = 0x02 << BitPosition;
		static constexpr Config FF_WU_1_OR_2 = 0x03 << BitPosition;
		static constexpr Config DataReady = 0x04 << BitPosition;
		static constexpr Config Click = 0x07 << BitPosition;
	};

	struct Source1 : public Group
	{
		using Group::Group;

		static constexpr Type BitPosition = 0;
		static constexpr Config Mask = 0b111 << BitPosition;

		static constexpr Config GND = 0 << BitPosition;
		static constexpr Config FF_WU_1 = 0x01 << BitPosition;
		static constexpr Config FF_WU_2 = 0x02 << BitPosition;
		static constexpr Config FF_WU_1_OR_2 = 0x03 << BitPosition;
		static constexpr Config DataReady = 0x04 << BitPosition;
		static constexpr Config Click = 0x07 << BitPosition;
	};
};

/*/
REGISTER(Control3, ControlRegister,

	REGISTER_BIT( IHL    = xpcc::Bit7 ); ///< hello
	REGISTER_BIT( PP_OD  = xpcc::Bit6 );
	REGISTER_BIT( I2CFG2 = xpcc::Bit5 );
	REGISTER_BIT( I2CFG1 = xpcc::Bit4 );
	REGISTER_BIT( I2CFG0 = xpcc::Bit3 );
	REGISTER_BIT( I1CFG2 = xpcc::Bit2 );
	REGISTER_BIT( I1CFG1 = xpcc::Bit1 );
	REGISTER_BIT( I1CFG0 = xpcc::Bit0 );

	REGISTER_CONFIG_GROUP( Interrupt, 3,

		REGISTER_CONFIG( GND          = 0x00 );
		REGISTER_CONFIG( FF_WU_1      = 0x01 );
		REGISTER_CONFIG( FF_WU_2      = 0x02 );
		REGISTER_CONFIG( FF_WU_1_OR_2 = 0x03 );
		REGISTER_CONFIG( DataReady    = 0x04 );
		REGISTER_CONFIG( Click        = 0x07 );,

			REGISTER_CONFIG_INSTANCE(Source2, 3),
			REGISTER_CONFIG_INSTANCE(Source1, 0)
	);
);

//*/

REGISTER
(Control1, ControlRegister,
	REGISTER_BIT( DR  = xpcc::Bit7 );
	REGISTER_BIT( PD  = xpcc::Bit6 );
	REGISTER_BIT( FS  = xpcc::Bit5 );
	REGISTER_BIT( STP = xpcc::Bit4 );
	REGISTER_BIT( STM = xpcc::Bit3 );
	REGISTER_BIT( Zen = xpcc::Bit2 );
	REGISTER_BIT( Yen = xpcc::Bit1 );
	REGISTER_BIT( Xen = xpcc::Bit0 );
);

#endif // XPCC_UNITTEST_REGISTER_DEFINITIONS_HPP
