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
 *
 * $Id: ad840x_impl.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__AD840x_HPP
#	error "Don't include this file directly, use 'ad840x.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
void
xpcc::AD840x<Spi, Cs, Rs, Shdn>::initialize()
{
	Spi::initialize();
	Cs::setOutput(xpcc::gpio::HIGH);
	Rs::setOutput(xpcc::gpio::HIGH);
	Shdn::setOutput(xpcc::gpio::HIGH);
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
xpcc::AD840x<Spi, Cs, Rs, Shdn>::reset()
{
	Rs::reset();
	xpcc::delay_us(1);		// wait at least 50ns
	Rs::set();
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
xpcc::AD840x<Spi, Cs, Rs, Shdn>::shutdown()
{
	Shdn::reset();
}

template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
xpcc::AD840x<Spi, Cs, Rs, Shdn>::resume()
{
	Shdn::set();
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
void
xpcc::AD840x<Spi, Cs, Rs, Shdn>::setValue(ad840x::Channel channel, uint8_t data)
{
	Cs::reset();
	
	// only the last 10 bits shifted in are used
	Spi::write(channel & 0x03);
	Spi::write(data);
	
	Cs::set();
}
