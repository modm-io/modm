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

#ifndef XPCC_I2C__SYNC_MASTER_HPP
	#error	"Don't include this file directly, use 'sync_master.hpp' instead!"
#endif

template <typename M>
xpcc::i2c::BusyState
xpcc::i2c::SyncMaster<M>::wait()
{
	while (true)
	{
		BusyState b = M::getBusyState();
		if (b != xpcc::i2c::BUSY) {
			return b;
		}
	}
}

template <typename M>
bool
xpcc::i2c::SyncMaster<M>::start(uint8_t slaveAddress, BusState *busState)
{
	if (M::start(slaveAddress))
	{
		wait();
		*busState = M::getBusState();
		return true;
	}
	else {
		return false;
	}
}

template <typename M>
bool
xpcc::i2c::SyncMaster<M>::startCheck(uint8_t slaveAddress)
{
	BusState busState;
	if (start(slaveAddress, &busState))
	{
		if (busState != xpcc::i2c::BUS_RESET)
		{
			return true;
		}
		else{
			M::stop();
			wait();
		}
	}
	return false;
}

template <typename M>
void
xpcc::i2c::SyncMaster<M>::stop()
{
	M::stop();
	wait();
}

template <typename M>
xpcc::i2c::BusState
xpcc::i2c::SyncMaster<M>::read(uint8_t *data, std::size_t size, ReadParameter param, SyncParams syncParams)
{
	M::read(data, size, param);
	wait();
	
	xpcc::i2c::BusState b = M::getBusState();
	if (param == xpcc::i2c::READ_STOP && syncParams == SYNC_STOP) {
		M::stop();
		wait();
	}
	return b;
}

template <typename M>
xpcc::i2c::BusState
xpcc::i2c::SyncMaster<M>::write(const uint8_t *data, std::size_t size, SyncParams syncParams)
{
	M::write(data, size);
	wait();
	
	xpcc::i2c::BusState b = M::getBusState();
	if (syncParams == SYNC_STOP) {
		M::stop();
		wait();
	}
	return b;
}

template <typename M>
xpcc::i2c::BusState
xpcc::i2c::SyncMaster<M>::restart(uint8_t slaveAddress)
{
	M::restart(slaveAddress);
	wait();
	return M::getBusState();
}
