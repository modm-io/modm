// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: master.hpp 747 2012-02-29 13:49:00Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__MASTER_HPP
#define XPCC_I2C__MASTER_HPP

#include <cstddef>
#include <stdint.h>

#include <xpcc/architecture/utils.hpp>
#include <xpcc/driver/interface.hpp>
#include "constants.hpp"
#include "delegate.hpp"

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{
		/**
		 * \brief	Interface of a I2C master
		 * 
		 * The operations are potentially asynchronous and can return just after the call.
		 * This behavior is implementation dependent. Check always BusyState for completion.
		 * 
		 * Performing transfers in background allows the use of DMA etc. and
		 * enables the program to do other things while the operation is
		 * going on.
		 * 
		 * Implementation hint: Every asynchronous functionality must be performed
		 * in an interrupt, no work or update methods in main loop are permitted,
		 * because a device driver (using this i2c::Master) may wait just after
		 * a method call for BusyState become not Busy.
		 *
		 * All methods but start and getBusyState may only be called by owner and only if BusyState is occupied
		 * An error is indicated by the BusState reading BUS_RESET.
		 * During asychnonous operation BusyState indicated BUSY.
		 *
		 * After start returned false bus control is not gained by caller.
		 * No operation has been started, try again later.
		 *
		 * After start returned true bus control is gained by caller.
		 * BusyState transitions to OCCUPIED.
		 * BusState transitions to BUS_STANDBY.
		 * The SCL line will be streched until further operation begins!
		 *
		 * After restart, write, read and writeRead
		 * BusyState remains OCCUPIED.
		 * BusState remains BUS_STANDBY.
		 *
		 * After write, read and writeRead complete asynchronously
		 * BusyState transitions to FREE.
		 * BusState transitions to BUS_HOLD or BUS_WRITE.
		 * BusState transitions to BUS_STOPPED, if chosen during setup.
		 *
		 * After stop
		 * BusyState transitions to FREE.
		 * BusState transitions to BUS_STOPPED.
		 *
		 * \author Georgi Grinshpun
		 * \author Niklas Hauser
		 * \ingroup	interface
		 */
		class Master : public Interface
		{
		public:
			/**
			 * \brief	Performed a software reset of bus driver.
			 */
			void
			reset();
			
			/**
			 * \brief	Requests bus control and generates a start condition.
			 *
			 * \param[in] slaveAddress	Shifted 7-bit address of the slave device.
			 *							LSB must be zero.
			 *
			 * \return	Caller gains control if \c true. Call has no effect if \c false.
			 */
			bool
			start(uint8_t slaveAddress);

			/**
			 * \brief	Restarts the bus without loosing control. Multiple calls
			 *			will result in generating only one start condition.
			 * 
			 * \param[in] slaveAddress	Shifted 7-bit address of the slave device.
			 * 							LSB must be zero.
			 */
			void
			restart(uint8_t slaveAddress);

			/**
			 * \brief	Read a block of data. Call this method only if BusState allows reading.
			 *
			 * \param[out] *data	Pointer to buffer where received data should be stored.
			 * 						This must remain valid during operation is in progress.
			 * \param[in] size		Number of bytes to be received
			 * \param[in] param		See description OperationParameter. Default is READ_STOP.
			 *
			 */
			void
			read(uint8_t *data, std::size_t size, OperationParameter param = READ_STOP);
			
			/**
			 * \brief	Write a block of data. Call this method if BusState allows writing.
			 * 
			 * \param	data	Pointer to buffer containing data to send. This must remain
			 * 					valid during operation is in progress.
			 * \param	size	Number of bytes to be send
			 * \param[in] param		See description OperationParameter. Default is WRITE_STOP.
			 */
			void
			write(uint8_t *data, std::size_t size, OperationParameter param = WRITE_STOP);
			
			/**
			 * \brief	Write and read blocks of data. Call this method if BusState allows writing.
			 * 
			 * \param	data	Pointer to the buffer containing data to be send
			 *					and where received data should be stored. This
			 *					must remain valid while the operation is in progress.
			 *					The data sent will be (partially) overwritten by the data received!
			 *					The Delegate will not be called after the write operation completed,
			 *					but only after the receive operation completed.
			 *					
			 * \param	writeSize	Number of bytes to be send
			 * \param	readSize	Number of bytes to be received
			 * \param[in] param		See description OperationParameter. Default is READ_STOP.
			 */
			void
			writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, OperationParameter param = READ_STOP);
			
			/**
			 * \brief	Attach a delegate, which is called in the interrupt at
			 *			the (un-)successful completion of an action (read or
			 *			write) or after a restart.
			 *			This allows for true asynchonous operation, multistage
			 *			driver design and performance gain by premature data
			 *			evaluation.
			 *			The delegate gets reset after it has been called,
			 *			meaning it has to be (re-)attached everytime a new
			 *			action is being committed.
			 * 
			 * \param	object	class that inherits from Delegate
			 */
			void
			attachDelegate(Delegate *object);
			
			/**
			 * \brief	Check current ErrorState.
			 */
			ErrorState
			getErrorState();
			
			/**
			 * \brief	Check current BusyState.
			 */
			BusyState
			getBusyState();
			
			/**
			 * \brief	Check the BusState after last operation.
			 * 
			 * This message should only be called by owner while BusyState is occupied.
			 * In other cases returned value is not relevant.
			 * 
			 * \return	BusState if BusyState is occupied
			 */
			BusState
			getBusState();
			
			/**
			 * \brief	writes a block of data, if bus state allows.
			 *
			 * \code
			 * if (start(slaveAddress)) {
			 *	   write(data, size, param);
			 *	   return true;
			 * }
			 * return false;
			 * \endcode
			 *
			 * \see		start()
			 * \see		write()
			 */
			bool
			startWrite(uint8_t slaveAddress, uint8_t *data, std::size_t size, OperationParameter param = WRITE_STOP);
			
			/**
			 * \brief	Read a block of data, if bus state allows.
			 *
			 * \code
			 * if (start(slaveAddress, xpcc::i2c::READ)) {
			 *	   read(data, size, param);
			 *	   return true;
			 * }
			 * return false;
			 * \endcode
			 *
			 * \see		start()
			 * \see		read()
			 */
			bool
			startRead(uint8_t slaveAddress, uint8_t *data, std::size_t size, OperationParameter param = READ_STOP);
			
			/**
			 * \brief	Write and read blocks of data, if bus state allows.
			 *
			 * \code
			 * if (start(slaveAddress)) {
			 *	   writeRead(data, writeSize, readSize, param);
			 *	   return true;
			 * }
			 * return false;
			 * \endcode
			 *
			 * \see		start()
			 * \see		writeRead()
			 */
			bool
			startWriteRead(uint8_t slaveAddress, uint8_t *data, std::size_t writeSize, std::size_t readSize, OperationParameter param = READ_STOP);
		};
	}
}

#endif // XPCC_I2C__MASTER_HPP
