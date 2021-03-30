/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_SPI_MASTER_HPP
#define MODM_INTERFACE_SPI_MASTER_HPP

#include <modm/processing/resumable.hpp>
#include "spi.hpp"

namespace modm
{

/**
 * Interface for a Spi Master with resource management, which allows to
 * synchronize multiple drivers requesting hardware access.
 *
 * @author	Niklas Hauser
 * @ingroup	modm_architecture_spi
 */
class SpiMaster : public ::modm::PeripheralDriver, public Spi
{
#ifdef __DOXYGEN__
public:
	/**
	 * Connect GPIOs to the peripheral and configure them.
	 *
	 * This configures the Sck, Mosi and Miso signals and connects them.
	 *
	 * @tparam	Signals
	 *		One Scl and one Mosi signal are required (one Miso signal is
	 *		optional) and can be passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed relative tolerance for the resulting baudrate
	 */
	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=5_pct >
	static void
	initialize();

	/// Sets a new data mode.
	static void
	setDataMode(DataMode mode);

	/// Sets a new data order.
	static void
	setDataOrder(DataOrder order);

	/**
	 * Request access to the spi master within a context.
	 * You may acquire the spi master multiple times within the same context.
	 *
	 * The configuration handler will only be called when aquiring the spi
	 * master for the first time (if it is not a `nullptr`).
	 *
	 * @warning		Aquires must be balanced with releases of the **same** context!
	 * @warning		Aquires are persistent even after calling `initialize()`!
	 *
	 * @return	`0` if another context is using the spi master, otherwise
	 * 			`>0` as the number of times this context acquired the master.
	 */
	static uint8_t
	acquire(void *ctx, ConfigurationHandler handler = nullptr);

	/**
	 * Release access to the spi master within a context.
	 *
	 * @warning		Releases must be balanced with acquires of the **same** context!
	 * @warning		Releases are persistent even after calling `initialize()`!
	 *
	 * @return	`0` if nothing can be released anymore (for any context)
	 * 			`>0` as the number of times this context can still release the master.
	 */
	static uint8_t
	release(void *ctx);

	/**
	 * Swap a single byte and wait for completion.
	 *
	 * @param	data
	 * 		data to be sent
	 * @return	received data
	 */
	static uint8_t
	transferBlocking(uint8_t data);

	/**
	 * Set the data buffers and length with options and starts a transfer.
	 * This may be hardware accelerated (DMA or Interrupt), but not guaranteed.
	 *
	 * @param[in]   tx
	 *      pointer to transmit buffer, set to `nullptr` to send dummy bytes
	 * @param[out]  rx
	 *      pointer to receive buffer, set to `nullptr` to discard received bytes
	 * @param       length
	 *      number of bytes to be shifted out
	 */
	static void
	transferBlocking(const uint8_t *tx, uint8_t *rx, std::size_t length);

	/**
	 * Swap a single byte and wait for completion non-blocking!.
	 *
	 * You must call this inside a Protothread or Resumable
	 * using `PT_CALL` or `RF_CALL` respectively.
	 * @warning	These methods differ from Resumables by lacking context protection!
	 * 			You must ensure that only one driver is accessing this resumable function
	 * 			by using `acquire(ctx)` and `release(ctx)`.
	 *
	 * @param	data
	 * 		data to be sent
	 * @return	received data
	 */
	static modm::ResumableResult<uint8_t>
	transfer(uint8_t data);

	/**
	 * Set the data buffers and length with options and
	 * starts a non-blocking transfer.
	 * This may be hardware accelerated (DMA or Interrupt), but not guaranteed.
	 *
	 * You must call this inside a Protothread or Resumable
	 * using `PT_CALL` or `RF_CALL` respectively.
	 * @warning	These methods differ from Resumables by lacking context protection!
	 * 			You must ensure that only one driver is accessing this resumable function
	 * 			by using `acquire(ctx)` and `release(ctx)`.
	 *
	 * @param[in]   tx
	 *      pointer to transmit buffer, set to `nullptr` to send dummy bytes
	 * @param[out]  rx
	 *      pointer to receive buffer, set to `nullptr` to discard received bytes
	 * @param       length
	 *      number of bytes to be shifted out
	 */
	static modm::ResumableResult<void>
	transfer(const uint8_t *tx, uint8_t *rx, std::size_t length);
#endif
};

} // namespace modm

#endif // MODM_INTERFACE_SPI_MASTER_HPP
