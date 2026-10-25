/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/fiber.hpp>
#include <modm/platform/gpio/base.hpp>

/**
 * Bit-banged I2C slave that polls its pins in a fiber and injects faults.
 *
 * The slave holds SCL low after every falling edge until it handled the bit,
 * so it keeps up with any master that supports clock stretching. It yields only
 * when it waited for a long time, which happens while it holds SCL low itself,
 * while the bus is idle or while the bus is stuck.
 *
 * The memory has 256 registers with the value of their address, except the ID
 * 0xC35A at register 0. The first byte of a write sets the register pointer,
 * reads auto-increment it. Further written bytes are ignored, so that a missed
 * repeated start cannot overwrite the ID. While `inject` is true, every transaction gets one
 * fault from a deterministic random sequence.
 */
template< class Scl, class Sda >
class I2cSlave
{
public:
	enum class
	Fault : uint8_t
	{
		None,
		NackAddress,    ///< does not acknowledge its address
		NackData,       ///< does not acknowledge the first data byte of a write
		Stretch,        ///< holds SCL low for 1-5 ms after the address
		StretchLong,    ///< holds SCL low for 60-80 ms, longer than the timeout
		HoldSda,        ///< ignores the NACK of a read and sends 1-2 more zero bytes
		Count
	};

	struct Counters
	{
		uint32_t starts;
		uint32_t addressed;
		uint32_t reads;
		uint32_t writes;
		uint32_t faults[uint8_t(Fault::Count)];
	};
	static inline volatile Counters counters{};
	static inline uint8_t address{0x3C};
	static inline uint8_t memory[256];
	static inline volatile bool inject{false};

	static void
	initialize()
	{
		for (size_t ii = 0; ii < sizeof(memory); ii++) memory[ii] = ii;
		memory[0] = 0xC3;
		memory[1] = 0x5A;
		using namespace modm::platform;
		Scl::setOutput(Gpio::OutputType::OpenDrain);
		Sda::setOutput(Gpio::OutputType::OpenDrain);
		Scl::configure(Gpio::InputType::PullUp);
		Sda::configure(Gpio::InputType::PullUp);
		Scl::set();
		Sda::set();
	}

	[[noreturn]] static void
	run()
	{
		while (true)
		{
			waitStart();
			// returns true on a repeated start
			while (transaction()) ;
		}
	}

private:
	enum class Event : uint8_t { Bit, Start, Stop };

	static inline uint32_t rng{0x9E3779B9};
	static inline uint8_t pointer{0};

	static uint32_t
	random()
	{ rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

	// Normal bus phases are shorter than 1000 polls, only longer waits yield
	static void
	spin(uint16_t &polls)
	{
		if (++polls > 1000) { polls = 0; modm::this_fiber::yield(); }
	}

	static void
	hold()
	{
		uint16_t polls{};
		while (Scl::read()) spin(polls);
		Scl::reset();
	}

	static void
	clock()
	{
		Scl::set();
		uint16_t polls{};
		while (not Scl::read()) spin(polls);
		hold();
	}

	// SDA falls while SCL is high, also after a STOP or a foreign transaction.
	// The master may run in the same CPU and start the transaction before this
	// fiber runs again, so any change of an idle bus is a start condition: SCL
	// is then held in its first low phase before the first bit.
	static void
	waitStart()
	{
		bool idle{false};
		uint16_t polls{};
		while (true)
		{
			const bool scl = Scl::read();
			const bool sda = Sda::read();
			if (scl and sda) idle = true;
			else if (idle) break;
			spin(polls);
		}
		counters.starts++;
		hold();
	}

	static Event
	readBit(bool &bit)
	{
		Scl::set();
		uint16_t polls{};
		while (not Scl::read()) spin(polls);
		polls = 0;
		bit = Sda::read();
		while (Scl::read())
		{
			// SCL may have fallen before SDA changed
			if (Sda::read() != bit and Scl::read())
			{
				if (not bit) return Event::Stop;
				counters.starts++;
				hold();
				return Event::Start;
			}
			spin(polls);
		}
		Scl::reset();
		return Event::Bit;
	}

	static Event
	readByte(uint8_t &byte)
	{
		Sda::set();
		for (uint8_t ii = 0; ii < 8; ii++)
		{
			bool bit;
			if (const Event event = readBit(bit); event != Event::Bit) return event;
			byte = (byte << 1) | bit;
		}
		return Event::Bit;
	}

	static void
	writeBit(bool bit)
	{
		if (bit) Sda::set(); else Sda::reset();
		clock();
	}

	static void
	ack()
	{
		Sda::reset();
		clock();
		// release SDA right after the ACK like a real device
		Sda::set();
		// The interrupt of a master in the same CPU sets up a repeated start
		// after the ACK, which this fiber would miss, so stretch it out
		modm::delay_us(100);
	}

	static Fault
	pick()
	{
		if (not inject) return Fault::None;
		const uint32_t r = random() % 100;
		if (r < 70) return Fault::None;
		if (r < 76) return Fault::NackAddress;
		if (r < 82) return Fault::NackData;
		if (r < 88) return Fault::Stretch;
		if (r < 92) return Fault::StretchLong;
		return Fault::HoldSda;
	}

	static bool
	transaction()
	{
		const Fault fault = pick();
		uint8_t byte{};
		if (const Event event = readByte(byte); event != Event::Bit) return event == Event::Start;
		if ((byte >> 1) != address) { Scl::set(); return false; }
		if (fault == Fault::NackAddress)
		{
			counters.faults[uint8_t(fault)]++;
			Scl::set();
			return false;
		}
		counters.addressed++;
		if (fault == Fault::Stretch or fault == Fault::StretchLong)
		{
			counters.faults[uint8_t(fault)]++;
			const uint32_t ms = (fault == Fault::Stretch) ? 1 + random() % 5 : 60 + random() % 20;
			modm::this_fiber::sleep_for(std::chrono::milliseconds(ms));
		}
		ack();
		return (byte & 1) ? read(fault) : write(fault);
	}

	static bool
	read(Fault fault)
	{
		counters.reads++;
		uint8_t extra = (fault == Fault::HoldSda) ? 1 + random() % 2 : 0;
		if (extra) counters.faults[uint8_t(fault)]++;
		bool zeros{false};
		while (true)
		{
			const uint8_t data = zeros ? 0 : memory[pointer++];
			for (int8_t ii = 7; ii >= 0; ii--) writeBit((data >> ii) & 1);
			Sda::set();
			bool nack;
			if (const Event event = readBit(nack); event != Event::Bit) return event == Event::Start;
			if (nack)
			{
				if (not extra) { Scl::set(); return false; }
				extra--;
				zeros = true;
			}
		}
	}

	static bool
	write(Fault fault)
	{
		counters.writes++;
		bool first{true};
		while (true)
		{
			uint8_t byte{};
			if (const Event event = readByte(byte); event != Event::Bit) return event == Event::Start;
			if (fault == Fault::NackData)
			{
				counters.faults[uint8_t(fault)]++;
				Scl::set();
				return false;
			}
			if (first) pointer = byte;
			first = false;
			ack();
		}
	}
};
