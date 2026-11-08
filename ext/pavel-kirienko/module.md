# O(1) Heap

A constant-complexity memory allocator for hard real-time systems: allocation
and deallocation take a constant, very low number of cycles, independent of the
heap state, and the worst-case fragmentation is known in advance.

Blocks are rounded up to the next power of two, so up to half the memory can be
lost to internal fragmentation and the largest block is the largest power of two
that fits into the arena.

This module only provides the library. Use the `modm:platform:heap` module with
`allocator=o1heap` to use it for `malloc()` and `new`.

See https://github.com/pavel-kirienko/o1heap.
