/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2012-2013, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TOUCHSCREEN_CALIBRATOR_HPP
#define XPCC_TOUCHSCREEN_CALIBRATOR_HPP

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

#endif // XPCC_TOUCHSCREEN_CALIBRATOR_HPP

#ifndef XPCC_TOUCHSCREEN_CALIBRATOR_HPP
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
