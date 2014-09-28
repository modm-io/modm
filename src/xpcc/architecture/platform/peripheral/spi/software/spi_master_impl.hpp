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
xpcc::SpiTransaction *xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::delegate(0);

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
xpcc::SpiTransaction::Transmission xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::transmission;

// ----------------------------------------------------------------------------
template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
template< class clockSource, uint32_t baudrate, uint16_t tolerance >
void ALWAYS_INLINE
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::initialize()
{
	SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::template initialize< clockSource, baudrate, tolerance >();
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
bool ALWAYS_INLINE
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::start(xpcc::SpiTransaction *transaction)
{
	return startBlocking(transaction);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
bool
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::startBlocking(xpcc::SpiTransaction *transaction)
{
	if (!delegate && transaction && transaction->attaching())
	{
		delegate = transaction;
		DEBUG_SW_SPI('\n');
		DEBUG_SW_SPI('A');

		while(1)
		{
			transmission.length = 0;
			transmission.next = xpcc::Spi::Operation::Stop;
			delegate->transmitting(transmission);

			uint8_t tx_byte = 0xff;
			uint8_t rx_byte;

			while(transmission.length-- > 0)
			{
				if (transmission.writeBuffer) tx_byte = *transmission.writeBuffer++;

				rx_byte = SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::writeReadBlocking(tx_byte);
				DEBUG_SW_SPI('w');

				if (transmission.readBuffer) *transmission.readBuffer++ = rx_byte;
			}
			DEBUG_SW_SPI(' ');

			if (transmission.next == xpcc::Spi::Operation::Stop)
			{
				DEBUG_SW_SPI('D');
				delegate->detaching(xpcc::Spi::DetachCause::NormalStop);
				delegate = 0;
				return true;
			}
		}
	}
	return false;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void inline
xpcc::SoftwareSpiMaster<SCK, MOSI, MISO, Baudrate>::reset(DetachCause cause)
{
	if (delegate) delegate->detaching(cause);
	delegate = 0;
}
