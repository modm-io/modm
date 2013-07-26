// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL__I2C_MASTER_HPP
#define XPCC_PERIPHERAL__I2C_MASTER_HPP

#include "../peripheral.hpp"

namespace xpcc
{
	struct I2c
	{
		static constexpr uint8_t WRITE = 0x00;	//!< Add the Write bit to the slave addres
		static constexpr uint8_t READ = 0x01;	//!< Add the Read bit to the slave address

		/// This tells the \c xpcc::i2c::Delegate why it was detached
		enum class DetachCause : uint8_t
		{
			NormalStop,		//!< All operations finished normally
			ErrorCondition,	//!< A bus error occured and the bus was reset
			SoftwareReset	//!< The master in initializing itself
		};

		/// Operations after a start or restart condition.
		/// @see \c xpcc::I2cDelegate
		enum class Operation
		{
			Stop = 0,		//!< Generate a Stop Condition
			Restart = 1,	//!< Generate a Restart
			Write = 2,		//!< Write data to the slave
			Read = 3,		//!< Read data from the slave
		};

		/// Further operations after write operation.
		enum class OperationAfterWrite
		{
			Stop = 0,
			Restart = 1,
			Write = 2,
		};

		/// Further operations after read operation.
		enum class OperationAfterRead
		{
			Stop = 0,
			Restart = 1,
		};

		/// State of a Delegate Adapter
		enum class AdapterState
		{
			NoError,
			Busy,
			Error
		};
	};

	/**
	 * \brief	Interface of a I2C master
	 *
	 * Performing transfers in background allows the use of DMA etc. and
	 * enables the program to do other things while the operation is
	 * going on.
	 *
	 * \author	Georgi Grinshpun
	 * \author	Niklas Hauser
	 * \ingroup	peripheral
	 * \ingroup i2c
	 */
	class I2cMaster : public ::xpcc::Peripheral, I2c
	{
		/// Errors that can happen during master operation
		enum class Error : uint8_t
		{
			NoError,			//!< No Error occurred
			AddressNack,		//!< Address was transmitted and NACK received
			DataNack,			//!< Data was transmitted and NACK received
			ArbitrationLost,	//!< Arbitration was lost during writing or reading
			BusCondition,		//!< Misplaced Start or Stop condition
			Unknown				//!< Unknown error condition
		};

		/// Baudrate of the I2C bus. Most slaves only work in Standard or Fast mode.
		enum class DataRate : uint32_t
		{
			Standard = 100000,	//!< Standard datarate of 100kHz
			Fast = 400000,		//!< Fast datarate of 400kHz
			High = 1700000		//!< High datarate of 1.7Mhz
		};

#ifdef __DOXYGEN__
	public:
		/**
		 * \brief	Requests bus control and starts the transfer.
		 *
		 * \param	delegate	object that inherits from the I2cDelegate class.
		 * \return	Caller gains control if \c true. Call has no effect if \c false.
		 */
		static bool
		start(I2cDelegate *delegate);

		/**
		 * \brief	Requests bus control and starts the transfer.
		 *			Blocks until delegate is detached.
		 *
		 * \param	delegate	object that inherits from the I2cDelegate class.
		 * \return	Caller gains control if \c true. Call has no effect if \c false.
		 */
		static bool
		startSync(I2cDelegate *delegate);

		/**
		 * \brief	Perform a software reset of the driver in case of an error.
		 *
		 * This method calls the stopped Delegate method and then detaches
		 * the delegate.
		 */
		static void
		reset(DetachCause cause=DetachCause::SoftwareReset);

		/**
		 * \brief	Check the error state of the driver.
		 *
		 * Since the error states are hardware and implementation specific,
		 * this is only the recommended interface and does not need to be implemented.
		 */
		static Error
		getErrorState();
#endif
	};

	/**
	 * \brief	Abstract class for delegation
	 *
	 * For true asynchronous operation, the communication driver should
	 * inherit from this class, allowing multistage driver design and
	 * performance gain by premature data evaluation.
	 * Be aware the methods may or may not be called during the I2C
	 * interrupt even before returning from initiating function.
	 * This Delegate will stay attached to I2C during whole operation.
	 *
	 * \author	Georgi Grinshpun
	 * \author	Niklas Hauser
	 * \ingroup	i2c
	 */
	class I2cDelegate : public I2c
	{
	public:
		struct Starting {
			Operation next;
			uint8_t address;
		};

		struct Reading {
			OperationAfterRead next;
			uint8_t *buffer;
			std::size_t size;
		};

		struct Writing {
			OperationAfterWrite next;
			const uint8_t *buffer;
			std::size_t size;
		};

		/*
		 * Is called one time while attaching signaling this module
		 * that it is being attached to the driver.
		 * \return if this delegate is able to be attached. Returning
		 * false the attaching will not be performed. After returning
		 * true, this delegate will be attached and stays so until stopped
		 * is called.
		 */
		virtual bool
		attaching() = 0;

		/*
		 * Is called at least one time after attaching returned true. Will
		 * also be called while restart.
		 */
		virtual Starting
		starting() = 0;

		/**
		 * Before a read operation starts is called to retrieve the buffer
		 * where incoming data to place to.
		 */
		virtual Reading
		reading() = 0;

		/**
		 * Before a write operation starts is called to retrieve buffer
		 * which contains the data to write.
		 */
		virtual Writing
		writing() = 0;

		/**
		 * Is called just before (or just after) this delegate is detached from i2c.
		 */
		virtual void
		stopped(DetachCause cause) = 0;
	};
}

#endif // XPCC_PERIPHERAL__I2C_MASTER_HPP
