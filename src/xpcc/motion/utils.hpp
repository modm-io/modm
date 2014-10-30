// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MOTION_UTILS
#define XPCC_MOTION_UTILS

#include <xpcc/math/geometry/location_2d.hpp>	// xpcc::Pair
#include <xpcc/math/interpolation.hpp>			// xpcc::interpolation::Linear

/**
 * Small template class that can convert encoder ticks per time to a duty cycle.
 *
 * In order to do so four fixed points are used for linear interpolation.
 * The BaseTime needs to be specified in milliseconds.
 *
 * @author eKiwi <electron.kiwi@gmail.com>
 * @ingroup motion
 */
template<uint16_t BaseTime = 1>
class
TicksPerTimeToPwm
{
public:
	/// Convert encoder ticks per BaseTime to a PWM duty cycle
	static inline int16_t
	interpolate(int16_t ticks)
	{
		return ticksToPwm.interpolate(ticks / static_cast<int16_t>(BaseTime));
	}

private:
	/// 2D Point
	typedef xpcc::Pair<int16_t, int16_t> Point;
	/// Points used for converting encoder ticks to PWM duty cycle
	static const Point supportingPoints[4];
	/// Interpolator
	static xpcc::interpolation::Linear<Point> ticksToPwm;
};


template<uint16_t BaseTime>
const typename TicksPerTimeToPwm<BaseTime>::Point
TicksPerTimeToPwm<BaseTime>::supportingPoints [4] =
		{
			{-300, -(5*300 + 70)},
			{-5, -(5*5 + 70)},
			{5, 5*5 + 70},
			{300,   5*300 + 70},
		};;

template<uint16_t BaseTime>
xpcc::interpolation::Linear<typename TicksPerTimeToPwm<BaseTime>::Point>
TicksPerTimeToPwm<BaseTime>::ticksToPwm(supportingPoints, 4);

#endif /* XPCC_MOTION_UTILS */
