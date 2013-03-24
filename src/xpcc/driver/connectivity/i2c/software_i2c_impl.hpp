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

#ifndef XPCC__SOFTWARE_I2C_HPP
#	error	"Don't include this file directly, use 'software_i2c.hpp' instead!"
#endif

template <typename Scl, typename Sda, int32_t Frequency>
Scl xpcc::SoftwareI2C<Scl, Sda, Frequency>::scl;
template <typename Scl, typename Sda, int32_t Frequency>
Sda xpcc::SoftwareI2C<Scl, Sda, Frequency>::sda;

template <typename Scl, typename Sda, int32_t Frequency>
xpcc::i2c::Delegate::NextOperation xpcc::SoftwareI2C<Scl, Sda, Frequency>::nextOperation;
template <typename Scl, typename Sda, int32_t Frequency>
xpcc::i2c::Delegate *xpcc::SoftwareI2C<Scl, Sda, Frequency>::myDelegate(0);
template <typename Scl, typename Sda, int32_t Frequency>
uint8_t xpcc::SoftwareI2C<Scl, Sda, Frequency>::errorState(NO_ERROR);

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::initialize(bool pullup)
{
	if (pullup)
	{
#if defined XPCC__CPU_ATXMEGA
		scl.setInput(::xpcc::atxmega::PULLUP);
#elif defined XPCC__CPU_ATMEGA
		scl.setInput(::xpcc::atmega::PULLUP);
#elif defined __ARM_STM32__
		scl.setInput(::xpcc::stm32::PULLUP);
#else
		scl.setInput();
#endif	
#if defined XPCC__CPU_ATXMEGA
		sda.setInput(::xpcc::atxmega::PULLUP);
#elif defined XPCC__CPU_ATMEGA
		sda.setInput(::xpcc::atmega::PULLUP);
#elif defined __ARM_STM32__
		sda.setInput(::xpcc::stm32::PULLUP);
#else
		sda.setInput();
#endif
	} else {
		scl.setInput();
		sda.setInput();
	}
	scl.reset();
	sda.reset();
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::reset(bool error)
{
	if (myDelegate) myDelegate->stopped(error ? xpcc::i2c::Delegate::ERROR_CONDITION : xpcc::i2c::Delegate::SOFTWARE_RESET);
	myDelegate = 0;
}

template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::start(xpcc::i2c::Delegate *delegate)
{
	if (!myDelegate && delegate && delegate->attaching())
	{
		myDelegate = delegate;
		
		while(1)
		{
			DEBUG_SW_I2C('\n');
			DEBUG_SW_I2C('s');
			startCondition();
			xpcc::i2c::Delegate::Starting s = myDelegate->started();
			uint8_t address = s.address;
			
			address &= 0xfe;
			if (s.next == xpcc::i2c::Delegate::READ_OP)
				address |= xpcc::i2c::READ;
			
			if (!write(address))
				return true;
			if (nextOperation == xpcc::i2c::Delegate::RESTART_OP) {DEBUG_SW_I2C('R');}
			nextOperation = s.next;
			
			do {
				switch (nextOperation)
				{
					case xpcc::i2c::Delegate::READ_OP:
					{
						xpcc::i2c::Delegate::Reading r = myDelegate->reading();
						for (uint8_t i=0; i < r.size-1; ++i) {
							*r.buffer++ = read(true);
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('a');
						}
						*r.buffer = read(false);
						DEBUG_SW_I2C('\n');
						DEBUG_SW_I2C('n');
						nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(r.next);
					}
						break;
						
					case xpcc::i2c::Delegate::WRITE_OP:
					{
						xpcc::i2c::Delegate::Writing w = myDelegate->writing();
						for (uint8_t i=0; i < w.size; ++i) {
							if (!write(*w.buffer++))
								return true;
							DEBUG_SW_I2C('\n');
							DEBUG_SW_I2C('A');
						}
						nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(w.next);
					}
						break;
					
					case xpcc::i2c::Delegate::STOP_OP:
						DEBUG_SW_I2C('S');
						myDelegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
						myDelegate = 0;
						stopCondition();
						return true;
						
					default:
						break;
				}
			} while (nextOperation != xpcc::i2c::Delegate::RESTART_OP);
		}
	}
	return false;
}

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t
xpcc::SoftwareI2C<Scl, Sda, Frequency>::getErrorState()
{
	return errorState;
}

// MARK: - private
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::error()
{
	DEBUG_SW_I2C('E');
	stopCondition();
	errorState = DATA_NACK;
	reset(true);
}

// MARK: bus control
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::startCondition()
{
	if (sda.read() == gpio::LOW)
	{// startcondition needs a high sda and scl, so we adjust here
		if (scl.read() == gpio::HIGH)
		{// a very illegal state
			// avoid generating a stop condition and hope no other is writing on the bus
			scl.setOutput(0);
			delay();
			sda.setInput();
			delay();
			scl.setInput();
			delay();
		}
		else { //  we are doing a restart
			sda.setInput();
			delay();
		}
	}
	else if(scl.read() == gpio::LOW){
		scl.setInput();
		delay();
	}
	
	// here both pins are HIGH, ready for start
	sda.setOutput(0);
	delay();
	scl.setOutput(0);
	delay();
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::stopCondition()
{
	scl.setOutput(0);
	sda.setOutput(0);
	
	delay();
	scl.setInput();
	delay();
	sda.setInput();
	delay();
}

// MARK: byte operations
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::write(uint8_t data)
{
	for(uint8_t i = 0; i < 8; ++i)
	{
		writeBit(data & 0x80);
		data <<= 1;
	}
	
	// release sda
	sda.setInput();
	
	// return acknowledge bit
	if (readBit()) {
		DEBUG_SW_I2C('N');
		error();
		return false;
	}
	return true;
}

template <typename Scl, typename Sda, int32_t Frequency>
uint8_t
xpcc::SoftwareI2C<Scl, Sda, Frequency>::read(bool ack)
{
	sda.setInput();
	
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
	sda.setInput();
	
	return data;
}

// MARK: bit operations
template <typename Scl, typename Sda, int32_t Frequency>
bool
xpcc::SoftwareI2C<Scl, Sda, Frequency>::readBit()
{
	delay();
	scl.setInput();
	
	delay();
	while (scl.read() == gpio::LOW)
		;
	
	bool bit = sda.read();
	
	scl.setOutput(0);
	
	return bit;
}

template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::writeBit(bool bit)
{
	if (bit) {
		sda.setInput();
	}
	else {
		sda.setOutput(0);
	}
	delay();
	
	scl.setInput();
	delay();
	
	while (scl.read() == gpio::LOW)
		;
	
	scl.setOutput(0);
}

// ----------------------------------------------------------------------------
template <typename Scl, typename Sda, int32_t Frequency>
void
xpcc::SoftwareI2C<Scl, Sda, Frequency>::delay()
{
	xpcc::delay_us(delayTime);
}
