#!/usr/bin/env python3
#
# Copyright (c) 2026, Kaelin Laundry
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

"""Test the ethernet_lwip_raw firmware using ping and UDP/TCP echo.

From the repository root:
    python examples/host/ethernet_echo.py <device-ip>
    python examples/host/ethernet_echo.py <device-ip> --stress
    python examples/host/ethernet_echo.py <device-ip> --throughput

Throughput is TCP echo goodput, i.e. framing and TCP retransmission overhead
would deduct from the 100 Mbit/s maximum bandwidth.
"""

import argparse
import concurrent.futures
import hashlib
import os
import socket
import struct
import subprocess
import time


def payload(size, seed=0):
    pattern = bytes((value + seed) % 251 for value in range(251))
    return (pattern * ((size + len(pattern) - 1) // len(pattern)))[:size]


def ping(address):
    if os.name == "nt":
        command = ["ping", "-n", "3", "-w", "2000", address]
    else:
        command = ["ping", "-c", "3", "-W", "2", address]
    return subprocess.run(command, stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL, timeout=5).returncode == 0


def udp_echo(address, port, size):
    expected = payload(size)
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(2)
        sock.sendto(expected, (address, port))
        received, peer = sock.recvfrom(65536)
    return received == expected and peer[0] == address


def tcp_echo(address, port, size, delay=0, read_delay=0, seed=0):
    expected = payload(size, seed)
    with socket.create_connection((address, port), timeout=5) as sock:
        sock.settimeout(30)
        sock.sendall(expected)
        sock.shutdown(socket.SHUT_WR)
        time.sleep(delay)

        received = bytearray()
        while chunk := sock.recv(65536):
            received.extend(chunk)
            if read_delay:
                time.sleep(read_delay)
    return received == expected


def reset_connection(address, port):
    sock = socket.create_connection((address, port), timeout=5)
    linger = "HH" if os.name == "nt" else "ii"
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                    struct.pack(linger, 1, 0))
    sock.sendall(payload(1024 * 1024))
    sock.close()
    time.sleep(0.5)
    return tcp_echo(address, port, 64 * 1024, seed=1)


def repeated_connections(address, port, count=50):
    return all(tcp_echo(address, port, 4 * 1024, seed=index)
               for index in range(count))


def tcp_stream(address, port, size, seed=0):
    expected = payload(size, seed)
    expected_digest = hashlib.sha256(expected).digest()

    with socket.create_connection((address, port), timeout=5) as sock:
        sock.settimeout(60)

        def send():
            sock.sendall(expected)
            sock.shutdown(socket.SHUT_WR)

        start = time.perf_counter()
        with concurrent.futures.ThreadPoolExecutor(max_workers=1) as pool:
            sender = pool.submit(send)
            received = 0
            digest = hashlib.sha256()
            while chunk := sock.recv(65536):
                digest.update(chunk)
                received += len(chunk)
            sender.result()
        elapsed = time.perf_counter() - start

    if received != size or digest.digest() != expected_digest:
        raise RuntimeError("TCP throughput data validation failed")
    return elapsed


def throughput(address, port):
    tcp_stream(address, port, 1024 * 1024, seed=17)
    size = 32 * 1024 * 1024
    elapsed = tcp_stream(address, port, size, seed=23)
    mbit_per_second = size * 8 / elapsed / 1_000_000
    mib_per_second = size / elapsed / (1024 * 1024)
    print(f"TCP throughput: {mbit_per_second:.1f} Mbit/s "
          f"({mib_per_second:.1f} MiB/s)")
    return True


def check(name, function):
    if not function():
        raise RuntimeError(f"{name} failed")
    print(f"{name}: ok")


def main():
    parser = argparse.ArgumentParser(description="Verify the lwIP UDP/TCP echo example")
    parser.add_argument("address")
    parser.add_argument("--port", type=int, default=5001)
    parser.add_argument("--stress", action="store_true")
    parser.add_argument("--throughput", action="store_true")
    args = parser.parse_args()

    try:
        check("Ping", lambda: ping(args.address))
    except (FileNotFoundError, subprocess.TimeoutExpired) as error:
        raise RuntimeError("ping command failed") from error

    for size in (1, 64, 512, 1472):
        check(f"UDP echo ({size} bytes)",
              lambda size=size: udp_echo(args.address, args.port, size))
    check("TCP echo (64 KiB)",
          lambda: tcp_echo(args.address, args.port, 64 * 1024))
    if args.throughput:
        check("TCP throughput", lambda: throughput(args.address, args.port))

    if args.stress:
        check("TCP delayed read (4 MiB)",
              lambda: tcp_echo(args.address, args.port, 4 * 1024 * 1024, delay=1))

        def concurrent_echo(index):
            return tcp_echo(args.address, args.port, 512 * 1024,
                            delay=0.25, read_delay=0.0005, seed=index)

        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
            check("TCP concurrent clients",
                  lambda: all(pool.map(concurrent_echo, range(4))))

        check("TCP reset recovery",
              lambda: reset_connection(args.address, args.port))
        check("TCP repeated reconnects",
              lambda: repeated_connections(args.address, args.port))


if __name__ == "__main__":
    main()
