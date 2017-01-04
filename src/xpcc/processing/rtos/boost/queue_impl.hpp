/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BOOST_QUEUE_HPP
#	error "Don't use this file directly, use 'queue.hpp' instead!"
#endif

template <typename T>
xpcc::rtos::Queue<T>::Queue(uint32_t length) :
	maxSize(length)
{
}

template <typename T>
xpcc::rtos::Queue<T>::~Queue()
{
}

template <typename T>
std::size_t
xpcc::rtos::Queue<T>::getSize() const
{
	boost::lock_guard<boost::timed_mutex> lock(mutex);
	deque.size();
}

template <typename T>
bool
xpcc::rtos::Queue<T>::append(const T& item, uint32_t timeout = -1)
{
	if (!mutex.timed_lock(boost::posix_time::milliseconds(timeout)) ||
			deque.size() >= maxSize) {
		return false;
	}
	
	deque.push_back(item);
	mutex.unlock();
}

template <typename T>
bool
xpcc::rtos::Queue<T>::prepend(const T& item, uint32_t timeout = -1)
{
	if (!mutex.timed_lock(boost::posix_time::milliseconds(timeout))) {
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
xpcc::rtos::Queue<T>::peek(T& item, uint32_t timeout = -1) const
{
	if (!mutex.timed_lock(boost::posix_time::milliseconds(timeout))) {
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
xpcc::rtos::Queue<T>::get(T& item, uint32_t timeout = -1)
{
	if (!mutex.timed_lock(boost::posix_time::milliseconds(timeout))) {
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
xpcc::rtos::Queue<T>::appendFromInterrupt(const T& item)
{
	return append(item);
}

template <typename T>
inline bool
xpcc::rtos::Queue<T>::prependFromInterrupt(const T& item)
{
	return prepend(item);
}

template <typename T>
inline bool
xpcc::rtos::Queue<T>::getFromInterrupt(T& item)
{
	return get(item);
}
