# Memory Traits

Memories have different traits, such as DMA-ability or access time. The memory
allocator functions (`malloc()`, `operator new`, etc) by default only return
DMA-able memories, ordered by fastest access time. To allocate memory with
specific traits, use the overloaded `operator new`:

```cpp
auto *p = new (modm::MemoryFastData) Foo;
auto *q = new (modm::MemoryExternal, std::nothrow) Foo;
```

!!! tip "You need to choose a heap implementation!"
    You can include the `modm:platform:heap` module or supply your own
    implementation.
