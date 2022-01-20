#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2022, Andrey Kunitsyn
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2022, Andrey Kunitsyn

"""
### UF2 Converter

UF2 is a [Microsoft file format](https://github.com/microsoft/uf2) to pass
to a on-device bootloader.

```sh
python3 modm/modm_tools/elf2uf2.py firmware.elf -o firmware.uf2 --target rp2040 \
    --range 0x10000000:0x15000000:CONTENTS \
    --range 0x20000000:0x20042000:NO_CONTENTS
```

(\* *only ARM Cortex-M targets*)
"""

import os
import struct

from pathlib import Path

verbose = False



UF2_FLAG_NOT_MAIN_FLASH     = 0x00000001
UF2_FLAG_FILE_CONTAINER     = 0x00001000
UF2_FLAG_FAMILY_ID_PRESENT  = 0x00002000
UF2_FLAG_MD5_PRESENT        = 0x00004000


uf2_config = {
    "rp2040": {
        "MAGIC_START0": 0x0A324655,
        "MAGIC_START1": 0x9E5D5157,
        "MAGIC_END": 0x0AB16F30,
        "FAMILY_ID": 0xe48bff56,
        "PAGE_SIZE": (1 << 8),
    }
}

#struct uf2_block {
#    // 32 byte header
#    uint32_t magic_start0;
#    uint32_t magic_start1;
#    uint32_t flags;
#    uint32_t target_addr;
#    uint32_t payload_size;
#    uint32_t block_no;
#    uint32_t num_blocks;
#    uint32_t file_size; // or familyID;
uf2_block = "<IIIIIIII"
#    uint8_t  data[476];

#    uint32_t magic_end;

ELF_MAGIC = 0x464c457f
EM_ARM = 0x28
EF_ARM_ABI_FLOAT_HARD = 0x00000400
PT_LOAD = 0x00000001

# struct elf_header:
# uint32_t    magic;        (0)
# uint8_t     arch_class;   (1)
# uint8_t     endianness;   (2)
# uint8_t     version;      (3)
# uint8_t     abi;          (4)
# uint8_t     abi_version;  (5)
# uint8_t     _pad[7];
# uint16_t    type;         (6)
# uint16_t    machine;      (7)
# uint32_t    version2;     (8)
elf_header = "<IBBBBBxxxxxxxHHI"
elf_header_size = struct.calcsize(elf_header)
# struct elf32_header:
# struct elf_header common;
#     uint32_t    entry;        (0)
#     uint32_t    ph_offset;    (1)
#     uint32_t    sh_offset;    (2)
#     uint32_t    flags;        (3)
#     uint16_t    eh_size;      (4)
#     uint16_t    ph_entry_size;(5)
#     uint16_t    ph_num;       (6)
#     uint16_t    sh_entry_size;(7)
#     uint16_t    sh_num;       (8)
#     uint16_t    sh_str_index; (9)
elf32_header = "<IIIIHHHHHH"
elf32_header_size = struct.calcsize(elf32_header)
# struct elf32_ph_entry:
#     uint32_t type;    (0)
#     uint32_t offset;  (1)
#     uint32_t vaddr;   (2)
#     uint32_t paddr;   (3)
#     uint32_t filez;   (4)
#     uint32_t memsz;   (5)
#     uint32_t flags;   (6)
#     uint32_t align;   (7)
elf32_ph_entry = "<IIIIIIII"
elf32_ph_entry_size = struct.calcsize(elf32_ph_entry)

def read_header(source_bytes):
    pos = 0
    eh = struct.unpack_from(elf_header,source_bytes,pos)
    pos += elf_header_size

    if eh[0] != ELF_MAGIC:
        raise Exception("Not an ELF file")
    if eh[3] != 1 or eh[8] != 1:
        raise Exception("Unrecognized ELF version")
    if eh[1] != 1 or eh[2] != 1:
        raise Exception("Require 32 bit little-endian ELF")
    if eh[7] != EM_ARM:
        raise Exception("Not an ARM executable")
    if eh[4] != 0:
        raise Exception("Unrecognized ABI")

    e32h = struct.unpack_from(elf32_header,source_bytes,pos)
    pos += elf32_header_size

    if (e32h[4] != (elf_header_size+elf32_header_size)):
        raise Exception("Invalid ELF32 format")

    if (e32h[5] != elf32_ph_entry_size):
        raise Exception("Invalid ELF32 program header")

    if (e32h[3] & EF_ARM_ABI_FLOAT_HARD) != 0 :
        raise Exception("HARD-FLOAT not supported")

    return e32h

def check_address_range(valid_ranges, addr, vaddr, size, uninitialized):
    for range in valid_ranges:
        if range["start"] <= addr and range["end"] >= (addr+size):
            if range["type"] == "NO_CONTENTS" and not uninitialized:
                raise Exception("ELF contains memory contents for uninitialized memory")
            if verbose:
                print(("{} segment {:08x}->{:08x} ({:08x}->{:08x})").format(uninitialized and "Uninitialized" or "Mapped", addr, addr + size, vaddr, vaddr+size))
            return range
    raise Exception("Memory segment {:08x}->{:08x} is outside of valid address range for device".format(addr, addr+size))

def read_and_check_elf32_ph_entries(buffer, eh, valid_ranges, pages, page_size):
    for i in range(eh[6]):
        entry = struct.unpack_from(elf32_ph_entry,buffer,eh[1]+i*elf32_ph_entry_size)
        if entry[0] == PT_LOAD and entry[5] !=0:
            mapped_size = min(entry[5],entry[4])
            if mapped_size != 0:
                ar = check_address_range(valid_ranges,entry[3],entry[2],mapped_size,False)
                #  we don"t download uninitialized, generally it is BSS and should be zero-ed by crt0.S, or it may be COPY areas which are undefined
                if ar["type"] != "CONTENTS":
                    if verbose:
                        print("  ignored");
                    continue
                addr = entry[3];
                remaining = mapped_size;
                file_offset = entry[1];
                while remaining > 0:
                    off = addr & (page_size - 1)
                    chlen = min(remaining, page_size - off)
                    key = addr - off
                    fragments = []
                    if key in pages:
                        fragments = pages[key]
                        for fragment in fragments:
                            if (off < fragment["page_offset"] + fragment["bytes"]) != ((off + chlen) <= fragment["page_offset"]):
                                raise Exception("In memory segments overlap")

                    fragments.append({"file_offset":file_offset,"page_offset":off,"bytes":chlen})
                    addr += chlen
                    file_offset += chlen
                    remaining -= chlen
                    pages[key] = fragments

            if entry[5] > entry[4]:
                # we have some uninitialized data too
                check_address_range(valid_ranges, entry[3] + entry[4], entry[2] + entry[4], entry[5] - entry[4], True);

def realize_page(buffer, fragments):
    result = bytes(476)
    for frag in fragments:
        data = buffer[frag["file_offset"]:frag["file_offset"]+frag["bytes"]]
        if len(data) != frag["bytes"]:
            raise Exception("failed extract")
        if frag["page_offset"] == 0:
            result = data + result[frag["page_offset"]+frag["bytes"]:]
        else:
            result = result[:frag["page_offset"]] + data + result[frag["page_offset"]+frag["bytes"]:]
        if len(result) != 476:
            raise Exception("failed concat")
    return result


def convert_data(source_bytes,target,ranges):
    eh = read_header(source_bytes)
    config = uf2_config[target]
    if verbose:
        print("Build for chip:{}".format(target))
    pages = {}
    read_and_check_elf32_ph_entries(source_bytes,eh,ranges,pages,config["PAGE_SIZE"])
    if len(pages) == 0:
        raise Exception("The input file has no memory pages")

    num_blocks = len(pages)
    page_num = 0
    file_content = bytes(0)
    for target_addr,pages in pages.items():
        if verbose:
            print("Page {} / {} {:08x}".format( page_num, num_blocks, target_addr))

        data = realize_page(source_bytes,pages)
        block = struct.pack(uf2_block,
                            config["MAGIC_START0"],
                            config["MAGIC_START1"],
                            UF2_FLAG_FAMILY_ID_PRESENT,
                            target_addr,
                            config["PAGE_SIZE"],
                            page_num,
                            num_blocks,
                            config["FAMILY_ID"]) + data + struct.pack("<I",config["MAGIC_END"])
        if len(block) != 512:
            raise Exception("Invalid block size generated")
        page_num += 1
        file_content += block
    return file_content

def convert(source, output, target, ranges):
    source_bytes = Path(source).read_bytes()
    uf2 = convert_data(source_bytes,target, ranges)
    Path(output).write_bytes(uf2)

def parse_range(strval):
    if strval.startswith("0x"):
        return int(strval[2:],16)
    return int(strval)

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert ELF to UF2 image.")
    parser.add_argument(
            dest="source",
            metavar="ELF",
            help="Input ELF binary")
    parser.add_argument(
            "-o", "--output",
            dest="output",
            required=True,
            help="Destination UF2 image")
    parser.add_argument(
            "--verbose",
            dest="verbose",
            action="store_true",
            help="Verbose output")
    parser.add_argument(
            "--target",
            dest="target",
            default="rp2040",
            help="Target chip")
    parser.add_argument(
            "--range",
            action="append",
            dest="ranges",
            required=True,
            help="Memory range in format start:end:type, where type NO_CONTENTS|CONTENTS|IGNORE")

    args = parser.parse_args()
    verbose = args.verbose
    ranges = []
    for r in args.ranges:
        start, end, t = r.split(":")
        ranges.append({
            "start": parse_range(start),
            "end": parse_range(end),
            "type": t
        })

    convert(args.source, args.output, args.target, ranges)
