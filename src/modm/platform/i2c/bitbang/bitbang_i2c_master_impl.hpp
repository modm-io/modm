/*
 * Copyright (c) 2010-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SOFTWARE_BITBANG_I2C_HPP
#	error	"Don't include this file directly, use 'bitbang_i2c_master.hpp' instead!"
#endif
#include <modm/architecture/interface/fiber.hpp>

template <class Scl, class Sda>
uint16_t modm::platform::BitBangI2cMaster<Scl, Sda>::delayTime(3);

template <class Scl, class Sda>
modm::I2cTransaction *modm::platform::BitBangI2cMaster<Scl, Sda>::transactionObject(nullptr);
template <class Scl, class Sda>
modm::I2cMaster::Error modm::platform::BitBangI2cMaster<Scl, Sda>::errorState(modm::I2cMaster::Error::NoError);
template <class Scl, class Sda>
modm::I2c::ConfigurationHandler modm::platform::BitBangI2cMaster<Scl, Sda>::configuration(nullptr);

template <class Scl, class Sda>
modm::I2cTransaction::Starting modm::platform::BitBangI2cMaster<Scl, Sda>::starting(0, modm::I2c::OperationAfterStart::Stop);
template <class Scl, class Sda>
modm::I2cTransaction::Writing modm::platform::BitBangI2cMaster<Scl, Sda>::writing(nullptr, 0, modm::I2c::OperationAfterWrite::Stop);
template <class Scl, class Sda>
modm::I2cTransaction::Reading modm::platform::BitBangI2cMaster<Scl, Sda>::reading(nullptr, 0, modm::I2c::OperationAfterRead::Stop);

template <class Scl, class Sda>
template< class SystemClock, modm::baudrate_t baudrate, modm::percent_t tolerance >
void
modm::platform::BitBangI2cMaster<Scl, Sda>::initialize()
{
	delayTime = 250'000'000ul / baudrate;
	if (delayTime == 0) delayTime = 1;

	SCL::set();
	SDA::set();
}

template <class Scl, class Sda>
template <class... Signals>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::connect(PullUps pullups, ResetDevices reset)
{
	using Connector = GpioConnector<Peripheral::BitBang, Signals...>;
	static_assert(sizeof...(Signals) == 2, "BitBangI2cMaster<Scl, Sda>::connect() requires one Scl and one Sda signal!");
	static_assert(Connector::template Contains<SCL> and Connector::template Contains<SDA>,
				  "BitBangI2cMaster<Scl, Sda> can only connect to the same Scl and Sda signals of the declaration!");
	const typename SCL::InputType input =
		(pullups == PullUps::Internal) ? SCL::InputType::PullUp : SCL::InputType::Floating;

	Connector::disconnect();
	SCL::configure(input);
	SDA::configure(input);
	SCL::setOutput(SCL::OutputType::OpenDrain);
	SDA::setOutput(SDA::OutputType::OpenDrain);
	busReset = reset;
	clearBus();
	Connector::connect();
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::start(modm::I2cTransaction *transaction, ConfigurationHandler handler)
{
	// only one transaction at a time: start() runs it to the end, also from another fiber
	// that calls start() while this one waits for clock stretching
	if (running or not transaction) return false;
	if (not transaction->attaching())
	{
		transaction->detaching(modm::I2c::DetachCause::FailedToAttach);
		return false;
	}
	errorState = Error::NoError;
	if (handler and configuration != handler) {
		configuration = handler;
		configuration();
	}
	transactionObject = transaction;
	running = true;
	run();
	// reset() from another fiber ended the transaction while this one waited
	if (aborted) { aborted = false; running = false; }
	return true;
}

template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::run()
{
	bool restart = false;
	while (true)
	{
		// every buffer is fetched before its START, so that a refused read ends with a STOP
		starting = transactionObject->starting();
		if (aborted) return;
		const bool isRead = starting.next == modm::I2c::OperationAfterStart::Read;
		if (isRead)
		{
			reading = transactionObject->reading();
			if (aborted) return;
			// a read of length 0 cannot NACK its last byte
			if (reading.length == 0)
			{
				step(END, Action::Refuse, State::Idle);
				if (restart and not stopCondition() and not aborted) clearBus();
				return finish(Error::Unknown);
			}
		}
		if (not startCondition())
		{
			step(BUSY_AT_START, Action::Start, State::Idle);
			return finish(Error::BusBusy);
		}
		step(START, restart ? Action::Restart : Action::Start, State::Address);

		addressPhase = true;
		if (not write((starting.address & 0xfe) | (isRead ? modm::I2c::Read : 0),
					  isRead ? State::Read : State::Write)) return;
		addressPhase = false;

		auto next = static_cast<modm::I2c::Operation>(starting.next);
		if (isRead)
		{
			for (; reading.length > 1; reading.length--)
				if (not read(*reading.buffer++, ACK)) return;
			if (not read(*reading.buffer, NACK)) return;
			next = static_cast<modm::I2c::Operation>(reading.next);
		}
		// writes in several buffers without a repeated START
		while (next == modm::I2c::Operation::Write)
		{
			writing = transactionObject->writing();
			if (aborted) return;
			for (; writing.length; writing.length--)
				if (not write(*writing.buffer++)) return;
			next = static_cast<modm::I2c::Operation>(writing.next);
		}
		if (next != modm::I2c::Operation::Restart) break;
		restart = true;
	}
	step(END, Action::Stop, State::Idle);
	if (not stopCondition())
	{
		if (not aborted) clearBus();
		return finish(Error::BusCondition);
	}
	finish(Error::NoError);
}

template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::finish(Error error)
{
	// reset() has already detached the transaction
	if (aborted) return;
	errorState = error;
	failure(error);
	modm::I2cTransaction *transaction = transactionObject;
	transactionObject = nullptr;
	running = false;
	// may call start() again
	transaction->detaching((error == Error::NoError) ?
			modm::I2c::DetachCause::NormalStop : modm::I2c::DetachCause::ErrorCondition);
}

// ----------------------------------------------------------------------------
template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::reset()
{
	// the fiber that runs the transaction stops at its next step and leaves the bus alone
	if (running) aborted = true;
	errorState = Error::SoftwareReset;
	step(DEADLINE, Action::Abort, State::Idle);
	failure(Error::SoftwareReset);
	if (modm::I2cTransaction *transaction = transactionObject)
	{
		transactionObject = nullptr;
		transaction->detaching(DetachCause::ErrorCondition);
	}
	SDA::set();
	// a device may still stretch the clock, the bus clear needs SCL high
	sclSetAndWait();
	clearBus();
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::clearBus()
{
	if (busReset != ResetDevices::NoReset) resetDevices<SCL, SDA>(uint32_t(busReset));
	return SCL::read() and SDA::read();
}

// ----------------------------------------------------------------------------
// MARK: - error handling
template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::error(Error error)
{
	// reset() has already detached the transaction and owns the bus
	if (aborted) return;
	delay2();
	SCL::reset();
	SDA::reset();
	delay2();
	// attempt a stop condition, if it fails a device holds the bus
	if (not stopCondition() and not aborted) clearBus();
	SCL::set();
	SDA::set();
	finish(error);
}

// ----------------------------------------------------------------------------
// MARK: - bus condition operations
template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::startCondition()
{
	// release data line
	SDA::set();
	delay4();
	// a device may still hold the data line, or the clock line
	if (SDA::read() == modm::Gpio::Low or not sclSetAndWait())
	{
		if (aborted) return false;
		SCL::set();
		clearBus();
		return false;
	}
	// now both pins are High, ready for start

	// pull down data line
	SDA::reset();
	delay2();
	// pull down clock line
	SCL::reset();
	delay2();

	// start condition generated
	return true;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::stopCondition()
{
	// pull down both lines
	SCL::reset();
	SDA::reset();

	// first release clock line
	if (not sclSetAndWait()) return false;
	delay2();
	// release data line
	SDA::set();
	delay4();

	return SDA::read() == modm::Gpio::High;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::sclSetAndWait()
{
	SCL::set();
	// short clock stretching is busy waited for about 1 ms, since switching
	// fibers may take a long time and draw out the transaction
	for (uint32_t ii = 500'000ul / delayTime; ii; ii--)
	{
		// the pull-up needs a moment to raise SCL after it was released
		delay4();
		if (SCL::read() == modm::Gpio::High) return true;
		delay4();
	}
	// long clock stretching yields until the timeout
	const bool released = modm::this_fiber::poll_for(stretchTimeout, []{ return SCL::read() == modm::Gpio::High; });
	return released and not (running and aborted);
}

// ----------------------------------------------------------------------------
// MARK: - byte operations
template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::write(uint8_t data, State next)
{
	modm::this_fiber::yield();
	if (aborted) return false;
	const Action action = addressPhase ? Action::Address : Action::Write;

	// shift through all 8 bits
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		if (const Error e = writeBit(data & 0x80); e != Error::NoError)
		{
			step(bitFlag, action, State::Idle);
			error(e);
			return false;
		}
		data <<= 1;
	}

	// release sda
	SDA::set();
	delay2();

	// rising clock edge for acknowledge bit
	if (not sclSetAndWait())
	{
		// the slave is allowed to stretch the clock, but not unreasonably long!
		step(STRETCH, action, State::Idle);
		error(Error::BusCondition);
		return false;
	}
	// sample the data line for acknowledge bit
	if(SDA::read() == modm::Gpio::High)
	{
		// we have not received an ACK
		step(NACKED, action, State::Idle);
		error(addressPhase ? Error::AddressNack : Error::DataNack);
		return false;
	}
	delay2();
	// falling clock edge
	SCL::reset();
	step(ACKED, action, next);

	return true;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::read(uint8_t &data, bool ack)
{
	modm::this_fiber::yield();
	if (aborted) return false;

	// release data line
	SDA::set();

	// shift through the 8 bits
	data = 0;
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (const Error e = readBit(data); e != Error::NoError)
		{
			step(bitFlag, Action::Read, State::Idle);
			error(e);
			return false;
		}
	}

	// generate acknowledge bit
	if (const Error e = writeBit(not ack); e != Error::NoError)
	{
		step(bitFlag, Action::Read, State::Idle);
		error(e);
		return false;
	}
	// release data line
	SDA::set();
	step(ack ? ACKED : NACKED, Action::Read, State::Read);
	return true;
}

// ----------------------------------------------------------------------------
// MARK: - bit operations
template <class Scl, class Sda>
modm::I2cMaster::Error
modm::platform::BitBangI2cMaster<Scl, Sda>::writeBit(bool bit)
{
	// set the data pin
	SDA::set(bit);
	delay2();
	// another controller drives a 0
	if (SDA::read() != bit) { bitFlag = LOST; return Error::ArbitrationLost; }
	// rising clock edge, the target samples the data line now
	if (not sclSetAndWait()) { bitFlag = STRETCH; return Error::BusCondition; }
	delay2();
	// another controller or a glitch changed a line while SCL was high
	const bool sdaStable = SDA::read() == bit;
	const bool stable = sdaStable and SCL::read();
	// falling clock edge
	SCL::reset();
	if (not stable) bitFlag = sdaStable ? GLITCH : LOST;
	return stable ? Error::NoError : Error::ArbitrationLost;
}

template <class Scl, class Sda>
modm::I2cMaster::Error
modm::platform::BitBangI2cMaster<Scl, Sda>::readBit(uint8_t &data)
{
	// target sets data line
	delay2();
	// rising clock edge, the controller samples the data line now
	if (not sclSetAndWait()) { bitFlag = STRETCH; return Error::BusCondition; }
	// copy bit into data
	const bool bit = SDA::read();
	if (bit) data |= 0x01;
	delay2();
	// a change while SCL is high is a misplaced start or stop condition,
	// a low SCL a glitch that clocked the device
	const bool stable = (SDA::read() == bit) and SCL::read();
	// falling clock edge
	SCL::reset();
	if (not stable) bitFlag = GLITCH;
	return stable ? Error::NoError : Error::BusCondition;
}
