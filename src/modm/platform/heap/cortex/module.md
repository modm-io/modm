# Heap Memory

Your applicaton is linked against the `newlib-nano` libc, which only requires
the implementation of the `void* sbrk(ptrdiff_t size)` hook to use the heap.

However, the `sbrk` mechanism can only grow and shrink forward and backwards in
memory and in particular it does not support discontinuous jumps from one memory
section to another. The limitation stems from being designed for use with a MMU
which would simply map another physical page into the linear virtual address
space so that the heap section appears continuous to `sbrk`.

Since we do not have a MMU on Cortex-M, this strategy limits the use of the
default newlib allocator to *one* continuous memory region. Therefore this
module implements alternative allocators for devices with multiple memory
regions with different traits and extends the C++ `operator new` to access
them.

See the `modm:architecture:memory` module for what kind of memory traits exist.


## Heap is not Implemented Error

This module is not included by default, and any attempt to use the heap
fails with one or multiple linker error messages similiar to this:

```
`_sbrk_r' referenced in section `.text._malloc_r'
    of libc_nano.a(lib_a-nano-mallocr.o): defined in discarded section
      `.Heap_is_not_implemented!_
           _Please_include_the__modm:platform:heap__module_in_your_project!'
    of libmodm.a(no_heap.o)
```

This is to prevent you from *accidentally* using the heap, which may not be
desirable for your application. If this occurs you have three choices. You can:

1. find and remove calls to malloc/new in your application, or
2. include this module with its predefined allocators, or
3. implement your own allocator.


## Predefined Allocators

There are several trade-offs to each allocator, however, as a rule of thumb,
choose:

- `newlib` (default for one continuous RAM region) for small and simple devices.
- `zephyr` (default for several RAM regions) to use all memories with their
  traits and with bounded execution time.
- `o1heap` for hard real-time code that needs the fastest constant-time
  allocator and can afford up to twice the memory.

The `tlsf` and `block` allocators have been removed and are replaced by
`zephyr` and `newlib` respectively with a warning. The options will be
removed in 2027q4.

These were chosen by stress testing them on hardware with the
`test/integration/heap` integration test. On an STM32H723 at 550 MHz a `malloc()` takes
on average ~400 cycles with `newlib` (but up to ~3000 cycles in a fragmented
heap), ~600 cycles with `zephyr` (at most ~1200) and ~220 cycles with `o1heap`
(at most ~300).

!!! warning "Multi-SRAM regions"
    For devices which contain separate memories laid out in a continuous way
    (often called SRAM1, SRAM2, SRAM3, etc.) the `newlib` strategy
    chooses the largest continuous memory region, *even though* unaligned
    accesses across memory regions may not be supported in hardware and lead to
    a bus fault! The `zephyr` and `o1heap` allocators treat each memory as a
    separate region and do not suffer from this issue.

!!! warning "Allocators are not interrupt- or thread-safe"
    All allocators call `__malloc_lock()` and `__malloc_unlock()`, which do
    nothing by default. Fibers are safe since they never switch inside the
    allocator. If you allocate from interrupts or from several cores, implement
    these two functions, for example by disabling interrupts.


### Newlib

The newlib-nano allocator is a first-fit linked list. It is fast on average and
uses memory efficiently, but its execution time grows with the number of free
blocks. Due to the limitations of the `sbrk` mechanism only the largest memory
region is used as heap! Depending on the device memory architecture this can
leave large memory regions unused, or put the entire heap into slow external
memory.


### Zephyr

The `sys_heap` allocator of the Zephyr RTOS (see `modm:zephyr-heap`) manages
memory in 8 byte chunks with power-of-two size buckets and searches only a
bounded number of free blocks, so its execution time is bounded, while its
fragmentation is close to newlib. Every memory region gets its own small heap
instance, which allows allocating memory by traits, see the
`modm:architecture:memory` module. It also supports `aligned_alloc()` and
aligned `new` natively.


### O(1) Heap

The o1heap allocator (see `modm:o1heap`) has the lowest and flattest execution
time of all allocators, since it rounds every block up to the next power of
two. This wastes up to half of the memory and limits the largest block to the
largest power of two that fits into a region. Every memory region gets its own
heap instance, so allocation by traits works like with `zephyr`. Alignments
larger than 8 bytes are not supported and return `nullptr`.


## Custom Allocator

To implement your own allocator **do not** include this module. Instead
initialize your heap in the function `__modm_initialize_memory()`, which gets
called by the startup script after hardware init, but before static constructors
are called (see `modm:platform:cortex-m` for details).

The simplest way to do so is to allocate a huge array into one of the heap
sections and use this as your heap. Consult `modm:platform:core` for what
heap sections your target provides!

```cpp
modm_section(".heap1") // always the main heap section
uint8_t heap_begin[10*1024]; // 10 kB heap
const uint8_t *const heap_end{heap_begin + sizeof(heap_begin)};

extern "C" void __modm_initialize_memory()
{
    // Initialize your specific allocator algorithm here
    allocator.initialize();
}
```

!!! warning "Static constructors are only called afterwards!"
    Since constructors may call the heap, it must be initialized before static
    constructors are called. Only trivially constructed (POD) objects are
    already initialized!


### Using the HeapTable

If you prefer a little more control, include the `modm:architecture:memory`
module to get access to the internal `modm::platform::HeapTable` API, which
lists memory regions by traits and sizes.

For example to find the largest continuous memory section with default traits
you can use this code:

```cpp
const uint8_t *heap_begin{nullptr};
const uint8_t *heap_end{nullptr};
extern "C" void __modm_initialize_memory()
{
    bool success = HeapTable::find_largest(&heap_begin, &heap_end,
                                           modm::MemoryDefault);
    modm_assert(success, "heap.init", "No default memory section found!");
}
```

If you want to know more about the available memory regions, you can iterate
over the heap table directly. This gives you full control over *where* you want
to place you heap. You can print this table at runtime to get a feel for it:

```cpp
for (const auto [traits, start, end, size] : modm::platform::HeapTable())
{
    MODM_LOG_INFO.printf("Memory section %#x @[0x%p,0x%p](%u)\n",
                         traits.value, start, end, size);
}
```


### Providing sbrk

To use the builtin allocator from newlib, all you need to provide is an
implementation of the `sbrk` function. A simple implementation for a
[`heap_begin`, `heap_end`] memory region looks like this:

```cpp
const uint8_t *heap_top{heap_begin};
extern "C" modm_used void* _sbrk_r(struct _reent *,  ptrdiff_t size)
{
    const uint8_t *const heap = heap_top;
    // malloc returns a nullptr when sbrk fails
    if (size > heap_end - heap_top) return (void*) -1;
    heap_top += size;
    return (void*) heap;
}
```


### Providing operator delete

Unfortunately virtual C++ destructors can emit a call to `operator delete` even
for classes with static allocation and also in program without a single call to
`operator new` or `malloc`. Therefore if this module is not included, calls to
`operator delete` are ignored and you must overwrite this behavior with this
function that only points to `free`.

```cpp
extern "C" void operator_delete(void* ptr)
{
    free(ptr);
}
```


### Wrapping malloc

To use a completely custom allocator, you need to replace the newlib allocator
by wrapping the `malloc`, `calloc`, `realloc` and `free` functions via the
linker by adding this to your project configuration:

```xml
<library>
  <!-- repos, modules, options, etc... -->
  <collectors>
    <collect name=":build:linkflags">-Wl,-wrap,_malloc_r</collect>
    <collect name=":build:linkflags">-Wl,-wrap,_calloc_r</collect>
    <collect name=":build:linkflags">-Wl,-wrap,_realloc_r</collect>
    <collect name=":build:linkflags">-Wl,-wrap,_memalign_r</collect>
    <collect name=":build:linkflags">-Wl,-wrap,_free_r</collect>
  </collectors>
</library>
```

And then implement the following functions with your custom allocator:

```cpp
extern "C" modm_used void *
__wrap__malloc_r(struct _reent *, size_t size)
{
    return allocator.malloc(size);
}
extern "C" modm_used void *
__wrap__calloc_r(struct _reent *, size_t n, size_t size)
{
    return allocator.calloc(n, size);
}
extern "C" modm_used void *
__wrap__realloc_r(struct _reent *, void *ptr, size_t size)
{
    return allocator.realloc(ptr, size);
}
extern "C" modm_used void *
__wrap__memalign_r(struct _reent *, size_t align, size_t size)
{
    return allocator.memalign(align, size);
}
extern "C" modm_used void
__wrap__free_r(struct _reent *, void *p)
{
    allocator.free(p);
}
```

This is particularly recommended if you need a thread-safe malloc, which you
implement here via the `_reent` struct. Consult newlib docs for details.

!!! warning "Mark these functions as `modm_used`"
    These functions and `_sbrk_r` are only called from the precompiled newlib,
    so with the `modm:build:lto` option enabled, the optimizer removes them
    unless they are marked `modm_used`.

!!! tip "sbrk is not called anymore"
    When wrapping these malloc functions, `_sbrk_r` is not called anymore, and
    therefore is thrown away by the linker, thus the linker error disappears.
    You therefore do not need to implement it, not even as a stub.

To also support memory traits, you need to overwrite the default implementation
of `malloc_traits(size, traits)` which would otherwise just ignore the traits:

```cpp
extern "C" void *
malloc_traits(size_t size, uint32_t ctraits)
{
    // Convert back from C land to C++ land:
    const modm::MemoryTraits traits{ctraits};
    if (traits & modm::MemoryTrait::AccessDMA) {
        // check for space in DMA-able heap regions
    } else {
        // check other regions
    }
    return ptr;
}
```


