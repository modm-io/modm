// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32F3_DMA_BASE_HPP
#define XPCC_STM32F3_DMA_BASE_HPP

#include <stdint.h>
#include "../../../type_ids.hpp"
#include "../../../device.hpp"

namespace xpcc
{
namespace stm32
{
/**
 * DMA
 *
 * \author	Kevin Laeufer
 * \ingroup	stm32
 */
class DmaBase
{
public:
	// Enums
	enum class
	Channel : uint32_t
	{
		Channel0 = 0,
		Channel1 = DMA_SxCR_CHSEL_0,
		Channel2 = DMA_SxCR_CHSEL_1,
		Channel3 = DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0,
		Channel4 = DMA_SxCR_CHSEL_2,
		Channel5 = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_0,
		Channel6 = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1,
		Channel7 = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0,
	};

	enum class
	MemoryBurstTransfer : uint32_t
	{
		Single 		= 0,
		Increment4 	= DMA_SxCR_MBURST_0,
		Increment8 	= DMA_SxCR_MBURST_1,
		Increment16 = DMA_SxCR_MBURST_1 | DMA_SxCR_MBURST_0,
	};

	enum class
	PeripheralBurstTransfer : uint32_t
	{
		Single 		= 0,
		Increment4 	= DMA_SxCR_PBURST_0,
		Increment8 	= DMA_SxCR_PBURST_1,
		Increment16 = DMA_SxCR_PBURST_1 | DMA_SxCR_PBURST_0,
	};

	enum class
	Priority : uint32_t
	{
		Low 		= 0,
		Medium  	= DMA_SxCR_PL_0,
		High 		= DMA_SxCR_PL_1,
		VeryHigh 	= DMA_SxCR_PL_1 | DMA_SxCR_PL_0,
	};

	/// In direct mode (if the FIFO is not used)
	/// MSIZE is forced by hardware to the same value as PSIZE
	enum class
	MemoryDataSize : uint32_t
	{
		Byte 		= 0,
		Bit8 		= Byte,
		HalfWord 	= DMA_SxCR_MSIZE_0,
		Bit16 		= HalfWord,
		Word 		= DMA_SxCR_MSIZE_1,
		Bit32 		= Word,
	};

	enum class
	PeripheralDataSize : uint32_t
	{
		Byte 		= 0,
		Bit8 		= Byte,
		HalfWord 	= DMA_SxCR_PSIZE_0,
		Bit16 		= HalfWord,
		Word 		= DMA_SxCR_PSIZE_1,
		Bit32 		= Word,
	};

	enum class
	MemoryIncrementMode : uint32_t
	{
		Fixed 		= 0,
		Increment 	= DMA_SxCR_MINC, ///< incremented according to MemoryDataSize
	};

	enum class
	PeripheralIncrementMode : uint32_t
	{
		Fixed 		= 0,
		Increment 	= DMA_SxCR_PINC, ///< incremented according to PeripheralDataSize
	};

	enum class
	DataTransferDirection : uint32_t
	{
		/// Source: DMA_SxPAR; Sink: DMA_SxM0AR
		PeripheralToMemory 	= 0,
		/// Source: DMA_SxM0AR; Sink: DMA_SxPAR
		MemoryToPeripheral 	= DMA_SxCR_DIR_0,
		/// Source: DMA_SxPAR; Sink: DMA_SxM0AR
		MemoryToMemory 		= DMA_SxCR_DIR_1,
	};

	enum class
	FlowControl : uint32_t
	{
		Dma 		= 0,
		Peripheral 	= DMA_SxCR_PFCTRL, ///< the peripheral is the flow controller
	};
};
}
}
#endif	// XPCC_STM32F3_DMA_BASE_HPP

