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

#ifndef XPCC__ADS7843_HPP
#	error "Don't include this file directly, use 'ads7843.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
xpcc::Ads7843<Spi, Cs, Int>::initialize()
{

}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
bool
xpcc::Ads7843<Spi, Cs, Int>::getAverage(uint16_t * buffer, int16_t & value)
{
	uint16_t temp[3];
	temp[0] = (buffer[0] + buffer[1] + buffer[2]) / 3;
	temp[1] = (buffer[3] + buffer[4] + buffer[5]) / 3;
	temp[2] = (buffer[6] + buffer[7] + buffer[8]) / 3;

	uint16_t m0 = abs(temp[0] - temp[1]);
	uint16_t m1 = abs(temp[1] - temp[2]);
	uint16_t m2 = abs(temp[2] - temp[0]);

	if (m0 > threshold && m1 > threshold && m2 > threshold) {
		return false;
	}

	if (m0 < m1)
	{
		if (m2 < m0) {
			value = (temp[0] + temp[2]) / 2;
		}
		else {
			value = (temp[0] + temp[1]) / 2;
		}
	}
	else if (m2 < m1) {
		value = (temp[0] + temp[2]) / 2;
	}
	else {
		value = (temp[1] + temp[2]) / 2;
	}

	return true;
}

template <typename Spi, typename Cs, typename Int>
bool
xpcc::Ads7843<Spi, Cs, Int>::read(glcd::Point * point)
{
	uint_fast8_t count = 0;
	uint16_t bufferX[9];
	uint16_t bufferY[9];

	do
	{
		bufferX[count] = readX();
		bufferY[count] = readY();
		count++;
	} while (!Int::read() && count < 9);

	if (count == 9) {
		glcd::Point p;
		if (getAverage(bufferX, p.x) && getAverage(bufferY, p.y)) {
			*point = p;
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint16_t
xpcc::Ads7843<Spi, Cs, Int>::readData(uint8_t command)
{
	Cs::reset();
	xpcc::delayMicroseconds(1);	// xpcc::delay_ns(100);
	Spi::transferBlocking(command);
	xpcc::delayMicroseconds(1);

	uint16_t temp = Spi::transferBlocking(0x00);
	temp <<= 8;
	xpcc::delayMicroseconds(1);

	temp |= Spi::transferBlocking(0x00);
	temp >>= 3;

	Cs::set();

	return (temp & 0xfff);
}
