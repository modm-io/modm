/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STDLIB_QUEUE_HPP
#	error "Don't use this file directly, use 'queue.hpp' instead!"
#endif

#include <chrono>

template <typename T>
modm::rtos::Queue<T>::Queue(uint32_t length) :
	maxSize(length)
{
}

template <typename T>
modm::rtos::Queue<T>::~Queue()
{
}

template <typename T>
std::size_t
modm::rtos::Queue<T>::getSize() const
{
	std::lock_guard<std::timed_mutex> lock(mutex);
	deque.size();
}

template <typename T>
bool
modm::rtos::Queue<T>::append(const T& item, uint32_t timeout = -1)
{
	if (!mutex.try_lock_for(std::chrono::milliseconds(timeout)) ||
			deque.size() >= maxSize) {
		return false;
	}
	
	deque.push_back(item);
	mutex.unlock();
}

template <typename T>
bool
modm::rtos::Queue<T>::prepend(const T& item, uint32_t timeout = -1)
{
	if (!mutex.try_lock_for(std::chrono::milliseconds(timeout))) {
		return false;
	}
	
	if (deque.empty()) {
		mutex.unlock();
		return false;
	}
	
	deque.push_front(item);
	mutex.unlock();
	
	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::rtos::Queue<T>::peek(T& item, uint32_t timeout = -1) const
{
	if (!mutex.try_lock_for(std::chrono::milliseconds(timeout))) {
		return false;
	}
	
	if (deque.empty()) {
		mutex.unlock();
		return false;
	}
	
	item = deque.front();
	
	mutex.unlock();
	return true;
}

template <typename T>
bool
modm::rtos::Queue<T>::get(T& item, uint32_t timeout = -1)
{
	if (!mutex.try_lock_for(std::chrono::milliseconds(timeout))) {
		return false;
	}
	
	if (deque.empty()) {
		mutex.unlock();
		return false;
	}
	
	item = deque.front();
	deque.pop_front();
	
	mutex.unlock();
	return true;
}

// ----------------------------------------------------------------------------
template <typename T>
inline bool
modm::rtos::Queue<T>::appendFromInterrupt(const T& item)
{
	return append(item);
}

template <typename T>
inline bool
modm::rtos::Queue<T>::prependFromInterrupt(const T& item)
{
	return prepend(item);
}

template <typename T>
inline bool
modm::rtos::Queue<T>::getFromInterrupt(T& item)
{
	return get(item);
}
