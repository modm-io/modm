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

#ifndef XPCC__MCP4922_HPP
	#error	"Don't include this file directly, use 'mcp4922.hpp' instead!"
#endif

template <typename Spi, typename Cs, typename Ldac>
Spi xpcc::Mcp4922<Spi, Cs, Ldac>::spi;

template <typename Spi, typename Cs, typename Ldac>
Cs xpcc::Mcp4922<Spi, Cs, Ldac>::cs;

template <typename Spi, typename Cs, typename Ldac>
Ldac xpcc::Mcp4922<Spi, Cs, Ldac>::ldac;

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::initialize()
{
	spi.initialize();
	
	cs.set();
	cs.setOutput();
	
	ldac.set();
	ldac.setOutput();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::setChannelA(uint16_t value, bool doubleGain)
{
	if (doubleGain) {
		writeRegister(BUF | SHDN | (value & 0x0fff));
	}
	else {
		writeRegister(GA | BUF | SHDN | (value & 0x0fff));
	}
}

template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::setChannelB(uint16_t value, bool doubleGain)
{
	if (doubleGain) {
		writeRegister(CHANNEL_B | BUF | SHDN | (value & 0x0fff));
	}
	else {
		writeRegister(CHANNEL_B | GA | BUF | SHDN | (value & 0x0fff));
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::update()
{
	xpcc::delay_us(0.04);
	ldac.reset();
	xpcc::delay_us(0.1);
	ldac.set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::writeRegister(uint16_t value)
{
	cs.reset();
	
	spi.write(static_cast<uint8_t>(value >> 8));
	spi.write(static_cast<uint8_t>(value & 0xff));
	
	cs.set();
}
