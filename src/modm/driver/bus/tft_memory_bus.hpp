/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2014, 2016, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TFT_MEMORY_BUS_HPP_
#define TFT_MEMORY_BUS_HPP_

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/delay.hpp>

namespace modm
{
/**
 * @author	Fabian Greif
 * @ingroup	modm_driver_memory_bus
 */
class TftMemoryBus16Bit
{
public:
	TftMemoryBus16Bit(
			volatile uint16_t * index,
			volatile uint16_t * data) :
		ptrIndex(index), ptrData(data)
	{
	}

	void
	writeIndex(uint16_t index)
	{
		*ptrIndex = index;
	}

	void
	writeData(uint16_t data)
	{
		*ptrData = data;
	}

	uint16_t
	readData()
	{
		return *ptrData;
	}

	void
	writeRegister(uint16_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

	uint16_t
	readRegister(uint16_t reg)
	{
		writeIndex(reg);
		return readData();
	}

private:
	volatile uint16_t * const ptrIndex;
	volatile uint16_t * const ptrData;
};

/// @ingroup	modm_driver_memory_bus
class TftMemoryBus8Bit
{
public:
	TftMemoryBus8Bit(
			volatile uint8_t * index,
			volatile uint8_t * data) :
		ptrIndex(index), ptrData(data)
	{
	}

	void
	writeIndex(uint8_t index)
	{
		*ptrIndex = 0;
		*ptrIndex = index;
	}

	void
	writeData(uint16_t data)
	{
		*ptrData = data >> 8;
		*ptrData = data & 0xff;
	}

//	uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	uint16_t
//	readRegister(uint16_t reg)
//	{
//		writeIndex(reg);
//		return readData();
//	}

private:
	volatile uint8_t * const ptrIndex;
	volatile uint8_t * const ptrData;
};

/**
 * 8080-style parallel bus made of a parallel PORT, chip select CS, read strobe RD and write strobe WR
 *
 * @author	Sascha Schade
 * @ingroup	modm_driver_memory_bus
 */
template <
	typename PORT,	/* Port */
	typename CS,	/* Chip select  */
	typename RD,	/* Read strobe  */
	typename WR		/* Write strobe */
	>
class MemoryBus
{
public:
	static void
	initialize()
	{
		CS::set();
		WR::set();
		RD::set();
		PORT::setInput();
	}

	static void
	write(const uint8_t data)
	{
		CS::reset();
		RD::set();

		PORT::setOutput();
		PORT::write(data);

		// t_AS
		modm::delay_us(1);
		WR::reset();

		modm::delay_us(1);
		WR::set();

		modm::delay_us(1);
		PORT::setInput();

		CS::set();
	}

	static uint8_t
	read()
	{
		uint8_t ret;
		CS::reset();
		WR::set();

		modm::delay_us(1);

		RD::reset();

		modm::delay_us(1);
		ret = PORT::read();

		RD::set();
		CS::set();

		return ret;
	}
};

/// @ingroup	modm_driver_memory_bus
template <
	typename PORT,	/* Port */
	typename CS,	/* Chip select */
	typename RD,	/* Read strobe */
	typename WR,	/* Write strobe */
	typename CD		/* command/data */
	>
class TftMemoryBus8BitGpio
{
public:
	typedef MemoryBus<PORT, CS, RD, WR> BUS;

	static void
	initialize()
	{
		BUS::initialize();
	}

	static void
	writeIndex(uint8_t index)
	{
		// *ptrIndex = 0;
		CD::reset();
		BUS::write(0);

		// *ptrIndex = index;
		BUS::write(index);
	}

	static void
	writeData(uint16_t data)
	{
		// *ptrData = data >> 8;
		CD::set();
		BUS::write(data >> 8);

		// *ptrData = data & 0xff;
		BUS::write(data & 0xff);
	}

//	static uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	static void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	static uint16_t
//	readRegister(uint16_t reg)
//	{
//		writeIndex(reg);
//		return readData();
//	}
};

}

#endif /* TFT_MEMORY_BUS_HPP_ */
