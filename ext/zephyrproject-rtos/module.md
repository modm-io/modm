# Zephyr Heap

The `sys_heap` allocator of the Zephyr RTOS: memory is managed in 8 byte chunks
with power-of-two size buckets and immediate coalescing. Allocation searches a
bounded number of buckets (`CONFIG_SYS_HEAP_ALLOC_LOOPS`), so its execution
time is bounded, while the fragmentation stays close to a best-fit allocator.
Aligned allocations and in-place reallocation are supported natively.

Heaps below 256 kB use 4 byte chunk headers, larger heaps 8 byte headers.

This module only provides the library with a small replacement for the Zephyr
headers. Use the `modm:platform:heap` module with `allocator=zephyr` to use it
for `malloc()` and `new`.

See https://docs.zephyrproject.org/latest/kernel/memory_management/heap.html.
