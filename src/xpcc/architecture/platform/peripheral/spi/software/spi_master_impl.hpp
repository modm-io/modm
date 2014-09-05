// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SPI_MASTER_HPP
#	error	"Don't include this file directly, use 'spi_master.hpp' instead!"
#endif

// debugging for serious dummies
//#define DEBUG_SW_SPI(x) xpcc::stm32::Uart1::write(x)
#define DEBUG_SW_SPI(x)

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::operationMode(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
xpcc::SpiDelegate *xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::myDelegate(0);


// ----------------------------------------------------------------------------
template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
template <uint32_t datarate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::initialize()
{
	SCK::reset();
	MOSI::reset();
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
bool
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::start(xpcc::SpiDelegate *delegate,
		DataMode mode,
		DataOrder order)
{
	return startBlocking(delegate, mode, order);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
bool
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::startBlocking(xpcc::SpiDelegate *delegate,
		DataMode mode,
		DataOrder order)
{
	if (!myDelegate && delegate)
	{
		operationMode = static_cast<uint8_t>(mode);
		SCK::set(operationMode & 0b10);
		if (order == DataOrder::LsbFirst) operationMode |= 0b100;

		if (delegate->attaching())
		{
			myDelegate = delegate;
			DEBUG_SW_SPI('\n');
			DEBUG_SW_SPI('A');

			while(1)
			{
				xpcc::SpiDelegate::Transmission t = myDelegate->transmitting();

				uint8_t tx_byte = 0xff;
				uint8_t rx_byte;

				for (std::size_t ii = 0; ii < t.length; ii++)
				{
					if (t.writeBuffer) tx_byte = t.writeBuffer[ii];

					rx_byte = writeReadBlocking(tx_byte);
					DEBUG_SW_SPI('w');

					if (t.readBuffer) t.readBuffer[ii] = rx_byte;
				}
				DEBUG_SW_SPI(' ');

				if (t.next == xpcc::Spi::Operation::Stop)
				{
					DEBUG_SW_SPI('D');
					myDelegate->detaching(xpcc::Spi::DetachCause::NormalStop);
					myDelegate = 0;
					return true;
				}
			}
		}
	}
	return false;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::reset(DetachCause cause)
{
	if (myDelegate) myDelegate->detaching(cause);
	myDelegate = 0;
}
//-----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::writeReadBlocking(uint8_t data)
{
	uint8_t input = 0;

	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			input >>= 1;
			MOSI::set(data & 0x01);
		}
		else {
			input <<= 1;
			MOSI::set(data & 0x80);
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> Low
		SCK::set(!(operationMode & 0b10));

		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			if (MISO::read()) input |= 0x80;
			data >>= 1;
		}
		else {
			if (MISO::read()) input |= 0x01;
			data <<= 1;
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(operationMode & 0b10);
	}

	return input;
}


template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::delay()
{
	xpcc::delayMicroseconds(delayTime);
}
