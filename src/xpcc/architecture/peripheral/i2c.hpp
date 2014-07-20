// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_I2C_MASTER_HPP
#define XPCC_PERIPHERAL_I2C_MASTER_HPP

#include "../peripheral.hpp"

/**
 * @ingroup 	peripheral
 * @defgroup	i2c		Inter-Integrated Circuit (I2C)
 */

namespace xpcc
{

/// @ingroup	i2c
struct I2c
{
	static constexpr uint8_t Write = 0x00;	///< Add the Write bit to the slave address
	static constexpr uint8_t Read  = 0x01;	///< Add the Read bit to the slave address

	/// This tells the `I2cDelegate` why it was detached
	/// @see I2cDelegate
	enum class
	DetachCause : uint8_t
	{
		NormalStop,		///< All operations finished normally
		ErrorCondition,	///< A bus error occurred and the bus was reset
		SoftwareReset	///< The master is initializing itself
	};

	///@{
	/// I2C Operations that the `I2cDelegate` can give the master on callback
	/// @see I2cDelegate

	/// Operations after a start or restart condition.
	enum class
	Operation
	{
		Stop = 0,		///< Generate a Stop Condition
		Restart = 1,	///< Generate a Restart
		Write = 2,		///< Write data to the slave
		Read = 3,		///< Read data from the slave
	};

	/// Further operations after write operation.
	enum class
	OperationAfterWrite
	{
		Stop = 0,
		Restart = 1,
		Write = 2,
	};

	/// Further operations after read operation.
	enum class
	OperationAfterRead
	{
		Stop = 0,
		Restart = 1,
	};
	///@}

	/// State of a Delegate Adapter
	enum class
	AdapterState
	{
		Idle,	///< No error occurred, detached normally
		Busy,	///< The adapter is busy with data transfer
		Error	///< An error occurred, check the masters `getErrorCode()`
	};
};

/**
 * Interface of a I2C master.
 *
 * Performing transfers in background allows the use of DMA etc. and
 * enables the program to do other things while the operation is
 * going on.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup i2c
 */
class I2cMaster : public ::xpcc::Peripheral, public I2c
{
public:
	/// Errors that can happen during master operation
	enum class
	Error : uint8_t
	{
		NoError,			///< No Error occurred
		AddressNack,		///< Address was transmitted and NACK received
		DataNack,			///< Data was transmitted and NACK received
		ArbitrationLost,	///< Arbitration was lost during writing or reading
		BusCondition,		///< Misplaced Start or Stop condition
		Unknown				///< Unknown error condition
	};

	/// Baudrate of the I2C bus. Most slaves only work in Standard or Fast mode.
	struct Baudrate
	{
		enum
		{
			Standard = 100000,	///< Standard datarate of 100kHz
			Fast = 400000,		///< Fast datarate of 400kHz
			High = 1700000,		///< High datarate of 1.7MHz (rarely supported)
			Super = 3400000		///< Super datarate of 3.4MHz (rarely supported)
		};
	};

#ifdef __DOXYGEN__
public:
	/**
	 * Initializes the hardware and sets the datarate.
	 *
	 * @tparam	clockSource
	 * 		the currently active system clock
	 * @tparam	baudrate
	 * 		the desired baudrate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 */
	template< class clockSource, uint32_t baudrate=DataRate::Standard,
			Tolerance tolerance = Tolerance::FivePercent >
	static void
	initialize();

	/**
	 * Requests bus control and starts the transfer.
	 *
	 * @param	delegate
	 *		object that inherits from the I2cDelegate class.
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	start(I2cTransaction *delegate);

	/**
	 * Requests bus control and starts the transfer, blocks until delegate is detached.
	 *
	 * @param	delegate
	 *		object that inherits from the I2cDelegate class.
	 * @return	Caller gains control if `true`. Call has no effect if `false`.
	 */
	static bool
	startBlocking(I2cTransaction *delegate);

	/**
	 * Perform a software reset of the driver in case of an error.
	 *
	 * This method calls the stopped Delegate method and then detaches
	 * the delegate.
	 */
	static void
	reset(DetachCause cause = DetachCause::SoftwareReset);

	/**
	 * Check the error state of the driver.
	 *
	 * The error states are hardware and implementation specific.
	 */
	static Error
	getErrorState();
#endif
};

/**
 * Abstract class for delegation.
 *
 * For true asynchronous operation, the communication driver should
 * inherit from this class, allowing multi-stage driver design and
 * performance gain by premature data evaluation.
 * Be aware the methods may or may not be called during the I2C
 * interrupt even before returning from initiating function.
 * This Delegate will stay attached to the `I2cMaster` during whole operation.
 *
 * @see I2cMaster
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup	i2c
 */
class I2cTransaction : public I2c
{
public:
	/// Contains the information required to begin an I2C transfer
	struct Starting {
		uint8_t address;	///< the slave address excluding read/write bit
		Operation next;		///< operation following the successful addressing of the slave
	};

	/// Contains the information required to begin a write operation
	struct Writing {
		const uint8_t *buffer;		///< buffer containing the data to write to the slave
		std::size_t length;			///< number of bytes to be written
		OperationAfterWrite next;	///< operation following the successful writing to the slave
	};

	/// Contains the information required to begin a read operation
	struct Reading {
		uint8_t *buffer;			///< buffer to be filled with the bytes received from the slave
		std::size_t length;			///< number of bytes to be read
		OperationAfterRead next;	///< operation following the reading
	};

public:
	/**
	 * This method is called when the I2cMaster is not currently
	 * in use by another delegate and can be attached.
	 *
	 * @return	`true` if the I2cMaster should attach this delegate,
	 * 			`false` if it should not attach it.
	 */
	virtual bool
	attaching() = 0;

	/**
	 * This is called when the I2cMaster is ready to (re-)start an operation.
	 *
	 * @return	the `Starting` struct containing slave address and the next operation
	 */
	virtual void
	starting(Starting &starting) = 0;

	/**
	 * This is called before the I2cMaster begins a write operation.
	 *
	 * @return	the `Writing` struct containg the write buffer and size and next operation
	 */
	virtual void
	writing(Writing &writing) = 0;

	/**
	 * This is called before the I2cMaster begins a read operation.
	 *
	 * @return	the `Reading` struct containg the read buffer and size and next operation
	 */
	virtual void
	reading(Reading &reading) = 0;

	/**
	 * This is called when the I2cMaster stops the operation and detached the delegate.
	 *
	 * @param	cause	specifies whether the detachment was expected (`NormalStop`),
	 * 					or a error occurred (`ErrorCondition`), which can, but does not need
	 * 					to be reacted upon.
	 */
	virtual void
	detaching(DetachCause cause) = 0;
};

}	// namespace xpcc

#endif // XPCC_PERIPHERAL_I2C_MASTER_HPP
