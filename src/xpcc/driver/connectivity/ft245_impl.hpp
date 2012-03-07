// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: ft245_impl.hpp 692 2012-01-13 23:51:02Z dergraaf $
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

template <typename PORT, typename RD, typename WR, typename RXF, typename TXE>
void
xpcc::Ft245<PORT, RD, WR, RXF, TXE>::initialize()
{
	rd.setOutput(xpcc::gpio::HIGH);
	wr.setOutput(xpcc::gpio::HIGH);
	rxf.setInput(xpcc::atxmega::PULLUP);
	txe.setInput(xpcc::atxmega::PULLUP);
	port.setInput(xpcc::atxmega::PULLUP);
}

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
