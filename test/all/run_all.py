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
import math
import shutil
import lbuild
import random
import tempfile
import argparse
import subprocess
import multiprocessing
from pathlib import Path
from collections import defaultdict

def repopath(path):
    return Path(__file__).absolute().parents[2] / path

build_dir = repopath("build")

# sys.path.append(str(repopath("ext/modm-devices")))
# from modm_devices.device_identifier import *

def get_targets(prefix=None, short=False):
    builder = lbuild.api.Builder()
    builder._load_repositories(repopath("repo.lb"))

    # Get me a condensed list of targets
    target_option = builder.parser.find_option(":target")
    minimal_targets = defaultdict(list)

    for target in target_option.values:
        if not any(target.startswith(p) for p in prefix):
            continue
        target_option.value = target
        target = target_option.value._identifier
        short_id = target.copy()

        if short:
            if target.platform == "avr":
                short_id.naming_schema = short_id.naming_schema \
                    .replace("{type}-{speed}{package}", "") \
                    .replace("-{speed}{package}", "")

            elif target.platform == "stm32":
                short_id.naming_schema = "{platform}{family}{name}"

            elif target.platform == "hosted":
                short_id.naming_schema = "{platform}"

            elif target.platform == "sam":
                short_id.naming_schema = "{platform}{family}{series}"
        else:
            # Remove temperature key for stm32
            if target.platform == "stm32":
                short_id.naming_schema = \
                    short_id.naming_schema.replace("{temperature}", "")

        short_id.set("platform", target.platform) # invalidate caches
        minimal_targets[short_id.string].append(target.string)

    target_list = []

    # Sort the targets by name in their category
    # And choose the last one (assumed to be the "largest" devices)
    for key, values in minimal_targets.items():
        target_list.append(sorted(values)[-1])
    return target_list


class CommandException(Exception):
    def __init__(self, output, errors):
        super().__init__()
        self.output = output
        self.errors = errors

def run_command(cmdline):
    try:
        cmdline = " ".join(cmdline)
        print(cmdline)
        p = subprocess.run(cmdline, shell=True, stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE, universal_newlines=True)
        return (p.stdout, p.stderr, p.returncode)
    except KeyboardInterrupt:
        raise multiprocessing.ProcessError()

def run_lbuild(command):
    cmdline = ["lbuild"] + command
    output, errors, retval = run_command(cmdline)
    if retval != 0:
        raise CommandException(output, errors)
    return output

def build_code(tempdir):
    cmdline = ["scons", "-C", tempdir, "--random", "--cache-show", "-k"]
    output, errors, retval = run_command(cmdline)
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

        prefix = build_dir / "test_all_"
        with tempfile.TemporaryDirectory(prefix=str(prefix)) as tempdir:
            lbuild_command = ["-c"]
            if self.device.startswith("at"):
                lbuild_command += ["avr.xml"]
                shutil.copyfile("avr.cpp", os.path.join(tempdir, "main.cpp"))
            else:
                lbuild_command += ["cortex-m.xml", "-D:::main_stack_size=512"]
                shutil.copyfile("cortex-m.cpp", os.path.join(tempdir, "main.cpp"))

            if self.cache_dir:
                lbuild_command += ["-D:::cache_dir={}".format(self.cache_dir)]

            lbuild_command += ["-D:target={}".format(self.device),
                               "-D:build:build.path={}/build".format(tempdir),
                               "-p", str(tempdir),
                               "build",
                               "--no-log"]

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

        sys.stdout.write("{:5}{:20} {:=6.1f}s\n".format(
                                self.format_result(), self.device, self.time))
        if self.errors:
            sys.stdout.write(self.errors)
        sys.stdout.flush()

        with open(os.path.join("log", "{}.log".format(self.device)), "w") as logfile:
            logfile.write("{}\n"
                          "Device: {}\n"
                          "{}\n"
                          "{}".format(" ".join(lbuild_command), self.device,
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
            nargs="*",
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
            default=os.cpu_count(),
            type=int,
            help="Number of parallel jobs")
    parser.add_argument(
            "--no-cache",
            dest="no_cache",
            default=False,
            action="store_true",
            help="Disable using CacheDir.")
    parser.add_argument(
            "--part",
            dest="part",
            default=0,
            type=int,
            help="Execute this part of the splitting.")
    parser.add_argument(
            "--split",
            dest="split",
            default=1,
            type=int,
            help="Split the devices into this many parts.")
    parser.add_argument(
            "--quick",
            dest="quick",
            default=False,
            action="store_true",
            help="Reduce device list for quick test.")
    parser.add_argument(
            "--quick-remaining",
            dest="quick_remaining",
            default=False,
            action="store_true",
            help="Run remaining tests.")
    args = parser.parse_args()

    if "failed" in args.prefix:
        devices = Path("failed.txt").read_text().strip().split("\n")
    else:
        devices = get_targets(args.prefix, args.quick)
        if args.quick_remaining:
            # Remove those devices we've already run in the quick test
            quick = get_targets(args.prefix, True)
            devices = list(set(devices) - set(quick))

    if not len(devices):
        print("No devices found to build! Check your input and 'ignored.txt'.")
        exit(1)

    # Choose the device split
    devices.sort()
    if args.split > 1:
        chunk_size = math.ceil(len(devices) / args.split)
        devices = devices[chunk_size*args.part:min(chunk_size*(args.part+1), len(devices))]

    # Create cache dir config if necessary
    if args.no_cache:
        cache_dir = None
    else:
        cache_dir = build_dir / "cache"
        cache_dir.mkdir(exist_ok=True, parents=True)
        (cache_dir / "config").write_text('{"prefix_len": 2}')

    # Translate cache limit to bytes
    cache_limit = args.cache_limit * 1e9

    # Create the logging folder
    os.makedirs("log", exist_ok=True)

    print("Using {} parallel jobs for {} devices".format(args.jobs, len(devices)))
    try:
        with multiprocessing.Pool(args.jobs) as pool:
            test_runs = pool.map(build_device,
                            [TestRun(x, cache_dir, cache_limit) for x in devices])

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

