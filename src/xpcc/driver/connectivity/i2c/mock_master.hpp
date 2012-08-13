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
 * $$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__MOCK_MASTER_HPP
#define XPCC_I2C__MOCK_MASTER_HPP

#include "master.hpp"

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{

		class MockMaster : public Master
		{
		public:
			enum ErrorState
			{
				NO_ERROR,			//!< No Error occurred
				DATA_NACK,			//!< Data was transmitted and NACK received
				ADDRESS_NACK,		//!< Data was transmitted and NACK received
				ARBITRATION_LOST,	//!< Arbitration was lost during writing or reading
				BUS_ERROR,			//!< Misplaced Start or Stop condition
				UNKNOWN_ERROR		//!< Unknown error condition
			};
			
			enum SlaveDevice
			{
				DEVICE_PRESENT,
				DEVICE_MISSING,
			};
			enum Behavior
			{
				AUTO_MODE,
				STEP_MODE,
			};

			static void
			initialize(SlaveDevice slaveDevice, Behavior behavior);

			static bool
			start(Delegate *delegate);
			
			static bool
			startSync(Delegate *delegate)
			{
				return start(delegate);
			}

			static uint8_t
			getErrorState();

			static void
			update();
			
		private:
			static Behavior behavior;
			static SlaveDevice slaveDevice;
			static ErrorState error;
			static Delegate *delegate;
			static Delegate::NextOperation nextOperation;
		};
	}
}

#endif	// XPCC_I2C__MOCK_MASTER_HPP
