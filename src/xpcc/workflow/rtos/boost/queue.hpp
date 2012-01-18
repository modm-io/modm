// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FREERTOS__QUEUE_HPP
#define XPCC_FREERTOS__QUEUE_HPP

#include <cstddef>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace xpcc
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

#endif // XPCC_FREERTOS__QUEUE_HPP
