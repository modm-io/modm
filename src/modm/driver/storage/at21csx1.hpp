// coding: utf-8
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

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/math/utils/crc.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

/// @ingroup modm_driver_at21csx1
struct at21csx1
{
	/// The 4-bit opcode replacing the I2C device type identifier in the
	/// device address byte. Only Eeprom and SecurityRegister have an API on
	/// this driver, the remaining opcodes document the rest of the command
	/// set, some of which is irreversible once issued.
	enum class
	Opcode : uint8_t
	{
		Eeprom = 0xA, ///< Read/write the main memory array
		SecurityRegister = 0xB, ///< Read/write the security register
		LockSecurityRegister = 0x2, ///< One-time, irreversible
		RomZoneRegister = 0x7,
		FreezeRomZone = 0x1, ///< One-time, irreversible
		ManufacturerId = 0xC,
		StandardSpeed = 0xD, ///< AT21CS01 only, the AT21CS11 NACKs this
		HighSpeed = 0xE,
	};

	/// Main array: 16 pages of 8 bytes
	static constexpr size_t EepromSize{128};
	static constexpr size_t PageSize{8};
	/// Security register: 4 pages of 8 bytes, the lower two read-only
	static constexpr size_t SecurityRegisterSize{32};
	/// Bytes of the factory-programmed serial number at security register 0x00
	static constexpr size_t SerialNumberSize{8};
	/// First byte of the serial number, a fixed product identifier
	static constexpr uint8_t ProductIdentifier{0xA0};

	/**
	 * High-Speed mode timings, in microseconds unless noted, from the
	 * AT21CSx1/AT21CS11 data sheet (DS20005857) AC characteristics. The
	 * AT21CS11 only ever runs at high speed; the AT21CSx1 powers up there too.
	 *
	 * The chosen values sit inside the specified windows with as much margin
	 * as the windows allow, which is not much: tLOW1 and tRD have a 1 us
	 * window and the read strobe must land before 2 us.
	 */
	struct Timing
	{
		static constexpr std::chrono::microseconds Reset{96};			///< tRESET, min, device idle
		static constexpr std::chrono::microseconds Discharge{150};		///< tDSCHG, min, device busy
		static constexpr std::chrono::microseconds ResetRecovery{8};	///< tRRT, min
		static constexpr std::chrono::microseconds ResponseRequest{1};	///< tDRR, 1 .. 2
		static constexpr std::chrono::microseconds ResponseStrobe{4};	///< tMSDR, 2 .. 6
		static constexpr std::chrono::microseconds ResponseAck{24};		///< tDACK, max
		static constexpr std::chrono::microseconds StartStop{150};		///< tHTSS, min
		static constexpr std::chrono::microseconds Bit{20};				///< tBIT, max 25
		static constexpr std::chrono::microseconds Low0{10};			///< tLOW0, 6 .. 16
		static constexpr std::chrono::microseconds Low1{1};				///< tLOW1, 1 .. 2
		static constexpr std::chrono::microseconds ReadLow{1};			///< tRD, 1 .. 2
		static constexpr std::chrono::nanoseconds  ReadStrobeNs{500};	///< tMRS - tRD
		static constexpr std::chrono::milliseconds WriteCycle{5};		///< tWR, max
	};

	/// The eight factory-programmed bytes at security register address 0x00
	struct SerialNumber
	{
		uint8_t data[SerialNumberSize];

		/// Fixed 0xA0 product identifier in byte 0
		constexpr uint8_t
		identifier() const
		{ return data[0]; }

		/// The 48-bit unique number in bytes 1 to 6, in the low 48 bits
		constexpr uint64_t
		unique() const
		{
			uint64_t value{};
			for (size_t ii = 1; ii <= 6; ii++) value = (value << 8) | data[ii];
			return value;
		}

		/// CRC-8/Maxim over bytes 0 to 6, stored in byte 7
		constexpr uint8_t
		crc() const
		{ return data[SerialNumberSize - 1]; }

		/// Whether the product identifier and the CRC-8 both check out. An
		/// unpopulated or miswired device reads as all-ones or all-zeroes,
		/// neither of which passes.
		bool
		isValid() const
		{
			return identifier() == ProductIdentifier and
				   modm::math::crc8_maxim(data, SerialNumberSize - 1) == crc();
		}
	};
};

/**
 * AT21CS01/AT21CS11 Single-Wire Serial EEPROM with a unique 64-bit serial number.
 *
 * @tparam	Pin		Open-drain capable GPIO tied to SI/O
 *
 * @author	Niklas Hauser
 * @ingroup modm_driver_at21csx1
 */
template<class Pin>
class At21csx1 : public at21csx1
{
public:
	/// Attempts made before a read is given up on, see the timing warning
	static constexpr uint8_t Retries{5};

	/// @param	address		The three preprogrammed client address bits (0 to 7)
	explicit
	At21csx1(uint8_t address = 0) : address_(address & 0b111) {}

	/// Configures the pin as an open-drain output, released high
	void
	initialize()
	{
		Pin::setOutput(Pin::OutputType::OpenDrain);
		Pin::set();
	}

	/// Resets the device and asks for a discovery response, which is the only
	/// way to find out whether anything is on the bus at all.
	bool
	ping()
	{
		return reset();
	}

	/// Reads `size` bytes of the main EEPROM array starting at `address`
	bool
	read(uint8_t address, uint8_t *data, size_t size)
	{ return readRegion(Opcode::Eeprom, address, data, size); }

	/// Reads `size` bytes of the security register starting at `address`
	bool
	readSecurityRegister(uint8_t address, uint8_t *data, size_t size)
	{ return readRegion(Opcode::SecurityRegister, address, data, size); }

	/// Reads the factory-programmed serial number and validates its CRC-8.
	/// Retries on a corrupted read, since a long enough interrupt anywhere in
	/// the transaction silently truncates it.
	bool
	readSerialNumber(SerialNumber &serial)
	{
		for (uint8_t attempt = 0; attempt < Retries; attempt++)
		{
			if (readRegion(Opcode::SecurityRegister, 0x00, serial.data, SerialNumberSize) and
				serial.isValid())
				return true;
		}
		return false;
	}

	/// Writes up to one page, without crossing a page boundary, and waits out
	/// the internal write cycle.
	bool
	write(uint8_t address, const uint8_t *data, size_t size)
	{ return writePage(Opcode::Eeprom, address, data, size); }

	/// Writes up to one page of the security register, without crossing a page
	/// boundary. Only the upper two pages are writable, the lower two hold the
	/// factory-programmed serial number and are read-only.
	bool
	writeSecurityRegister(uint8_t address, const uint8_t *data, size_t size)
	{
		if (address < SerialNumberSize * 2) return false;
		return writePage(Opcode::SecurityRegister, address, data, size);
	}

	/// Writes `size` bytes anywhere in the main array, splitting the transfer
	/// at page boundaries.
	bool
	writeAll(uint8_t address, const uint8_t *data, size_t size)
	{
		while (size)
		{
			const size_t chunk = std::min(size, PageSize - (address % PageSize));
			if (not write(address, data, chunk)) return false;
			address += chunk; data += chunk; size -= chunk;
		}
		return true;
	}

private:
	/// The device address byte: opcode, client address, read/write bit
	constexpr uint8_t
	deviceAddress(Opcode opcode, bool read) const
	{ return uint8_t(uint8_t(opcode) << 4) | uint8_t(address_ << 1) | uint8_t(read); }

	/// Drives SI/O low long enough for the device to lose its stored charge,
	/// then requests the discovery response acknowledge.
	bool
	reset()
	{
		Pin::reset();
		modm::this_fiber::sleep_for(Timing::Discharge); // covers tRESET as well
		Pin::set();
		modm::this_fiber::sleep_for(Timing::ResetRecovery);

		bool acknowledged;
		{
			modm::atomic::Lock _;
			Pin::reset();
			modm::delay(Timing::ResponseRequest);
			Pin::set();
			modm::delay(Timing::ResponseStrobe - Timing::ResponseRequest);
			acknowledged = not Pin::read();
		}
		// Let the device finish driving its acknowledge before the start bit
		modm::this_fiber::sleep_for(Timing::ResponseAck);
		return acknowledged;
	}

	/// Holds the line high long enough to count as a start or stop condition
	void
	startStop()
	{
		Pin::set();
		modm::this_fiber::sleep_for(Timing::StartStop);
	}

	/// One input bit frame: the low time decides the value
	void
	writeBit(bool value)
	{
		modm::atomic::Lock _;
		Pin::reset();
		modm::delay(value ? Timing::Low1 : Timing::Low0);
		Pin::set();
		modm::delay(Timing::Bit - (value ? Timing::Low1 : Timing::Low0));
	}

	/// One output bit frame: the device either holds the line down past the
	/// strobe point or lets the pull-up take it high
	bool
	readBit()
	{
		bool value;
		{
			modm::atomic::Lock _;
			Pin::reset();
			modm::delay(Timing::ReadLow);
			Pin::set();
			modm::delay(Timing::ReadStrobeNs);
			value = Pin::read();
		}
		modm::delay(Timing::Bit - Timing::ReadLow);
		return value;
	}

	/// Sends one byte MSB first and returns true if the device acknowledged
	bool
	writeByte(uint8_t byte)
	{
		for (uint8_t bit = 0; bit < 8; bit++, byte <<= 1) writeBit(byte & 0x80);
		return not readBit(); // the device pulls low to ACK
	}

	/// Receives one byte MSB first, then acknowledges to ask for another
	uint8_t
	readByte(bool acknowledge)
	{
		uint8_t byte{};
		for (uint8_t bit = 0; bit < 8; bit++) byte = uint8_t(byte << 1) | uint8_t(readBit());
		writeBit(not acknowledge); // a logic '0' is the ACK
		return byte;
	}

	/// Random read: a dummy write to place the address pointer, a repeated
	/// start, then a sequential read out of the addressed region.
	bool
	readRegion(Opcode opcode, uint8_t address, uint8_t *data, size_t size)
	{
		if (not reset()) return false;

		startStop();
		if (not writeByte(deviceAddress(opcode, false))) return false;
		if (not writeByte(address)) return false;

		startStop();
		if (not writeByte(deviceAddress(opcode, true))) return false;

		for (size_t ii = 0; ii < size; ii++) data[ii] = readByte(ii + 1 < size);
		startStop();
		return true;
	}

	bool
	writePage(Opcode opcode, uint8_t address, const uint8_t *data, size_t size)
	{
		if (size == 0) return true;
		// A write may not cross a page boundary, it would wrap around inside it
		if ((address % PageSize) + size > PageSize) return false;
		if (not reset()) return false;

		startStop();
		if (not writeByte(deviceAddress(opcode, false))) return false;
		if (not writeByte(address)) return false;
		for (size_t ii = 0; ii < size; ii++)
			if (not writeByte(data[ii])) return false;

		startStop(); // the stop condition starts the internal write cycle
		modm::this_fiber::sleep_for(Timing::WriteCycle);
		return true;
	}

	uint8_t address_;
};

} // namespace modm
