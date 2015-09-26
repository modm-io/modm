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
:	KeyFrameAnimationBase(nullptr,0, animator...)
{
}

template< typename T, class... Args >
template< uint16_t N >
xpcc::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(const KeyFrame<T, size> (&frames)[N], Animation<Args>&... animator)
:	KeyFrameAnimationBase(frames, N, animator...)
{
}

template< typename T, class... Args >
xpcc::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(
		const KeyFrame<T, size> *frames, uint16_t length,
		Animation<Args>&... animator)
:	animator{(&animator)...}, frames(frames), length(length),
	currentFrame(0), repeat(0), mode(modeCycleMask)
{
}

template< typename T, class... Args >
bool
xpcc::ui::KeyFrameAnimationBase<T, Args...>::start(uint8_t repeat)
{
	// sanity check
	if (frames && length > 0)
	{
		// stop all animations
		cancel();
		// start
		currentFrame = 1;
		this->repeat = repeat;
		return true;
	}
	return false;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::stop()
{
	if (frames && length > 0)
	{
		// for all animators
		for (uint_fast8_t ii = 0; ii < size; ii++)
		{
			// it we are running reversed, the "last" frame is the first one!
			animator[ii]->setValue(frames[(mode & reversedMask) ? 0 : length-1].value[ii]);
		}
	}
	cancel();
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::cancel()
{
	// disable
	currentFrame = 0;
	mode &= ~reversedMask;
	for (uint_fast8_t ii = 0; ii < size; ii++)
	{
		animator[ii]->stop();
	}
}

template< typename T, class... Args >
bool
xpcc::ui::KeyFrameAnimationBase<T, Args...>::isAnimating() const
{
	return (currentFrame > 0);
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::update()
{
	// are we running?
	if (currentFrame > 0)
	{
		// wait until the key frame finished
		if (!animator[0]->isAnimating())
		{
			// set the next keyframe
			for (uint_fast8_t ii = 0; ii < size; ii++)
			{
				animator[ii]->animateTo(
						frames[currentFrame-1].value[ii],
						frames[currentFrame-1].length);
			}

			// which frame is next?
			if (mode & modeCycleMask)
			{
				// we are running in a cycle
				// 1...N, 1...N, etc
				if (currentFrame >= length)
				{
					// last frame = N
					if (!checkRepeat()) return;
					// start at the beginning again
					currentFrame = 1;
				}
				else currentFrame++;
			}
			else
			{
				// we are running up, down, up, down
				// 1...N, N...1, etc
				if (!(mode & reversedMask))
				{
					// we are incrementing
					if (currentFrame >= length)
					{
						// next frame = N-1
						currentFrame = length - 1;
						// decrement next
						mode |= reversedMask;
					}
					else currentFrame++;
				}
				else
				{
					// we are decrementing
					if (currentFrame == 1)
					{
						// last frame = 1
						if (!checkRepeat()) return;
						// next frame = 2
						currentFrame = 2;
						// increment next
						mode &= ~reversedMask;
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
	if (repeat == 1)
	{
		// stop running
		currentFrame = 0;
		return false;
	}

	if (repeat > 0) repeat--;
	return true;
}

// MARK: - getters and setters
template< typename T, class... Args >
xpcc::ui::KeyFrame<T, xpcc::ui::KeyFrameAnimationBase<T, Args...>::size> *
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getKeyFrames() const
{
	return frames;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::setKeyFrames(KeyFrame<T, size> *frames, uint16_t length)
{
	cancel();
	this->frames = frames;
	this->length = length;
}

template< typename T, class... Args >
uint16_t
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getLength() const
{
	return length;
}

template< typename T, class... Args >
xpcc::ui::KeyFrameAnimationMode
xpcc::ui::KeyFrameAnimationBase<T, Args...>::getMode() const
{
	return mode;
}

template< typename T, class... Args >
void
xpcc::ui::KeyFrameAnimationBase<T, Args...>::setMode(KeyFrameAnimationMode const mode)
{
	// this clears the reversed bit mask!
	this->mode = static_cast<uint8_t>(mode);
}

