/*
 * Copyright (c) 2010-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SOFTWARE_I2C_HPP
#	error	"Don't include this file directly, use 'i2c_master.hpp' instead!"
#endif

// debugging for serious dummies
/*
#include "../../uart/stm32/usart_2.hpp"
#define DEBUG_SW_I2C(x) modm::platform::Usart2::write(x)
/*/
#define DEBUG_SW_I2C(x)
//*/

template <class SCL, class SDA>
uint16_t modm::SoftwareI2cMaster<SCL, SDA>::delayTime(3);

template <class SCL, class SDA>
modm::I2c::Operation modm::SoftwareI2cMaster<SCL, SDA>::nextOperation;
template <class SCL, class SDA>
modm::I2cTransaction *modm::SoftwareI2cMaster<SCL, SDA>::transactionObject(nullptr);
template <class SCL, class SDA>
modm::I2cMaster::Error modm::SoftwareI2cMaster<SCL, SDA>::errorState(modm::I2cMaster::Error::NoError);
template <class SCL, class SDA>
modm::I2c::ConfigurationHandler modm::SoftwareI2cMaster<SCL, SDA>::configuration(nullptr);

template <class SCL, class SDA>
modm::I2cTransaction::Starting modm::SoftwareI2cMaster<SCL, SDA>::starting(0, modm::I2c::OperationAfterStart::Stop);
template <class SCL, class SDA>
modm::I2cTransaction::Writing modm::SoftwareI2cMaster<SCL, SDA>::writing(nullptr, 0, modm::I2c::OperationAfterWrite::Stop);
template <class SCL, class SDA>
modm::I2cTransaction::Reading modm::SoftwareI2cMaster<SCL, SDA>::reading(nullptr, 0, modm::I2c::OperationAfterRead::Stop);

template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::start(modm::I2cTransaction *transaction, ConfigurationHandler handler)
{
	if (!transactionObject and transaction)
	{
		if (!transaction->attaching())
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

			while (1)
			{
				// generate (Re-)Start condition
				if (!startCondition()) return true;

				// ask the transaction object about address and next operation.
				starting = transactionObject->starting();
				uint8_t address = (starting.address & 0xfe);

				// set the correct addressing bit
				DEBUG_SW_I2C((starting.next == modm::I2c::OperationAfterStart::Read) ? 'r': 'w');
				if (starting.next == modm::I2c::OperationAfterStart::Read)
					address |= modm::I2c::Read;

				// write address
				if (!write(address)) return true;
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
								if (!read(*reading.buffer++, ACK)) return true;
								reading.length--;
							}
							// read last byte, conclude with NACK
							if (!read(*reading.buffer, NACK)) return true;
							// what next?
							nextOperation = static_cast<modm::I2c::Operation>(reading.next);
							break;

						case modm::I2c::Operation::Write:
							// ask TO about writing
							writing = transactionObject->writing();
							while (writing.length > 0)
							{
								// write the entire buffer
								if (!write(*writing.buffer++)) return true;
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
template <class SCL, class SDA>
void
modm::SoftwareI2cMaster<SCL, SDA>::reset()
{
	DEBUG_SW_I2C('E');
	DEBUG_SW_I2C('0' + static_cast<uint8_t>(Error::SoftwareReset));
	errorState = Error::SoftwareReset;
	if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
	transactionObject = nullptr;
}

// ----------------------------------------------------------------------------
// MARK: - error handling
template <class SCL, class SDA>
void
modm::SoftwareI2cMaster<SCL, SDA>::error(Error error)
{
	DEBUG_SW_I2C('E');
	DEBUG_SW_I2C('0' + static_cast<uint8_t>(error));
	delay2();
	SCL::reset();
	SDA::reset();
	delay2();
	// attempt a stop condition, if it fails there is nothing else we can do
	if (!stopCondition()) {
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
template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::startCondition()
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
	if (!sclSetAndWait())
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

template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::stopCondition()
{
	DEBUG_SW_I2C('S');
	// pull down both lines
	SCL::reset();
	SDA::reset();

	// first release clock line
	if (!sclSetAndWait())
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

template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::sclSetAndWait()
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
template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::write(uint8_t data)
{
	DEBUG_SW_I2C('W');
	// shift through all 8 bits
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		if (!writeBit(data & 0x80))
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
	if (!sclSetAndWait())
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

template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::read(uint8_t &data, bool ack)
{
	DEBUG_SW_I2C('R');
	// release data line
	SDA::set();

	// shift through the 8 bits
	data = 0;
	for(uint_fast8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (!readBit(data))
		{
			// slaves don't stretch the clock here
			// this must be arbitration.
			error(Error::ArbitrationLost);
			return false;
		}
	}

	DEBUG_SW_I2C(ack ? 'A' : 'N');
	// generate acknowledge bit
	if (!writeBit(!ack))
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
template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::writeBit(bool bit)
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

template <class SCL, class SDA>
bool
modm::SoftwareI2cMaster<SCL, SDA>::readBit(uint8_t &data)
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
