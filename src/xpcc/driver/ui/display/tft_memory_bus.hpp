
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

}

#endif /* TFT_MEMORY_BUS_HPP_ */
