// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_I2C_HPP
#	error	"Don't include this file directly, use 'i2c_master.hpp' instead!"
#endif

// debugging for serious dummies
/*
#include "../../uart/stm32/usart_2.hpp"
#define DEBUG_SW_I2C(x) xpcc::stm32::Usart2::write(x)
/*/
#define DEBUG_SW_I2C(x)
//*/

template <typename SCL, typename SDA, uint32_t BaudRate>
SCL xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::scl;
template <typename SCL, typename SDA, uint32_t BaudRate>
SDA xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::sda;

template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2c::Operation xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::nextOperation;
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction *xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::transactionObject(nullptr);
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cMaster::Error xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::errorState(xpcc::I2cMaster::Error::NoError);

template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Starting xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::starting(0, xpcc::I2c::OperationAfterStart::Stop);
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Writing xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::writing(nullptr, 0, xpcc::I2c::OperationAfterWrite::Stop);
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Reading xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::reading(nullptr, 0, xpcc::I2c::OperationAfterRead::Stop);

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::start(xpcc::I2cTransaction *transaction, Configuration_t configuration)
{
	if (!transactionObject && transaction)
	{
		if (!transaction->attaching())
		{
			transaction->detaching(xpcc::I2c::DetachCause::FailedToAttach);
			// return false; // done at the end of the function
		}
		else
		{
			errorState = Error::NoError;
			// call the configuration function
			if (configuration) configuration();
			// save the transaction object
			transactionObject = transaction;

			while (1)
			{
				// generate Start condition
				if (!startCondition()) return true;

				// ask the transaction object about address and next operation.
				starting = transactionObject->starting();
				uint8_t address = (starting.address & 0xfe);

				// set the correct addressing bit
				if (starting.next == xpcc::I2c::OperationAfterStart::Read)
				{
					address |= xpcc::I2c::Read;
					DEBUG_SW_I2C('r');
				}
				else DEBUG_SW_I2C('w');

				// write address
				if (!write(address)) return true;
				// we use address=0 as a hint for error management that write errors are now data NACKs
				starting.address = 0;

				if (nextOperation == xpcc::I2c::Operation::Restart) {DEBUG_SW_I2C('R');}
				nextOperation = static_cast<xpcc::I2c::Operation>(starting.next);

				do
				{
					switch (nextOperation)
					{
						case xpcc::I2c::Operation::Read:
							reading = transactionObject->reading();
							while (reading.length > 1)
							{
								if (!read(*reading.buffer, ACK)) return true;
								reading.buffer++;
								reading.length--;
							}
							if (!read(*reading.buffer++, NACK)) return true;
							nextOperation = static_cast<xpcc::I2c::Operation>(reading.next);
							break;

						case xpcc::I2c::Operation::Write:
							writing = transactionObject->writing();
							while (writing.length > 0)
							{
								if (!write(*writing.buffer++)) return true;
								writing.length--;
							}
							nextOperation = static_cast<xpcc::I2c::Operation>(writing.next);
							break;

						default:
						case xpcc::I2c::Operation::Stop:
							transactionObject->detaching(xpcc::I2c::DetachCause::NormalStop);
							transactionObject = nullptr;
							stopCondition();
							return true;
					}
				}
				while (nextOperation != xpcc::I2c::Operation::Restart);
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::reset()
{
	errorState = Error::SoftwareReset;
	if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
	transactionObject = nullptr;
}

// ----------------------------------------------------------------------------
// MARK: - private
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::error(Error error)
{
	DEBUG_SW_I2C('E');
	DEBUG_SW_I2C('0' + static_cast<uint8_t>(error));
	scl.set();
	sda.set();
	errorState = error;

	if (transactionObject) transactionObject->detaching(DetachCause::ErrorCondition);
	transactionObject = nullptr;
}

// ----------------------------------------------------------------------------
// MARK: bus control
template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::startCondition()
{
	DEBUG_SW_I2C('\n');
	DEBUG_SW_I2C('s');
	sda.set();
	delay();
	if(sda.read() == xpcc::Gpio::Low)
	{
		error(Error::BusCondition);
		return false;
	}
	if (!sclSetAndWait())
	{
		error(Error::BusCondition);
		return false;
	}
	// here both pins are High, ready for start

	sda.reset();
	delay();
	scl.reset();
	delay();

	return true;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::stopCondition()
{
	DEBUG_SW_I2C('S');
	scl.reset();
	sda.reset();

	if (!sclSetAndWait())
	{
		error(Error::BusCondition);
		return false;
	}

	delay();
	sda.set();
	delay();

	if (sda.read() == xpcc::Gpio::Low)
	{
		error(Error::BusCondition);
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------
// MARK: byte operations
template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::write(uint8_t data)
{
	DEBUG_SW_I2C('W');
	for(uint8_t i = 0; i < 8; ++i)
	{
		if (!writeBit(data & 0x80))
		{
			error(Error::ArbitrationLost);
			return false;
		}
		data <<= 1;
	}

	// release sda
	sda.set();
	delay();

	if (!sclSetAndWait())
	{
		error(Error::ArbitrationLost);
		return false;
	}
	if(sda.read() == xpcc::Gpio::High)
	{
		DEBUG_SW_I2C('n');
		error(starting.address ? Error::AddressNack : Error::DataNack);
		return false;
	}
	DEBUG_SW_I2C('a');
	delay();
	scl.reset();

	return true;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::read(uint8_t &data, bool ack)
{
	DEBUG_SW_I2C('R');
	sda.set();

	data = 0;
	for(uint8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (!readBit(data))
		{
			error(Error::ArbitrationLost);
			return false;
		}
	}

	DEBUG_SW_I2C(ack ? 'A' : 'N');
	// generate acknowledge bit
	if (!writeBit(!ack))
	{
		error(Error::ArbitrationLost);
		return false;
	}
	// release sda
	sda.set();
	return true;
}

// ----------------------------------------------------------------------------
// MARK: bit operations
template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::readBit(uint8_t &data)
{
	delay();
	if (sclSetAndWait())
	{
		delay();
		if(sda.read() == xpcc::Gpio::High)
			data |= 0x01;

		scl.reset();
		return true;
	}
	return false;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::writeBit(bool bit)
{
	sda.set(bit);
	delay();

	if (sclSetAndWait())
	{
		delay();
		scl.reset();

		return true;
	}
	return false;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::sclSetAndWait()
{
	scl.set();
	// wait for clock stretching by slave
	// only wait a maximum of 250 half clock cycles
	uint_fast8_t deadlockPreventer = 250;
	while (scl.read() == xpcc::Gpio::Low && deadlockPreventer)
	{
		xpcc::delayMicroseconds(delayTime/2);
		deadlockPreventer--;
		// double the read amount
		if (scl.read() == xpcc::Gpio::High) return true;
		xpcc::delayMicroseconds(delayTime/2);
	}
	// if extreme clock stretching occurs, then there might be an external error
	return deadlockPreventer > 0;
}

// ----------------------------------------------------------------------------
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::delay()
{
	xpcc::delayMicroseconds(delayTime);
}
