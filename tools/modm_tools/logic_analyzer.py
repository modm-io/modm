#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2026, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Logic Analyzer

Reads the samples of the `modm:driver:logic_analyzer` from the target via
OpenOCD and SWD, prints the capture metadata, writes a VCD file and optionally
decodes I2C.

When the driver module is selected, the build system copies this tool into
`modm/modm_tools` of the generated project:

```sh
PYTHONPATH=modm python3 -m modm_tools.logic_analyzer --elf path/to/project.elf \
    --channels A8,B8 --vcd capture.vcd --i2c A8,B8
```

In a modm checkout use `PYTHONPATH=tools` instead. Channels are named by port
letter and pin number, for example `A8,B8,C6,D2`, or `all` for every pin of the
captured ports. If `--channels` is omitted together with `--vcd`, all pins are
exported. `--serial`/`--probe` selects the debug probe when several are
connected and `--info` only prints the metadata.
"""

import argparse, os, struct, subprocess, tempfile
from pathlib import Path

# magic, version, rate, samples, ports, timer,
# port[4], base[4], buffer[4], ndtr[4], circular, running, count[4], head[4]
HEADER = struct.Struct("<32I")
MAGIC = 0x4C4F4741
VERSION = 3


def symbol_address(elf, name="modm_logic_analyzer"):
    from elftools.elf.elffile import ELFFile
    with open(elf, "rb") as f:
        for section in ELFFile(f).iter_sections():
            if section.name != ".symtab":
                continue
            for symbol in section.iter_symbols():
                if symbol.name == name:
                    return symbol["st_value"]
    raise ValueError(f"Symbol {name} not found in {elf}")


def read_memory(config, serial, regions, openocd="openocd"):
    """Reads the (address, size) regions with OpenOCD and returns their bytes."""
    with tempfile.TemporaryDirectory() as tmp:
        files = [Path(tmp, f"r{index}.bin") for index in range(len(regions))]
        cmd = [openocd]
        if serial:
            cmd += ["-c", f"adapter serial {serial}"]
        cmd += ["-c", "gdb port disabled", "-c", "tcl port disabled", "-c", "telnet port disabled"]
        for cfg in config:
            cmd += ["-f", cfg]
        cmd += ["-c", "init"]
        for path, (address, size) in zip(files, regions):
            cmd += ["-c", f"dump_image {path} {address:#x} {size}"]
        cmd += ["-c", "shutdown"]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError("OpenOCD failed to read the capture:\n" +
                               (result.stdout + result.stderr)[-4000:])
        return [path.read_bytes() for path in files]


def read_capture(elf, config, serial=None, openocd="openocd"):
    address = symbol_address(elf)
    (header,) = read_memory(config, serial, [(address, HEADER.size)], openocd)
    values = HEADER.unpack(header)
    magic, version, rate, samples, ports, timer = values[:6]
    if magic != MAGIC:
        raise ValueError(f"No logic analyzer descriptor at {address:#x}")
    if version != VERSION:
        raise ValueError(f"Unsupported logic analyzer descriptor version {version}")
    if ports not in (1, 2, 3, 4):
        raise ValueError(f"Invalid number of logic analyzer ports: {ports}")
    if rate == 0 or samples == 0:
        raise ValueError("The logic analyzer has not been started on the target")

    port_index = list(values[6:10])[:ports]
    bases = list(values[10:14])[:ports]
    buffer_addresses = list(values[14:18])[:ports]
    ndtrs = list(values[18:22])[:ports]
    circular, running = values[22:24]
    counts = list(values[24:28])[:ports]
    heads = list(values[28:32])[:ports]

    regions = [(buffer, samples * 4) for buffer in buffer_addresses]
    if running:
        regions += [(ndtr, 4) for ndtr in ndtrs]
    data = read_memory(config, serial, regions, openocd)
    samples_data = data[:ports]
    if running:
        for index, remaining_data in enumerate(data[ports:]):
            (remaining,) = struct.unpack("<I", remaining_data)
            if circular:
                # The last `samples` values are always valid in a ring buffer.
                written = (samples - remaining) % samples
                counts[index] = samples
                heads[index] = written
            else:
                counts[index] = max(0, samples - remaining)
                heads[index] = 0

    result = {}
    for index, raw in enumerate(samples_data):
        values = list(struct.unpack(f"<{samples}I", raw))
        count = min(counts[index], samples)
        head = heads[index] % samples
        result[chr(ord("A") + port_index[index])] = (values[head:] + values[:head])[:count]

    return {
        "rate": rate,
        "samples": samples,
        "ports": ports,
        "timer": timer,
        "circular": bool(circular),
        "running": bool(running),
        "base": bases,
        "count": [counts[index] for index in range(ports)],
        "buffers": result,
    }


def edges(rate, buffers, channels):
    """Returns {channel: [(time_ns, level)]} with the initial level at time 0."""
    result = {}
    for port, values in buffers.items():
        for channel in channels:
            if not channel.startswith(port):
                continue
            try:
                pin = int(channel[1:])
            except ValueError:
                raise ValueError(f"Invalid channel name '{channel}', expected e.g. A8") from None
            if not 0 <= pin <= 15:
                raise ValueError(f"Invalid pin number in channel '{channel}'")
            mask = 1 << pin
            changes, last = [], None
            for index, value in enumerate(values):
                level = int(bool(value & mask))
                if level != last:
                    changes.append((round(index * 1e9 / rate), level))
                    last = level
            result[channel] = changes
    for channel in channels:
        if channel not in result:
            raise ValueError(f"Channel '{channel}' is not one of the captured ports "
                             f"{', '.join(buffers)}")
    return result


def write_vcd(path, rate, channels, changes):
    ids = {channel: chr(33 + index) for index, channel in enumerate(channels)}
    events = sorted((time, channels.index(channel), level)
                    for channel in channels for time, level in changes.get(channel, []))
    with open(path, "w") as f:
        f.write(f"$comment rate {rate} Hz, channels {','.join(channels)} $end\n")
        f.write("$timescale 1ns $end\n$scope module logic_analyzer $end\n")
        for channel in channels:
            f.write(f"$var wire 1 {ids[channel]} P{channel} $end\n")
        f.write("$upscope $end\n$enddefinitions $end\n")
        last = None
        for time, index, level in events:
            if time != last:
                f.write(f"#{time}\n")
                last = time
            f.write(f"{level}{ids[channels[index]]}\n")


def decode_i2c(scl_changes, sda_changes):
    events = sorted([(time, 0, level) for time, level in scl_changes[1:]] +
                    [(time, 1, level) for time, level in sda_changes[1:]])
    scl, sda = scl_changes[0][1], sda_changes[0][1]
    output, bits = [], []
    for time, channel, level in events:
        if channel == 1:
            if scl and sda and not level:
                output.append(f"{time / 1000:9.1f}us START")
                bits = []
            elif scl and not sda and level:
                output.append(f"{time / 1000:9.1f}us STOP")
                bits = []
            sda = level
        else:
            if not scl and level:
                bits.append(sda)
                if len(bits) == 9:
                    byte = int("".join(map(str, bits[:8])), 2)
                    output.append(f"{time / 1000:9.1f}us 0x{byte:02X} "
                                  f"{'NACK' if bits[8] else 'ACK'}")
                    bits = []
            scl = level
    return output


def main():
    parser = argparse.ArgumentParser(
        description="Read and export a modm logic analyzer capture.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""examples:
  %(prog)s --elf build/project.elf --vcd capture.vcd
  %(prog)s --elf build/project.elf --channels A8,B8 --vcd capture.vcd
  %(prog)s --elf build/project.elf --i2c B6,B9 --vcd i2c.vcd
  %(prog)s --elf build/project.elf --info
""")
    parser.add_argument("--elf", required=True, help="ELF file of the running firmware")
    parser.add_argument("--config", "-f", action="append", default=[],
                        help="OpenOCD configuration (default: modm/openocd.cfg)")
    parser.add_argument("--serial", "--probe", dest="serial",
                        help="Serial number of the debug probe")
    parser.add_argument("--openocd", default=os.environ.get("MODM_OPENOCD_BINARY", "openocd"),
                        help="OpenOCD binary (default: $MODM_OPENOCD_BINARY or openocd)")
    parser.add_argument("--channels",
                        help="Pins to export, e.g. A8,B8 or all (default: all pins with --vcd)")
    parser.add_argument("--vcd", help="VCD output file")
    parser.add_argument("--i2c", help="Decode I2C on SCL,SDA, e.g. A8,B8")
    parser.add_argument("--info", action="store_true",
                        help="Only print the capture metadata")
    args = parser.parse_args()

    if not args.config:
        default = Path("modm/openocd.cfg")
        if not default.exists():
            parser.error("--config is required when modm/openocd.cfg does not exist")
        args.config = [str(default)]

    try:
        capture = read_capture(args.elf, args.config, args.serial, args.openocd)
    except (ValueError, RuntimeError) as error:
        parser.error(str(error))
    buffers = capture["buffers"]
    counts = capture["count"]
    count_text = str(counts[0]) if len(set(counts)) == 1 else \
        ", ".join(f"P{port} {count}" for port, count in zip(buffers, counts))
    print(f"{count_text} samples at {capture['rate']} Hz of ports "
          f"{', '.join(buffers)} (timer TIM{capture['timer']}, "
          f"{'circular' if capture['circular'] else 'one-shot'}"
          f"{', still running' if capture['running'] else ''})")
    if args.info:
        return

    all_pins = [f"{port}{pin}" for port in buffers for pin in range(16)]
    if args.channels:
        if args.channels.strip().lower() == "all":
            channels = all_pins
        else:
            channels = list(dict.fromkeys(
                channel.strip().upper() for channel in args.channels.split(",")))
    elif args.vcd:
        channels = all_pins
    elif args.i2c:
        channels = []
    else:
        parser.error("specify --channels, --vcd or --i2c (or --info)")

    i2c = [channel.strip().upper() for channel in args.i2c.split(",")] if args.i2c else []
    if len(i2c) not in (0, 2):
        parser.error("--i2c needs exactly two channels, e.g. A8,B8")
    # Decode I2C even if the channels are not exported to the VCD.
    all_channels = list(dict.fromkeys(channels + i2c))
    changes = edges(capture["rate"], buffers, all_channels)
    for channel in channels:
        print(f"P{channel}: {len(changes[channel]) - 1} edges")
    if args.vcd:
        write_vcd(args.vcd, capture["rate"], channels, changes)
        print(f"Wrote {args.vcd}")
    if i2c:
        scl, sda = i2c
        for line in decode_i2c(changes[scl], changes[sda]):
            print(line)


if __name__ == "__main__":
    main()
