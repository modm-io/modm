// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_SPI_HPP
#define XPCC_PERIPHERAL_SPI_HPP

#include <xpcc/processing/coroutine.hpp>
#include "../peripheral.hpp"

/**
 * @ingroup 	peripheral
 * @defgroup	spi		Serial Peripheral Interface (SPI)
 */

namespace xpcc
{

/// @ingroup spi
struct Spi
{
	/// Spi Data Mode, Mode0 is the most common mode
	enum class
	DataMode : uint8_t
	{
		Mode0 = 0b00,	///< clock normal,   sample on rising  edge
		Mode1 = 0b01,	///< clock normal,   sample on falling edge
		Mode2 = 0b10,	///< clock inverted, sample on rising  edge
		Mode3 = 0b11,	///< clock inverted, sample on falling edge
	};

	/// Spi Data Order, MsbFirst is the most common mode
	enum class
	DataOrder : uint8_t
	{
		MsbFirst = 0b0,
		LsbFirst = 0b1,
	};

	/// This tells the `SpiTransaction` why it was detached
	/// @see SpiTransaction
	enum class
	DetachCause : uint8_t
	{
		NormalStop,		///< All operations finished normally
		ErrorCondition,	///< A bus error occurred and the bus was reset
		SoftwareReset	///< The master is initializing itself
	};

	/// Operations after a transmission.
	enum class
	Operation
	{
		Stop = 0,		///< Stop after this transmission
		Transmit = 1,	///< start another transmission after this one
	};
};

/**
 * Interface for a simple SPI without resource management.
 * This interface should be more efficient than the SpiMaster, but it does not
 * synchronize multiple hardware access.
 * Use this interface if you need the *fastest possible* data transfer and only
 * have one slave on the bus or implement the resource management yourself.
 *
 * @warning	You can only either use the `SpiSimpleMaster` or `SpiMaster` driver
 * 			on the same hardware at the same time.
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SpiSimpleMaster : public ::xpcc::Peripheral, public Spi
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
	writeReadBlocking(uint8_t data);

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
	writeRead(uint8_t data);

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

/**
 * Interface for a SPI master with resource management.
 *
 * In order to allow resource management the SPI hardware, the selection of
 * the (active low) Slave Select pin must also be part of this management.
 * This will remove the need to synchronize hardware access on device driver
 * level while also providing asynchronous and, if available, DMA supported
 * data transfer.
 *
 * @warning	You can only either use the `SpiSimpleMaster` or `SpiMaster` driver
 * 			on the same hardware at the same time.
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
	 * 		the targets sytem clock
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 */
	template< class clockSource, uint32_t baudrate
            uint16_t tolerance = Tolerance::FivePercent >
	static void
	initialize();

	/**
	 * Requests bus control and starts the transfer.
	 *
	 * @param	transaction
	 * 		object that inherits from the SpiTransaction class.
	 *
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	start(SpiTransaction *transaction);

	/**
	 * Perform a software reset of the driver in case of an error.
	 *
	 * This method calls the `detaching()` method of the transaction object
     * and then detaches it.
	 */
	static void
	reset(DetachCause cause = DetachCause::SoftwareReset);
#endif
};

/**
 * Abstract class for transactions.
 *
 * For true asynchronous operation, the communication driver should
 * inherit from this class, allowing resource control of the hardware.
 * This transaction object will stay attached to the `SpiMaster` during the operation.
 *
 * @see SpiMaster
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SpiTransaction : public Spi
{
public:
	/// Contains the information required to make a SPI transfer
	struct Transmission {
		const uint8_t *writeBuffer;	///< data to write, set to `nullptr` to transmit dummy bytes
		uint8_t *readBuffer;		///< data to read, set `nullptr` to discard received bytes
		std::size_t length;			///< number of bytes to be transmitted
		Operation next;				///< operation following the transmission
	};

public:
	/**
	 * This method is called when the SpiMaster is not currently
	 * in use by another transaction and can be attached.
	 * Use this callback to pull the Slave Select pin low.
	 *
	 * @return	`true` if the SpiMaster should attach this transaction,
	 * 			`false` if it should not attach it.
	 */
	virtual bool
	attaching() = 0;

	/**
	 * This is called when the SpiMaster is ready to start a data transmission.
	 *
	 * @param[out]	transmission    `Transmission` struct containing write/read buffers, size and next operation
	 */
	virtual void
	transmitting(Transmission &transmission) = 0;

	/**
	 * This is called when all transmissions finished and transaction object is about to
	 * be detached. The SpiMaster will not be free until this method returns.
	 * Use this callback to pull the Slave Select pin high.
	 *
	 * @param	cause	specifies whether the detachment was expected (`NormalStop`),
	 * 					or a error occurred (`ErrorCondition`), which can, but does not need
	 * 					to be reacted upon.
	 */
	virtual void
	detaching(DetachCause cause) = 0;
};

} // namespace xpcc

#endif // XPCC_PERIPHERAL_SPI_HPP
