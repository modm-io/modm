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

// debugging for serious dummies
/*
#include "../../uart/stm32/usart_2.hpp"
#define DEBUG_SW_I2C(x) modm::platform::Usart2::write(x)
/*/
#define DEBUG_SW_I2C(x)
//*/

template <class Scl, class Sda>
uint16_t modm::platform::BitBangI2cMaster<Scl, Sda>::delayTime(3);

template <class Scl, class Sda>
modm::I2c::Operation modm::platform::BitBangI2cMaster<Scl, Sda>::nextOperation;
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
	if (reset != ResetDevices::NoReset) resetDevices<SCL>(uint32_t(reset));
	Connector::connect();
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::start(modm::I2cTransaction *transaction, ConfigurationHandler handler)
{
	if ((not transactionObject) and transaction)
	{
		if (not transaction->attaching())
		{
			transaction->detaching(modm::I2c::DetachCause::FailedToAttach);
			// return false; // done at the end of the function
		}
		else
		{
			// reset error state
			errorState = Error::NoError;
			// call the configuration function
			if (handler and configuration != handler) {
				configuration = handler;
				configuration();
			}
			// save the transaction object
			transactionObject = transaction;

			while (true)
			{
				// generate (Re-)Start condition
				if (not startCondition()) return true;

				// ask the transaction object about address and next operation.
				starting = transactionObject->starting();
				uint8_t address = (starting.address & 0xfe);

				// set the correct addressing bit
				DEBUG_SW_I2C((starting.next == modm::I2c::OperationAfterStart::Read) ? 'r': 'w');
				if (starting.next == modm::I2c::OperationAfterStart::Read)
					address |= modm::I2c::Read;

				// write address
				if (not write(address)) return true;
				// we use address=0 as a hint for error management that write errors are now *data* NACKs
				starting.address = 0;

				// if the inner loop required a restart, we arrive here again
				if (nextOperation == modm::I2c::Operation::Restart) {DEBUG_SW_I2C('R');}
				// what is the first operation after (re-)start?
				nextOperation = static_cast<modm::I2c::Operation>(starting.next);

				do
				{
					switch (nextOperation)
					{
						case modm::I2c::Operation::Read:
							// ask TO about reading
							reading = transactionObject->reading();
							while (reading.length > 1)
							{
								// continue reading, by sending ACKs
								if (not read(*reading.buffer++, ACK)) return true;
								reading.length--;
							}
							// read last byte, conclude with NACK
							if (not read(*reading.buffer, NACK)) return true;
							// what next?
							nextOperation = static_cast<modm::I2c::Operation>(reading.next);
							break;

						case modm::I2c::Operation::Write:
							// ask TO about writing
							writing = transactionObject->writing();
							while (writing.length > 0)
							{
								// write the entire buffer
								if (not write(*writing.buffer++)) return true;
								writing.length--;
							}
							// what next?
							nextOperation = static_cast<modm::I2c::Operation>(writing.next);
							break;

						default:
						case modm::I2c::Operation::Stop:
							transactionObject->detaching(modm::I2c::DetachCause::NormalStop);
							transactionObject = nullptr;
							stopCondition();
							return true;
					}
				} // continue doing this inner loop, until a restart is required
				while (nextOperation != modm::I2c::Operation::Restart);
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::reset()
{
	DEBUG_SW_I2C('E');
	DEBUG_SW_I2C('0' + static_cast<uint8_t>(Error::SoftwareReset));
	errorState = Error::SoftwareReset;
	if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
	transactionObject = nullptr;
}

// ----------------------------------------------------------------------------
// MARK: - error handling
template <class Scl, class Sda>
void
modm::platform::BitBangI2cMaster<Scl, Sda>::error(Error error)
{
	DEBUG_SW_I2C('E');
	DEBUG_SW_I2C('0' + static_cast<uint8_t>(error));
	delay2();
	SCL::reset();
	SDA::reset();
	delay2();
	// attempt a stop condition, if it fails there is nothing else we can do
	if (not stopCondition()) {
		errorState = error;
		return;
	}

	// release both lines
	SCL::set();
	SDA::set();
	// save error state
	errorState = error;

	if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
	transactionObject = nullptr;
}

// ----------------------------------------------------------------------------
// MARK: - bus condition operations
template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::startCondition()
{
	DEBUG_SW_I2C('\n');
	DEBUG_SW_I2C('s');
	// release data line
	SDA::set();
	delay4();
	if(SDA::read() == modm::Gpio::Low)
	{
		// could not release data line
		errorState = Error::BusBusy;
		if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
		transactionObject = nullptr;
		return false;
	}
	// release the clock line
	if (not sclSetAndWait())
	{
		// could not release clock line
		errorState = Error::BusBusy;
		if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
		transactionObject = nullptr;
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
	DEBUG_SW_I2C('S');
	// pull down both lines
	SCL::reset();
	SDA::reset();

	// first release clock line
	if (not sclSetAndWait())
	{
		// could not release clock line
		errorState = Error::BusCondition;
		if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
		transactionObject = nullptr;
		return false;
	}
	delay2();
	// release data line
	SDA::set();
	delay4();

	if (SDA::read() == modm::Gpio::Low)
	{
		// could not release data line
		errorState = Error::BusCondition;
		if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
		transactionObject = nullptr;
		return false;
	}
	return true;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::sclSetAndWait()
{
	SCL::set();
	// wait for clock stretching by slave
	// only wait a maximum of 250 half clock cycles
	uint_fast8_t deadlockPreventer = 250;
	while (SCL::read() == modm::Gpio::Low && deadlockPreventer)
	{
		delay4();
		deadlockPreventer--;
		// double the read amount
		if (SCL::read() == modm::Gpio::High) return true;
		delay4();
	}
	// if extreme clock stretching occurs, then there might be an external error
	return deadlockPreventer > 0;
}

// ----------------------------------------------------------------------------
// MARK: - byte operations
template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::write(uint8_t data)
{
	DEBUG_SW_I2C('W');
	// shift through all 8 bits
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		if (not writeBit(data & 0x80))
		{
			// arbitration error
			error(Error::ArbitrationLost);
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
		error(Error::BusCondition);
		return false;
	}
	// sample the data line for acknowledge bit
	if(SDA::read() == modm::Gpio::High)
	{
		DEBUG_SW_I2C('n');
		// we have not received an ACK
		// depending on context, this could be AddressNack or DataNack
		error(starting.address ? Error::AddressNack : Error::DataNack);
		return false;
	}
	DEBUG_SW_I2C('a');
	delay2();
	// falling clock edge
	SCL::reset();

	return true;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::read(uint8_t &data, bool ack)
{
	DEBUG_SW_I2C('R');
	// release data line
	SDA::set();

	// shift through the 8 bits
	data = 0;
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (not readBit(data))
		{
			// slaves don't stretch the clock here
			// this must be arbitration.
			error(Error::ArbitrationLost);
			return false;
		}
	}

	DEBUG_SW_I2C(ack ? 'A' : 'N');
	// generate acknowledge bit
	if (not writeBit(not ack))
	{
		// arbitration too
		error(Error::ArbitrationLost);
		return false;
	}
	// release data line
	SDA::set();
	return true;
}

// ----------------------------------------------------------------------------
// MARK: - bit operations
template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::writeBit(bool bit)
{
	// set the data pin
	SDA::set(bit);
	delay2();

	// rising clock edge, the slave samples the data line now
	if ((SDA::read() == bit) && sclSetAndWait())
	{
		delay2();
		// falling clock edge
		SCL::reset();
		// everything ok
		return true;
	}
	// too much clock stretching
	return false;
}

template <class Scl, class Sda>
bool
modm::platform::BitBangI2cMaster<Scl, Sda>::readBit(uint8_t &data)
{
	// slave sets data line
	delay2();
	// rising clock edge, the master samples the data line now
	if (sclSetAndWait())
	{
		// copy bit into data
		if(SDA::read() == modm::Gpio::High)
			data |= 0x01;

		delay2();
		// falling clock edge
		SCL::reset();
		return true;
	}
	// too much clock stretching
	return false;
}
