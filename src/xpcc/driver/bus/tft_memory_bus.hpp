/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2014, 2016, Niklas Hauser
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

namespace xpcc
{
/**
 * @author	Fabian Greif
 * @ingroup	driver_bus
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

	xpcc_always_inline void
	writeIndex(uint16_t index)
	{
		*ptrIndex = index;
	}

	xpcc_always_inline void
	writeData(uint16_t data)
	{
		*ptrData = data;
	}

	xpcc_always_inline uint16_t
	readData()
	{
		return *ptrData;
	}

	xpcc_always_inline void
	writeRegister(uint16_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

	xpcc_always_inline uint16_t
	readRegister(uint16_t reg)
	{
		writeIndex(reg);
		return readData();
	}

private:
	volatile uint16_t * const ptrIndex;
	volatile uint16_t * const ptrData;
};

/// @ingroup	driver_bus
class TftMemoryBus8Bit
{
public:
	TftMemoryBus8Bit(
			volatile uint8_t * index,
			volatile uint8_t * data) :
		ptrIndex(index), ptrData(data)
	{
	}

	xpcc_always_inline void
	writeIndex(uint8_t index)
	{
		*ptrIndex = 0;
		*ptrIndex = index;
	}

	xpcc_always_inline void
	writeData(uint16_t data)
	{
		*ptrData = data >> 8;
		*ptrData = data & 0xff;
	}

//	xpcc_always_inline uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	xpcc_always_inline void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	xpcc_always_inline uint16_t
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
 * @ingroup	driver_bus
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
	static xpcc_always_inline void
	initialize()
	{
		CS::set();
		WR::set();
		RD::set();
		PORT::setInput();
	}

	static xpcc_always_inline void
	write(const uint8_t data)
	{
		CS::reset();
		RD::set();

		PORT::setOutput();
		PORT::write(data);

		// t_AS
		xpcc::delayMicroseconds(1);
		WR::reset();

		xpcc::delayMicroseconds(1);
		WR::set();

		xpcc::delayMicroseconds(1);
		PORT::setInput();

		CS::set();
	}

	static xpcc_always_inline uint8_t
	read()
	{
		uint8_t ret;
		CS::reset();
		WR::set();

		xpcc::delayMicroseconds(1);

		RD::reset();

		xpcc::delayMicroseconds(1);
		ret = PORT::read();

		RD::set();
		CS::set();

		return ret;
	}
};

/// @ingroup	driver_bus
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

	static xpcc_always_inline void
	initialize()
	{
		BUS::initialize();
	}

	static xpcc_always_inline void
	writeIndex(uint8_t index)
	{
		// *ptrIndex = 0;
		CD::reset();
		BUS::write(0);

		// *ptrIndex = index;
		BUS::write(index);
	}

	static xpcc_always_inline void
	writeData(uint16_t data)
	{
		// *ptrData = data >> 8;
		CD::set();
		BUS::write(data >> 8);

		// *ptrData = data & 0xff;
		BUS::write(data & 0xff);
	}

//	static xpcc_always_inline uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	static xpcc_always_inline void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	static xpcc_always_inline uint16_t
//	readRegister(uint16_t reg)
//	{
//		writeIndex(reg);
//		return readData();
//	}
};

}

#endif /* TFT_MEMORY_BUS_HPP_ */
