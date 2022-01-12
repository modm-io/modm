/*
 * Copyright (c) 2019, Niklas Hauser
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/debug/logger.hpp>
#include <string>

using namespace Board;
using namespace std::string_literals;

using PushButton = GpioInverted<GpioInputA1>;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

MODM_ISR(EXTI0)
{ MODM_LOG_DEBUG << "EXTI0 called!" << modm::endl; }
MODM_ISR(EXTI1)
{ MODM_LOG_DEBUG << "EXTI1 called!" << modm::endl; }
MODM_ISR(EXTI2)
{ MODM_LOG_DEBUG << "EXTI2 called!" << modm::endl; }
MODM_ISR(EXTI3)
{ MODM_LOG_DEBUG << "EXTI3 called!" << modm::endl; }

// But we forgot about EXTI4
// MODM_ISR(EXTI4)
// { MODM_LOG_DEBUG << "EXTI4 called!" << modm::endl; }

[[maybe_unused]]
static modm::Abandonment
core_assertion_handler(const modm::AssertionInfo &info)
{
	if (info.name == "nvic.undef"s) {
		MODM_LOG_ERROR.printf("Ignoring undefined IRQ handler %d!\n", int8_t(info.context));
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
// Comment this line to abandon execution
MODM_ASSERTION_HANDLER(core_assertion_handler);

int main()
{
	Board::initialize();
	PushButton::setInput(Gpio::InputType::PullUp);

	// initialize Uart2 for MODM_LOG_*
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	// Enable the Interrupt handlers
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	// Give them the highest priority
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_SetPriority(EXTI1_IRQn, 0);
	NVIC_SetPriority(EXTI2_IRQn, 0);
	NVIC_SetPriority(EXTI3_IRQn, 0);
	NVIC_SetPriority(EXTI4_IRQn, 0);

	MODM_LOG_INFO << "Push the Button to trigger EXTI interrupts!" << modm::endl;
	int ii{0};

	while (true)
	{
		if(PushButton::read())
		{
			NVIC_SetPendingIRQ(IRQn_Type(int(EXTI0_IRQn) + ii));
			ii = (ii + 1) % 5;
			// wait for user reaction
			modm::delay(500ms);
		}
	}

	return 0;
}
