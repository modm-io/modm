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

template <typename SCL, typename SDA, uint32_t Baudrate>
SCL xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::scl;
template <typename SCL, typename SDA, uint32_t Baudrate>
SDA xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::sda;

template <typename SCL, typename SDA, uint32_t Baudrate>
xpcc::I2c::Operation xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::nextOperation;
template <typename SCL, typename SDA, uint32_t Baudrate>
xpcc::I2cDelegate *xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::myDelegate(0);
template <typename SCL, typename SDA, uint32_t Baudrate>
xpcc::I2cMaster::Error xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::errorState(xpcc::I2cMaster::Error::NoError);

// ----------------------------------------------------------------------------
template <typename SCL, typename SDA, uint32_t Baudrate>
template <class clockSource, uint32_t baudrate, uint16_t tolerance>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::initialize()
{
	scl.set();
	sda.set();
}

template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::reset(DetachCause cause)
{
	if (myDelegate) myDelegate->stopped(cause);
	myDelegate = 0;
}

template <typename SCL, typename SDA, uint32_t Baudrate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::start(xpcc::I2cDelegate *delegate)
{
	if (!myDelegate && delegate && delegate->attaching())
	{
		myDelegate = delegate;

		while(1)
		{
			DEBUG_SW_I2C('\n');
			DEBUG_SW_I2C('s');
			startCondition();

			xpcc::I2cDelegate::Starting s = myDelegate->starting();
			uint8_t address = s.address;

			address &= 0xfe;
			if (s.next == xpcc::I2c::Operation::Read)
				address |= xpcc::I2c::Read;

			if (!write(address))
				return true;
			if (nextOperation == xpcc::I2c::Operation::Restart) {DEBUG_SW_I2C('R');}
			nextOperation = s.next;

			do {
				switch (nextOperation)
				{
					case xpcc::I2c::Operation::Read:
					{
						xpcc::I2cDelegate::Reading r = myDelegate->reading();
						for (uint8_t i=0; i < r.size-1; ++i) {
							*r.buffer++ = read(true);
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('a');
						}
						*r.buffer = read(false);
						DEBUG_SW_I2C('\n');
						DEBUG_SW_I2C('n');
						nextOperation = static_cast<xpcc::I2c::Operation>(r.next);
					}
						break;

					case xpcc::I2c::Operation::Write:
					{
						xpcc::I2cDelegate::Writing w = myDelegate->writing();
						for (uint8_t i=0; i < w.size; ++i) {
							if (!write(*w.buffer++))
								return true;
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('A');
						}
						nextOperation = static_cast<xpcc::I2cDelegate::Operation>(w.next);
					}
						break;

					case xpcc::I2c::Operation::Stop:
						DEBUG_SW_I2C('S');
						myDelegate->stopped(xpcc::I2c::DetachCause::NormalStop);
						myDelegate = 0;
						stopCondition();
						return true;

					default:
						break;
				}
			} while (nextOperation != xpcc::I2c::Operation::Restart);
		}
	}
	return false;
}

template <typename SCL, typename SDA, uint32_t Baudrate>
xpcc::I2cMaster::Error
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::getErrorState()
{
	return errorState;
}

// MARK: - private
template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::error()
{
	DEBUG_SW_I2C('E');
	stopCondition();
	errorState = Error::DataNack;
	reset(DetachCause::ErrorCondition);
}

// MARK: bus control
template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::startCondition()
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

template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::stopCondition()
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
template <typename SCL, typename SDA, uint32_t Baudrate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}

	// release sda
	sda.set();

	// return acknowledge bit
	if (readBit()) {
		DEBUG_SW_I2C('N');
		error();
		return false;
	}
	return true;
}

template <typename SCL, typename SDA, uint32_t Baudrate>
uint8_t
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::read(bool ack)
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
template <typename SCL, typename SDA, uint32_t Baudrate>
bool
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::readBit()
{
	delay();
	sclSetAndWait();

	delay();

	bool bit = sda.read();

	scl.reset();

	return bit;
}

template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::writeBit(bool bit)
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

template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::sclSetAndWait()
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
template <typename SCL, typename SDA, uint32_t Baudrate>
void
xpcc::SoftwareI2cMaster<SCL, SDA, Baudrate>::delay()
{
	xpcc::delay_us(delayTime);
}
