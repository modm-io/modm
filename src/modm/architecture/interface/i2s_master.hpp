/*
 * Copyright (c) 2021, Marton Lednczki
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2S_MASTER_HPP
#define MODM_INTERFACE_I2S_MASTER_HPP

#include <modm/processing/resumable.hpp>
#include "i2s.hpp"

namespace modm
{

/**
 * Interface for a I2S Master
 *
 * @author	Marton Ledneczki
 * @ingroup	modm_architecture_i2s
 */
class I2sMaster : public ::modm::PeripheralDriver
{
#ifdef __DOXYGEN__
public:
	/**
	 * @brief Connect GPIOs to the peripheral and configure them.
	 *
	 * This configures the CK (serial clock), MCK (master clock),
	 * SD (serial data) and WS (word select) signals and connects them.
	 *
	 * @tparam	Signals One CK, SD, WS, MCK signal are required and can be
	 * passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * @brief Initializes the hardware and sets the samplerate.
	 *
	 * @tparam SystemClock the currently active system clock
	 * @tparam samplerate the desired sample rate in Hz
	 * @tparam tolerance the allowed relative tolerance for the resulting samplerate
	 */
	template< class SystemClock, frequency_t samplerate, percent_t tolerance=pct(0.019) >
	static void
	initialize();


	/**
	 * @brief Set the buffer
	 *
	 * @param address Pointer to the buffer. The user has to ensure the buffer is in DMA-able memory.
	 * @param length Size of the buffer
	 */
	static void
	setTxBuffer(uintptr_t address, std::size_t length);

	/**
	 * @brief Set transfer complete handler function
	 *
	 * The transfer complete handler should set the next tx buffer using 
	 * `setTxBuffer()` and then restart the DMA transfer using `startDma()`.
	 */
	static void
	setTransferCompleteIrqHandler(DmaBase::IrqHandler handleDmaTransferComplete);

	/**
	 * @brief Starts the I2S peripheral
	 */
	static void
	start();

	/**
	 * @brief (Re-)Starts a DMA transfer
	 *
	 * This will be usually called inside the transfer complete handler after
	 * a (new) tx buffer address was set.
	 *
	 * @see setTransferCompleteIrqHandler()
	 */
	static void
	startDma();

	/**
	 * @brief Stops I2S
	 */
	static void
	stop();

#endif
};

} // namespace modm

#endif // MODM_INTERFACE_SPI_MASTER_HPP
