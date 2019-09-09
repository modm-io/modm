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
    option = builder.parser.find_option(":target")
    ignored = list(filter(lambda d: "#" not in d, repopath("test/all/ignored.txt").read_text().strip().splitlines()))
    raw_targets = sorted(d for d in option.values if not any(d.startswith(i) for i in ignored))
    minimal_targets = defaultdict(list)

    for target in raw_targets:
        option.value = target
        target = option.value._identifier
        short_id = target.copy()

        if target.platform == "avr":
            short_id.naming_schema = short_id.naming_schema \
                .replace("{type}-{speed}{package}", "") \
                .replace("-{speed}{package}", "")

        elif target.platform == "stm32":
            short_id.naming_schema = "{platform}{family}{name}"

        elif target.platform == "hosted":
            short_id.naming_schema = "{platform}"

        short_id.set("platform", target.platform) # invalidate caches
        minimal_targets[short_id.string].append(target)

    target_list = []

    # Force "hosted-linux" for hosted target family
    minimal_targets["hosted"] = list(filter(lambda d: "linux" in d.string, minimal_targets["hosted"]))

    for key, values in minimal_targets.items():
        target_list.append(sorted(values, key=lambda d: d.string)[-1])

    return target_list


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--test", "-t", action='store_true', help="Test mode: generate only a few targets")
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
        device_list = ["hosted-linux", "at90can128-16ai", "stm32g474cet6"]
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
            shutil.make_archive(str(cwd / 'modm-api-docs'), 'gztar', str(output_dir))
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
        tempdir = Path(tempdir)
        print("Generating documentation for {} ...".format(device))
        lbuild_options = '-r {1}/repo.lb -D "modm:target={0}" -p {0}/'.format(device, modm_path)
        options = ''
        if device.startswith("stm32"):
            options += ' -m"modm:architecture:**" -m"modm:build:**" -m"modm:cmsis:**" -m"modm:communication:**" -m"modm:container:**" -m"modm:debug:**" -m"modm:docs:**" -m"modm:driver:**" -m"modm:fatfs:**" -m"modm:freertos:**" -m"modm:io:**" -m"modm:math:**" -m"modm:platform:**" -m"modm:processing:**" -m"modm:ros:**" -m"modm:tlsf:**" -m"modm:ui:**" -m"modm:unittest:**" -m"modm:utils"'
        elif device.startswith("at"):
            lbuild_options += ' -D"modm:platform:clock:f_cpu=16000000"'
            options += ' -m"modm:**"'
        else:
            options += ' -m"modm:**"'
        lbuild_command = 'lbuild {} build {}'.format(lbuild_options, options)
        print('Executing: ' + lbuild_command)
        os.system(lbuild_command)
        print('Executing: (cd {}/modm/docs/ && doxypress doxypress.json)'.format(device))
        os.system('(cd {}/modm/docs/ && doxypress doxypress.json > /dev/null 2>&1)'.format(device))
        (tempdir / device / "modm/docs/html").rename(tempdir / 'output/develop/api' / device)
        print("Finished generating documentation for device {}.".format(device))
        return True
    except:
        print("Error generating documentation for device {}.".format(device))
        return False


def template_overview(output_dir, device_list, template_path):
    html = Environment(loader=FileSystemLoader(template_path)).get_template("docs_modm_io_index.html.in").render(
        devices=device_list,
        date=datetime.datetime.now().strftime("%d.%m.%Y, %H:%M"),
        num_devices=len(device_list))
    with open(str(output_dir) + "/index.html","w+") as f:
        f.write(html)
    with open(str(output_dir) + "/robots.txt","w+") as f:
        robots_txt = "User-agent: *\nDisallow: /\n"""
        f.write(robots_txt)


if __name__ == "__main__":
    exit(main())
