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

#ifndef MODM_UI_KEY_FRAME_HPP
#	error	"Don't include this file directly, use 'modm/ui/animator/key_frame.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template< typename T, class... Args >
modm::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(AnimationBase<Args>&... animators)
:	KeyFrameAnimationBase(nullptr,0, animators...)
{
}

template< typename T, class... Args >
template< uint16_t N >
modm::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(const KeyFrame<T, size> (&frames)[N], AnimationBase<Args>&... animators)
:	KeyFrameAnimationBase(frames, N, animators...)
{
}

template< typename T, class... Args >
modm::ui::KeyFrameAnimationBase<T, Args...>::KeyFrameAnimationBase(
		const KeyFrame<T, size> *frames, uint16_t length,
		AnimationBase<Args>&... animators)
:	animators{(&animators)...}, frames(frames), length(length),
	currentFrame(0), repeat(0), mode(modeCycleMask)
{
}

template< typename T, class... Args >
bool
modm::ui::KeyFrameAnimationBase<T, Args...>::start(uint8_t repeat)
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
modm::ui::KeyFrameAnimationBase<T, Args...>::stop()
{
	if (frames && length > 0)
	{
		// for all animators
		for (uint_fast8_t ii = 0; ii < size; ii++)
		{
			// it we are running reversed, the "last" frame is the first one!
			animators[ii]->setValue(frames[(mode & reversedMask) ? 0 : length-1].value[ii]);
		}
	}
	cancel();
}

template< typename T, class... Args >
void
modm::ui::KeyFrameAnimationBase<T, Args...>::cancel()
{
	// disable
	currentFrame = 0;
	mode &= ~reversedMask;
	for (auto *animator : animators)
	{
		animator->stop();
	}
}

template< typename T, class... Args >
uint16_t
modm::ui::KeyFrameAnimationBase<T, Args...>::getCurrentKeyFrame() const
{
	return currentFrame ? (currentFrame - 1) : 0;
}

template< typename T, class... Args >
bool
modm::ui::KeyFrameAnimationBase<T, Args...>::isAnimating() const
{
	return (currentFrame > 0);
}

template< typename T, class... Args >
void
modm::ui::KeyFrameAnimationBase<T, Args...>::update()
{
	// are we running?
	if (currentFrame > 0)
	{
		// wait until the key frame finished
		if (!animators[0]->isAnimating())
		{
			// set the next keyframe
			for (uint_fast8_t ii = 0; ii < size; ii++)
			{
				animators[ii]->animateTo(
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
	// Update all animators
	for (auto *animator : animators) animator->update();
}

template< typename T, class... Args >
bool
modm::ui::KeyFrameAnimationBase<T, Args...>::checkRepeat()
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
modm::ui::KeyFrame<T, modm::ui::KeyFrameAnimationBase<T, Args...>::size> *
modm::ui::KeyFrameAnimationBase<T, Args...>::getKeyFrames() const
{
	return frames;
}

template< typename T, class... Args >
void
modm::ui::KeyFrameAnimationBase<T, Args...>::setKeyFrames(KeyFrame<T, size> *frames, uint16_t length)
{
	cancel();
	this->frames = frames;
	this->length = length;
}

template< typename T, class... Args >
uint16_t
modm::ui::KeyFrameAnimationBase<T, Args...>::getLength() const
{
	return length;
}

template< typename T, class... Args >
modm::ui::KeyFrameAnimationMode
modm::ui::KeyFrameAnimationBase<T, Args...>::getMode() const
{
	return static_cast<KeyFrameAnimationMode>(mode);
}

template< typename T, class... Args >
void
modm::ui::KeyFrameAnimationBase<T, Args...>::setMode(KeyFrameAnimationMode const mode)
{
	// this clears the reversed bit mask!
	this->mode = static_cast<uint8_t>(mode);
}

