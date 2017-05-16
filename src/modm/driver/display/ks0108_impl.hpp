/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_KS0108_HPP
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
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::initialize()
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
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::update()
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
			this->writeData(this->display_buffer[i][page]);
			
			this->selectRightChip();
			this->waitBusy();
			this->writeData(this->display_buffer[i + 64][page]);
		}
	}
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeByte(uint8_t data)
{
	port.setOutput();
	port.write(data);
	rw.reset();
	
	delayMicroseconds(DATA_SET_UP_TIME);
	
	e.set();
	delayMicroseconds(MIN_E_HIGH_TIME);
	
	e.reset();
	delayMicroseconds(MIN_E_LOW_TIME);
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
uint8_t
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::readByte()
{
	port.setInput();
	rw.set();
	
	delayMicroseconds(DATA_SET_UP_TIME);
	
	e.set();
	delayMicroseconds(MIN_E_HIGH_TIME);
	
	uint8_t data = port.read();
	
	e.reset();
	delayMicroseconds(MIN_E_LOW_TIME);
	
	return data;
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::waitBusy()
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
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeData(uint8_t data)
{
	rs.set();
	this->writeByte(data);
}

// ----------------------------------------------------------------------------
template < typename E, typename RW, typename RS,
		   typename PIN_CS1, typename PIN_CS2, typename PORT >
void
modm::Ks0108<E, RW, RS, PIN_CS1, PIN_CS2, PORT>::writeCommand(uint8_t command)
{
	rs.reset();
	this->writeByte(command);
}
