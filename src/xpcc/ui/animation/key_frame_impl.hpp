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

#include <xpcc/architecture/platform.hpp>

// ----------------------------------------------------------------------------
template< typename T >
xpcc::ui::KeyFrameAnimation<T>::KeyFrameAnimation(Animation<T> &animator)
:	animator(animator), keyFrames(0), numberOfFrames(0), currentFrame(-1),
	repeat(-1), mode(modeCycleMask)
{
}

template< typename T >
bool
xpcc::ui::KeyFrameAnimation<T>::start(uint8_t repeat)
{
	// there must exist a keyframe array with at least one frame
	if (keyFrames && numberOfFrames > 0) {
		cancel();
		currentFrame = 0;
		this->repeat = repeat;
		return true;
	}
	return false;
}

template< typename T >
void
xpcc::ui::KeyFrameAnimation<T>::end()
{
	if (keyFrames) {
		animator.setValue(keyFrames[(mode & reversedMask) ? 0 : numberOfFrames].value);
	}
	cancel();
}

template< typename T >
void
xpcc::ui::KeyFrameAnimation<T>::cancel()
{
	currentFrame = -1;
	mode &= ~reversedMask;
	animator.stop();
	timeout.stop();
}

template< typename T >
bool
xpcc::ui::KeyFrameAnimation<T>::isAnimating() const
{
	return (currentFrame != static_cast<uint8_t>(-1));
}

template< typename T >
void
xpcc::ui::KeyFrameAnimation<T>::update()
{
	if (currentFrame != static_cast<uint8_t>(-1))
	{
		if (!animator.isAnimating() && timeout.isExpired())
		{
			if (mode & modeCycleMask)
			{
				if (currentFrame == numberOfFrames - 1) {
					if (!checkRepeat()) return;
					currentFrame = 0;
				}
				else currentFrame++;
			}
			else {
				if (!(mode & reversedMask)) {
					if (currentFrame == numberOfFrames - 1) {
						if (!checkRepeat()) return;
						currentFrame = numberOfFrames - 2;
						mode &= ~reversedMask;
					}
					else currentFrame++;
				}
				else {
					if (currentFrame == 0) {
						if (!checkRepeat()) return;
						currentFrame = 1;
						mode |= reversedMask;
					}
					else currentFrame--;
				}
			}

			if (!animator.animateTo(keyFrames[currentFrame].time, keyFrames[currentFrame].value))
			{
				timeout.restart(keyFrames[currentFrame].time);
			}
		}
	}
}

template< typename T >
bool
xpcc::ui::KeyFrameAnimation<T>::checkRepeat() {
	if (repeat != static_cast<uint8_t>(-1)) {
		if (repeat == 0) {
			currentFrame = -1;
			return false;
		}
		repeat--;
	}
	return true;
}

// MARK: getters and setters

template< typename T >
xpcc::ui::KeyFrame<T> *
xpcc::ui::KeyFrameAnimation<T>::getKeyFrames() const {
	return keyFrames;
}

template< typename T >
void
xpcc::ui::KeyFrameAnimation<T>::setKeyFrames(KeyFrame<T> *frames, uint8_t numberOfFrames) {
	end();
	this->keyFrames = frames;
	this->numberOfFrames = numberOfFrames;
}

template< typename T >
uint8_t
xpcc::ui::KeyFrameAnimation<T>::getNumberOfFrames() const {
	return numberOfFrames;
}

template< typename T >
xpcc::ui::KeyFrameAnimationMode const &
xpcc::ui::KeyFrameAnimation<T>::getMode() const {
	return mode;
}

template< typename T >
void
xpcc::ui::KeyFrameAnimation<T>::setMode(KeyFrameAnimationMode const &mode) {
	// this clears the reversed bit mask!
	this->mode = static_cast<uint8_t>(mode);
}

