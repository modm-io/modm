// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------
/**
\defgroup	container	Containers
\brief 		Common data structures (list, queue etc.)

A container is a holder object that stores a collection other objects
(its elements). They are implemented as class templates, which allows a great
flexibility in the types supported as elements.

Many containers have several member functions in common, and share
functionalities. The decision of which type of container to use for a specific
need does not generally depend only on the functionality offered by the
container, but also on the efficiency of some of its members (complexity).
This is especially true for sequence containers, which offer different
trade-offs in complexity between inserting/removing elements and accessing
them.

xpcc::Stack and xpcc::Queue are implemented as container adaptors. Container
adaptors are not full container classes, but classes that provide a specific interface
relying on an object of one of the container classes (such as xpcc::BoundedDeque or
xpcc::LinkedList) to handle the elements. The underlying container is encapsulated
in such a way that its elements are accessed by the members of the container
class independently of the underlying container class used.

Sequence containers:
 - xpcc::DynamicArray
 - xpcc::LinkedList
 - xpcc::DoublyLinkedList
 - xpcc::BoundedDeque

Container adaptors:
 - xpcc::Queue
 - xpcc::Stack
 
 - xpcc::BoundedStack
 - xpcc::BoundedQueue

Other:
 - xpcc::SmartPointer
 - xpcc::Pair

Two special containers worth mentioning hide in \ref atomic "atomic" section:
 - xpcc::atomic::Queue
 - xpcc::atomic::Container

The first is a simple, interrupt-save queue (but only for the AVRs).
Whenever you need an  to exchange data between a interrupt routine and
the normal program consider using this queue.

The atomic container wraps objects and provides atomic access to
them. This comes in handy when simple objects are accessed by an interrupt
and the main program. The container provides secure access without much work
in this case.

<h3>Generic Interface</h3>

All implementation share a common set of function. Not every container implement
every one of it, only a subset which is sufficient for the container.

 - append
 - prepend
 - (insert)
 - removeFront
 - removeBack
 - (remove)
 - getFront
 - getBack
 
 - push (only Stack and Queue)
 - pop (only Stack and Queue)
 - get (only Stack and Queue)

 - at
 - operator []
 
 - clear
 
 - isEmpty
 - isFull
 - getSize
 - getMaxSize
 - getCapacity 
*/

#include "container/deque.hpp"
#include "container/queue.hpp"
#include "container/stack.hpp"

#include "container/linked_list.hpp"
#include "container/doubly_linked_list.hpp"

#include "container/dynamic_array.hpp"

#include "container/pair.hpp"
#include "container/smart_pointer.hpp"


