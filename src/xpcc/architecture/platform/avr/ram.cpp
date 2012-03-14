// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#include <stdint.h>
#include <stdlib.h>

#include "ram.hpp"

// ----------------------------------------------------------------------------
#define	BLOCK_SIZE			8		// must be a power of two!
#define	MAX_BLOCK_PARTS		127		// maximal number of parts forming a memory field

#ifdef	__AVR__
	
	// Needed for definition of AVR_STACK_POINTER_REG.
	#include <avr/io.h>
	
	#define STACK_POINTER() ((char *)AVR_STACK_POINTER_REG)
	
	extern char	__heap_start;
	extern char	__heap_end;
	
	void initializeMemory(void) __attribute__ ((naked)) \
								__attribute__ ((section (".init5")));

#else

	#define	ALLOC_TEST
	#define	ALLOC_TEST_SIZE	4096

	static uint8_t mempool[ALLOC_TEST_SIZE];

	#define	__heap_start	mempool[0]
	#define	__heap_end		mempool[ALLOC_TEST_SIZE]

	#define STACK_POINTER() (mempool + ALLOC_TEST_SIZE)

#endif

//static size_t headMargin = 32;	// TODO implement stack pointer check
static uint8_t *heapStart = (uint8_t *) &__heap_start;
static uint8_t *heapEnd = (uint8_t *) &__heap_end;

static uint8_t *freeHint;		// TODO implement freeHint usefull ;-)
static uint8_t *currentHeapEnd;

// ----------------------------------------------------------------------------
// This function is called before the constructors and main()
void
initializeMemory(void)
{
	*heapStart = 0;		// dummy byte
	 heapStart++;
	*heapStart = -MAX_BLOCK_PARTS;
	
	currentHeapEnd = heapStart + MAX_BLOCK_PARTS * BLOCK_SIZE;
	*(currentHeapEnd - 1) = -MAX_BLOCK_PARTS;
	
	freeHint = heapStart;
}

// ----------------------------------------------------------------------------
/* Implizite Listen
 * 
 * TODO description
 */
void *
xpcc::avr::allocateMemory(std::size_t requestedSize)
{
	if (requestedSize == 0 ||
		requestedSize > MAX_BLOCK_PARTS * BLOCK_SIZE)
	{
		return 0;
	}
	
	requestedSize += 2;	// bytes needed for the mangement
	uint8_t neededSlots = requestedSize / BLOCK_SIZE;
	if (requestedSize & (BLOCK_SIZE - 1)) {
		neededSlots += 1;
	}
	
	uint8_t *p = freeHint;
	do
	{
		int8_t slots = *p;
		
		if (slots < 0)
		{
			slots = -slots;
			
			if (slots >= neededSlots)
			{
				// write the marker on the first an last slot of the field of 
				// new allocated slots
				*p = neededSlots;
				*(p + neededSlots * BLOCK_SIZE - 1) = neededSlots;
				
				if (slots > neededSlots)
				{
					// if the allocated space are smaller than original one,
					// than we get a new slice of free slots and need set the 
					// new marker for this field
					*(p + neededSlots * BLOCK_SIZE) = -(slots - neededSlots);
					*(p + slots  * BLOCK_SIZE - 1) = -(slots - neededSlots);
				}
				return (void *) (p + 1);
			}
		}
		
		p += slots * BLOCK_SIZE;
	}
	while (p < currentHeapEnd);
	
	// allocate more memory from the heap
	int8_t slots = *(p - 1);
	if (slots < 0) {
		p += slots * BLOCK_SIZE;
	}
	int16_t slotsAvailable = (heapEnd - p) / BLOCK_SIZE;
	
	/* TODO implement something like this
	if (__brkval == 0)
		__brkval = __malloc_heap_start;
	cp = __malloc_heap_end;
	if (cp == 0)
		cp = STACK_POINTER() - __malloc_margin;
	if (cp <= __brkval)
	  // Memory exhausted.
	  return 0;
	avail = cp - __brkval;
	*/
	
	if (slotsAvailable < neededSlots) {
		return 0;
	}
	
	*p = neededSlots;
	*(p + neededSlots * BLOCK_SIZE - 1) = neededSlots;
	
	currentHeapEnd = p + neededSlots * BLOCK_SIZE;
	
	return (void *) (p + 1);
}

// ----------------------------------------------------------------------------
void
xpcc::avr::freeMemory(void *ptr)
{
	if (ptr == 0) {
		return;
	}
	
	uint8_t *p = (uint8_t *) ptr;
	p -= 1;
	
	int8_t slots = *p;
	uint16_t freeSlots = slots;
	
	// check whether the slots above are free
	slots = *(p + slots * BLOCK_SIZE);
	if (slots < 0) {
		freeSlots += -slots;
	}
	
	// check the slots below
	slots = *(p - 1);
	if (slots < 0) {
		p += slots * BLOCK_SIZE;
		freeSlots += -slots;
	}
	
	// write the markers
	if (freeSlots > MAX_BLOCK_PARTS)
	{
		// memory area is to big, split it in two parts
		*p = -MAX_BLOCK_PARTS;
		*(p + MAX_BLOCK_PARTS * BLOCK_SIZE - 1) = -MAX_BLOCK_PARTS;
		
		*(p + MAX_BLOCK_PARTS * BLOCK_SIZE) = -(freeSlots - MAX_BLOCK_PARTS);
		*(p + freeSlots * BLOCK_SIZE - 1) = -(freeSlots - MAX_BLOCK_PARTS);
	}
	else
	{
		*p = -freeSlots;
		*(p + freeSlots * BLOCK_SIZE - 1) = -freeSlots;
	}
	
	if (p < freeHint) {
		freeHint = p;
	}
}

// ----------------------------------------------------------------------------

#ifdef ALLOC_TEST

#include <iostream>
#include <time.h>
#include <cstring>

void
printMemoryLayout(void)
{
	std::cout << "start";
	
	uint8_t *p = heapStart;
	do
	{
		int8_t slots = *p;
		
		if (slots < 0)
		{
			slots = -slots;
			std::cout << " > f" << (int) slots;
		}
		else {
			std::cout << " > u" << (int) slots;
		}
		
		p += slots * BLOCK_SIZE;
	}
	while (p < currentHeapEnd);
	
	std::cout << std::endl;
}

void *
allocate(size_t size)
{
	return xpcc::avr::allocateMemory(size);
}

void
release(void *p)
{
	xpcc::avr::freeMemory(p);
}

int
main(void)
{
	void *handles[32];
	size_t sizes[32];
	int i = 0, j = 0, k = 0, l = 0, m = 0, om = 0, p = 0, f = 0;
	size_t s;
	
	initializeMemory();
	
	srand(time(0));

	for (k = 0; k < 100; k++)
	{
		memset(handles, 0, sizeof handles);
		memset(sizes, 0, sizeof sizes);

		j = rand() % 16 + 15;
		l = rand() % 80 + 7;

		for (i = s = 0; i < j && s < 4096; i++)
		{
			sizes[i] = rand() % l + 1;
			s += sizes[i];
		}
		
		j = i;
		
		for (m = om = 1, p = 1, f = 0; m < 1000; m++)
		{
			for (i = s = 0; i < j; i++) {
				if (handles[i])
					s++;
			}
			if (s == (unsigned)j)
				break;
			
			if (m / om > 10) {
				p <<= 1;
				p |= 1;
			}
			
			for (i = 0; i < j; i++)
			{
				if (rand() & p) {
					if (!handles[i] &&
					    (handles[i] = allocate(sizes[i])) == 0) {
						f++;
					}
				}
			}
			for (i = 0; i < j; i++)
			{
				if (rand() & 1) {
					release(handles[i]);
					handles[i] = 0;
				}
			}
		}
		if (f) {
			std::cout << f << "alloc failure" << ((f == 1) ? "" : "s") << " total\n";
		}
		printMemoryLayout();
		for (i = 0; i < j; i++) {
			release(handles[i]);
		}
		std::cout << "after cleanup\n";
		printMemoryLayout();
		std::cout << "\n";
	}
	
	return 0;
}

#endif	// ALLOC_TEST
