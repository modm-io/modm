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

The `modm:architecture:atomic` module also provides `modm::atomic::Queue`, a
lock-free single-producer single-consumer queue for exchanging data between an
interrupt and the main program. Use `std::atomic` to access single objects from
both.

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