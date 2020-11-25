/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_UI_KEY_FRAME_HPP
#define MODM_UI_KEY_FRAME_HPP

#include <stdint.h>
#include "base.hpp"

namespace modm
{

namespace ui
{

/// @ingroup modm_ui_animation
enum class
KeyFrameAnimationMode
{
	Repeat = 0b01,		///< repeat the animation
	Autoreverse = 0b10	///< run the animation forwards then backwards
};

/// @cond
template<typename T, typename... Args>
struct KeyFrameBase
{
	static constexpr std::size_t size = sizeof...(Args);

	typename Animation<T>::TimeType length = 0;
	T value[size] = {};

	KeyFrameBase() = default;
	KeyFrameBase(typename Animation<T>::TimeType length)
	:	length(length) {}

	KeyFrameBase(typename Animation<T>::TimeType length, Args... values)
	:	length(length), value{values...} {}
} modm_packed;

template<typename T, int remaining, typename... Args>
struct KeyFrameHelper
{
	typedef typename KeyFrameHelper<T, remaining - 1, T, Args...>::type type;
};

template<typename T, typename... Args>
struct KeyFrameHelper<T, 0, Args...>
{
	typedef KeyFrameBase<T, Args...> type;
};

template<typename T = uint8_t, uint8_t N = 1>
using KeyFrame = typename KeyFrameHelper<T, N>::type;
/// @endcond

#ifdef __DOXYGEN__
/**
 * This struct specifies one key frame of an animation.
 *
 * @tparam	T	the type of the values in this frame
 * @tparam	N	the number of values in this frame
 *
 * @author	Niklas Hauser
 * @ingroup modm_ui_animation
 */
template<typename T = uint8_t, uint8_t N = 1>
struct KeyFrame
{
	/// the length of this frame in ms
	TimeType length;
	/// the values as an array
	T value[N];

	/// Constructor with length and multiple values of type T
	KeyFrame(TimeType length, T... values);
};
#endif

/// @cond
template< typename T, class... Args >
class KeyFrameAnimationBase
{
	static constexpr std::size_t size = sizeof...(Args);

public:
	KeyFrameAnimationBase(AnimationBase<Args>&... animator);

	template< uint16_t N >
	KeyFrameAnimationBase(const KeyFrame<T, size> (&frames)[N], AnimationBase<Args>&... animator);

	KeyFrameAnimationBase(const KeyFrame<T, size> *frames, uint16_t length, AnimationBase<Args>&... animator);

	bool
	start(uint8_t repeat=0);

	void
	stop();

	void
	cancel();

	bool
	isAnimating() const;

	void
	update();

	KeyFrame<T, size> *
	getKeyFrames() const;

	uint16_t
	getCurrentKeyFrame() const;

	void
	setKeyFrames(KeyFrame<T, size> *frames, uint16_t length);

	template< uint16_t N >
	void
	setKeyFrames(KeyFrame<T, size> (&frames)[N])
	{ setKeyFrames(frames, N); }

	uint16_t
	getLength() const;

	KeyFrameAnimationMode
	getMode() const;

	void
	setMode(KeyFrameAnimationMode const mode);

private:
	bool
	checkRepeat();

	AnimationBase<T> *animators[size];
	const KeyFrame<T, size> *frames;
	uint16_t length;
	uint16_t currentFrame;
	uint8_t repeat;

	// used for KeyFrameAnimationMode and reversed bit
	uint8_t mode;
	static constexpr uint8_t reversedMask = 0x80;
	static constexpr uint8_t modeCycleMask = static_cast<uint8_t>(KeyFrameAnimationMode::Repeat);
	static constexpr uint8_t modeReverseMask = static_cast<uint8_t>(KeyFrameAnimationMode::Autoreverse);
};

template<typename T, int remaining, class... Args>
struct KeyFrameAnimationHelper
{
	typedef typename KeyFrameAnimationHelper<T, remaining - 1, T, Args...>::type type;
};

template<typename T, class... Args>
struct KeyFrameAnimationHelper<T, 0, Args...>
{
	typedef KeyFrameAnimationBase<T, Args...> type;
};

template<typename T = uint8_t, uint8_t N = 1>
using KeyFrameAnimation = typename KeyFrameAnimationHelper<T, N>::type;
/// @endcond

#ifdef __DOXYGEN__
/**
 * This class takes an array of keyframes and applies them to an animation.
 *
 * @author	Niklas Hauser
 * @ingroup modm_ui_animation
 */
template<typename T = uint8_t, uint8_t N = 1>
class KeyFrameAnimation
{
public:
	/// The constructor requires references to all N animator objects
	KeyFrameAnimation(Animation<T>... &animator);

	/// @param	frames	pointer to the keyframe array
	/// @param	length	the number of keyframes in the array
	/// The constructor requires references to all N animator objects
	KeyFrameAnimation(KeyFrame<T, N> *frames, uint16_t length, Animation<T>... &animator);

	/// start the keyframe animation
	/// @param	repeat	number of repeats of the entire animation,
	///					set to `0` for infinity
	/// @return	`true` if keyframes are valid and animation started,
	///			`false` otherwise
	bool
	start(uint8_t repeat=0);

	/// Stops the animation and sets the last frame of the cycle.
	void
	stop();

	/// Stops the animation right now.
	void
	cancel();

	/// @return `true` if animation is currently running,
	///			`false` if otherwise
	bool
	isAnimating() const;

	/// must be called regularly
	void
	update();

	/// @return	the pointer to the key frame array
	KeyFrame<T, N> *
	getKeyFrames() const;

	/// @return	get the currently animating key frame index, or 0
	uint16_t
	getCurrentKeyFrame() const;

	/// @param	frames	pointer to the keyframe array
	/// @param	length	the number of keyframes in the array
	void
	setKeyFrames(KeyFrame<T, N> *frames, uint16_t length);

	uint16_t
	getLength() const;

	KeyFrameAnimationMode const
	getMode() const;

	void
	setMode(KeyFrameAnimationMode const mode);
};
#endif

}	// namespace ui

}	// namespace modm

#include "key_frame_impl.hpp"

#endif	// MODM_UI_KEY_FRAME_HPP
