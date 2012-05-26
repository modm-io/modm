// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC__TLC594X_HPP
#	error "Don't include this file directly, use 'tlc594x.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
uint8_t xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::status[CHANNELS*3/2];
template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
uint8_t xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::gs[CHANNELS*3/2];
template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
uint8_t xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::dc[CHANNELS*3/4];

// ----------------------------------------------------------------------------
template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::initialize(uint16_t channels, uint8_t dots, bool writeCH, bool writeDC)
{
	Xlat::setOutput(xpcc::gpio::LOW);
	Vprog::setOutput(xpcc::gpio::LOW);
	Xerr::setInput();
	Spi::setBuffer(CHANNELS*3/2, gs, status);
	
	if (dots != 0xff) setAllDotCorrection(dots, writeDC);
	if (channels != 0xffff) setAllChannels(channels, writeCH);
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::latch()
{
	Xlat::set();
	// datasheet says 20ns but that is unreliable
	// => wait for at least 900ns
	xpcc::delay_us(0.9);
	Xlat::reset();
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::setChannel(uint16_t channel, uint16_t value)
{
	if (channel > CHANNELS-1) return;
	
	/* Grayscale PWM gs has the following format:
	 *      v =channel
	 *      1            0
	 * bbbbbbbbbbbb aaaaaaaaaaaa
	 *   |       \     /      |
	 * bbbbbbbb  bbbbaaaa bbaaaaaa
	 *    0         1         2
	 *    ^ =index
	 * 
	 * This pattern is repeated every 2 channels.
	 * The channel index relation is like this:
	 * channel index
	 *    0      1
	 *    1      0
	 */
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 1;
	
	if (channel & 0x01) {
		gs[index] = static_cast<uint8_t>(value >> 4);
		gs[index+1] = static_cast<uint8_t>(value << 4) | (gs[index+1] & 0x0f);
	} else {
		gs[index] = (static_cast<uint8_t>(value >> 8) & 0x0f) | (gs[index] & 0xf0);
		gs[index+1] = value;
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::setAllChannels(uint16_t value, bool update)
{
	uint8_t first  = static_cast<uint8_t>(value >> 4);
	uint8_t second = static_cast<uint8_t>(value << 4) | (static_cast<uint8_t>(value >> 8) & 0x0f);
	
	for (uint_fast16_t i=0; i < CHANNELS*3/2;) {
		gs[i++] = first;
		gs[i++] = second;
		gs[i++] = static_cast<uint8_t>(value);
	}
	if (update) writeChannels(true);
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
uint16_t
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::getChannel(uint16_t channel)
{
	if (channel > CHANNELS-1) return 0;
	
	uint16_t value;
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 1;
	
	if (channel & 0x01) {
		value = gs[index];
		value = (value << 4) | (gs[index+1] >> 4);
	} else {
		value = (gs[index] & 0x0f);
		value = (value << 8) | gs[index+1];
	}
	return value;
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::setDotCorrection(uint16_t channel, uint8_t value)
{
	if (channel > CHANNELS-1) return;
	
	/* Dot Correction data has the following format:
	 *   v =channel
	 *   3      2      1      0
	 * dddddd cccccc bbbbbb aaaaaa
	 *   |       \      /     |
	 * ddddddcc  ccccbbbb bbaaaaaa
	 *    0         1        2
	 *    ^ =index
	 * 
	 * This pattern is repeated every 4 channels.
	 * The channel index relation is like this:
	 * channel index
	 *    0      2
	 *    1      1
	 *    2      0
	 *    3      0
	 */
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 2;
	
	switch (channel & 0x03)
	{
		case 0:
			dc[index] = (dc[index] & 0xc0) | value;
			break;
			
		case 1:
			dc[index] = (dc[index] & 0xf0) | (value >> 2);
			dc[index+1] = (dc[index+1] & 0x3f) | (value << 6);
			break;
			
		case 2:
			dc[index] = (dc[index] & 0xfc) | (value >> 4);
			dc[index+1] = (dc[index+1] & 0x0f) | (value << 4);
			break;
//		case 3:
		default:
			dc[index] = (dc[index] & 0x03) | (value << 2);
			break;
	}
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
void
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::setAllDotCorrection(uint8_t value, bool update)
{
	uint8_t first  = (value << 2) | (value >> 4);
	uint8_t second = (value << 4) | (value >> 2);
	uint8_t third  = (value << 6) | value;
	
	for (uint_fast16_t i=0; i < CHANNELS*3/4;) {
		dc[i++] = first;
		dc[i++] = second;
		dc[i++] = third;
	}
	if (update) writeDotCorrection();
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
uint8_t
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::getDotCorrection(uint16_t channel)
{
	if (channel > CHANNELS-1) return 0;
	
	uint8_t value;
	uint16_t index = (CHANNELS - 1 - channel) * 3 >> 2;
	
	switch (channel & 0x03)
	{
		case 0:
			value = dc[index] & 0x3f;
			break;
			
		case 1:
			value = ((dc[index] & 0x0f) << 2) | ((dc[index+1] & 0xc0) >> 6);
			break;
			
		case 2:
			value = ((dc[index] & 0x03) << 4) | ((dc[index+1] & 0xf0) >> 4);
			break;
//		case 3:
		default:
			value = (dc[index] & 0xfc) >> 2;
			break;
	}
	return value;
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
bool
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::writeChannels(bool flush)
{
	if (flush)
	{
		if (!Spi::transferSync(xpcc::SpiMaster::TRANSFER_SEND_BUFFER_SAVE_RECEIVE))
			return false;
		latch();
		return true;
	}
	
	return Spi::transfer(xpcc::SpiMaster::TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
}

template<uint16_t CHANNELS, typename Spi, typename Xlat, typename Vprog, typename Xerr>
bool
xpcc::TLC594X<CHANNELS, Spi, Xlat, Vprog, Xerr>::writeDotCorrection()
{
	Vprog::set();
	// set new buffer
	Spi::setBuffer(CHANNELS*3/4, dc);
	
	// transfer
	if (!Spi::transferSync(xpcc::SpiMaster::TRANSFER_SEND_BUFFER_DISCARD_RECEIVE)) {
		// reset Grayscale PWM buffer
		Spi::setBuffer(CHANNELS*3/2, gs, status);
		Vprog::reset();
		return false;
	}
	latch();
	
	// reset Grayscale PWM buffer
	Spi::setBuffer(CHANNELS*3/2, gs, status);
	Vprog::reset();
	
	return true;
}
