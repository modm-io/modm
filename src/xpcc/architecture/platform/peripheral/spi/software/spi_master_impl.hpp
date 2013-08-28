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
//#define DEBUG_SW_SPI(x) xpcc::stm::Uart1::write(x)
#define DEBUG_SW_SPI(x)

#include <xpcc/architecture/driver/atomic.hpp>

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::timingMode(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
xpcc::SpiDelegate *xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::myDelegate(0);


// ----------------------------------------------------------------------------
template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
template <uint32_t datarate>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::initialize(Mode mode)
{
	xpcc::atomic::Lock lock;
	timingMode = static_cast<uint8_t>(mode);
	SCK::set(timingMode & 0b10);
	MOSI::reset();
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::reset(DetachCause cause)
{
	if (myDelegate) myDelegate->detaching(cause);
	myDelegate = 0;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::start(xpcc::SpiDelegate *delegate)
{
	return startBlocking(delegate);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
bool
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::startBlocking(xpcc::SpiDelegate *delegate)
{
	if (!myDelegate && delegate && delegate->attaching())
	{
		myDelegate = delegate;
		DEBUG_SW_SPI('\n');
		DEBUG_SW_SPI('A');

		while(1)
		{
			xpcc::SpiDelegate::Transmission t = myDelegate->transmitting();

			uint8_t tx_byte = 0xff;
			uint8_t rx_byte;

			for (std::size_t ii = 0; ii < t.size; ii++)
			{
				if (t.writeBuffer) tx_byte = t.writeBuffer[static_cast<uint8_t>(t.options) & 0b10 ? ii : t.size-ii-1];

				rx_byte = writeReadBlocking(tx_byte);
				DEBUG_SW_SPI('w');

				if (t.readBuffer) t.readBuffer[static_cast<uint8_t>(t.options) & 0b01 ? ii : t.size-ii-1] = rx_byte;
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
	return false;
}

//-----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
uint8_t
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::writeReadBlocking(uint8_t data)
{
	uint8_t input = 0;

	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (timingMode & 0b01)
			delay();

		input <<= 1;
		if (data & 0x80) {
			MOSI::set();
		}
		else {
			MOSI::reset();
		}
		// CPHA=0, sample on rising edge
		if (!(timingMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> High
		SCK::set(!(timingMode & 0b10));

		// CPHA=1, sample on falling edge
		if (timingMode & 0b01)
			delay();

		if (MISO::read()) {
			input |= 1;
		}
		data <<= 1;

		// CPHA=0, sample on rising edge
		if (!(timingMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(timingMode & 0b10);
	}

	return input;
}


template <typename SCK, typename MOSI, typename MISO, uint32_t Frequency>
void
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
