/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FREERTOS_QUEUE_HPP
#define MODM_FREERTOS_QUEUE_HPP

#include <cstddef>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace modm
{
	namespace rtos
	{
		/**
		 * \brief	Queue base class
		 * 
		 * \ingroup	freertos
		 */
		class QueueBase
		{
		public:
			/**
			 * \brief	Create a Queue
			 * 
			 * \param	length		The maximum number of items the queue can
			 * 						contain.
			 * \param	itemSize	The size of one item. All items in the queue
			 * 						must have the same size.
			 */
			QueueBase(unsigned portBASE_TYPE length,
					unsigned portBASE_TYPE itemSize);
			
			~QueueBase();
			
			/// Get the number of items stored in the queue
			inline std::size_t
			getSize() const
			{
				return uxQueueMessagesWaiting(this->handle);
			}
			
			/**
			 * \brief	Post an item to the back of a queue.
			 * 
			 * This function must not be called from an interrupt service
			 * routine. Use appendFromInterrupt() for this purpose.
			 */
			bool
			append(const void *item, portTickType timeout = portMAX_DELAY);
			
			bool
			prepend(const void *item, portTickType timeout = portMAX_DELAY);
			
			bool
			peek(void *item, portTickType timeout = portMAX_DELAY) const;
			
			bool
			get(void *item, portTickType timeout = portMAX_DELAY);
			
			bool
			appendFromInterrupt(const void *item);
			
			bool
			prependFromInterrupt(const void *item);
			
			bool
			getFromInterrupt(void *item);
			
		protected:
			xQueueHandle handle;
			
		private:
			// disable copy constructor
			QueueBase(const QueueBase& other);
			
			// disable assignment operator
			QueueBase&
			operator = (const QueueBase& other);
		};
		
		/**
		 * \brief	Queue
		 * 
		 * This is a type-safe wrapper around QueueBase
		 * 
		 * \ingroup	freertos
		 */
		template<typename T>
		class Queue : private QueueBase
		{
		public:
			Queue(unsigned portBASE_TYPE length);
			
			using QueueBase::getSize;
			
			inline bool
			append(const T& item, portTickType timeout = portMAX_DELAY)
			{
				return QueueBase::append(&item, timeout);
			}
			
			inline bool
			prepend(const T& item, portTickType timeout = portMAX_DELAY)
			{
				return QueueBase::prepend(&item, timeout);
			}
			
			inline bool
			peek(T& item, portTickType timeout = portMAX_DELAY)
			{
				return QueueBase::peek(&item, timeout);
			}
			
			inline bool
			get(T& item, portTickType timeout = portMAX_DELAY)
			{
				return QueueBase::get(&item, timeout);
			}
			
			inline bool
			appendFromInterrupt(const T& item)
			{
				return QueueBase::appendFromInterrupt(&item);
			}
			
			inline bool
			prependFromInterrupt(const T& item)
			{
				return QueueBase::prependFromInterrupt(&item);
			}
			
			inline bool
			getFromInterrupt(T& item)
			{
				return QueueBase::getFromInterrupt(&item);
			}
			
		private:
			// disable copy constructor
			Queue(const Queue& other);
			
			// disable assignment operator
			Queue&
			operator = (const Queue& other);
		};
	}
}

#endif // MODM_FREERTOS_QUEUE_HPP
