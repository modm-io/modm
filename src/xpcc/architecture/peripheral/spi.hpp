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

	/// This tells the `SpiDelegate` why it was detached
	/// @see SpiDelegate
	enum class
	DetachCause : uint8_t
	{
		NormalStop,		///< All operations finished normally
		ErrorCondition,	///< A bus error occured and the bus was reset
		SoftwareReset	///< The master is initializing itself
	};

	/// transfer options for the receive and transmit buffers
	/// @see SpiBlockMaster
	enum class
	BufferOptions : uint8_t
	{
		TxRxIncrement = 0b11,           ///< Increments both Tx and Rx buffer
		TxIncrementRxDecrement = 0b10,  ///< Increments Tx, but decrements Rx buffer
		TxDecrementRxIncrement = 0b01,  ///< Decrements Tx, but increments Rx buffer
		TxRxDecrement = 0b00,           ///< Decrements both Tx and Rx buffer
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
 * @warning	You can only either use the `SimpleSpi` or `SpiMaster` driver on
 * 			the same hardware at the same time.
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SimpleSpi : public ::xpcc::Peripheral, public Spi
{
#ifdef __DOXYGEN__
public:
	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 */
	template< uint32_t baudrate >
	static void
	initialize();

	/// Sets a new data mode.
	static void
	setDataMode(DataMode mode);

	/// Sets a new data order.
	static void
	setDataOrder(DataOrder order);

	// synchronous
	/**
	 * Write a single byte, wait for completion.
	 *
	 * @return	received byte.
	 */
	static uint8_t
	writeReadBlocking(uint8_t data);

	/// Write a single byte, wait for completion and discard received byte.
	static void
	writeBlocking(uint8_t data);

	// asynchronous
	/**
	 * Write a single byte, and return immediately.
	 *
	 * @return	`true` if data has been sent, `false` if buffer is full
	 */
	static bool
	write(uint8_t data);

	/// @return	last byte that has been received.
	static uint8_t
	getResult();

    /**
     * Set the data buffers and length with options and starts a transfer.
     * This may be hardware accelerated (DMA or Interrupt), but not guaranteed.
     *
     * @param[in]   tx
     *      pointer to transmit buffer, set to `0` to send dummy bytes
     * @param[out]  rx
     *      pointer to receive buffer, set to `0` to discard received bytes
     * @param       length
     *      number of bytes to be shifted out
     * @param       options
     *      buffer options, @see BufferOptions
     *
     * @return  `true`  if transfer request was successfully serviced,
     *          `false` if another transfer is already progress.
     */
    static bool
    transfer(uint8_t * tx, uint8_t * rx, std::size_t length, BufferOptions options=BufferOptions::TxRxIncrement);

	/// @return	`true` if last byte has been sent and the swapped byte can be read.
	static bool
	isFinished();
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
 * @warning	You can only either use the `SimpleSpi` or `SpiMaster` driver on
 * 			the same hardware at the same time.
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SpiMaster : public ::xpcc::Peripheral, public Spi
{
#ifdef __DOXYGEN__
public:
	/**
	 * Initialize the hardware and sets the baudrate.
	 *
	 * @tparam	baudrate	the desired baudrate in Hz
	 */
	template< uint32_t baudrate >
	static void
	initialize();

	/**
	 * Requests bus control and starts the transfer.
	 *
	 * @param	delegate
	 * 		object that inherits from the SpiDelegate class.
	 * @param	mode
	 * 		desired data mode for this transfer
	 * @param	order
	 * 		desired data order for this transfer
	 *
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	start(SpiDelegate *delegate,
			DataMode mode=DataMode::Mode0,
			DataOrder order=DataOrder::MsbFirst);

	/**
	 * Requests bus control and starts the transfer, blocks until delegate is detached.
	 *
	 * @param	delegate
	 * 		object that inherits from the SpiDelegate class.
	 * @param	mode
	 * 		desired timing mode for this transfer
	 * @param	order
	 * 		desired data order for this transfer
	 *
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	startBlocking(SpiDelegate *delegate,
			DataMode mode=DataMode::Mode0,
			DataOrder order=DataOrder::MsbFirst);

	/**
	 * Perform a software reset of the driver in case of an error.
	 *
	 * This method calls the `detaching()` Delegate method and then detaches
	 * the delegate.
	 */
	static void
	reset(DetachCause cause = DetachCause::SoftwareReset);
#endif
};

/**
 * Abstract class for delegation.
 *
 * For true asynchronous operation, the communication driver should
 * inherit from this class, allowing resource control of the hardware.
 * This Delegate will stay attached to the `SpiMaster` during the operation.
 *
 * @see SpiMaster
 *
 * @author	Niklas Hauser
 * @ingroup	spi
 */
class SpiDelegate : public Spi
{
public:
	/// Contains the information required to make a SPI transfer
	struct Transmission {
		const uint8_t *writeBuffer;	///< data to write, set to `0` to transmit dummy bytes
		uint8_t *readBuffer;		///< data to read, set `0` to discard received bytes
		std::size_t size;			///< number of bytes to be transmitted
		BufferOptions options;		///< buffer options, @see BufferOptions
		Operation next;				///< operation following the transmission
	};

public:
	/**
	 * This method is called when the SpiMaster is not currently
	 * in use by another delegate and can be attached.
	 * Use this callback to pull the Slave Select pin low.
	 *
	 * @return	`true` if the SpiMaster should attach this delegate,
	 * 			`false` if it should not attach it.
	 */
	virtual bool
	attaching() = 0;

	/**
	 * This is called when the SpiMaster is ready to start a transmission.
	 *
	 * @return	the `Transmission` struct
	 */
	virtual Transmission
	transmitting() = 0;

	/**
	 * This is called when the transmission finished and delegate is about to
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
