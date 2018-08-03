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

LOGGER = logging.getLogger("run")
LBUILD_COMMAND = ["lbuild"]


class CommandException(Exception):
    pass


def run_command(cmdline):
    try:
        p = subprocess.Popen(cmdline, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output, _ = p.communicate()

        if p.returncode != 0:
            raise CommandException(output.decode("utf-8"))
        return output.decode("utf-8")
    except KeyboardInterrupt:
        raise multiprocessing.ProcessError()

def run_lbuild(command):
    cmdline = LBUILD_COMMAND + command
    LOGGER.debug(" ".join(cmdline))
    return run_command(cmdline)

def build_code(tempdir):
    cmdline = ["scons", "-C", tempdir]
    return run_command(cmdline)


class TestRunResult(enum.Enum):
    FAIL_BUILD = "-  "
    FAIL_COMPILE = " x "
    SUCCESS = "  o"


class TestRun:
    def __init__(self, device):
        self.device = device
        self.output = ""
        self.result = TestRunResult.FAIL_BUILD
        self.time = None

    def run(self):
        start_time = time.time()
        if self.device.startswith("at"):
            lbuild_command = ["-c", "avr.xml"]
        elif self.device.startswith("stm32"):
            lbuild_command = ["-c", "stm32.xml"]
        lbuild_command.append("-D:target={}".format(self.device))

        modules = run_lbuild(lbuild_command + ["discover-modules"]).splitlines()
        modules = [m for m in modules if m.startswith("modm:platform")]
        # remove :platform:fault*
        modules = [m for m in modules if not m.startswith("modm:platform:fault")]

        with tempfile.TemporaryDirectory() as tempdir:
            lbuild_command.extend(["-p", str(tempdir), "build", "--no-log"] + ["-m={}".format(m) for m in modules])
            try:
                self.result = TestRunResult.FAIL_BUILD
                self.output += run_lbuild(lbuild_command)
                self.result = TestRunResult.FAIL_COMPILE
                self.output += build_code(tempdir)
                self.result = TestRunResult.SUCCESS
            except CommandException as error:
                self.output += str(error)

        end_time = time.time()
        self.time = end_time - start_time

        sys.stdout.write("{:5}{:20} {:=3.1f}s\n".format(self.format_result(), self.device, self.time))
        sys.stdout.flush()

        with open(os.path.join("log", "{}.log".format(self.device)), "w") as logfile:
            logfile.write("{}\n"
                          "Device: {}\n"
                          "{}\n"
                          "{}".format(" ".join(LBUILD_COMMAND + lbuild_command), self.device, self.format_result(), self.output))

    def format_result(self):
        return self.result.value

    def __str__(self):
        return "{} {}".format(self.device, self.format_result())


def build_device(run):
    run.run()
    return run

def main():
    if len(sys.argv) == 2 and sys.argv[1] == "failed":
        with open("failed.txt", "r") as failed_file:
            devices = failed_file.read().strip().split("\n")
    else:
        try:
            stdout = run_lbuild(["-c", "avr.xml", "discover-option-values", "--option=:target"])
            devices = stdout.strip().split("\n")
            # filter for only the devices specified
            for arg in sys.argv[1:]:
                devices = [d for d in devices if d.startswith(arg)]
            # print(devices)
        except CommandException as error:
            print(error)
            exit(1)

    with open("ignored.txt", "r") as ignored_file:
        ignored_devices = ignored_file.read().strip().split("\n")
        ignored_devices = [d for d in ignored_devices if "#" not in d]
    # filter out non-supported devices
    devices = [d for d in devices if d not in ignored_devices]

    os.makedirs("log", exist_ok=True)

    try:
        #devices = [device for device in devices if device.startswith("atx")]
        with multiprocessing.Pool() as pool:
            test_runs = pool.map(build_device, [TestRun(x) for x in devices])

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

        with open("failed.txt", "w") as failed_file:
            failed_file.write("\n".join(failed))

        if len(failed) > 0:
            sys.exit(2)
    except KeyboardInterrupt:
        print("Interrupted!")
        sys.exit(1)

if __name__ == "__main__":
    main()
