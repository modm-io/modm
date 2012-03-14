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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SCP1000_HPP
	#error	"Don't include this file directly, use 'scp1000.hpp' instead!"
#endif

template < typename Spi, typename Cs, typename Int >
Spi xpcc::Scp1000<Spi, Cs, Int>::spi;

template < typename Spi, typename Cs, typename Int >
Cs xpcc::Scp1000<Spi, Cs, Int>::chipSelect;

template < typename Spi, typename Cs, typename Int >
Int xpcc::Scp1000<Spi, Cs, Int>::interruptPin;

template < typename Spi, typename Cs, typename Int >
bool xpcc::Scp1000<Spi, Cs, Int>::newTemperature(false);

template < typename Spi, typename Cs, typename Int >
bool xpcc::Scp1000<Spi, Cs, Int>::newPressure(false);

template < typename Spi, typename Cs, typename Int >
uint8_t xpcc::Scp1000<Spi, Cs, Int>::temperature[2];

template < typename Spi, typename Cs, typename Int >
uint8_t xpcc::Scp1000<Spi, Cs, Int>::pressure[3];

// ----------------------------------------------------------------------------
template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::initialize(Operation opMode)
{
	chipSelect.setOutput();
	chipSelect.set();
#if defined XPCC__CPU_ATXMEGA
	interruptPin.setInput(::xpcc::atxmega::PULLDOWN);
#else
	interruptPin.setInput();
#endif
	bool result;
	// Reset the chip
	result = reset();
	if (result) {
		result &= setOperation(opMode);
	}
	return result;
}

template < typename Spi, typename Cs, typename Int >
void
xpcc::Scp1000<Spi, Cs, Int>::readTemperature()
{
	read16BitRegister(REGISTER_TEMPOUT, &temperature[0]);
	newTemperature = true;
}

template < typename Spi, typename Cs, typename Int >
void
xpcc::Scp1000<Spi, Cs, Int>::readPressure()
{
	pressure[0] = read8BitRegister(REGISTER_DATARD8);
	read16BitRegister(REGISTER_DATARD16, &pressure[1]);
	
	newPressure = true;
}

template < typename Spi, typename Cs, typename Int >
uint8_t*
xpcc::Scp1000<Spi, Cs, Int>::getTemperature()
{
	newTemperature = false;
	return &temperature[0];
}

template < typename Spi, typename Cs, typename Int >
uint8_t*
xpcc::Scp1000<Spi, Cs, Int>::getPressure()
{
	newPressure = false;
	return &pressure[0];
}


template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::setOperation(Operation opMode)
{
	writeRegister(REGISTER_OPERATION, opMode);
	
	uint8_t retries = 16;
	// wait for the sensor to complete setting the operation
	while (--retries && (readStatus(true) & OPERATION_STATUS_RUNNING)) {
		xpcc::delay_ms(1);
	}
	
	// The sensor took too long to complete the operation
	if (retries)
		return true;
	
	return false;
}

template < typename Spi, typename Cs, typename Int >
uint8_t
xpcc::Scp1000<Spi, Cs, Int>::readStatus(bool opStatus)
{
	Register address = REGISTER_STATUS;
	if (opStatus) {
		address = REGISTER_OPSTATUS;
	}
	return read8BitRegister(address);
}

template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::reset(uint8_t timeout=50)
{
	writeRegister(REGISTER_RSTR, RESET);
	
	// wait a bit to give the Scp1000 some time to restart
	xpcc::delay_ms(151);
	
	uint8_t retries = timeout;
	// wait for the sensor to complete start up, this should take 160ms
	while (--retries && (readStatus() & STATUS_STARTUP_RUNNING_bm)) {
		xpcc::delay_ms(1);
	}
	
	if (retries > 0) {
		return true;
	}
	
	// The sensor took too long to start up
	return false;
}

template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::isNewTemperatureAvailable()
{
	return newTemperature;
}

template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::isNewPressureAvailable()
{
	return newPressure;
}

template < typename Spi, typename Cs, typename Int >
bool
xpcc::Scp1000<Spi, Cs, Int>::isNewDataReady()
{
	return interruptPin.read();
}

template < typename Spi, typename Cs, typename Int >
void
xpcc::Scp1000<Spi, Cs, Int>::writeRegister(Register reg, uint8_t data)
{
	chipSelect.reset();
	spi.write(reg<<2|0x02);
	spi.write(data);
	chipSelect.set();
}

template < typename Spi, typename Cs, typename Int >
uint8_t
xpcc::Scp1000<Spi, Cs, Int>::read8BitRegister(Register reg)
{
	uint8_t result;
	chipSelect.reset();
	spi.write(reg<<2);
	result = spi.write(0x00);
	chipSelect.set();
	return result;
}

template < typename Spi, typename Cs, typename Int >
void
xpcc::Scp1000<Spi, Cs, Int>::read16BitRegister(Register reg, uint8_t *buffer)
{
	chipSelect.reset();
	spi.write(reg<<2);
	buffer[0] = spi.write(0x00);
	buffer[1] = spi.write(0x00);
	chipSelect.set();
}
