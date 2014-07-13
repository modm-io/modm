// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_LINEAR_ANIMATION_HPP
#	error	"Don't include this file directly, use 'xpcc/ui/animation/base.hpp' instead!"
#endif
// ----------------------------------------------------------------------------

template< typename T >
xpcc::ui::Animation<T>::Animation(T &value)
:	currentValue(value), endValue(0), animationTime(0), previous(0)
{
}

template< typename T >
void inline
xpcc::ui::Animation<T>::setValue(T value)
{
	animationTime = 0;
	currentValue = value;
}

template< typename T >
T ALWAYS_INLINE
xpcc::ui::Animation<T>::getValue() const
{
	return currentValue;
}

template< typename T >
bool ALWAYS_INLINE
xpcc::ui::Animation<T>::isAnimating() const
{
	return (animationTime > 0);
}

template< typename T >
void ALWAYS_INLINE
xpcc::ui::Animation<T>::stop()
{
	animationTime = 0;
	endValue = currentValue;
	computations.deltaValue = 0;
}

template< typename T >
bool
xpcc::ui::Animation<T>::animateTo(TimeType time, T value)
{
	// if the time is zero, or the value is already reached, set the value immediately
	if (time == 0 || value == currentValue) {
		setValue(value);
		return false;
	}

	endValue = value;
	animationTime = time;
	computations.initialize(currentValue, endValue, animationTime);
	previous = xpcc::Clock::now();
	return true;
}

template< typename T >
bool
xpcc::ui::Animation<T>::update()
{
	// this should be called exactly once every 1 ms
	// but if the clock gets incremented by more than 1 ms, or the main loop is
	// busy, then we need to count these "missing" steps and apply them.

	// if we are not fading at the moment, we do not need to check anything
	if (animationTime > 0)
	{
		// buffer the delta time
		xpcc::Timestamp now = xpcc::Clock::now();
		// this cast requires us to be updates once at least every 255ms
		// If this method is not called every few ms, the animation does
		// not look good anyways, so this limitation is okay.
		uint8_t delta = (now - previous).getTime();

		// check if at least 1ms has passed
		if (delta)
		{
			// save the current time for the next comparison
			previous = now;

			// update the values for the number of passed ms
			while (delta--)
			{
				if (--animationTime == 0) {
					animationTime = 0;
					currentValue = endValue;
					return true;
				}

				currentValue = computations.step();
			}

			return true;
		}
	}

	return false;
}
