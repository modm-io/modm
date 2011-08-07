// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__INTERFACE_HPP
#define XPCC_I2C__INTERFACE_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/driver/interface.hpp>

#include <cstddef>

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{
		/**
		 * \brief	Generate an Acknowledge
		 * \ingroup	i2c
		 */
		static const bool ACK = true;
		
		/**
		 * \brief	Generate a No Acknowledge signal
		 * \ingroup	i2c
		 */
		static const bool NACK = false;
		
		/**
		 * \brief	Next operation is write-operation
		 * 
		 * To be used with start() and repeatedStart().
		 * \ingroup	i2c
		 */
		static const uint8_t WRITE = 0x00;
		
		/**
		 * \brief	Next operation is write-operation
		 * 
		 * To be used with start() and repeatedStart().
		 * \ingroup	i2c
		 */
		static const uint8_t READ = 0x01;
		
		/**
		 * \brief	Status of a asynchronous operation
		 * \ingroup	i2c	
		 */
		enum Status
		{
			SUCCESS = 0,	//!< Last operation was successful
			ERROR = 1,		//!< Error in last operation
		};
		
		/**
		 * \brief	Interface of a I2C master
		 * 
		 * This class defines the public interface for a synchronous master.
		 * This means that every transaction is blocking and have to be
		 * finished before the functions return.
		 * 
		 * Example:
		 * \code
		 * SynchronousMaster master;
		 * 
		 * if (master.start(0xE2 | xpcc::i2c::WRITE))
		 * {
		 *     master.write(0xAB);
		 *     master.write(0xAB);
		 * }
		 * master.stop();
		 * \endcode
		 * 
		 * \ingroup	interface
		 */
		class SynchronousMaster : public Interface
		{
		public:
			/**
			 * \brief	Start transmission	
			 * 
			 * Generates a start condition and sends the slave-address. Remember
			 * to call restartTransmisison() or stop() afterwards.
			 * 
			 * \param	address	7-bit address of the slave device. LSB defines
			 * 					weather it is a write or read operation. Use
			 * 					xpcc::i2c::READ or xpcc::i2c::WRITE to specify it.
			 * \return	\c true if the slave responds and is accessible,
			 * 			\c false otherwise.
			 * 
			 * Example:
			 * \code
			 * start(0xE2 | xpcc::i2c::WRITE);
			 * \endcode
			 */
			bool
			start(uint8_t address);
			
			/**
			 * \brief	Repeated start
			 * 
			 * Generates a repeated start condition and sends the
			 * slave-address. Remember to call stop() afterwards.
			 * 	
			 * \param	address	7-bit address of the slave device. LSB defines
			 * 					weather it is a write or read operation. Use
			 * 					xpcc::i2c::READ or xpcc::i2c::WRITE to specify it.
			 * \return	\c true if the slave responds and is accessible,
			 * 			\c false otherwise.  
			 */
			bool
			repeatedStart(uint8_t address);
			
			/**
			 * \brief	Read a byte
			 * 
			 * \param	ack		Send an acknowledge signal (\c true or
			 * 					xpcc::i2c::ACK) to indicate that more data
			 * 					should be read. Use \c false or xpcc::i2c::NACK
			 * 					if it is the last byte.
			 * \return	Received byte
			 */
			uint8_t
			read(bool ack);
			
			/**
			 * \brief	Write a byte
			 * 
			 * \param	data	Data to be transmitted.
			 * \return	\c true if slave device generates an acknowledge,
			 * 			\c false otherwise.
			 */
			bool
			write(uint8_t data);
			
			/**
			 * \brief	End transmission
			 * 
			 * Generates a stop condition. Must always be called after
			 * a start() or repeatedStart() even if the
			 * device is not accessible.
			 */
			void
			stop();
		};
		
		/**
		 * \brief	Interface for a I2C master with asynchronous transfers
		 * 
		 * A master with asynchronous transfers operates in the background
		 * until the operation is finished. This allows the use of DMA etc. and
		 * enables the program to do other things while the operation is
		 * going on.
		 * 
		 * \ingroup	interface
		 */
		class AsynchronousMaster : public Interface
		{
		public:
			/**
			 * \brief	Read a block of data
			 * 
			 * \param[in] address	7-bit address of the slave device.
			 * 					LSB must be zero.
			 * \param[out] *data	Pointer to buffer where received data
			 * 					should be stored 
			 * \param[in] size		Number of bytes to be received
			 */
			void
			read(uint8_t address, uint8_t *data, std::size_t size);
			
			/**
			 * \brief	Write a block of data
			 * 
			 * \param	address	7-bit address of the slave device. LSB must
			 * 					be zero.
			 * \param	data	Pointer to buffer containing data to send
			 * \param	size	Number of byte to be send
			 */
			void
			write(uint8_t address, const uint8_t *data, std::size_t size);
			
			/**
			 * \brief	Perform a combined write-/read-operation
			 * 
			 * First \c writeSize bytes of \c *data are written to the slave,
			 * then \c readSize bytes are read back into \c *data. 
			 * 
			 * \param	address		7-bit address of the slave device.
			 * 						LSB must be zero.
			 * \param[in,out] *data	Buffer for sending/receiving 		
			 * \param	writeSize	Number of bytes to write
			 * \param	readSize	Number of bytes to read afterwards
			 */
			void
			writeRead(uint8_t address, uint8_t *data,
					std::size_t writeSize, std::size_t readSize);
			
			/**
			 * \brief	Check if currently an operation is pending
			 * 
			 * A new operation must not be issued until this method
			 * returns \c false.
			 * 
			 * \return	\c true if any operation (transmission or reception) 
			 * 			is pending, \c false if the device is ready to receive
			 * 			a new command.
			 */
			bool
			isBusy();
			
			/**
			 * \brief	Check the outcome  
			 * 
			 * This message should only be called after isBusy() returned
			 * \c false.
			 * 
			 * \return	xpcc::i2c::SUCCESS if the last operation was successful,
			 * 			xpcc::i2c::ERROR if any error occurred.	 
			 */
			Status
			getStatus();
		};
		
		/**
		 * \brief	
		 * \tparam	T	SynchronousMaster	
		 */
		template <typename T>
		class AsynchronousInterface : public AsynchronousMaster
		{
		public:
			static void
			read(uint8_t address, uint8_t *data, std::size_t size);
			
			static void
			write(uint8_t address, const uint8_t *data, std::size_t size);
			
			static void
			writeRead(uint8_t address, uint8_t *data,
					std::size_t writeSize, std::size_t readSize);
			
			static inline bool
			isBusy();
			
			static inline Status
			getStatus();
			
		private:
			static bool error;
			static T interface;
		};
	}
}

#include "interface_impl.hpp"

#endif // XPCC_I2C__INTERFACE_HPP
