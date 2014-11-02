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
:	Animation(value, nullptr)
{
}

template< typename T >
xpcc::ui::Animation<T>::Animation(T &value, Callback_t callback)
:	callback(callback), currentValue(value), endValue(0),
	 animationTime(0), previous(0)
{
}

template< typename T >
void inline
xpcc::ui::Animation<T>::attachCallback(Callback_t callback)
{
	this->callback = callback;
}

template< typename T >
void inline
xpcc::ui::Animation<T>::setValue(T value)
{
	animationTime = 0;
	currentValue = value;
	if (callback) callback(currentValue);
}

template< typename T >
T inline
xpcc::ui::Animation<T>::getValue() const
{
	return currentValue;
}

template< typename T >
bool inline
xpcc::ui::Animation<T>::isAnimating() const
{
	return (animationTime > 0);
}

template< typename T >
void inline
xpcc::ui::Animation<T>::stop()
{
	endValue = currentValue;
	interpolation.stop();
	setValue(currentValue);
}

template< typename T >
bool
xpcc::ui::Animation<T>::animateTo(T value, TimeType time)
{
	if (value == currentValue || time == 0)
	{
		setValue(value);
		return false;
	}

	endValue = value;
	animationTime = time;
	interpolation.initialize(currentValue, endValue, animationTime);
	previous = xpcc::Clock::now();
	return true;
}

template< typename T >
bool
xpcc::ui::Animation<T>::update()
{
	// this should be called at least once every 1 ms
	// but if the clock gets incremented by more than 1 ms, or the main loop is
	// busy, then we need to count these "missing" steps and apply them.

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
				if (--animationTime == 0)
				{
					setValue(endValue);
					return true;
				}
				currentValue = interpolation.step();
			}

			if (callback) callback(currentValue);

			return true;
		}
	}

	return false;
}
