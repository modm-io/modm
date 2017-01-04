/*
 * Copyright (c) 2010, 2016, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

#include "deque.hpp"
#include "queue.hpp"
#include "stack.hpp"

#include "linked_list.hpp"
#include "doubly_linked_list.hpp"

#include "dynamic_array.hpp"

#include "pair.hpp"
#include "smart_pointer.hpp"


