/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "i2c_master_base.hpp"
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <algorithm>

// The state machine follows the peripheral models and was stress tested on hardware
// with random interrupt latency. Rules that came out of it, each one a failure that was
// measured without it:
//  - every buffer is fetched before its START, so its length and the operation after
//    it decide the read method (EV6_1, POS, method 2) up front,
//  - a read of length 0 is refused: before the START if it is the first segment, else
//    the bus still ends with a STOP and the transfer fails with Error::Unknown,
//  - the transaction is detached when its STOP is requested.

namespace modm::platform
{

bool
I2cMasterBase::start(I2cTransaction *t, ConfigurationHandler handler)
{
	if (not t) return false;
	modm::atomic::Lock lock;
	if (not attach(t, handler)) return false;
	begin();
	return true;
}

void
I2cMasterBase::reset()
{
	I2C_TypeDef *const r = regs();
	bool afterRead;
	{
		modm::atomic::Lock lock;
		afterRead = active and reading;
		if (active)
		{
			// A STOP after the current byte, with the interrupts off: a device cut off in
			// the middle of a byte may not reset on START or STOP and take the next
			// transfer's bits as data (measured with a CS43L22, which then wrote them
			// into its registers).
			r->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
			r->CR1 = (r->CR1 & ~(I2C_CR1_ACK | I2C_CR1_POS | I2C_CR1_START)) | I2C_CR1_STOP;
		}
	}
	// at most a few bytes at 10 kHz
	for (int us = 0; us < 4000 and busBusy(); us += 10)
	{
		if (r->SR1 & I2C_SR1_RXNE) (void) r->DR;	// a full receiver holds SCL
		modm::delay_us(10);
	}
	const bool stuck = busBusy();
	{
		modm::atomic::Lock lock;
		go(state, DEADLINE, Action::Reset, State::Idle);
		hwReset();
		detachAll();
	}
	// a device still holds the bus: clear it, outside the lock
	if (stuck and recoverBus) { recoverBus(afterRead); hwReset(); }
}

bool
I2cMasterBase::busBusy() const
{ return regs()->SR2 & I2C_SR2_BUSY; }

// ends the transfer
void
I2cMasterBase::detach(DetachCause cause)
{
	finish(cause);
}

void
I2cMasterBase::unexpected(uint8_t flag)
{
	go(state, flag, Action::Unexpected, State::Idle);
	hwReset();
	error = Error::Unknown;
	detach(DetachCause::ErrorCondition);
}


void
I2cMasterBase::configure(uint8_t peripheralFrequencyMHz, uint8_t riseTime, uint16_t clockControl)
{
	freq = peripheralFrequencyMHz;
	trise = riseTime;
	ccr = clockControl;
	hwReset();
}

// SWRST, then CR2/CCR/TRISE again: the reset clears them
void
I2cMasterBase::hwReset()
{
	regs()->CR1 = I2C_CR1_SWRST;
	regs()->CR1 = 0;
	regs()->CR2 = freq;
	regs()->CCR = ccr;
	regs()->TRISE = trise;
	regs()->CR1 = I2C_CR1_PE;
	state = State::Idle;
}

void
I2cMasterBase::end(bool stop)
{ regs()->CR1 |= stop ? I2C_CR1_STOP : I2C_CR1_START; }

void
I2cMasterBase::buffer(bool on)
{
	if (on) regs()->CR2 |= I2C_CR2_ITBUFEN;
	else regs()->CR2 &= ~I2C_CR2_ITBUFEN;
}

void
I2cMasterBase::isr()
{
	I2C_TypeDef *const r = regs();
	const uint32_t sr1 = r->SR1;
	const bool buf = r->CR2 & I2C_CR2_ITBUFEN;
	const State before = state;
	const bool last = next == Operation::Stop;
	const uint8_t sr = last ? 0 : 1;

	if (buf and (sr1 & I2C_SR1_RXNE))
	{
		if (state == State::Stopping)
		{
			// a byte after the last one, the transaction is already detached
			(void) r->DR;
			buffer(false);
			return go(before, RXNE, Action::Extra, State::Stopping);
		}
		if (not active) { (void) r->DR; return; }
		if (state == State::RxEnd)
		{
			const uint8_t b = r->DR;
			if (index < length) rbuf[index++] = b;
			buffer(last);
			go(before, RXNE, Action(uint8_t(Action::ReadLastP) + sr), last ? State::Stopping : State::SbWait);
			return finishSegment();
		}
		if (state != State::Rx) return unexpected(RXNE);
		const size_t remaining = length - index;
		if (remaining > 3) { rbuf[index++] = r->DR; return go(before, RXNE, Action::Read, State::Rx); }
		if (remaining == 3) { buffer(false); return go(before, RXNE, Action::Wait, State::Rx); }
		return unexpected(RXNE);
	}
	if (sr1 & I2C_SR1_SB)
	{
		if (state != State::SbWait) return unexpected(SB);
		r->DR = address | (reading ? 1 : 0);
		if (reading and length > 1) r->CR1 |= I2C_CR1_ACK;
		return go(before, SB, Action::Address, State::Addr);
	}
	if (sr1 & I2C_SR1_ADDR)
	{
		if (state != State::Addr) return unexpected(ADDR);
		if (reading and length == 1)
		{
			// EV6_1: ACK = 0 before the ADDR clear, STOP/START right after it, with
			// interrupts off, so that it is set before the byte ends
			{
				modm::atomic::Lock lock;
				r->CR1 &= ~I2C_CR1_ACK;
				(void) r->SR1; (void) r->SR2;
				end(last);
			}
			buffer(true);
			return go(before, ADDR, Action(uint8_t(Action::ClearRx1P) + sr), State::RxEnd);
		}
		if (reading and length == 2)
		{
			// POS method for two bytes: POS = 1 makes ACK act on the next byte, ACK = 0
			// right after the ADDR clear while SCL is stretched, so the first byte is
			// acknowledged and the second NACKed whatever the interrupt latency, then BTF
			// holds both. Method 1 (ACK clear after the first RXNE) is not latency proof.
			{
				modm::atomic::Lock lock;
				r->CR1 |= I2C_CR1_POS | I2C_CR1_ACK;
				(void) r->SR1; (void) r->SR2;
				r->CR1 &= ~I2C_CR1_ACK;
			}
			buffer(false);
			return go(before, ADDR, Action::ClearRxPos, State::RxPos);
		}
		(void) r->SR1; (void) r->SR2;
		if (reading) { buffer(true); return go(before, ADDR, Action::ClearRx, State::Rx); }
		if (not length and not more())
		{
			// a ping, or only empty write buffers
			const bool stop = next == Operation::Stop;
			end(stop); buffer(false);
			go(before, ADDR, Action(uint8_t(Action::ClearPingP) + (stop ? 0 : 1)), stop ? State::Stopping : State::SbWait);
			return finishSegment();
		}
		r->DR = wbuf[index++];
		buffer(index < length or next == Operation::Write);
		return go(before, ADDR, Action::ClearTx, State::Tx);
	}
	if (sr1 & I2C_SR1_AF)
	{
		// the first error of the transfer stays (a refused read ends with a write ping)
		if (error == Error::NoError) error = (state == State::Addr) ? Error::AddressNack : Error::DataNack;
		r->SR1 = ~I2C_SR1_AF;
		r->CR1 |= I2C_CR1_STOP;
		buffer(false);
		go(before, AF, Action::Nack, State::Stopping);
		return detach(DetachCause::ErrorCondition);
	}
	if (sr1 & I2C_SR1_BERR)
	{
		error = Error::BusCondition;
		r->SR1 = ~I2C_SR1_BERR;
		return go(before, BERR, Action::Berr, before);
	}
	if (sr1 & I2C_SR1_ARLO)
	{
		r->SR1 = ~I2C_SR1_ARLO;
		buffer(false);
		go(before, ARLO, Action::Arlo, State::BusyOther);
		error = Error::ArbitrationLost;
		return detach(DetachCause::ErrorCondition);
	}
	// BTF stays set after STOP/START is programmed at BTF until the condition is on the
	// bus, it is stale in SbWait and Stopping. The event interrupt comes again and again
	// meanwhile, so wait for the condition here: a START or STOP that never goes out (a
	// disturbed bus) would starve everything below this interrupt, the timeout that calls
	// reset() too.
	if ((sr1 & I2C_SR1_BTF) and (state == State::SbWait or state == State::Stopping))
	{
		constexpr uint32_t pending = I2C_CR1_START | I2C_CR1_STOP;
		for (int us = 0; us < 1000 and (r->CR1 & pending); us++) modm::delay_us(1);
		if (not (r->CR1 & pending)) return;
		go(before, BTF, Action::Reset, State::Idle);
		hwReset();
		if (busBusy() and recoverBus) { recoverBus(false); hwReset(); }
		if (not active) return;
		error = Error::BusCondition;
		return detach(DetachCause::ErrorCondition);
	}
	if ((sr1 & I2C_SR1_BTF) and active)
	{
		if (state == State::RxPos)
		{
			// POS: both bytes in, the second NACKed. POS is cleared in the same write that
			// sets STOP/START: a CR1 read-modify-write after the DR reads can land after the
			// hardware cleared START/STOP and set it again (a second START with no address,
			// or a second STOP that breaks the next START).
			r->CR1 = (r->CR1 & ~I2C_CR1_POS) | (last ? I2C_CR1_STOP : I2C_CR1_START);
			rbuf[index++] = r->DR;
			rbuf[index++] = r->DR;
			go(before, BTF, Action(uint8_t(Action::BtfPosP) + sr), last ? State::Stopping : State::SbWait);
			return finishSegment();
		}
		if (state == State::Rx)
		{
			// method 2: three bytes left, DataN-2 in DR, DataN-1 in the shift register
			if (length - index != 3) return unexpected(BTF);
			{
				modm::atomic::Lock lock;
				r->CR1 &= ~I2C_CR1_ACK;
				rbuf[index++] = r->DR;
				end(last);
				rbuf[index++] = r->DR;
			}
			buffer(true);
			return go(before, BTF, Action(uint8_t(Action::BtfEndP) + sr), State::RxEnd);
		}
		if (state != State::Tx) return unexpected(BTF);
		if (index < length or more())
		{
			r->DR = wbuf[index++];
			buffer(index < length or next == Operation::Write);
			return go(before, BTF, Action::Write, State::Tx);
		}
		// more() may have fetched buffers: the operation after this one is `next` now
		const bool stop = next == Operation::Stop;
		end(stop); buffer(false);
		go(before, BTF, Action(uint8_t(Action::EndTxP) + (stop ? 0 : 1)), stop ? State::Stopping : State::SbWait);
		return finishSegment();
	}
	if (buf and (sr1 & I2C_SR1_TXE) and state == State::Tx)
	{
		if (index >= length and not more()) { buffer(false); return; }	// wait for BTF
		r->DR = wbuf[index++];
		buffer(index < length or next == Operation::Write);
		return go(before, TXE, Action::Write, State::Tx);
	}
}

void
I2cMasterBase::begin()
{
	I2C_TypeDef *const r = regs();
	active = true;
	error = Error::NoError;
	if (state == State::Stopping)
	{
		// the previous STOP first: a START queued behind a STOP can break (ES096 2.8.4)
		for (int us = 0; us < 200 and (r->CR1 & I2C_CR1_STOP); us++) modm::delay_us(1);
		go(State::Stopping, POLL, Action::None, State::Idle);
	}
	if (r->SR2 & I2C_SR2_BUSY)
	{
		for (int us = 0; us < 1000 and (r->SR2 & I2C_SR2_BUSY); us++) modm::delay_us(1);
		if (r->SR2 & I2C_SR2_BUSY)
		{
			// SWRST does not release a device that holds SDA: the bus clear (busy waits)
			hwReset();
			if ((r->SR2 & I2C_SR2_BUSY) and recoverBus) { recoverBus(false); hwReset(); }
			go(State::Idle, BUSY_AT_START, Action::Reset, State::Idle);
			if (r->SR2 & I2C_SR2_BUSY) { error = Error::BusBusy; return detach(DetachCause::ErrorCondition); }
		}
	}
	if (not segment(transaction->starting())) return refuse();
	go(State::Idle, START, Action::Start, State::SbWait);
	r->CR2 = (r->CR2 & ~I2C_CR2_ITBUFEN) | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
	r->CR1 = (r->CR1 & ~I2C_CR1_POS) | I2C_CR1_ACK | I2C_CR1_START;
}

bool
I2cMasterBase::segment(I2cTransaction::Starting s)
{
	address = s.address & 0xfe;
	return fetch(Operation(s.next));
}

// a write buffer that is used up while the transaction has another one
bool
I2cMasterBase::more()
{
	while (index >= length and next == Operation::Write)
		if (not fetch(Operation::Write)) return false;
	return index < length;
}

// the segment is over (STOP or START requested): the transaction is detached at its STOP,
// at a Restart the next segment is fetched now, its START is already programmed
void
I2cMasterBase::finishSegment()
{
	if (next == Operation::Stop)
		return detach((error == Error::NoError) ? DetachCause::NormalStop : DetachCause::ErrorCondition);
	const auto s = transaction->starting();
	if (not segment(s))
	{
		// a read of nothing, refused after its repeated START is requested: the device is
		// addressed for writing and the transfer ends with STOP (a ping), then fails
		error = Error::Unknown;
		address = s.address & 0xfe; reading = false; length = index = 0; next = Operation::Stop;
	}
}

// the first segment is refused, before its START
void
I2cMasterBase::refuse()
{
	go(state, START, Action::Unexpected, State::Idle);
	error = Error::Unknown;
	if (state != State::Idle) hwReset();
	detach(DetachCause::ErrorCondition);
}

} // namespace modm::platform
