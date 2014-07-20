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
//#define DEBUG_SW_I2C(x) xpcc::lpc::BufferedUart1::write(x)
#define DEBUG_SW_I2C(x)

template <typename SCL, typename SDA, uint32_t BaudRate>
SCL xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::scl;
template <typename SCL, typename SDA, uint32_t BaudRate>
SDA xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::sda;

template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2c::Operation xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::nextOperation;
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction *xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::delegate(0);
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cMaster::Error xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::errorState(xpcc::I2cMaster::Error::NoError);

template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Starting xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::starting;
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Writing xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::writing;
template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cTransaction::Reading xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::reading;

// ----------------------------------------------------------------------------
template <typename SCL, typename SDA, uint32_t BaudRate>
template <class clockSource, uint32_t baudrate, uint16_t tolerance>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::initialize()
{
	scl.set();
	sda.set();
}

template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::reset(DetachCause cause)
{
	if (delegate) delegate->detaching(cause);
	delegate = 0;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::start(xpcc::I2cTransaction *transaction)
{
	if (!delegate && transaction && transaction->attaching())
	{
		delegate = transaction;

		while(1)
		{
			DEBUG_SW_I2C('\n');
			DEBUG_SW_I2C('s');
			startCondition();

			starting.address = 0;
			starting.next = xpcc::I2c::Operation::Stop;
			delegate->starting(starting);
			uint8_t address = (starting.address & 0xfe);

			if (starting.next == xpcc::I2c::Operation::Read)
				address |= xpcc::I2c::Read;

			if (!write(address))
				return true;
			if (nextOperation == xpcc::I2c::Operation::Restart) {DEBUG_SW_I2C('R');}
			nextOperation = starting.next;

			do {
				switch (nextOperation)
				{
					case xpcc::I2c::Operation::Read:
					{
						reading.length = 0;
						reading.next = xpcc::I2c::OperationAfterRead::Stop;
						delegate->reading(reading);
						while(reading.length > 1) {
							*reading.buffer++ = read(true);
							--reading.length;
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('a');
						}
						*reading.buffer = read(false);
						DEBUG_SW_I2C('\n');
						DEBUG_SW_I2C('n');
						nextOperation = static_cast<xpcc::I2c::Operation>(reading.next);
					}
						break;

					case xpcc::I2c::Operation::Write:
					{
						writing.length = 0;
						writing.next = xpcc::I2c::OperationAfterWrite::Stop;
						delegate->writing(writing);
						while(writing.length > 0) {
							if (!write(*writing.buffer++))
								return true;
							--writing.length;
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('A');
						}
						nextOperation = static_cast<xpcc::I2c::Operation>(writing.next);
					}
						break;

					default:
					case xpcc::I2c::Operation::Stop:
						DEBUG_SW_I2C('S');
						delegate->detaching(xpcc::I2c::DetachCause::NormalStop);
						delegate = 0;
						stopCondition();
						return true;
				}
			} while (nextOperation != xpcc::I2c::Operation::Restart);
		}
	}
	return false;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
xpcc::I2cMaster::Error
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::getErrorState()
{
	return errorState;
}

// MARK: - private
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::error()
{
	DEBUG_SW_I2C('E');
	stopCondition();
	errorState = Error::DataNack;
	reset(DetachCause::ErrorCondition);
}

// MARK: bus control
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::startCondition()
{
	sda.set();
	while((sda.read() == xpcc::Gpio::Low))
		;
	delay();

	sclSetAndWait();
	delay();

	// here both pins are High, ready for start
	sda.reset();
	delay();
	scl.reset();
	delay();
}

template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::stopCondition()
{
	scl.reset();
	sda.reset();

	delay();
	sclSetAndWait();
	delay();
	sda.set();
	delay();
}

// MARK: byte operations
template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}

	// release sda
	sda.set();

	delay();
	sclSetAndWait();

	bool bit = sda.read();

	delay();

	// return acknowledge bit
	if (bit) {
		DEBUG_SW_I2C('N');
		error();
		return false;
	}
	return true;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
uint8_t
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::read(bool ack)
{
	sda.set();

	uint8_t data = 0;
	for(uint8_t i = 0; i < 8; ++i)
	{
		data <<= 1;
		if (readBit()) {
			data |= 0x01;
		}
	}

	// generate acknowledge bit
	writeBit(!ack);

	// release sda
	sda.set();

	return data;
}

// MARK: bit operations
template <typename SCL, typename SDA, uint32_t BaudRate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::readBit()
{
	delay();
	sclSetAndWait();

	delay();

	bool bit = sda.read();

	scl.reset();

	return bit;
}

template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::writeBit(bool bit)
{
	if (bit) {
		sda.set();
	}
	else {
		sda.reset();
	}
	delay();

	sclSetAndWait();

	delay();

	scl.reset();
}

template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::sclSetAndWait()
{
	scl.set();
	// wait for clock stretching by slave
	// only wait a maximum of 250 half clock cycles
	uint_fast8_t deadlockPreventer = 250;
	while (scl.read() == xpcc::Gpio::Low && deadlockPreventer)
	{
		xpcc::delay_us(delayTime/2);
		deadlockPreventer--;
		// double the read amount
		if (scl.read() != xpcc::Gpio::Low) return;
		xpcc::delay_us(delayTime/2);
	}
	// if extreme clock stretching occurs, then there might be an external error
	if (!deadlockPreventer)
	{
		error();
	}
}

// ----------------------------------------------------------------------------
template <typename SCL, typename SDA, uint32_t BaudRate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, BaudRate>::delay()
{
	xpcc::delay_us(delayTime);
}
