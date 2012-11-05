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

#include "mock_master.hpp"

xpcc::i2c::MockMaster::Behavior xpcc::i2c::MockMaster::behavior = xpcc::i2c::MockMaster::AUTO_MODE;
xpcc::i2c::MockMaster::SlaveDevice xpcc::i2c::MockMaster::slaveDevice = xpcc::i2c::MockMaster::DEVICE_PRESENT;
xpcc::i2c::Delegate *xpcc::i2c::MockMaster::delegate;
xpcc::i2c::MockMaster::ErrorState xpcc::i2c::MockMaster::error;
xpcc::i2c::Delegate::NextOperation xpcc::i2c::MockMaster::nextOperation;

void
xpcc::i2c::MockMaster::initialize(SlaveDevice slaveDevice, Behavior behavior)
{
	MockMaster::slaveDevice = slaveDevice;
	MockMaster::behavior = behavior;
}

uint8_t
xpcc::i2c::MockMaster::getErrorState()
{
	return error;
}

bool
xpcc::i2c::MockMaster::start(Delegate *delegate)
{
	if (!MockMaster::delegate && delegate && delegate->attaching()){
		MockMaster::delegate = delegate;
		nextOperation = Delegate::RESTART_OP;

		if (behavior == AUTO_MODE){
			while (MockMaster::delegate)
				update();
		}

		return true;
	}
	else
		return false;
}

void xpcc::i2c::MockMaster::update(){
	if (delegate)
	{
		switch (nextOperation)
		{
			case Delegate::STOP_OP:
				delegate->stopped(Delegate::NORMAL_STOP);
				delegate = 0;
				break;
				
			case Delegate::RESTART_OP:
				if (slaveDevice != DEVICE_MISSING){
					nextOperation = delegate->starting().next;
				}
				else{
					delegate->starting();
					error = ADDRESS_NACK;
					delegate->stopped(Delegate::ERROR_CONDITION);
					delegate = 0;
				}
				break;
				
			case Delegate::READ_OP:
				{
					Delegate::Reading r = delegate->reading();
					if (r.buffer && r.size){
						nextOperation = (Delegate::NextOperation)r.next;
					}
					else{
						error = UNKNOWN_ERROR;
						delegate->stopped(Delegate::ERROR_CONDITION);
						delegate = 0;
					}
				}
				break;
				
			case Delegate::WRITE_OP:
			{
				Delegate::Writing w = delegate->writing();
				if (w.buffer && w.size){
					nextOperation = (Delegate::NextOperation)w.next;
				}
				else {
					error = UNKNOWN_ERROR;
					delegate->stopped(Delegate::ERROR_CONDITION);
					delegate = 0;
				}
			}
			break;
		}
	}
}
