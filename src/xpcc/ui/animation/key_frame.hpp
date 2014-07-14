// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_KEY_FRAME_HPP
#define XPCC_UI_KEY_FRAME_HPP

#include <stdint.h>
#include "base.hpp"

namespace xpcc
{

namespace ui
{

enum class
KeyFrameAnimationMode
{
	Cycle = 0b01,		///< cycle through the keyframes
	Reverse = 0b10		///< reverse through keyframes
};


template<typename T, typename... Args>
struct KeyFrameBase
{
	static constexpr std::size_t size = sizeof...(Args);

	typename Animation<T>::TimeType time;
	T value[size];

	KeyFrameBase(typename Animation<T>::TimeType time, Args... values) : time(time), value{values...} {}
};

template<typename T, int rem, typename... Args>
struct KeyFrameHelper
{
	typedef typename KeyFrameHelper<T, rem - 1, T, Args...>::type type;
};

template<typename T, typename... Args>
struct KeyFrameHelper<T, 0, Args...>
{
	typedef KeyFrameBase<T, Args...> type;
};

/**
* This struct specifies one key frame of an animation.
*
* @author	Niklas Hauser
* @ingroup ui
*/
template<typename T = uint8_t, uint8_t N = 1>
using KeyFrame = typename KeyFrameHelper<T, N>::type;


/**
* This class takes an array of keyframes and applies them to an animation.
*
* @author	Niklas Hauser
* @ingroup ui
*/
template< typename T, class... Args >
class KeyFrameAnimationBase
{
	static constexpr std::size_t size = sizeof...(Args);

public:
	KeyFrameAnimationBase(Animation<Args>&... animator);

	bool
	start(uint8_t repeat=-1);

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

	void
	setKeyFrames(KeyFrame<T, size> *frames, uint8_t numberOfFrames);

	uint8_t
	getNumberOfFrames() const;

	KeyFrameAnimationMode const &
	getMode() const;

	void
	setMode(KeyFrameAnimationMode const &mode);

private:
	bool
	checkRepeat();

	Animation<T> *animator[size];
	KeyFrame<T, size> *keyFrames;
	uint8_t numberOfFrames;
	uint8_t currentFrame;
	uint8_t repeat;
	xpcc::Timeout<> timeout;

	// used for KeyFrameAnimationMode and reversed bit
	uint8_t mode;
	static constexpr uint8_t reversedMask = 0x80;
	static constexpr uint8_t modeCycleMask = static_cast<uint8_t>(KeyFrameAnimationMode::Cycle);
	static constexpr uint8_t modeReverseMask = static_cast<uint8_t>(KeyFrameAnimationMode::Reverse);
};

template<typename T, int rem, class... Args>
struct KeyFrameAnimationHelper
{
	typedef typename KeyFrameAnimationHelper<T, rem - 1, T, Args...>::type type;
};

template<typename T, class... Args>
struct KeyFrameAnimationHelper<T, 0, Args...>
{
	typedef KeyFrameAnimationBase<T, Args...> type;
};

/**
* This class takes an array of keyframes and applies them to an animation.
*
* @author	Niklas Hauser
* @ingroup ui
*/
template<typename T = uint8_t, uint8_t N = 1>
using KeyFrameAnimation = typename KeyFrameAnimationHelper<T, N>::type;

}	// namespace ui

}	// namespace xpcc

#include "key_frame_impl.hpp"

#endif	// XPCC_UI_KEY_FRAME_HPP
