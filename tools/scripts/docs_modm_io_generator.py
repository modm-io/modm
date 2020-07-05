#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
# Copyright (c) 2018-2019, Raphael Lehmann
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import sys
import shutil
import lbuild
import zipfile
import tempfile
import argparse
import datetime
import multiprocessing
import os, sys
from pathlib import Path
from jinja2 import Environment, FileSystemLoader
from collections import defaultdict

def repopath(path):
    return Path(__file__).absolute().parents[2] / path
def relpath(path):
    return os.path.relpath(path, str(repopath(".")))

sys.path.append(str(repopath("ext/modm-devices/tools/device")))
from modm_devices.device_identifier import *


def get_targets():
    builder = lbuild.api.Builder()
    builder._load_repositories(repopath("repo.lb"))

    # Get me a condensed list of targets
    target_option = builder.parser.find_option(":target")
    ignored = list(filter(lambda d: "#" not in d, repopath("test/all/ignored.txt").read_text().strip().splitlines()))
    raw_targets = sorted(d for d in target_option.values if not any(d.startswith(i) for i in ignored))
    minimal_targets = defaultdict(list)

    for target in raw_targets:
        target_option.value = target
        target = target_option.value._identifier
        short_id = target.copy()

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

        short_id.set("platform", target.platform) # invalidate caches
        minimal_targets[short_id.string].append(target)

    target_list = []

    # Force "hosted-linux" for hosted target family
    minimal_targets["hosted"] = list(filter(lambda d: "linux" in d.string, minimal_targets["hosted"]))

    # Sort the targets by name in their category
    # And choose the last one (assumed to be the "largest" devices)
    for key, values in minimal_targets.items():
        target_list.append(sorted(values, key=lambda d: d.string)[-1])

    return target_list


def main():
    parser = argparse.ArgumentParser()
    test_group = parser.add_mutually_exclusive_group()
    test_group.add_argument("--test", "-t", action='store_true', help="Test mode: generate only a few targets. List includes targets with multiple board modules.")
    test_group.add_argument("--test2", "-t2", action='store_true', help="Test mode: generate only a few targets. List has targets from the real target list.")
    parser.add_argument("--jobs", "-j", type=int, default=2, help="Number of parallel doxygen processes")
    parser.add_argument("--local-temp", "-l", action='store_true', help="Create temporary directory inside current working directory")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--compress", "-c", action='store_true', help="Compress output into gzip archive")
    group.add_argument("--output", "-o", type=str, help="Output directory (absolute path)")
    parser.add_argument("--overwrite", "-f", action='store_true', help="Overwrite existing data in output directory (Removes all files from output directory.)")
    args = parser.parse_args()

    device_list = []
    if args.test:
        # test list
        device_list = ["hosted-linux", "atmega328p-au", "stm32f103c8t6", "stm32g474cet6"]
    elif args.test2:
        device_list = ["hosted-linux", "atmega328p-pu", "stm32f103zgt7", "stm32g474vet7", "samd21g18a-uu"]
    else:
        device_list = get_targets()

    template_path = os.path.realpath(os.path.dirname(sys.argv[0]))
    cwd = Path().cwd()
    if args.local_temp:
        temp_dir = str(cwd)
    else:
        temp_dir = None
    with tempfile.TemporaryDirectory(dir=temp_dir) as tempdir:
        path_tempdir = Path(tempdir)
        modm_path = os.path.abspath(os.path.dirname(sys.argv[0]) + "/../..")
        print("Modm Path: {}".format(modm_path))
        print("Temporary directory: {}".format(str(tempdir)))
        output_dir = (path_tempdir / "output")
        (output_dir / "develop/api").mkdir(parents=True)
        os.chdir(tempdir)
        print("Starting to generate documentation...")
        template_overview(output_dir, device_list, template_path)
        print("... for {} devices, estimated memory footprint is {} MB".format(len(device_list), (len(device_list)*70)+2000))
        with multiprocessing.Pool(args.jobs) as pool:
            # We can only pass one argument to pool.map
            devices = ["{}|{}|{}".format(modm_path, path_tempdir, d) for d in device_list]
            results = pool.map(create_target, devices)
        # output_dir.rename(cwd / 'modm-api-docs')
        if args.compress:
            print("Zipping docs ...")
            # Zipping may take more than 10 minutes
            os.system("tar -czvf {} {}".format(str(cwd / 'modm-api-docs.tar.gz'), str(output_dir)))
            # shutil.make_archive(str(cwd / 'modm-api-docs'), 'gztar', str(output_dir))
        else:
            final_output_dir = Path(args.output)
            if args.overwrite:
                for i in final_output_dir.iterdir():
                    print('Removing {}'.format(i))
                    if i.is_dir():
                        shutil.rmtree(i)
                    else:
                        os.remove(i)
            print('Moving {} -> {}'.format(output_dir, final_output_dir))
            #shutil.move(str(output_dir) + '/', str(final_output_dir))
            output_dir.rename(final_output_dir)
        return results.count(False)


def create_target(argument):
    modm_path, tempdir, device = argument.split("|")
    try:
        print("Generating documentation for {} ...".format(device))

        options = ["modm:target={0}".format(device)]
        if device.startswith("at"):
            options.append("modm:platform:core:f_cpu=16000000")
        builder = lbuild.api.Builder(options=options)
        builder.load([Path(modm_path) / "repo.lb", Path(modm_path) / "test/repo.lb"])
        modules = sorted(builder.parser.modules.keys())

        # Only allow the first board module to be built (they overwrite each others files)
        first_board = next((m for m in modules if ":board:" in m), None)
        modules = [m for m in modules if ":board" not in m or m == first_board]

        # Remove :architecture modules. Only the :architecture modules for which actual implementations
        #  exist are include as dependencies of the :platform modules.
        modules = [m for m in modules if ":architecture" not in m]

        builder.build(device, modules)

        print('Executing: (cd {}/modm/docs/ && doxypress doxypress.json)'.format(device))
        os.system('(cd {}/modm/docs/ && doxypress doxypress.json > /dev/null 2>&1)'.format(device))
        (Path(tempdir) / device / "modm/docs/html").rename(Path(tempdir) / 'output/develop/api' / device)
        print("Finished generating documentation for device {}.".format(device))
        return True
    except Exception as e:
        print("Error generating documentation for device {}: {}".format(device, e))
        return False


def template_overview(output_dir, device_list, template_path):
    html = Environment(loader=FileSystemLoader(template_path)).get_template("docs_modm_io_index.html.in").render(
        devices=device_list,
        date=datetime.datetime.now().strftime("%d.%m.%Y, %H:%M"),
        num_devices=len(device_list))
    with open(str(output_dir) + "/index.html","w+") as f:
        f.write(html)
    with open(str(output_dir) + "/robots.txt","w+") as f:
        robots_txt = "User-agent: *\n"
        f.write(robots_txt)


if __name__ == "__main__":
    exit(main())
