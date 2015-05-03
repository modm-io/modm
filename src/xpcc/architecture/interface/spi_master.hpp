// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_SPI_MASTER_HPP
#define XPCC_INTERFACE_SPI_MASTER_HPP

#include <xpcc/processing/resumable.hpp>
#include "spi.hpp"

namespace xpcc
{

/**
 * Interface for a Spi Master with resource management, which allows to
 * synchronize multiple drivers requesting hardware access.
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SpiMaster : public ::xpcc::Peripheral, public Spi
{
#ifdef __DOXYGEN__
public:
	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	clockSource
	 * 		the currently active system clock
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 */
	template< class clockSource, uint32_t baudrate,
			uint16_t tolerance = Tolerance::FivePercent >
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
	 * You may aquire the spi master multiple times within the same context.
	 *
	 * The configuration handler will only be called when aquiring the spi
	 * master for the first time (if it is not a `nullptr`).
	 *
	 * @warning		Aquires must be balanced with releases of the **same** context!
	 * @warning		Aquires are persistent even after calling `initialize()`!
	 *
	 * @return	`0` if another context is using the spi master, otherwise
	 * 			`>0` as the number of times this context aquired the master.
	 */
	static uint8_t
	aquire(void *ctx, ConfigurationHandler handler = nullptr);

	/**
	 * Release access to the spi master within a context.
	 *
	 * @warning		Releases must be balanced with aquires of the **same** context!
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
	transferBlocking(uint8_t *tx, uint8_t *rx, std::size_t length);

	/**
	 * Swap a single byte and wait for completion non-blocking!.
	 *
	 * You must call this inside a Protothread or Resumable
	 * using `PT_CALL` or `CO_CALL` respectively.
	 * @warning	These methods differ from Resumables by lacking context protection!
	 * 			You must ensure that only one driver is accessing this resumable function
	 * 			by using `aquire(ctx)` and `release(ctx)`.
	 *
	 * @param	data
	 * 		data to be sent
	 * @return	received data
	 */
	static xpcc::ResumableResult<uint8_t>
	transfer(uint8_t data);

	/**
	 * Set the data buffers and length with options and
	 * starts a non-blocking transfer.
	 * This may be hardware accelerated (DMA or Interrupt), but not guaranteed.
	 *
	 * You must call this inside a Protothread or Resumable
	 * using `PT_CALL` or `CO_CALL` respectively.
	 * @warning	These methods differ from Resumables by lacking context protection!
	 * 			You must ensure that only one driver is accessing this resumable function
	 * 			by using `aquire(ctx)` and `release(ctx)`.
	 *
	 * @param[in]   tx
	 *      pointer to transmit buffer, set to `nullptr` to send dummy bytes
	 * @param[out]  rx
	 *      pointer to receive buffer, set to `nullptr` to discard received bytes
	 * @param       length
	 *      number of bytes to be shifted out
	 */
	static xpcc::ResumableResult<void>
	transfer(uint8_t *tx, uint8_t *rx, std::size_t length);
#endif
};

} // namespace xpcc

#endif // XPCC_INTERFACE_SPI_MASTER_HPP
