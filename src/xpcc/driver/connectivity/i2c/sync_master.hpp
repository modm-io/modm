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
 * $Id: sync_master.hpp 695 2012-01-16 11:21:23Z georgi-g $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__SYNC_MASTER_HPP
#define XPCC_I2C__SYNC_MASTER_HPP

#include "master.hpp"

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{

		/**
		 * \brief	Optional Parameter for syncronous operation.
		 * \ingroup	i2c
		 */
		enum SyncParams
		{
			SYNC_NO_STOP = 0,		//!< BusyState stays occupied
			SYNC_STOP = 1,			//!< Bus is automatically given up
		};


		/**
		 * \brief	This is a wrapper class for i2c master which makes access to it synchronous.
		 * 			That means All methods do not return while master is busy. In addition read and write
		 * 			methods give the resulting bus state as a return value and are optionally able to
		 * 			give up bus control on return. That is the default behavior.
		 */
		template<typename M>
		class SyncMaster
		{

		public:
			/**
			 * \brief	Behaves like the master Start method, but does not
			 * 			return until operation is finished.
			 *
			 * 			Does not block if busyState is not FREE
			 *
			 * \param[in]	slaveAddress	Slave address.
			 * \param[out]	busState		BusState after completion.
			 *
			 * \return	True if control is gained
			 * 			False if no control is gained
			 */
			static bool
			start(uint8_t slaveAddress, BusState *busState);

			/**
			 * \brief	Behaves like the Start method, but performs an additional BusState check,
			 * 			so return values have different meaning. Use this method if you don't need to
			 * 			distinguish bus error and bus busy.
			 *
			 * \return	True if control is gained and the resulting state is not RESET
			 * 			false otherwise, that means  no control is gained or control was gained, but due to
			 * 			error in BusState it has been given up.
			 */
			static bool
			startCheck(uint8_t slaveAddress);

			/**
			 * \brief	Behaves like the Stop method but does not return until operation is finished.
			 */
			static void
			stop();

			/**
			 * \brief	Behaves like the read method, but does not return until operation is finished.
			 * 			If params == READ_RESTART, syncparams has no effect. Bus remains occupied.
			 *
			 * \return	Actual BusState as result to the Operation.
			 */
			static BusState
			read(uint8_t *data, std::size_t size, ReadParameter param = xpcc::i2c::READ_STOP, SyncParams syncParams = SYNC_STOP);

			/**
			 * \brief	Behaves like the write method, but does not return until operation is finished.
			 *
			 * \return	Actual BusState as result to the Operation.
			 */
			static BusState
			write(const uint8_t *data, std::size_t size, SyncParams syncParams = SYNC_STOP);

			/**
			 * \brief	Behaves like the restart method, but does not return until operation is finished.
			 *
			 * \return	Actual BusState as result to the Operation.
			 */
			static BusState
			restart(uint8_t slaveAddress);
			
		private:
			static BusyState
			wait();
		};
	}
}

#include "sync_master_impl.hpp"

#endif /* XPCC_I2C__SYNC_MASTER_HPP */
