// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_I2C_HPP
#define XPCC_INTERFACE_I2C_HPP

#include <xpcc/architecture/interface.hpp>
#include <xpcc/architecture/driver/delay.hpp>

/**
 * @ingroup		interface
 * @defgroup	i2c		Inter-Integrated Circuit (I2C)
 */

namespace xpcc
{

/// @ingroup	i2c
struct I2c
{
	static constexpr uint8_t Write = 0x00;	///< Add the Write bit to the slave address
	static constexpr uint8_t Read  = 0x01;	///< Add the Read bit to the slave address

	/// The signature of the configuration function.
	using Configuration_t = void(*)();

	/// This tells the `I2cTransaction` why it was detached
	/// @see I2cTransaction
	enum class
	DetachCause : uint8_t
	{
		NormalStop,		///< All operations finished normally
		ErrorCondition,	///< A bus error occurred and the bus was reset
		FailedToAttach	///< The I2cTransaction failed to attach to the I2cMaster
	};

	///@{
	/// I2C Operations that the `I2cTransaction` can give the master on callback
	/// @see I2cTransaction

	/// All possible I2C Operations.
	enum class
	Operation : uint8_t
	{
		Stop    = 0,	///< Generate a Stop Condition
		Restart = 1,	///< Generate a Restart
		Write   = 2,	///< Write data to the slave
		Read    = 3,	///< Read data from the slave
	};

	/// Further operations after start operation.
	enum class
	OperationAfterStart : uint8_t
	{
		Stop  = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Write = uint8_t(Operation::Write),		///< Write data to the slave
		Read  = uint8_t(Operation::Read),		///< Read data from the slave
	};

	/// Further operations after write operation.
	enum class
	OperationAfterWrite : uint8_t
	{
		Stop    = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Restart = uint8_t(Operation::Restart),	///< Generate a Restart
		Write   = uint8_t(Operation::Write),	///< Write data to the slave
	};

	/// Further operations after read operation.
	enum class
	OperationAfterRead : uint8_t
	{
		Stop    = uint8_t(Operation::Stop),		///< Generate a Stop Condition
		Restart = uint8_t(Operation::Restart),	///< Generate a Restart
	};
	///@}

	/// State of a Transaction Adapter
	enum class
	AdapterState : uint8_t
	{
		Idle,	///< No error occurred, detached normally
		Busy,	///< The adapter is busy with data transfer
		Error	///< An error occurred, check the masters `getErrorCode()`
	};

	/**
	 * Reset all slave devices connected to an I2C bus.
	 *
	 * During normal operation, I2C slave device may pull the SDA line low.
	 * However, if the master is resetted during a transaction, the I2C clock
	 * may stop while the slave is outputting a low data bit and the slave will
	 * continue to hold this bit (forever, and ever and ever).
	 * The I2C master is then unable to generate a I2C start condition, since SDA
	 * is still held low by the slave device, resulting in a deadlock.
	 *
	 * "You can always get it back to standby mode by allowing the SDA line to
	 * float high and give it 9 clocks.
	 * This assures that the device will not receive the acknowledge bit at
	 * the end the current byte and will abort the command and go to standby."
	 *
	 * @see	Application Note AN572 by Microchip
	 * @warning	Must be called **after** connecting SDA and SCL to I2cMaster!
	 * @warning	The clock frequency is hardcoded to 50kHz, so this function blocks for 180µs.
	 *
	 * @tparam	Scl		The I2C clock pin of the bus to be reset.
 	 */
	template< class Scl >
	void
	resetDevices()
	{
		Scl::reset();
		for (uint_fast8_t ii = 0; ii < 9*2; ++ii) {
			Scl::toggle();
			xpcc::delayMicroseconds(10);
		}
		Scl::set();
	}
};

}	// namespace xpcc

#endif // XPCC_INTERFACE_I2C_HPP
