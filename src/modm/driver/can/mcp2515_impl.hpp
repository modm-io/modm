/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2015, 2017-2018, 2025, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP2515_HPP
	#error	"Don't include this file directly, use 'mcp2515.hpp' instead!"
#endif
#include <cstring>

// ----------------------------------------------------------------------------

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
bool
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::initializeWithPrescaler(
	uint8_t prescaler /* 2 .. 128 */,
	uint8_t sjw       /* in 1TQ .. 3TQ */,
	uint8_t prop      /* in 1TQ .. 8TQ */,
	uint8_t ps1       /* in 1TQ .. 8TQ */,
	uint8_t ps2       /* in 2TQ .. 8TQ */)
{
	using namespace mcp2515;

	// software reset for the mcp2515, after this the chip is back in the
	// configuration mode
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();
	SpiMaster::transfer(RESET);
	if (this->releaseMaster()) Cs::set();

	// wait a bit to give the MCP2515 some time to restart
	modm::this_fiber::sleep_for(30ms);

	// Build CNF1 .. 3 from parameters
	// Configuration is stored at increasing addresses in MCP2515,
	// so prepare CNF3, CNF2 and CNF1 in that order.
	uint8_t buffer[6]{WRITE, CNF3};
	buffer[2] = (ps2 - 1); // CNF3
	buffer[3] = 0x80 | ((ps1 - 1) << 3) | (prop - 1); // CNF2
	buffer[4] = ((sjw - 1) << 6) | ((prescaler / 2 - 1) & 0x3f); // CNF1
	buffer[5] = RX1IE | RX0IE;

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();
	SpiMaster::transfer(buffer, nullptr, sizeof(buffer));
	if (this->releaseMaster()) Cs::set();

	// set TXnRTS pins as inwrites
	writeRegister(TXRTSCTRL, 0);

	// disable RXnBF pins (high impedance state)
	writeRegister(BFPCTRL, 0);

	// check if we could read back some of the values
	return readRegister(CNF2) == buffer[3];
}

// ----------------------------------------------------------------------------
template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
void
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::setFilter(const uint8_t filter[32])
{
	using namespace mcp2515;

	// change to configuration mode
	bitModify(CANCTRL, 0xe0, REQOP2);

	modm::this_fiber::poll([&]
	{
		modm::this_fiber::sleep_for(1ms);
		return (readRegister(CANSTAT) & 0xe0) == REQOP2;
	});
	writeRegister(RXB0CTRL, BUKT);
	writeRegister(RXB1CTRL, 0);

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	uint8_t buffer[2+12]{WRITE};
	for (uint8_t i = 0; i < 0x30; i += 0x10)
	{
		const auto length{size_t((i == 0x20) ? 8 : 12)};
		buffer[1] = i;
		std::memcpy(&buffer[2], filter, length);
		filter += length;

		Cs::reset();
		SpiMaster::transfer(buffer, nullptr, length);
		Cs::set();

		modm::this_fiber::sleep_for(1us);
	}
	this->releaseMaster();
	bitModify(CANCTRL, 0xe0, 0);

}

// ----------------------------------------------------------------------------
template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
void
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::setMode(Can::Mode mode)
{
	using namespace mcp2515;

	uint8_t reg{};
	if (mode == Can::Mode::ListenOnly)
		reg = REQOP1 | REQOP0;
	else if (mode == Can::Mode::LoopBack)
		reg = REQOP1;

	// set the new mode
	bitModify(CANCTRL, REQOP2 | REQOP1 | REQOP0, reg);

	modm::this_fiber::poll([&]
	{
		modm::this_fiber::sleep_for(1ms);
		// wait for the new mode to become active
		return (readRegister(CANSTAT) &	(OPMOD2 | OPMOD1 | OPMOD0)) == reg;
	});
}

// ----------------------------------------------------------------------------
template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
bool
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::readMessage(can::Message& message)
{
	using namespace mcp2515;

	// read status flag of the device
	const uint8_t status = readStatus(RX_STATUS);
	uint8_t address{};

	if (status & FLAG_RXB0_FULL)
		address = READ_RX;			// message in buffer 0
	else if (status & FLAG_RXB1_FULL)
		address = READ_RX | 0x04;	// message in buffer 1 (RXB1SIDH)
	else
		// Error: no message available
		return false;

	uint8_t buffer[14]{address};

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();
	SpiMaster::transfer(buffer, buffer, sizeof(buffer));
	if (this->releaseMaster()) Cs::set();

	message.flags.rtr = status & FLAG_RTR;
	message.flags.extended = buffer[2] & MCP2515_IDE;
	if (message.flags.extended)
	{
		message.identifier =
			(uint32_t(buffer[1]) << 21) |
			(uint32_t(buffer[2] & 0x1C) << 13) | (uint32_t(buffer[2] & 0x03) << 16) |
			(buffer[3] << 8) |
			buffer[4];
	}
	else
	{
		message.identifier = (buffer[1] << 11) | (buffer[2] >> 5);
	}

	message.length = buffer[5] & 0x0F;
	std::memcpy(message.data, buffer + 6, message.length);

	// RX0IF or RX1IF respectivly were already cleared automatically by rising CS.
	// See section 12.4 in datasheet.
	return true;
}

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
void
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::update()
{
	// check if the device has received a message(pin = LOW)
	// if yes: read it and put it into the rxQueue
	if(can::Message message; not Int::read() and readMessage(message))
		rxQueue.push(message);

	/// check if device accepts messages and start emptying the transmit queue if not empty
	if (txQueue.isNotEmpty() and writeMessage(txQueue.get()))
		txQueue.pop();
}

// ----------------------------------------------------------------------------
template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
bool
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::writeMessage(const can::Message& message)
{
	using namespace mcp2515;
	uint8_t address{};

	const uint8_t status = readStatus(READ_STATUS);
	constexpr uint8_t mask{TXB2CNTRL_TXREQ | TXB1CNTRL_TXREQ | TXB0CNTRL_TXREQ};
	// send if ready, else return that nothing was sent
	if ((status & mask) != mask)
	{
		if ((status & TXB0CNTRL_TXREQ) == 0)
			address = 0x00;  // TXB0SIDH
		else if ((status & TXB1CNTRL_TXREQ) == 0)
			address = 0x02;  // TXB1SIDH
		else if ((status & TXB2CNTRL_TXREQ) == 0)
			address = 0x04;  // TXB2SIDH
		else
			// all buffer are in use => could not send the message
			return 0;

		// prepare write buffer
		uint8_t buffer[14];
		buffer[0] = WRITE_TX | address;

		if (message.flags.extended)
		{
			const uint8_t id[]{MCP2515_FILTER_EXTENDED(message.identifier)};
			std::memcpy(buffer + 1, id, sizeof(id));
		}
		else
		{
			const uint8_t id[]{MCP2515_FILTER(message.identifier)};
			std::memcpy(buffer + 1, id, sizeof(id));
		}

		if (message.flags.rtr)
		{
			buffer[5] = MCP2515_RTR | message.length;
		}
		else
		{
			buffer[5] = message.length;
			std::memcpy(buffer + 6, message.data, message.length);
		}

		modm::this_fiber::poll([&]{ return this->acquireMaster(); });
		Cs::reset();
		SpiMaster::transfer(buffer, nullptr, 6 + message.length);
		Cs::set();

		modm::this_fiber::sleep_for(1us);

		// send message via RTS command
		Cs::reset();
		// 0 2 4 => 1 2 4
		SpiMaster::transfer(RTS | (address ? address : 1));
		if (this->releaseMaster()) Cs::set();
	}

	return true;
}

// ----------------------------------------------------------------------------

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
void
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::writeRegister(uint8_t address, uint8_t data)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	uint8_t buffer[]{WRITE, address, data};
	SpiMaster::transfer(buffer, nullptr, sizeof(buffer));

	if (this->releaseMaster()) Cs::set();
}

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
uint8_t
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::readRegister(uint8_t address)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	uint8_t buffer[]{READ, address, 0xff};
	SpiMaster::transfer(buffer, buffer, sizeof(buffer));

	if (this->releaseMaster()) Cs::set();
	return buffer[2];
}

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
void
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::bitModify(uint8_t address, uint8_t mask, uint8_t data)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	uint8_t buffer[]{BIT_MODIFY, address, mask, data};
	SpiMaster::transfer(buffer, nullptr, sizeof(buffer));

	if (this->releaseMaster()) Cs::set();
}

template <class SpiMaster, class Cs, class Int, size_t TxBufferSize, size_t RxBufferSize>
uint8_t
modm::Mcp2515<SpiMaster, Cs, Int, TxBufferSize, RxBufferSize>::readStatus(uint8_t type)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	uint8_t buffer[]{type, 0xff};
	SpiMaster::transfer(buffer, buffer, sizeof(buffer));

	if (this->releaseMaster()) Cs::set();
	return buffer[1];
}
