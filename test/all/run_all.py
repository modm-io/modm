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
import string
import shutil
import tempfile
import logging
import subprocess
import multiprocessing
from pathlib import Path

LOGGER = logging.getLogger("run")
LBUILD_COMMAND = ["lbuild"]
cpus = 4 if os.getenv("CIRCLECI") else os.cpu_count()

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
    LOGGER.debug(" ".join(cmdline))
    output, errors, retval = run_command(cmdline)
    if retval != 0:
        raise CommandException(output, errors)
    return output

def run_scons(tempdir):
    cmdline = ["scons", "-C", tempdir, "--random", "--cache-show"]
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
    def __init__(self, device, cache_dir=None):
        self.device = device
        self.output = ""
        self.errors = ""
        self.result = TestRunResult.FAIL_BUILD
        self.time = None
        self.cache_dir = cache_dir

    def run(self):
        start_time = time.time()

        with tempfile.TemporaryDirectory() as tempdir:
            if self.device.startswith("at"):
                lbuild_command = ["-c", "avr.xml"]
                shutil.copyfile("avr.cpp", os.path.join(tempdir, "main.cpp"))
            elif self.device.startswith("stm32"):
                lbuild_command = ["-c", "stm32.xml", "-D:::main_stack_size=512"]
                shutil.copyfile("stm32.cpp", os.path.join(tempdir, "main.cpp"))

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

        end_time = time.time()
        self.time = end_time - start_time

        sys.stdout.write("{:5}{:20} {:=3.1f}s\n".format(self.format_result(), self.device, self.time))
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

def main():
    if len(sys.argv) == 2 and sys.argv[1] == "failed":
        devices = Path("failed.txt").read_text().strip().split("\n")
    else:
        try:
            stdout = run_lbuild(["-c", "avr.xml", "discover", "--values", "--name :target"])
            devices = stdout.strip().splitlines()
            # filter for only the devices specified
            for arg in sys.argv[1:]:
                devices = [d for d in devices if d.startswith(arg)]
            # print(devices)
        except CommandException as error:
            print(error)
            exit(1)

    ignored_devices = Path("ignored.txt").read_text().strip().split("\n")
    ignored_devices = [d for d in ignored_devices if "#" not in d]
    # filter out non-supported devices
    devices = [d for d in devices if d not in ignored_devices]

    os.makedirs("log", exist_ok=True)

    if "no-cache" in sys.argv:
        cache_dir = None
    else:
        cache_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../build/cache"))
        Path(cache_dir).mkdir(exist_ok=True, parents=True)
        (Path(cache_dir) / "config").write_text('{"prefix_len": 2}')

    try:
        #devices = [device for device in devices if device.startswith("atx")]
        with multiprocessing.Pool(cpus) as pool:
            test_runs = pool.map(build_device, [TestRun(x, cache_dir) for x in devices])

        succeded = []
        failed = []
        print()
        for run in sorted(test_runs, key=lambda x: x.device):
            if run.result == TestRunResult.SUCCESS:
                succeded.append(run.device)
            else:
                failed.append(run.device)

        print()
        print("SUCCESS", len(succeded))
        print("FAIL   ", len(failed))
        print("------------------")
        print("Total: ", len(succeded) + len(failed))
        print()

        Path("failed.txt").write_text("\n".join(failed))
        if len(failed) > 0:
            sys.exit(2)
    except KeyboardInterrupt:
        print("Interrupted!")
        sys.exit(1)

if __name__ == "__main__":
    main()
