/*
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_BDMA_HAL_HPP
#define MODM_STM32_BDMA_HAL_HPP

#include "bdma_base.hpp"

namespace modm::platform
{

/**
 * BDMA channel low-level register abstraction
 *
 * @tparam base_addr base address of the channel registers
 *
 * @ingroup	modm_platform_dma
 */
template<uintptr_t base_addr>
class BdmaChannelHal : public BdmaBase
{
	static BDMA_Channel_TypeDef*
	reg()
	{
		return reinterpret_cast<BDMA_Channel_TypeDef*>(base_addr);
	}

public:
	/**
	 * Configure the DMA channel (HAL)
	 *
	 * Stops the DMA channel and writes the new values to its control register.
	 *
	 * @param[in] direction Direction of the DMA channel
	 * @param[in] memoryDataSize Size of data in memory (byte, halfword, word)
	 * @param[in] peripheralDataSize Size of data in peripheral (byte, halfword, word)
	 * @param[in] memoryIncrement Defines whether the memory address is incremented
	 * 			  after a transfer completed
	 * @param[in] peripheralIncrement Defines whether the peripheral address is
	 * 			  incremented after a transfer completed
	 * @param[in] priority Priority of the DMA channel
	 * @param[in] circularMode Transfer data in circular mode?
	 */
	static void
	configure(DataTransferDirection direction, MemoryDataSize memoryDataSize,
			PeripheralDataSize peripheralDataSize,
			MemoryIncrementMode memoryIncrement,
			PeripheralIncrementMode peripheralIncrement,
			Priority priority = Priority::Medium,
			CircularMode circularMode = CircularMode::Disabled)
	{
		stop();

		reg()->CCR = uint32_t(direction) | uint32_t(memoryDataSize) |
				uint32_t(peripheralDataSize) | uint32_t(memoryIncrement) |
				uint32_t(peripheralIncrement) | uint32_t(priority) |
				uint32_t(circularMode);
	}

	/**
	 * Start the transfer of the DMA channel
	 */
	static void
	start()
	{
		reg()->CCR |= BDMA_CCR_EN;
	}

	/**
	 * Stop a DMA channel transfer
	 */
	static void
	stop()
	{
		reg()->CCR &= ~BDMA_CCR_EN;
		while (reg()->CCR & BDMA_CCR_EN); // wait for stream to be stopped
	}

	static DataTransferDirection
	getDataTransferDirection()
	{
		return static_cast<DataTransferDirection>(
			reg()->CCR & (BDMA_CCR_MEM2MEM | BDMA_CCR_DIR));
	}

	/**
	 * Set the memory address of the DMA channel
	 *
	 * @note In Mem2Mem mode use this method to set the memory source address.
	 *
	 * @param[in] address Source address
	 */
	static void
	setMemoryAddress(uintptr_t address)
	{
		reg()->CM0AR = address;
	}

	/**
	 * Set the peripheral address of the DMA channel
	 *
	 * @note In Mem2Mem mode use this method to set the memory destination address.
	 *
	 * @param[in] address Destination address
	 */
	static void
	setPeripheralAddress(uintptr_t address)
	{
		reg()->CPAR = address;
	}

	/**
	 * Enable/disable memory increment
	 *
	 * When enabled, the memory address is incremented by the size of the data
	 * (e.g. 1 for byte transfers, 4 for word transfers) after the transfer
	 * completed.
	 *
	 * @param[in] increment Enable/disable
	 */
	static void
	setMemoryIncrementMode(bool increment)
	{
		if (increment)
			reg()->CCR |= uint32_t(MemoryIncrementMode::Increment);
		else
			reg()->CCR &= ~uint32_t(MemoryIncrementMode::Increment);
	}

	/**
	 * Enable/disable peripheral increment
	 *
	 * When enabled, the peripheral address is incremented by the size of the data
	 * (e.g. 1 for byte transfers, 4 for word transfers) after the transfer
	 * completed.
	 *
	 * @param[in] increment Enable/disable
	 */
	static void
	setPeripheralIncrementMode(bool increment)
	{
		if (increment)
			reg()->CCR |= uint32_t(PeripheralIncrementMode::Increment);
		else
			reg()->CCR &= ~uint32_t(PeripheralIncrementMode::Increment);
	}

	/**
	 * Set length of data to transfer
	 */
	static void
	setDataLength(std::size_t length)
	{
		reg()->CNDTR = length;
	}

	/**
	 * Enable IRQ of this DMA channel (e.g. transfer complete or error)
	 */
	static void
	enableInterrupt(InterruptEnable_t irq)
	{
		reg()->CCR |= irq.value;
	}
	/**
	 * Disable IRQ of this DMA channel (e.g. transfer complete or error)
	 */
	static void
	disableInterrupt(InterruptEnable_t irq)
	{
		reg()->CCR &= ~(irq.value);
	}
};

} // namespace modm::platform

#endif // MODM_STM32_BDMA_HAL_HPP
