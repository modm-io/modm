# Offset of FLASH Section Origin

Add an offset to the default start address of the flash memory. This might be
required for bootloaders located there.

!!! warning "Vector Table Relocation"
    Not all offsets are compatible with the vector table relocation.
