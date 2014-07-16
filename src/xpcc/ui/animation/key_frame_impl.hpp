// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_KEY_FRAME_HPP
#	error	"Don't include this file directly, use 'xpcc/ui/animator/key_frame.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template< typename T, class... Args >
xpcc::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(Animation<Args>&... animator)
:	animator{(&animator)...}, keyFrames(0), numberOfFrames(0), currentFrame(-1),
	repeat(-1), mode(modeCycleMask)
{
}

template< typename T, class... Args >
bool
xpcc::ui::KeyFrameAnimationBase<T, Args...>::start(uint8_t repeat)
{
	// there must exist a keyframe array with at least one frame
	if (keyFrames && numberOfFrames > 0) {
		cancel();
		currentFrame = 0;
		this->repeat = repeat - 1;
		return true;
	}
	return false;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::stop()
{
	if (keyFrames)
	{
		for (uint_fast8_t ii = 0; ii < size; ii++)
		{
			animator[ii]->setValue(keyFrames[(mode & reversedMask) ? 0 : numberOfFrames-1].value[ii]);
		}
	}
	cancel();
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::cancel()
{
	currentFrame = -1;
	mode &= ~reversedMask;
	for (uint_fast8_t ii = 0; ii < size; ii++)
	{
		animator[ii]->stop();
	}
	timeout.stop();
}

template< typename T, class... Args >
bool
xpcc::ui::KeyFrameAnimationBase<T, Args...>::isAnimating() const
{
	return (currentFrame != static_cast<uint8_t>(-1));
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::update()
{
	if (currentFrame != static_cast<uint8_t>(-1))
	{
		if (!animator[0]->isAnimating() && timeout.isExpired())
		{
			for (uint_fast8_t ii = 0; ii < size; ii++)
			{
				if (!animator[ii]->animateTo(keyFrames[currentFrame].time, keyFrames[currentFrame].value[ii]))
				{
					timeout.restart(keyFrames[currentFrame].time);
				}
			}

			if (mode & modeCycleMask)
			{
				if (currentFrame == numberOfFrames - 1)
				{
					if (!checkRepeat()) return;
					currentFrame = 0;
				}
				else currentFrame++;
			}
			else
			{
				if (!(mode & reversedMask))
				{
					if (currentFrame == numberOfFrames - 1)
					{
						if (!checkRepeat()) return;
						currentFrame = numberOfFrames - 2;
						mode &= ~reversedMask;
					}
					else currentFrame++;
				}
				else
				{
					if (currentFrame == 0)
					{
						if (!checkRepeat()) return;
						currentFrame = 1;
						mode |= reversedMask;
					}
					else currentFrame--;
				}
			}
		}
	}
}

template< typename T, class... Args >
bool
xpcc::ui::KeyFrameAnimationBase<T, Args...>::checkRepeat()
{
	if (repeat != static_cast<uint8_t>(-1))
	{
		if (repeat == 0)
		{
            repeat = 0;
			currentFrame = -1;
			return false;
		}
		repeat--;
	}
	return true;
}

// MARK: getters and setters

template< typename T, class... Args >
xpcc::ui::KeyFrame<T, xpcc::ui::KeyFrameAnimationBase<T, Args...>::size> *
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getKeyFrames() const
{
	return keyFrames;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::setKeyFrames(KeyFrame<T, size> *frames, uint8_t numberOfFrames)
{
	cancel();
	this->keyFrames = frames;
	this->numberOfFrames = numberOfFrames;
}

template< typename T, class... Args >
uint8_t
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getNumberOfFrames() const
{
	return numberOfFrames;
}

template< typename T, class... Args >
xpcc::ui::KeyFrameAnimationMode const &
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getMode() const
{
	return mode;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::setMode(KeyFrameAnimationMode const &mode)
{
	// this clears the reversed bit mask!
	this->mode = static_cast<uint8_t>(mode);
}

