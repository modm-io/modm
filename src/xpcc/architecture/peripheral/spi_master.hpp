// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_SPI_MASTER_HPP
#define XPCC_PERIPHERAL_SPI_MASTER_HPP

#include <xpcc/processing/coroutine.hpp>
#include "spi.hpp"
#include "../peripheral.hpp"

/**
 * @ingroup 	peripheral
 * @defgroup	spi		Serial Peripheral Interface (SPI)
 */

namespace xpcc
{

/**
 * Interface for a simple SPI without resource management.
 * This interface should be more efficient than the SpiMaster, but it does not
 * synchronize multiple hardware access.
 * Use this interface if you need the *fastest possible* data transfer and only
 * have one slave on the bus or implement the resource management yourself.
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
	 * You must call this inside a Protothread or Coroutine
	 * using `PT_CALL` or `CO_CALL` respectively.
	 * These methods differ from Coroutines by lacking context protection!
	 * You must ensure that only one driver is accessing this coroutine.
	 *
	 * @param	data
	 * 		data to be sent
	 * @return	received data
	 */
	static xpcc::co::Result<uint8_t>
	transfer(uint8_t data);

	/**
	 * Set the data buffers and length with options and
	 * starts a non-blocking transfer.
	 *
	 * You must call this inside a Protothread or Coroutine
	 * using `PT_CALL` or `CO_CALL` respectively.
	 * These methods differ from Coroutines by lacking context protection!
	 * You must ensure that only one driver is accessing this coroutine.
	 *
	 * @param[in]   tx
	 *      pointer to transmit buffer, set to `nullptr` to send dummy bytes
	 * @param[out]  rx
	 *      pointer to receive buffer, set to `nullptr` to discard received bytes
	 * @param       length
	 *      number of bytes to be shifted out
	 */
	static xpcc::co::Result<void>
	transfer(uint8_t *tx, uint8_t *rx, std::size_t length);
#endif
};

} // namespace xpcc

#endif // XPCC_PERIPHERAL_SPI_MASTER_HPP
