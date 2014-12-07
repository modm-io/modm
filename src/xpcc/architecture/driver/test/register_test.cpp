// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "register_test.hpp"
#include "register_definitions.hpp"

constexpr Control3::Bit Control3::IHL;
constexpr Control3::Bit Control3::PP_OD;

//constexpr Control3::Group::Config Control3::InterruptSource::GND;
//constexpr Control3::Group::Config Control3::InterruptSource::Click;
//constexpr Control3::Group::Config Control3::InterruptSource::DataReady;

//constexpr Control3::Group::Config Control3::InterruptSource::Mask;
constexpr Control3::Group::Config Control3::Source2::Mask;
constexpr Control3::Group::Config Control3::Source2::Click;
constexpr Control3::Group::Config Control3::Source1::Click;

void
RegisterTest::testExclusiveness()
{
	TEST_ASSERT_EQUALS(Control3::IHL.value, 0x80);

	Test valueDirect = Test::A | Test::B;
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(valueDirect), 0x07);
	Test valueA = Test::A;
	Test valueB = Test::B;
	Test valueIndirect = valueA | valueB | Test::A;
	TEST_ASSERT_EQUALS(static_cast<uint8_t>(valueIndirect), 0x07);

	STest vD = STest::A | STest::B;
	vD = vD | STest::A;

//	Control3 control3 = Control3::IHL;
//	ControlRegister controlRegister = Control3::IHL;
//
//	TEST_ASSERT_EQUALS(control3.value, 0x80);
//	TEST_ASSERT_EQUALS(controlRegister.value, 0x80);
//
//	control3 = Control3::Source2::Click;
//
//	TEST_ASSERT_EQUALS(control3.value, 0b111000);
//	TEST_ASSERT_EQUALS(Control3::Source2::Mask.value, 0b111000);
//
//	control3 = Control3::Source1::Click;
//
//	TEST_ASSERT_EQUALS(control3.value, 0b111);

////	Control1::Source2 source2 = Control3::Source2::GND;
////	Control3 control3 = source2 | Control3::Source2::GND;
////	control3 ^= ~Control3::Source2::GND;
////	Control1 control1 = Control1::DR | Control1::PD | Control1::FS;
////
////	Control1 control11 = control1 | Control1::STP;
//////	control11 |= Control1::STP;
////
////	ControlRegister controlRegister1 = control1;
////	ControlRegister controlRegister3 = Control3::IHL | Control3::Source2::GND;
////
////	uint8_t raw = controlRegister1.value;
////	raw |= Control1::PD.value;
////	uint8_t bitValue = Control1::PD.value;
////
////	bool isZero = control1 & Control1::PD;
////	bool isSame = control3 == Control3::Source2::GND;
}

