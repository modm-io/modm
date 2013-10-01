// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__KS0108_HPP
	#error	"Don't include this file directly, use 'ks0108.hpp' instead!"
#endif

/* from the Ks0108b datasheet:
 * 
 *   f_clk = 50 .. 400kHz
 *   t_clk = 1/f_clk = 2.5 .. 20µs
 *   
 *   e_high_min = 450ns
 *   e_low_min = 450ns
 *   data_delay = 320ns
 * 
 *   DB7 busy flag (high = busy, low = ready)
 *   t_busy = 1-3 * t_clk
 *          = 2.5 .. 60µs (depending on executed command and specific display)
 */

#define	KS0108_DISPLAY_ON 		0x3f
#define KS0108_SET_Y_ADDRESS	0x40	// + address (0..63)
#define KS0108_SET_X_PAGE		0xB8	// + page (0..7)
#define KS0108_START_LINE		0xC0	// + z address (0..63)

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::initialize()
{
	// configure pins
	e.reset();
	e.setOutput();
	
	rw.reset();
	rw.setOutput();
	
	rs.reset();
	rs.setOutput();
	
	cs1.reset();
	cs1.setOutput();
	
	cs2.reset();
	cs2.setOutput();
	
	port.setInput();
	
	rs.reset();		// select command mode
	
	this->selectLeftChip();
	this->waitBusy();
	this->writeCommand(KS0108_DISPLAY_ON);
	this->waitBusy();
	this->writeCommand(KS0108_START_LINE | 0);
	
	this->selectRightChip();
	this->waitBusy();
	this->writeCommand(KS0108_DISPLAY_ON);
	this->waitBusy();
	this->writeCommand(KS0108_START_LINE | 0);
	
	this->clear();
	this->update();
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::update()
{
	this->selectLeftChip();
	this->waitBusy();
	this->writeCommand(KS0108_SET_Y_ADDRESS | 0);
	
	this->selectRightChip();
	this->waitBusy();
	this->writeCommand(KS0108_SET_Y_ADDRESS | 0);
	
	for (uint8_t page = 0; page < 8; ++page)
	{
		this->selectLeftChip();
		this->waitBusy();
		this->writeCommand(KS0108_SET_X_PAGE | page);
		
		this->selectRightChip();
		this->waitBusy();
		this->writeCommand(KS0108_SET_X_PAGE | page);
		
		uint8_t i;
		for (i = 0; i < 64; ++i)
		{
			this->selectLeftChip();
			this->waitBusy();
			this->writeData(this->buffer[i][page]);
			
			this->selectRightChip();
			this->waitBusy();
			this->writeData(this->buffer[i + 64][page]);
		}
	}
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeByte(uint8_t data)
{
	port.setOutput();
	port.write(data);
	rw.reset();
	
	delay_us(DATA_SET_UP_TIME);
	
	e.set();
	delay_us(MIN_E_HIGH_TIME);
	
	e.reset();
	delay_us(MIN_E_LOW_TIME);
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
uint8_t
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::readByte()
{
	port.setInput();
	rw.set();
	
	delay_us(DATA_SET_UP_TIME);
	
	e.set();
	delay_us(MIN_E_HIGH_TIME);
	
	uint8_t data = port.read();
	
	e.reset();
	delay_us(MIN_E_LOW_TIME);
	
	return data;
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::waitBusy()
{
	rs.reset();
	
	uint8_t isBusy;
	uint8_t iteration = 60;		// maximum busy wait of ~60µs
								// (~1µs per iteration)
	do {
		isBusy = this->readByte() & 0x80;
	} while (isBusy && (--iteration > 0));
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeData(uint8_t data)
{
	rs.set();
	this->writeByte(data);
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
xpcc::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeCommand(uint8_t command)
{
	rs.reset();
	this->writeByte(command);
}
