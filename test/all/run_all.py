#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, Fabian Greif
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import sys
import time
import enum
import shutil
import random
import tempfile
import argparse
import subprocess
import multiprocessing
from pathlib import Path

LBUILD_COMMAND = ["lbuild"]
jobs = 4 if os.getenv("CIRCLECI") else os.cpu_count()
if os.getenv("GITHUB_ACTIONS") is not None: jobs = 8;
build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../build"))

class CommandException(Exception):
    def __init__(self, output, errors):
        super().__init__()
        self.output = output
        self.errors = errors


def run_command(cmdline):
    try:
        cmdline = " ".join(cmdline)
        p = subprocess.run(cmdline, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
        return (p.stdout, p.stderr, p.returncode)
    except KeyboardInterrupt:
        raise multiprocessing.ProcessError()

def run_lbuild(command):
    cmdline = LBUILD_COMMAND + command
    output, errors, retval = run_command(cmdline)
    if retval != 0:
        raise CommandException(output, errors)
    return output

def run_scons(tempdir):
    cmdline = ["scons", "-C", tempdir, "--random", "--cache-show", "-k"]
    return run_command(cmdline)

def build_code(tempdir):
    output, errors, retval = run_scons(tempdir)
    if retval != 0:
        raise CommandException(output, errors)
    return output


class TestRunResult(enum.Enum):
    FAIL_BUILD = "-  "
    FAIL_COMPILE = " x "
    SUCCESS = "  o"


class TestRun:
    def __init__(self, device, cache_dir, cache_limit):
        self.device = device
        self.output = ""
        self.errors = ""
        self.result = TestRunResult.FAIL_BUILD
        self.time = None
        self.cache_dir = cache_dir
        self.cache_limit = cache_limit

    def run(self):
        start_time = time.time()

        prefix = os.path.join(build_dir, "test_all_")
        with tempfile.TemporaryDirectory(prefix=prefix) as tempdir:
            if self.device.startswith("at"):
                lbuild_command = ["-c", "avr.xml"]
                shutil.copyfile("avr.cpp", os.path.join(tempdir, "main.cpp"))
            else:
                lbuild_command = ["-c", "cortex-m.xml", "-D:::main_stack_size=512"]
                shutil.copyfile("cortex-m.cpp", os.path.join(tempdir, "main.cpp"))

            if self.cache_dir:
                lbuild_command.append("-D:::cache_dir={}".format(self.cache_dir))

            lbuild_command.extend(["-D:target={}".format(self.device),
                                   "-D:build:build.path={}/build".format(tempdir),
                                   "-p", str(tempdir),
                                   "build",
                                   "--no-log"])
            try:
                self.result = TestRunResult.FAIL_BUILD
                self.output += run_lbuild(lbuild_command)
                self.result = TestRunResult.FAIL_COMPILE
                self.output += build_code(tempdir)
                self.result = TestRunResult.SUCCESS
            except CommandException as error:
                self.output += error.output
                self.errors = error.errors

        # Try and keep the cache_dir under a certain size
        if self.cache_dir and self.cache_limit:
            def _try(op, retval=None):
                try: return op();
                except: return retval;
            files = [f for f in Path(self.cache_dir).glob('*/*')
                     if _try(lambda: f.is_file(), False)]
            size = sum(_try(lambda: f.stat().st_size, 0) for f in files)
            if size > self.cache_limit:
                # assuming evenly distributed files
                random.shuffle(files)
                files = files[:int(((size - self.cache_limit) * len(files))/size)]
                for file in files:
                    _try(lambda: file.unlink())

        end_time = time.time()
        self.time = end_time - start_time

        sys.stdout.write("{:5}{:20} {:=6.1f}s\n".format(self.format_result(), self.device, self.time))
        if self.errors:
            sys.stdout.write(self.errors)
        sys.stdout.flush()

        with open(os.path.join("log", "{}.log".format(self.device)), "w") as logfile:
            logfile.write("{}\n"
                          "Device: {}\n"
                          "{}\n"
                          "{}".format(" ".join(LBUILD_COMMAND + lbuild_command), self.device,
                                      self.format_result(), self.output + self.errors))

    def format_result(self):
        return self.result.value

    def __str__(self):
        return "{} {}".format(self.device, self.format_result())


def build_device(run):
    run.run()
    return run


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run platform tests')
    parser.add_argument(
            "prefix",
            action="append",
            help="Device prefix to filter for")
    parser.add_argument(
            "--cache-limit",
            dest="cache_limit",
            default=0,
            type=int,
            help="Scons CacheDir size limit in GB")
    parser.add_argument(
            "--jobs",
            dest="jobs",
            default=jobs,
            type=int,
            help="Number of parallel jobs")
    parser.add_argument(
            "--no-cache",
            dest="no_cache",
            default=False,
            action="store_true",
            help="Disable using CacheDir.")
    args = parser.parse_args()

    if "failed" in args.prefix:
        raw_devices = Path("failed.txt").read_text().strip().split("\n")
    else:
        try:
            stdout = run_lbuild(["-c", "avr.xml", "discover", "--values", "--name :target"])
            raw_devices = stdout.strip().splitlines()
            # filter for only the devices specified
            for prefix in args.prefix:
                raw_devices = (d for d in raw_devices if d.startswith(prefix))
            # print(devices)
        except CommandException as error:
            print(error)
            exit(1)

    ignored_devices = Path("ignored.txt").read_text().strip().split("\n")
    ignored_devices = [d for d in ignored_devices if "#" not in d]
    # filter out non-supported devices
    raw_devices = filter(lambda d: not any(d.startswith(i) for i in ignored_devices), raw_devices)

    # Filter out the temperature key for STM32, which has no impact on HAL generation
    short_devices = set()
    devices = []
    for d in raw_devices:
        if d.startswith("stm32"):
            sd = d[:12] + d[13:]
            if sd not in short_devices:
                devices.append(d)
                short_devices.add(sd)
        else:
            devices.append(d)

    if not len(devices):
        print("No devices found to build! Check your input and 'ignored.txt'.")
        exit(1)

    os.makedirs("log", exist_ok=True)

    if args.no_cache:
        cache_dir = None
    else:
        cache_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../build/cache"))
        Path(cache_dir).mkdir(exist_ok=True, parents=True)
        (Path(cache_dir) / "config").write_text('{"prefix_len": 2}')

    # Translate cache limit to bytes
    cache_limit = args.cache_limit * 1e9

    print("Using {}x parallel jobs".format(args.jobs))
    try:
        with multiprocessing.Pool(args.jobs) as pool:
            test_runs = pool.map(build_device, [TestRun(x, cache_dir, cache_limit) for x in devices])

        succeeded = []
        failed = []
        print()
        for run in sorted(test_runs, key=lambda x: x.device):
            if run.result == TestRunResult.SUCCESS:
                succeeded.append(run.device)
            else:
                failed.append(run.device)

        print()
        print("SUCCESS {:=4d}".format(len(succeeded)))
        print("FAIL    {:=4d}".format(len(failed)))
        print("------------------")
        print("Total   {:=4d}".format(len(succeeded) + len(failed)))
        print()

        Path("failed.txt").write_text("\n".join(failed))
        if len(failed) > 0:
            sys.exit(2)
    except KeyboardInterrupt:
        print("Interrupted!")
        sys.exit(1)

