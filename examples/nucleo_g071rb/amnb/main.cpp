/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/communication/amnb.hpp>
#include <modm/processing.hpp>

using namespace Board;
using namespace std::chrono_literals;
using namespace modm::amnb;
// ----------------------------------------------------------------------------

Listener listeners[] =
{
	{1, [](uint8_t sender, const uint32_t& data)
		{
			MODM_LOG_INFO << "Node2 and Node3 received Broadcast 1 from '" << sender;
			MODM_LOG_INFO << "': " << data << modm::endl;
		}
	},
	{2, [](uint8_t sender)
		{
			MODM_LOG_INFO << "Node2 and Node3 received Broadcast 2 from '" << sender << "'" << modm::endl;
		}
	},
};
Action actions[] =
{
	{1, []() -> Response
		{
			static uint8_t counter{0};
			MODM_LOG_INFO << "Node1 and Node3 received Action 1" << modm::endl;
			return counter++;
		}
	},
	{2, [](const uint32_t& data) -> Response
		{
			static uint8_t counter{0};
			MODM_LOG_INFO << "Node1 and Node3 received Action 2 with argument: " << data << modm::endl;
			return ErrorResponse(counter++);
		}
	},
};

// Two nodes on the same device on different UARTs of course!
DeviceWrapper<Usart1> device1;
DeviceWrapper<Usart3> device2;
DeviceWrapper<Usart4> device3;
Node node1(device1, 1, actions);
Node node2(device2, 2, listeners);
Node node3(device3, 3, actions, listeners);

// You need to connect D1 with D15 and with A0
using PinNode1 = GpioC4; // D1
using PinNode2 = GpioB8; // D15
using PinNode3 = GpioA0; // A0

class Thread : public modm::pt::Protothread
{
	modm::ShortPeriodicTimer tmr{1s};
	uint32_t counter{0};
	Result<uint8_t> res1;
	Result<uint8_t, uint8_t> res2;

public:
	bool inline
	update()
	{
		PT_BEGIN();

		while(true)
		{
			PT_WAIT_UNTIL(tmr.execute());

			node1.broadcast(1, counter++);
			node3.broadcast(2);

			res1 = PT_CALL(node2.request<uint8_t>(1, 1));
			MODM_LOG_INFO << "Node1 responded with: " << res1.error();
			if (res1) { MODM_LOG_INFO << " " << *res1 << modm::endl; }

			res2 = PT_CALL(node1.request<uint8_t, uint8_t>(3, 2, counter));
			MODM_LOG_INFO << "Node3 responded with: " << res2.error();
			if (res2.hasUserError()) {
				MODM_LOG_INFO << " " << *res2.userError() << modm::endl;
			}
		}

		PT_END();
	}
}
thread;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput();

	Usart1::connect<PinNode1::Tx>();
	Usart1::initialize<SystemClock, 115200>(Usart1::Parity::Even, Usart1::WordLength::Bit9);
	// Use Single-Wire Half-Duplex Mode
	PinNode1::configure(Gpio::OutputType::OpenDrain);
	PinNode1::configure(Gpio::InputType::PullUp);
	USART1->CR1 &= ~USART_CR1_UE;
	USART1->CR3 = USART_CR3_HDSEL;
	USART1->CR1 |= USART_CR1_UE;

	Usart3::connect<PinNode2::Tx>();
	Usart3::initialize<SystemClock, 115200>(Usart1::Parity::Even, Usart1::WordLength::Bit9);
	// Use Single-Wire Half-Duplex Mode
	PinNode2::configure(Gpio::OutputType::OpenDrain);
	PinNode2::configure(Gpio::InputType::PullUp);
	USART3->CR1 &= ~USART_CR1_UE;
	USART3->CR3 = USART_CR3_HDSEL;
	USART3->CR1 |= USART_CR1_UE;

	Usart4::connect<PinNode3::Tx>();
	Usart4::initialize<SystemClock, 115200>(Usart1::Parity::Even, Usart1::WordLength::Bit9);
	// Use Single-Wire Half-Duplex Mode
	PinNode3::configure(Gpio::OutputType::OpenDrain);
	PinNode3::configure(Gpio::InputType::PullUp);
	USART4->CR1 &= ~USART_CR1_UE;
	USART4->CR3 = USART_CR3_HDSEL;
	USART4->CR1 |= USART_CR1_UE;

	while (true)
	{
		node1.update();
		node2.update();
		node3.update();
		thread.update();
	}

	return 0;
}
