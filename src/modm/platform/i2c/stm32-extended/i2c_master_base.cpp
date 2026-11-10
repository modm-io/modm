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
//    it decide NBYTES, RELOAD and AUTOEND up front,
//  - a read of length 0 is refused: before the START if it is the first segment, else
//    the bus still ends with a STOP and the transfer fails with Error::Unknown,
//  - the transaction is detached when its STOP is done (STOPF).

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
			r->CR1 = I2C_CR1_PE;
			r->CR2 = (r->CR2 & ~(I2C_CR2_RELOAD | I2C_CR2_START)) | I2C_CR2_STOP;
		}
	}
	// at most a few bytes at 10 kHz
	for (int us = 0; us < 4000 and busBusy(); us += 10)
	{
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
{ return regs()->ISR & I2C_ISR_BUSY; }

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

// the next buffer, NBYTES restarts with it
bool
I2cMasterBase::fetch(Operation op)
{
	planned = 0;
	return I2cMasterCommon::fetch(op);
}


void
I2cMasterBase::configure(uint32_t timingRegister)
{
	timingr = timingRegister;
	regs()->CR1 = 0;
	regs()->OAR1 = 0;
	regs()->OAR2 = 0;
	hwReset();
}

#ifdef I2C_TIMEOUTR_TIMOUTEN
void
I2cMasterBase::setTimeout(uint16_t ticks)
{
	// TIMEOUTEN must be cleared before the reload value is changed
	regs()->TIMEOUTR = 0;
	if (ticks) regs()->TIMEOUTR = I2C_TIMEOUTR_TIMOUTEN | I2C_TIMEOUTR_TEXTEN |
			(ticks - 1) | ((ticks - 1) << I2C_TIMEOUTR_TIMEOUTB_Pos);
}
#endif

void
I2cMasterBase::hwReset()
{
	// PE must stay low for three APB clock cycles, the reads ensure that
	regs()->CR1 = 0;
	for (int ii = 0; ii < 3; ii++) (void) regs()->CR1;
	regs()->TIMINGR = timingr;
	regs()->CR1 = I2C_CR1_PE | I2C_CR1_ERRIE;
	state = State::Idle;
}

void
I2cMasterBase::isr()
{
	I2C_TypeDef *const r = regs();
	const uint32_t isr = r->ISR;
	const State before = state;
	if (not active)
	{
		// A transfer that ended early (lost arbitration, ...) can leave a byte in RXDR or an
		// empty TXDR behind: RXNE and TXIS are cleared only by an access, so without one the
		// interrupt comes again and again. Measured with test/integration/i2c_stress on
		// DISCO-F746NG: RXNE with the bus idle, forever. begin() enables them again.
		if (isr & I2C_ISR_RXNE) (void) r->RXDR;
		r->CR1 = I2C_CR1_PE | I2C_CR1_ERRIE;
		r->ICR =I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_OVRCF;
#ifdef I2C_ICR_TIMOUTCF
		r->ICR = I2C_ICR_TIMOUTCF;
#endif
		return;
	}
	if (isr & I2C_ISR_NACKF)
	{
		// the STOP follows automatically
		error = (before == State::Addr) ? Error::AddressNack : Error::DataNack;
		r->ICR = I2C_ICR_NACKCF;
		r->ISR = I2C_ISR_TXE;
		go(before, NACKF, Action::Nack, State::Stopping);
	}
	else if (isr & I2C_ISR_BERR)
	{
		error = Error::BusCondition;
		r->ICR = I2C_ICR_BERRCF;
		go(before, BERR, Action::Berr, before);
	}
	else if (isr & I2C_ISR_ARLO)
	{
		r->ICR = I2C_ICR_ARLOCF;
		go(before, ARLO, Action::Arlo, State::Idle);
		error = Error::ArbitrationLost;
		detach(DetachCause::ErrorCondition);
	}
#ifdef I2C_ISR_TIMEOUT
	else if (isr & I2C_ISR_TIMEOUT)
	{
		// the SMBus bus timeout of setTimeout(): SCL held low too long
		r->ICR = I2C_ICR_TIMOUTCF;
		go(before, TIMEOUT, Action::Unexpected, State::Idle);
		hwReset();
		error = Error::BusCondition;
		detach(DetachCause::ErrorCondition);
	}
#endif
	else if (isr & I2C_ISR_RXNE)
	{
		if (not reading or index >= length) return unexpected(RXNE);
		rbuf[index++] = r->RXDR;
		go(before, RXNE, Action::Read, State::RxAck);
	}
	else if (isr & I2C_ISR_TXIS)
	{
		if (reading or index >= length) return unexpected(TXIS);
		r->TXDR = wbuf[index++];
		go(before, TXIS, Action::Write, State::Tx);
	}
	else if (isr & I2C_ISR_TCR)
	{
		// A read with a byte of the chunk unread: the chunk's last byte arrived while RXDR
		// was still full; it is acknowledged and TCR set, but it never reaches RXDR and no
		// RXNE comes, and after a reload SCL stays low for good (measured on STM32F303,
		// about once per 100k reloads with interrupt latency, not in ES0204). The byte is
		// lost, so the transfer ends here instead of hanging.
		if (reading and index < planned) return unexpected(TCR);
		if (planned >= length)
		{
			// a write that goes on in the next buffer; empty buffers are skipped
			if (next != Operation::Write) return unexpected(TCR);
			while (next == Operation::Write) { fetch(Operation::Write); if (length) break; }
			if (not length)
			{
				// Only empty buffers followed. RM0316 does not say what STOP/START do while
				// TCR is set; measured on STM32F303: STOP is sent, START is taken as a
				// reload. So STOP now, or NBYTES = 0 without RELOAD, which sets TC, and the
				// repeated START from there.
				if (next == Operation::Stop)
				{
					r->CR2 = (r->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_START)) |
							 I2C_CR2_STOP;
					return go(before, TCR, Action::Stop, State::Stopping);
				}
				r->CR2 = r->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_START | I2C_CR2_STOP);
				return go(before, TCR, Action::Reload, State::TcWait);
			}
		}
		r->CR2 = (r->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_START | I2C_CR2_STOP)) |
				 chunk();
		go(before, TCR, Action::Reload, reading ? State::Rx : State::TxWait);
	}
	else if (isr & I2C_ISR_TC)
	{
		if (next != Operation::Restart) return unexpected(TC);
		if (not segment(transaction->starting())) return stop(before, TC);
		go(before, TC, Action::Next, State::Addr);
	}
	else if (isr & I2C_ISR_STOPF)
	{
		r->ICR = I2C_ICR_STOPCF;
		r->ISR = I2C_ISR_TXE;
		go(before, STOPF, Action::Done, State::Idle);
		detach((error == Error::NoError) ? DetachCause::NormalStop : DetachCause::ErrorCondition);
	}
}

void
I2cMasterBase::begin()
{
	I2C_TypeDef *const r = regs();
	active = true;
	error = Error::NoError;
	if (r->ISR & I2C_ISR_BUSY)
	{
		// the previous transfer's STOP or another controller: wait up to 1 ms, then reset
		for (int us = 0; us < 1000 and (r->ISR & I2C_ISR_BUSY); us++) modm::delay_us(1);
		if (r->ISR & I2C_ISR_BUSY)
		{
			// a device holds the bus: a peripheral reset, then the bus clear (busy waits)
			hwReset();
			if ((r->ISR & I2C_ISR_BUSY) and recoverBus) { recoverBus(false); hwReset(); }
			go(State::Idle, BUSY_AT_START, Action::Reset, State::Idle);
			if (r->ISR & I2C_ISR_BUSY) { error = Error::BusBusy; return detach(DetachCause::ErrorCondition); }
		}
	}
	go(State::Idle, START, Action::Start, State::Addr);
	r->CR1 = I2C_CR1_PE | I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_RXIE | I2C_CR1_TXIE;
	if (not segment(transaction->starting())) refuse();
}

// fetches the segment that follows a START and sends the START
bool
I2cMasterBase::segment(I2cTransaction::Starting s)
{
	address = s.address;
	if (not fetch(Operation(s.next))) return false;
	// empty buffers before anything is sent carry no data and are skipped: NBYTES = 0 with
	// RELOAD at START sets TC, not TCR (measured on STM32F303)
	while (not reading and not length and next == Operation::Write) fetch(Operation::Write);
	regs()->CR2 = (uint32_t(address) & 0xfe) | (reading ? I2C_CR2_RD_WRN : 0) | chunk() | I2C_CR2_START;
	return true;
}

uint32_t
I2cMasterBase::chunk()
{
	const size_t n = std::min<size_t>(length - planned, maxChunk);
	planned += n;
	uint32_t cr2 = uint32_t(n) << I2C_CR2_NBYTES_Pos;
	if (planned < length or next == Operation::Write) cr2 |= I2C_CR2_RELOAD;
	else if (next == Operation::Stop) cr2 |= I2C_CR2_AUTOEND;
	return cr2;
}

// the next segment is refused while the bus is held after TC: STOP, the transfer fails
void
I2cMasterBase::stop(State before, uint8_t flag)
{
	error = Error::Unknown;
	regs()->CR2 = (regs()->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_START)) | I2C_CR2_STOP;
	go(before, flag, Action::Stop, State::Stopping);
}

// the first segment is refused, before its START
void
I2cMasterBase::refuse()
{
	go(state, START, Action::Unexpected, State::Idle);
	error = Error::Unknown;
	regs()->CR1 = I2C_CR1_PE | I2C_CR1_ERRIE;
	detach(DetachCause::ErrorCondition);
}

} // namespace modm::platform
