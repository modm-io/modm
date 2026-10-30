# Containers

modm uses the containers of the C++ standard library, which are available on
all targets including AVRs. This module only provides containers that are
missing from the standard library:

- `modm::BoundedDeque`: A double ended queue with a fixed capacity, which is
  organized as a ring buffer and does not use the heap. Its interface follows
  `std::deque`, so it can also be used with `std::queue` and `std::stack`.

```cpp
modm::BoundedDeque<uint8_t, 10> deque;
deque.push_back(1);
if (not deque.full()) deque.push_front(0);
// or check and push in one step
bool success = deque.append(2);

std::queue<uint8_t, modm::BoundedDeque<uint8_t, 10>> queue;
std::stack<uint8_t, modm::BoundedDeque<uint8_t, 10>> stack;
```

The following containers are deprecated and will be removed:

| Deprecated                | Replacement                                |
|---------------------------|--------------------------------------------|
| `modm::DynamicArray`      | `std::vector`                              |
| `modm::LinkedList`        | `std::list`                                |
| `modm::DoublyLinkedList`  | `std::list`                                |
| `modm::Pair`              | `std::pair`                                |
| `modm::Queue`             | `std::queue`                               |
| `modm::Stack`             | `std::stack`                               |
| `modm::BoundedQueue`      | `std::queue<T, modm::BoundedDeque<T, N>>`  |
| `modm::BoundedStack`      | `std::stack<T, modm::BoundedDeque<T, N>>`  |

The `modm:architecture:atomic` module also provides `modm::atomic::Queue`, a
lock-free single-producer single-consumer queue for exchanging data between an
interrupt and the main program. Use `std::atomic` to access single objects from
both.
