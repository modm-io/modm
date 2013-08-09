// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__BLOCK_ALLOCATOR_HPP
	#error	"Don't include this file directly use 'block_allocator.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
/*
 * Align heapStart to two bytes after a four byte boundary. When using two
 * bytes for management the payload is aligned to four bytes.
 * 
 * (__heap_start % 4) -->  heapStart
 *             0      -->  __heap_start + 2
 *             1      -->  __heap_start + 1
 *             2      -->  __heap_start + 0
 *             3      -->  __heap_start + 3
 * 
 * In the consecutive blocks a four byte aligned word is shared by two
 * adjacent block for their management data. Consider the following example:
 * 
 * address:  0123 4567 89ab cdef 0123 4567 89ab cdef 0123 4567 89ab cdef 01
 *           UUmm pppp pppp pppp mmMM PPPP PPPP PPPP PPPP PPPP PPPP PPPP MM
 * 
 * 'U' = Unused
 * 'm' = Management data for block with payload 'pppp pppp pppp'
 * 'M' = Management data for double block with payload 
 * 			'PPPP PPPP PPPP PPPP PPPP PPPP PPPP'
 * 
 * Example:
 * heapStart = 0x0000 0000
 * heapEnd   = 0x0000 0040 (64 bytes)
 * blocksize = 16
 * 
 * start     = 0x0000 0002
 * memory    = 60
 * size      = 3
 * end       = 0x0000 0002 + 3 * 16 = 0x0000 0032
 * 
 * Memory Map:
 *       0123 4567 89ab cdef 0123 4567 89ab cdef
 * start --\
 *         v  
 *    0  UUmm pppp pppp pppp mmMM PPPP PPPP PPPP
 *    2  MMmm pppp pppp pppp mm
 *                             ^
 *                             \-- end
 */
template <typename T, unsigned int BLOCK_SIZE >
ALWAYS_INLINE void
xpcc::BlockAllocator<T, BLOCK_SIZE>::initialize(void * heapStart, void * heapEnd)
{
	start = alignPointer(heapStart);
	
	// 2(4) bytes needed for the management data at the end
	uintptr_t memory = (uintptr_t) heapEnd - (uintptr_t) start - sizeof(T);
	
	// integer division which will automatically round down
	std::size_t size = memory / (BLOCK_SIZE * sizeof(T));
	 
	end = (uint16_t *)((uintptr_t) start + (size * BLOCK_SIZE * sizeof(T)));
	
	*start = -size;
	*(end - 1) = -size;
	
	freeHint = start;
}

// ----------------------------------------------------------------------------
/* 
 * 
 * 
 */
template <typename T, unsigned int BLOCK_SIZE >
ALWAYS_INLINE void *
xpcc::BlockAllocator<T, BLOCK_SIZE>::allocate(std::size_t requestedSize)
{
	requestedSize += 4;	// bytes needed for the management
	
	std::size_t neededSlots = (requestedSize + (BLOCK_SIZE * sizeof(T) - 1)) / 
			(BLOCK_SIZE * sizeof(T));
	
	T *p = freeHint;
	do
	{
		SignedType slots = *p;
		
		if (slots < 0)
		{
			// slots < 0 => free slots
			slots = -slots;
			
			T freeSlots = slots;
			if (freeSlots >= neededSlots)
			{
				// write the marker on the first an last slot of the field of 
				// new allocated slots
				*p = neededSlots;
				*(p + neededSlots * BLOCK_SIZE - 1) = neededSlots;
				
				if (freeSlots > neededSlots)
				{
					// if the allocated space are smaller than original one,
					// than we get a new slice of free slots and need set the 
					// new marker for this field
					*(p + neededSlots * BLOCK_SIZE) = -(freeSlots - neededSlots);
					*(p + freeSlots  * BLOCK_SIZE - 1) = -(freeSlots - neededSlots);
				}
				return (void *) (p + 1);
			}
		}
		
		p += slots * BLOCK_SIZE;
	}
	while (p < end);
	
	return 0;
}

// ----------------------------------------------------------------------------
template <typename T, unsigned int BLOCK_SIZE >
ALWAYS_INLINE void
xpcc::BlockAllocator<T, BLOCK_SIZE>::free(void *ptr)
{
	if (ptr == 0) {
		return;
	}
	
	T *p = (T *) ptr;
	p -= 1;
	
	SignedType slots = *p;
	T freeSlots = slots;
	
	// check whether the slots above are free
	if (p + slots * BLOCK_SIZE < end) {
		slots = *(p + slots * BLOCK_SIZE);
		if (slots < 0) {
			freeSlots += -slots;
		}
	}
	
	// check the slots below
	if (p - 1 >= start) {
		slots = *(p - 1);
		if (slots < 0) {
			p += slots * BLOCK_SIZE;
			freeSlots += -slots;
		}
	}
	
	// write the markers
//	if (freeSlots > MAX_BLOCK_PARTS)
//	{
//		// memory area is to big, split it in two parts
//		*p = -MAX_BLOCK_PARTS;
//		*(p + MAX_BLOCK_PARTS * BLOCK_SIZE - 1) = -MAX_BLOCK_PARTS;
//		
//		*(p + MAX_BLOCK_PARTS * BLOCK_SIZE) = -(freeSlots - MAX_BLOCK_PARTS);
//		*(p + freeSlots * BLOCK_SIZE - 1) = -(freeSlots - MAX_BLOCK_PARTS);
//	}
//	else
//	{
		*p = -freeSlots;
		*(p + freeSlots * BLOCK_SIZE - 1) = -freeSlots;
//	}
	
	if (p < freeHint) {
		freeHint = p;
	}
}

// ----------------------------------------------------------------------------
template <typename T, unsigned int BLOCK_SIZE >
ALWAYS_INLINE std::size_t
xpcc::BlockAllocator<T, BLOCK_SIZE>::getAvailableSize() const
{
	T *p = start;
	std::size_t size = 0;
	
	do {
		SignedType slots = *p;
		
		if (slots < 0)
		{
			// slots < 0 => free slots
			slots = -slots;
			size += slots * BLOCK_SIZE * sizeof(T);
		}
		
		p += slots * BLOCK_SIZE;
	}
	while (p < end);
	
	return size;
}

// ----------------------------------------------------------------------------
template<typename T, unsigned int BLOCK_SIZE >
ALWAYS_INLINE T *
xpcc::BlockAllocator<T, BLOCK_SIZE>::alignPointer(void * ptr) const
{
	// (XPCC__ALIGNMENT - 1) is used as a bitmask
	std::size_t misalignment = ((uintptr_t) ptr & (XPCC__ALIGNMENT - 1));
#if XPCC__ALIGNMENT == 8
	const uint8_t offset[8] = { 6, 5, 4, 3, 2, 1, 0, 7 };
#else
	const uint8_t offset[4] = { 2, 1, 0, 3 };
#endif
	
	return (T *) (((uintptr_t) ptr) + offset[misalignment]);
}
