
#ifndef TFT_MEMORY_BUS_HPP_
#define TFT_MEMORY_BUS_HPP_

namespace xpcc
{
/**
 *
 * @author	Fabian Greif
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

	ALWAYS_INLINE void
	writeIndex(uint16_t index)
	{
		*ptrIndex = index;
	}

	ALWAYS_INLINE void
	writeData(uint16_t data)
	{
		*ptrData = data;
	}

	ALWAYS_INLINE uint16_t
	readData()
	{
		return *ptrData;
	}

	ALWAYS_INLINE void
	writeRegister(uint16_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

	ALWAYS_INLINE uint16_t
	readRegister(uint16_t reg)
	{
		writeIndex(reg);
		return readData();
	}

private:
	volatile uint16_t * const ptrIndex;
	volatile uint16_t * const ptrData;
};

class TftMemoryBus8Bit
{
public:
	TftMemoryBus8Bit(
			volatile uint8_t * index,
			volatile uint8_t * data) :
		ptrIndex(index), ptrData(data)
	{
	}

	ALWAYS_INLINE void
	writeIndex(uint8_t index)
	{
		*ptrIndex = 0;
		*ptrIndex = index;
	}

	ALWAYS_INLINE void
	writeData(uint16_t data)
	{
		*ptrData = data >> 8;
		*ptrData = data & 0xff;
	}

//	ALWAYS_INLINE uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	ALWAYS_INLINE void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	ALWAYS_INLINE uint16_t
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
 * @author	strongly-typed
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
	static ALWAYS_INLINE void
	initialize()
	{
		CS::set();
		WR::set();
		RD::set();
		PORT::setInput();
	}

	static ALWAYS_INLINE void
	write(const uint8_t data)
	{
		CS::reset();
		RD::set();

		PORT::setOutput();
		PORT::write(data);

		// t_AS
		xpcc::delay_us(1);
		WR::reset();

		xpcc::delay_us(1);
		WR::set();

		xpcc::delay_us(1);
		PORT::setInput();

		CS::set();
	}

	static ALWAYS_INLINE uint8_t
	read()
	{
		uint8_t ret;
		CS::reset();
		WR::set();

		xpcc::delay_us(1);

		RD::reset();

		xpcc::delay_us(1);
		ret = PORT::read();

		RD::set();
		CS::set();

		return ret;
	}
};

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

	static ALWAYS_INLINE void
	initialize()
	{
		BUS::initialize();
	}

	static ALWAYS_INLINE void
	writeIndex(uint8_t index)
	{
		// *ptrIndex = 0;
		CD::reset();
		BUS::write(0);

		// *ptrIndex = index;
		BUS::write(index);
	}

	static ALWAYS_INLINE void
	writeData(uint16_t data)
	{
		// *ptrData = data >> 8;
		CD::set();
		BUS::write(data >> 8);

		// *ptrData = data & 0xff;
		BUS::write(data & 0xff);
	}

//	static ALWAYS_INLINE uint16_t
//	readData()
//	{
//		return *ptrData;
//	}

	static ALWAYS_INLINE void
	writeRegister(uint8_t reg, uint16_t value)
	{
		writeIndex(reg);
		writeData(value);
	}

//	static ALWAYS_INLINE uint16_t
//	readRegister(uint16_t reg)
//	{
//		writeIndex(reg);
//		return readData();
//	}
};

}

#endif /* TFT_MEMORY_BUS_HPP_ */
