
#include <stdint.h>
#include <stdlib.h>

// TODO
#define	NEW__BLOCK_SIZE		16
#define	NEW__SLOTS			30

// ----------------------------------------------------------------------------
static uint8_t storage[NEW__SLOTS * NEW__BLOCK_SIZE];
static  int8_t storage_mangement[NEW__SLOTS] = {
	-NEW__SLOTS,
};

// ----------------------------------------------------------------------------
static void *
allocate_memory(uint8_t size__)
{
	div_t res;
	uint8_t i;
	uint8_t needed_slots;
	
	if (size__ == 0)
		return NULL;
	
	// calculate the number of needed slots for the given size
	res = div(size__, NEW__BLOCK_SIZE);
	needed_slots = res.quot;
	if (res.rem > 0)
		needed_slots += 1;
	
	i = 0;
	do
	{
		if (storage_mangement[i] < 0)
		{
			uint8_t free_size = -storage_mangement[i];
			
			if (free_size >= needed_slots)
			{
				// write the marker on the first an last slot of the field of 
				// new allocated slots
				storage_mangement[i] = needed_slots;
				storage_mangement[i + needed_slots - 1] = needed_slots;
				
				if (free_size > needed_slots)
				{
					// if the allocated space are smaller than original free one,
					// than we get a new slice of free slots and need set the 
					// new marker for this field
					storage_mangement[i + needed_slots] = -(free_size - needed_slots);
					storage_mangement[i + free_size - 1] = -(free_size - needed_slots);
				}
				
				return &storage[i * NEW__BLOCK_SIZE];
			}
			else {
				i += free_size;
			}
		}
		else {
			i += storage_mangement[i];
		}
	}
	while (i < NEW__SLOTS);
	
	return NULL;
}

// ----------------------------------------------------------------------------
static void
free_memory(void *ptr)
{
	uint8_t index;
	uint8_t number_of_slots;
	uint8_t free_slots;
	uint8_t i;
	
	if (ptr == NULL)
		return;
	
	// calculate the current slot number
	index = ((uint16_t) ptr - (uint16_t) storage) / NEW__BLOCK_SIZE;
	number_of_slots = storage_mangement[index];
	
	free_slots = number_of_slots;
	
	// check whether the slots above are free
	i = index + number_of_slots;
	if (i < NEW__SLOTS)
	{
		if (storage_mangement[i] < 0) {
			free_slots += -storage_mangement[i];
		}
	}
	
	// check the slots below
	if (index != 0)
	{
		i = index - 1;
		if (storage_mangement[i] < 0) {
			index += storage_mangement[i];
			free_slots += -storage_mangement[i];
		}
	}
	
	// write the two new marker
	storage_mangement[index] = -free_slots;
	if (index + free_slots - 1 < NEW__SLOTS) {
		storage_mangement[index + free_slots - 1] = -free_slots;
	}
}

// -----------------------------------------------------------------------------
void *
operator new(size_t size) {
	return allocate_memory(size);
}

// -----------------------------------------------------------------------------
void *
operator new[](size_t size) {
	return allocate_memory(size);
}

// -----------------------------------------------------------------------------
void
operator delete(void* ptr) {
	free_memory(ptr);
}

// -----------------------------------------------------------------------------
void
operator delete[](void* ptr) {
	free_memory(ptr);
}
