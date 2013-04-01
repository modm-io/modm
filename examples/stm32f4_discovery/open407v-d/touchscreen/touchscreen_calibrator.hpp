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

#ifndef XPCC__TOUCHSCREEN_CALIBRATOR_HPP
#define XPCC__TOUCHSCREEN_CALIBRATOR_HPP

namespace xpcc
{
	class TouchscreenCalibrator
	{
	public:
		TouchscreenCalibrator();
		
		/**
		 * 
		 */
		bool
		calibrate(xpcc::glcd::Point * display, xpcc::glcd::Point * sample);
		
		void
		translate(xpcc::glcd::Point * raw, xpcc::glcd::Point * translated);
		
	private:
		float An;
		float Bn;
		float Cn;
		float Dn;
		float En;
		float Fn;
		float scale;
	};
}

#endif // XPCC__TOUCHSCREEN_CALIBRATOR_HPP

#ifndef XPCC__TOUCHSCREEN_CALIBRATOR_HPP
#	error "Don't include this file directly, use touchscreen_calibrator.hpp instead!"
#endif

xpcc::TouchscreenCalibrator::TouchscreenCalibrator() :
	scale(0.f)
{
}

bool
xpcc::TouchscreenCalibrator::calibrate(
		xpcc::glcd::Point * display, xpcc::glcd::Point * sample)
{
	// K��(X0��X2) (Y1��Y2)��(X1��X2) (Y0��Y2)
	scale =	((sample[0].x - sample[2].x) * (sample[1].y - sample[2].y)) -
			((sample[1].x - sample[2].x) * (sample[0].y - sample[2].y));
	
	if (scale == 0) {
		return false;
	}
	
	// A��((XD0��XD2) (Y1��Y2)��(XD1��XD2) (Y0��Y2))��K
	An =	((display[0].x - display[2].x) * (sample[1].y - sample[2].y)) -
			((display[1].x - display[2].x) * (sample[0].y - sample[2].y));
	// B��((X0��X2) (XD1��XD2)��(XD0��XD2) (X1��X2))��K	*/
	Bn =	((sample[0].x - sample[2].x) * (display[1].x - display[2].x)) -
			((display[0].x - display[2].x) * (sample[1].x - sample[2].x));
	// C��(Y0(X2XD1��X1XD2)+Y1(X0XD2��X2XD0)+Y2(X1XD0��X0XD1))��K */
	Cn =	(sample[2].x * display[1].x - sample[1].x * display[2].x) * sample[0].y +
			(sample[0].x * display[2].x - sample[2].x * display[0].x) * sample[1].y +
			(sample[1].x * display[0].x - sample[0].x * display[1].x) * sample[2].y;
	
	// D��((YD0��YD2) (Y1��Y2)��(YD1��YD2) (Y0��Y2))��K	*/
	Dn =	((display[0].y - display[2].y) * (sample[1].y - sample[2].y)) -
			((display[1].y - display[2].y) * (sample[0].y - sample[2].y));
	// E��((X0��X2) (YD1��YD2)��(YD0��YD2) (X1��X2))��K	*/
	En =	((sample[0].x - sample[2].x) * (display[1].y - display[2].y)) -
			((display[0].y - display[2].y) * (sample[1].x - sample[2].x));
	// F��(Y0(X2YD1��X1YD2)+Y1(X0YD2��X2YD0)+Y2(X1YD0��X0YD1))��K */
	Fn =	(sample[2].x * display[1].y - sample[1].x * display[2].y) * sample[0].y +
			(sample[0].x * display[2].y - sample[2].x * display[0].y) * sample[1].y +
			(sample[1].x * display[0].y - sample[0].x * display[1].y) * sample[2].y;
	
	return true;
}

void
xpcc::TouchscreenCalibrator::translate(xpcc::glcd::Point * raw, xpcc::glcd::Point * translated)
{
	if (scale != 0)
	{
		/* XD = AX+BY+C */
		translated->x =
				((An * raw->x) +
				 (Bn * raw->y) + Cn) / scale;
		/* YD = DX+EY+F */
		translated->y =
				((Dn * raw->x)	+
				 (En * raw->y) + Fn) / scale;
	}
}
