#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2022, Andrey Kunitsyn
# Copyright (c) 2023, Thomas Rush
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### UF2 Converter

UF2 is a [Microsoft file format](https://github.com/microsoft/uf2) to pass
to a on-device bootloader.

```sh
python3 -m modm_tools.elf2uf2 firmware.elf -o firmware.uf2 --target rp2040 \
    --range 0x10000000:0x15000000:CONTENTS \
    --range 0x20000000:0x20042000:NO_CONTENTS
```

(\* *only ARM Cortex-M targets*)
"""

import struct

from pathlib import Path

verbose = False
warned = False

# uf2_config is a dictionary with the family name (which is basically just the
#  start of the part name) as the key, and a two item tuple as the value.
#  The first value of the tuple is the family id (a 32-bit number that was
#  randomly assigned and agreed upon), and the second is a boolean indicating
#  whether or not an FPU is available for this part.

# IMPORTANT: The search for the family name using the target is straightforward,
#  EXCEPT for three names which overlap: stm32f407vg, stm32f407, and stm32f4.
#  Using 'startswith()' requires that they MUST be checked in that order.
#  If target was 'stm32f407vgt6', for example, and the entry 'stm32f4' came
#  first in the name list, then 'target.startswith(entry)' would be true and
#  the search would end at that point, not reaching the correct entry of
#  'stm32f407vg'. Therefore, if any entries are added to this dictionary, the
#  order of those three must NOT be changed.

uf2_config = {
    "rp2040":    ( 0xe48bff56, False ), "samd21":      ( 0x68ed2b88, False ),
    # same5x shares its id with samd51
    "samd51":    ( 0x55114460, True ),  "same5":       ( 0x55114460, True ),
    "saml21":    ( 0x1851780a, False ), "stm32f0":     ( 0x647824b6, False ),
    "stm32f1":   ( 0x5ee21072, False ), "stm32f2":     ( 0x5d1a0a2e, False ),
    "stm32f3":   ( 0x6b846188, True ),  "stm32f407vg": ( 0x8fb060fe, True ),
    "stm32f407": ( 0x6d0922fa, True ),  "stm32f4":     ( 0x57755a57, True ),
    "stm32f7":   ( 0x53b80f00, True ),  "stm32g0":     ( 0x300f5633, False ),
    "stm32g4":   ( 0x4c71240a, True ),  "stm32h7":     ( 0x6db66082, True ),
    "stm32l0":   ( 0x202e3a91, False ), "stm32l1":     ( 0x1e1f432d, False ),
    "stm32l4":   ( 0x00ff6919, True ),  "stm32l5":     ( 0x04240bdf, True ),
    "stm32wb":   ( 0x70d16653, True ),  "stm32wl":     ( 0x21460ff0, True )
}

# UF2 constants
UF2_MAGIC_START0 = b'UF2\n'
UF2_MAGIC_START1 = 0x9E5D5157 # Randomly selected
UF2_MAGIC_END    = 0x0AB16F30 # Ditto

UF2_FLAG_NOT_MAIN_FLASH     = 0x00000001
UF2_FLAG_FILE_CONTAINER     = 0x00001000
UF2_FLAG_FAMILY_ID_PRESENT  = 0x00002000
UF2_FLAG_MD5_PRESENT        = 0x00004000

UF2_PAGE_SIZE = (1 << 8)

#struct uf2_block {
#    // 32 byte header
#    uint32_t magic_start0;  encoded as a byte string '4s' for python
#    uint32_t magic_start1;
#    uint32_t flags;
#    uint32_t target_addr;
#    uint32_t payload_size;
#    uint32_t block_no;
#    uint32_t num_blocks;
#    uint32_t file_size; // or familyID;
uf2_block = "<4s7I"
#    uint8_t  data[476];
#    uint32_t magic_end;

# Constants in ELF headers
ELF_MAGIC = b'\x7fELF'
EM_ARM = 0x28
EF_ARM_ABI_FLOAT_HARD = 0x00000400
PT_LOAD = 0x00000001

# Elf header structures
#  Not all the fields in the structs are used by this program; the ones that
#  are have an index by their names. The python struct unpack strings have
#  extra 'x's to bypass the unused fields.
# struct elf_header:
#     uint32_t    magic;        (0)  encoded as a byte string '4s' for python
#     uint8_t     arch_class;   (1)
#     uint8_t     endianness;   (2)
#     uint8_t     version;      (3)
#     uint8_t     abi;          (4)
#     uint8_t     abi_version;
#     uint8_t     _pad[7];
#     uint16_t    type;
#     uint16_t    machine;      (5)
#     uint32_t    version2;     (6)
elf_header = "<4s4B10xHI"
elf_header_size = struct.calcsize(elf_header)
# struct elf32_header:
# struct elf_header common;
#     uint32_t    entry;
#     uint32_t    ph_offset;    (0)
#     uint32_t    sh_offset;
#     uint32_t    flags;        (1)
#     uint16_t    eh_size;      (2)
#     uint16_t    ph_entry_size;(3)
#     uint16_t    ph_num;       (4)
#     uint16_t    sh_entry_size;
#     uint16_t    sh_num;
#     uint16_t    sh_str_index;
elf32_header = "<4xI4xI3H6x"
elf32_header_size = struct.calcsize(elf32_header)
# struct elf32_ph_entry:
#     uint32_t type;    (0)
#     uint32_t offset;  (1)
#     uint32_t vaddr;   (2)
#     uint32_t paddr;   (3)
#     uint32_t filesz;  (4)
#     uint32_t memsz;   (5)
#     uint32_t flags;
#     uint32_t align;
elf32_ph_entry = "<6I8x"
elf32_ph_entry_size = struct.calcsize(elf32_ph_entry)


# -----------------------------------------------------------------------------
def read_header(source_bytes, has_fpu):
    magic, arch_class, endianness, version, abi, machine, version2 = struct.unpack_from(elf_header, source_bytes, 0)

    if magic != ELF_MAGIC:
        raise Exception("Not an ELF file")
    if version != 1 or version2 != 1:
        raise Exception("Unrecognized ELF version")
    if arch_class != 1 or endianness != 1:
        raise Exception("Require 32 bit little-endian ELF")
    if machine != EM_ARM:
        raise Exception("Not an ARM executable")
    if abi != 0:
        raise Exception("Unrecognized ABI")

    ph_offset, flags, eh_size, ph_entry_size, ph_num = struct.unpack_from(elf32_header, source_bytes, elf_header_size)

    if (eh_size != (elf_header_size+elf32_header_size)):
        raise Exception("Invalid ELF32 format")

    if (ph_entry_size != elf32_ph_entry_size):
        raise Exception("Invalid ELF32 program header")

    if (flags & EF_ARM_ABI_FLOAT_HARD) and not has_fpu:
        raise Exception("HARD-FLOAT not supported")

    return ph_offset, ph_num


def check_address_range(valid_ranges, addr, size, is_phys_addr):
    for range in valid_ranges:
        if range["start"] <= addr and range["end"] >= (addr+size):
            if is_phys_addr and range["type"] == "NO_CONTENTS" and size != 0:
                err = "ELF has contents for uninitialized memory at {:08x}->{:08x}".format(addr,addr+size)
                raise Exception(err)
            return

    # No range was found. With some targets, external memory is added, or a contiguous region is split
    #  into 2 or more regions in the device file while the linkerscript provides a way to lump it into
    #  one. These added memory ranges (typically RAM) do not show up in the build system commands for
    #  this program, and strict adherence to the command-provided ranges can cause unnecessary failure.
    #  If 'size' is zero, the conversion continues without any message. When 'size' is non-zero, then if
    #  'addr' is a paddr (physical address, meaning there is content to be loaded), an exception is
    #  raised and the program aborts; otherwise a warning is printed and the conversion continues.
    #
    #  (As a side effect of continuing with missing ranges: when executing from the command line, the
    #  only range that MUST be provided is for the flash, with a type of 'CONTENTS'.)

    global warned
    if size != 0:
        err = "Memory segment {:08x}->{:08x} is outside of supplied address ranges.".format(addr, addr+size)
        if is_phys_addr:
            raise Exception(err)
        if not warned:
            # This message is printed once; the range message is printed for each offending segment.
            print("Warning: ranges supplied don't match or don't include all those in ELF file.")
            warned = True
        print(err)


def read_and_check_elf32_ph_entries(buffer, ph_offset, ph_num, valid_ranges, pages, page_size):
    for ph in range(ph_num):
        type, offset, vaddr, paddr, filesz, memsz = struct.unpack_from(elf32_ph_entry,buffer,ph_offset+ph*elf32_ph_entry_size)
        if type == PT_LOAD and memsz !=0:
            check_address_range(valid_ranges, paddr, filesz, True)
            check_address_range(valid_ranges, vaddr, memsz, False)
            if verbose:
                print(("{} segment {:08x}->{:08x} ({:08x}->{:08x})").format(memsz>filesz \
                    and "Uninitialized" or "Mapped",paddr,paddr+filesz,vaddr,vaddr+memsz))
            if filesz != 0:
                addr = paddr
                remaining = filesz
                file_offset = offset
                while remaining > 0:
                    off = addr & (page_size - 1)
                    chlen = min(remaining, page_size - off)
                    key = addr - off
                    fragments = []
                    if key in pages:
                        fragments = pages[key]
                        for fragment in fragments:
                            page_offset, byte_count = fragment[1:]
                            if (off < page_offset + byte_count) != ((off + chlen) <= page_offset):
                                raise Exception("In memory segments overlap")
                    fragments.append(tuple([file_offset, off, chlen]))
                    addr += chlen
                    file_offset += chlen
                    remaining -= chlen
                    pages[key] = fragments


def realize_page(buffer, page):
    result = bytes(476)
    for frag in page:
        file_offset, page_offset, byte_count = frag
        data = buffer[file_offset:file_offset+byte_count]
        if len(data) != byte_count:
            raise Exception("failed extract")
        if page_offset == 0:
            result = data + result[byte_count:]
        else:
            result = result[:page_offset] + data + result[page_offset+byte_count:]
        if len(result) != 476:
            raise Exception("failed concat")
    return result


def convert_data(source_bytes, target, family, ranges):
    family_id, has_fpu = uf2_config[family]
    ph_offset, ph_num = read_header(source_bytes, has_fpu)
    if verbose:
        print("Build for chip {} in UF2 family {}".format(target, family))
    pages = {}
    read_and_check_elf32_ph_entries(source_bytes, ph_offset, ph_num, ranges, pages, UF2_PAGE_SIZE)
    if len(pages) == 0:
        raise Exception("The input file has no memory pages")

    num_blocks = len(pages)
    page_num = 0
    file_content = bytes(0)
    for target_addr, page in pages.items():
        if verbose:
            print("Page {} / {} {:08x}".format(page_num, num_blocks, target_addr))

        data = realize_page(source_bytes,page)
        block = struct.pack(uf2_block,
                            UF2_MAGIC_START0,
                            UF2_MAGIC_START1,
                            UF2_FLAG_FAMILY_ID_PRESENT,
                            target_addr,
                            UF2_PAGE_SIZE,
                            page_num,
                            num_blocks,
                            family_id) + data + struct.pack("<I",UF2_MAGIC_END)
        if len(block) != 512:
            raise Exception("Invalid block size generated")
        page_num += 1
        file_content += block
    return file_content


def check_target(target):
    for entry in list(uf2_config):
        if target.startswith(entry):
            return entry
    err = "UF2 family id not found for target {}".format(target)
    raise Exception(err)


def check_valid_range(ranges):
    # Sanity check for supplied ranges.
    no_content = True
    err = ''
    for range in ranges:
        if range["start"] >= range["end"]:
            err += "Supplied memory range {:08x}->{:08x} has length <= 0\n".format(range["start"],range["end"])
        if range["type"] == "CONTENTS":
            no_content = False
    if no_content:
        err += "No ranges with type 'CONTENTS'\n"
    if len(err) > 0:
        raise Exception(err)


def convert(source, output, target, ranges):
    family = check_target(target)
    check_valid_range(ranges)
    source_bytes = Path(source).read_bytes()
    uf2 = convert_data(source_bytes, target, family, ranges)
    Path(output).write_bytes(uf2)


def parse_range(strval):
    if strval.startswith("0x"):
        return int(strval,16)
    return int(strval)


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert ELF to UF2 image.")
    parser.add_argument(
            dest="source",
            metavar="ELF",
            help="Input ELF binary")
    parser.add_argument(
            "--verbose",
            action="store_true",
            help="Verbose output")
    parser.add_argument(
            "-o", "--output",
            required=True,
            help="Destination UF2 image")
    parser.add_argument(
            "--target",
            required=True,
            help="Target chip")
    parser.add_argument(
            "--range",
            action="append",
            dest="ranges",
            required=True,
            help="Memory range in format start:end:type, where type NO_CONTENTS|CONTENTS|IGNORE")

    args = parser.parse_args()
    verbose = args.verbose
    target = args.target.lower()
    ranges = []
    for r in args.ranges:
        start, end, t = r.split(":")
        ranges.append({
            "start": parse_range(start),
            "end": parse_range(end),
            "type": t
        })


    convert(args.source, args.output, target, ranges)
