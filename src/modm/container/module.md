# Containers

A container is a holder object that stores a collection other objects (its
elements). They are implemented as class templates, which allows a great
flexibility in the types supported as elements.

Many containers have several member functions in common, and share
functionalities. The decision of which type of container to use for a specific
need does not generally depend only on the functionality offered by the
container, but also on the efficiency of some of its members (complexity).
This is especially true for sequence containers, which offer different
trade-offs in complexity between inserting/removing elements and accessing
them.

`modm::Stack` and `modm::Queue` are implemented as container adapters. Container
adapters are not full container classes, but classes that provide a specific
interface relying on an object of one of the container classes (such as
`modm::BoundedDeque` or `modm::LinkedList`) to handle the elements.
The underlying container is encapsulated in such a way that its elements are
accessed by the members of the container class independently of the underlying
container class used.

Sequence containers:

- `modm::DynamicArray`
- `modm::LinkedList`
- `modm::DoublyLinkedList`
- `modm::BoundedDeque`

Container adapters:

- `modm::Queue`
- `modm::Stack`
- `modm::BoundedStack`
- `modm::BoundedQueue`

Other:

- `modm::SmartPointer`
- `modm::Pair`

Two special containers hiding in the `modm:architecture:atomic` module:

- `modm::atomic::Queue`
- `modm::atomic::Container`

The first is a simple, interrupt-safe queue (but only for the AVRs).
Whenever you need to exchange data between a interrupt routine and the normal
program consider using this queue.

The atomic container wraps objects and provides atomic access to
them. This comes in handy when simple objects are accessed by an interrupt
and the main program. The container provides secure access without much work
in this case.

## Generic Interface

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
- operator[]

- clear

- isEmpty
- isFull
- getSize
- getMaxSize
- getCapacity