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

- `newlib` for devices with one large continuous RAM region.
- `block` for devices with one very small RAM region.
- `tlsf` for devices with multiple, different discontinuous RAM regions.

!!! warning "Multi-SRAM regions"
    For devices which contain separate memories laid out in a continuous way
    (often called SRAM1, SRAM2, SRAM3, etc.) the `newlib` and `block` strategies
    choose the largest continuous memory region, *even though* unaligned
    accesses across memory regions may not be supported in hardware and lead to
    a bus fault! Consider using the TLSF implementation, which does not suffer
    from this issue.

!!! warning "Allocators are not interrupt- or thread-safe"
    No locking is implemented by default, if you need this feature, consider
    implementing your own custom allocator algorithm!


### Newlib

The newlib-nano allocator is a simple linked list, its overhead is therefore
low, but the access time may not be good.
Due to the limitations of the `sbrk` mechanism only the largest memory region is
used as heap! Depending on the device memory architecture this can leave large
memory regions unused.

Consider using the TLSF allocator for devices with multiple discontinuous
memories.


### Block

For devices with very small memories, we recommend using the block allocator
strategy, which uses a very light-weight and simple algorithm. This also only
operates on one continuous memory region as heap.

!!! bug "The Block allocator does not implement realloc!"
    This is a bug in `modm:driver:block.allocator` and currently a `modm_assert`
    will fail.


### TLSF

To use all non-statically allocated memory for heap, use the TLSF strategy,
which natively supports multiple memory regions. This implementation treats
all internal memories as separate regions, so unaligned access across memory
boundaries is not an issue. To request heap memory of different traits, see
the `modm:architecture:memory` module.

!!! note "TLSF has static overhead"
    The TLSF implementation has a static overhead of about 1kB per memory trait
    group, however, these can then contain multiple discontinuous regions. The
    upside of this large static allocation is very fast allocation times of
    O(1), but we recommend using TLSF only for devices with multiple large
    memory regions.


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
extern "C" void* _sbrk_r(struct _reent *,  ptrdiff_t size)
{
    const uint8_t *const heap = heap_top;
    heap_top += size;
    modm_assert(heap_top < heap_end, "heap.sbrk", "Heap overflowed!");
    return (void*) heap;
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
    <collect name=":build:linkflags">-Wl,-wrap,_free_r</collect>
  </collectors>
</library>
```

And then implement the following functions with your custom allocator:

```cpp
extern "C" void *
__wrap__malloc_r(struct _reent *, size_t size)
{
    return allocator.malloc(size);
}
extern "C" void *
__wrap__calloc_r(struct _reent *, size_t size)
{
    return allocator.calloc(size);
}
extern "C" void *
__wrap__realloc_r(struct _reent *, void *ptr, size_t size)
{
    return allocator.realloc(ptr, size);
}
extern "C" void 
__wrap__free_r(struct _reent *, void *p)
{
    allocator.free(p);
}
```

This is particularly recommended if you need a thread-safe malloc, which you
implement here via the `_reent` struct. Consult newlib docs for details.

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


