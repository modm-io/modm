// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__FT245_HPP
#	error	"Don't include this file directly, use 'ft245.hpp' instead!"
#endif

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
PORT xpcc::Ft245<PORT, RD, WR, RXF, TXE>::port;

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
RD xpcc::Ft245<PORT, RD, WR, RXF, TXE>::rd;

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
WR xpcc::Ft245<PORT, RD, WR, RXF, TXE>::wr;

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
RXF xpcc::Ft245<PORT, RD, WR, RXF, TXE>::rxf;

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
TXE xpcc::Ft245<PORT, RD, WR, RXF, TXE>::txe;

// ----------------------------------------------------------------------------
template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
bool
xpcc::Ft245<PORT, RD, WR, RXF, TXE>::read(uint8_t &c)
{
	// When RXF is high, do not read data from the FIFO
	if (rxf.read()) {
		return false;
	}
	else {
		// port is assumed to be input
		rd.reset();
		rxf.read(); // waste time
		c = port.read();
		rd.set();
	}
	return true;
}

// ----------------------------------------------------------------------------
template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
uint8_t
xpcc::Ft245<PORT, RD, WR, RXF, TXE>::read(uint8_t *buffer, uint8_t n)
{
	uint8_t rcvd = 0;
	uint8_t delay = 20;		// TODO Make depend on CPU frequency
	while (1)
	{
		if (rxf.read())
		{
			// no new data, but wait a little longer
			if (--delay == 0) {
				return rcvd;
			}
		}
		else {
			// data available
			rd.reset();
			rxf.read(); // waste time
			*buffer++ = port.read();
			rd.set();
			if (++rcvd == n) {
				return rcvd;
			}
			delay = 20;
		}
	}
}

// ----------------------------------------------------------------------------
template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
void
xpcc::Ft245<PORT, RD, WR, RXF, TXE>::write(uint8_t c)
{
	// When TXE is high do not write data into the FIFO
	while (txe.read())
		;
	
	wr.set();
	port.setOutput();
	port.write(c);
	wr.reset();
	port.setInput();
}

// ----------------------------------------------------------------------------
template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
void
xpcc::Ft245<PORT, RD, WR, RXF, TXE>::write(const uint8_t *buffer, uint8_t n)
{
	port.setOutput();
	
	for (uint_fast8_t i = 0; i < n; ++i)
	{
		wr.set();
		port.write(*buffer++);

		// When TXE is high do not write data into the FIFO
		while (txe.read())
			;
		
		// data is written to the FIFO on the falling edge of WR
		wr.reset();
	}
	port.setInput();
}
