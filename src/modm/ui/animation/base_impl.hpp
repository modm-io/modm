/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_UI_LINEAR_ANIMATION_HPP
#	error	"Don't include this file directly, use 'modm/ui/animation/base.hpp' instead!"
#endif
// ----------------------------------------------------------------------------

template< typename T >
modm::ui::Animation<T>::Animation(T &value, Handler handler)
:	handler(handler), currentValue(value)
{
}

template< typename T >
void inline
modm::ui::Animation<T>::attachCallback(Handler handler)
{
	this->handler = handler;
}

template< typename T >
void inline
modm::ui::Animation<T>::setValue(T value)
{
	animationTime = 0;
	currentValue = value;
	endValue = value;
	interpolation.stop();
	if (handler) handler(currentValue);
}

template< typename T >
T inline
modm::ui::Animation<T>::getValue() const
{
	return currentValue;
}

template< typename T >
bool inline
modm::ui::Animation<T>::isAnimating() const
{
	return (animationTime > 0);
}

template< typename T >
void inline
modm::ui::Animation<T>::stop()
{
	setValue(currentValue);
}

template< typename T >
bool
modm::ui::Animation<T>::animateTo(T value, uint32_t time)
{
	if(time == 0)
	{
		setValue(value);
		return true;
	}

	endValue = value;
	animationTime = time;
	previous = modm::Clock::now();
	interpolation.initialize(currentValue, endValue, animationTime);
	if (endValue == currentValue and handler) handler(currentValue);
	return true;
}

template< typename T >
bool
modm::ui::Animation<T>::update()
{
	// this should be called at least once every 1 ms
	// but if the clock gets incremented by more than 1 ms, or the main loop is
	// busy, then we need to count these "missing" steps and apply them.

	// are we even running?
	if (animationTime > 0)
	{
		// buffer the delta time
		const modm::ShortTimestamp now = modm::Clock::now();
		// this cast requires us to be updated once at least every 255ms
		// If this method is not called every few ms, the animation does
		// not look good anyways, so this limitation is okay.
		uint_fast8_t delta = (now - previous).count();

		// check if at least 1ms has passed
		if (delta)
		{
			// save the current time for the next comparison
			previous = now;

			// check if we are going to be finished with the animation in delta ms
			if (animationTime <= delta)
			{
				// don't bother with the calculations, just set the final value
				setValue(endValue);
				return true;
			}
			// subtract the current time steps
			animationTime -= delta;

			// run the calculations
			while (delta--) interpolation.step();

			// get the calculated value for this step
			const T newValue = interpolation.getValue();

			if (currentValue != newValue)
			{
				currentValue = newValue;
				// invoke the handler with this value
				if (handler) handler(currentValue);
				return true;
			}
		}
	}

	return false;
}
