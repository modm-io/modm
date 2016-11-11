Dynamic memory allocation strategy

By default, the arm-none-eabi toolchain ships with the `newlib` libc, which uses
`dlmalloc` as the underlying allocator algorithm and only requires the
implementation of the `void * sbrk(ptrdiff_t size)` hook. However, this limits
the allocator to use just _one_ memory region, which must then also be of
_continuous_ extend, since `sbrk` can only grow and shrink, but not jump.
Therefore, when using the `newlib` strategy, only the largest memory region is
used as heap! Depending on the device memory architecture this can leave large
memory regions unused.

For devices with very small memories, we recommend using the block allocator
strategy, which uses a very light-weight and simple algorithm. This also only
operates on one continuous memory region as heap.

!!! note
    Memories can have different traits, such as DMA-ability or access time. The
    default memory allocator functions (malloc, new, etc) only return DMA-able
    memories, ordered by fastest access time. Similarly the search for the
    largest memory region only considers DMA-able memory.

!!! warning
    For devices which contain separate memories laid out in a continuous way
    (often called SRAM1, SRAM2, etc.) the `newlib` and `block_allocator`
    strategies choose the largest continuous memory region, *even though*
    unaligned accesses across memory regions may not be supported in hardware
    and lead to a bus fault! Consider using the TLSF implementation, which does
    not suffer from this issue.

To use all non-statically allocated memory for heap, use the TLSF strategy,
which natively supports multiple memory regions. Our implementation treats
all internal memories as separate regions, so unaligned accesses across memory
boundaries are not an issue. To request heap memory of different traits, see
`xpcc::MemoryTraits`.

!!! note
    The TLSF implementation has a static overhead of about 1kB per memory trait
    group, however, these can then contain multiple non-continuous memory
    regions. The upside of this large static allocation is very fast allocation
    times of O(1), but we recommend using TLSF only for devices with multiple
    memory regions.
