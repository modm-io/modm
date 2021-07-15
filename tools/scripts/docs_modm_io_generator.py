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
def rename_board(name):
    return name.replace("_", "-").replace(".", "-").replace(":", "-").upper()\
               .replace("BLUE-PILL", "Blue Pill") \
               .replace("BLACK-PILL", "Black Pill") \
               .replace("ARDUINO-UNO", "Arduino UNO") \
               .replace("ARDUINO-NANO", "Arduino NANO") \
               .replace("RASPBERRYPI", "Raspberry Pi")

sys.path.append(str(repopath("ext/modm-devices")))
from modm_devices.device_identifier import *

def get_targets():
    builder = lbuild.api.Builder()
    builder._load_repositories(repopath("repo.lb"))

    # Get me a condensed list of targets
    target_option = builder.parser.find_option(":target")
    minimal_targets = defaultdict(list)

    for target in target_option.values:
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

    # Generate one device per family
    family_list = list({d.get("family", "hosted"): d for d in target_list}.values())

    # We must include all :board module targets manually
    board_list = []
    for board in repopath("src/modm/board").glob("*/board.xml"):
        target = re.search(r"< *option +name=\"modm:target\" *>(.*?)</ *option *>", board.read_text())[1]
        board_list.append( (board.parent.name.replace("_", "-"), target) )

    return target_list, board_list, family_list


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
    parser.add_argument("--deduplicate", "-d", action='store_true', help="Deduplicate identical files with symlinks.")
    args = parser.parse_args()

    device_list, board_list, family_list = get_targets()
    if args.test:
        device_list = ["stm32f103c8t6", "stm32f417zgt6"] + family_list
        device_list = list(set(device_list))
        board_list = [("arduino-nano", "atmega328p-au"), ("arduino-uno", "atmega328p-au"), ("nucleo-g474re", "stm32g474ret6"),
                      ("blue-pill", "stm32f103c8t6"), ("feather-m0", "samd21g18a-uu")]
    elif args.test2:
        device_list = ["hosted-linux", "atmega328p-pu", "stm32f103zgt7", "stm32g474vet7"]
        board_list = []

    template_path = os.path.realpath(os.path.dirname(sys.argv[0]))
    cwd = Path().cwd()
    if args.local_temp:
        temp_dir = str(cwd)
    else:
        temp_dir = None
    with tempfile.TemporaryDirectory(dir=temp_dir) as tempdir:
        tempdir = Path(tempdir)
        modm_path = os.path.abspath(os.path.dirname(sys.argv[0]) + "/../..")
        print("Modm Path: {}".format(modm_path))
        print("Temporary directory: {}".format(str(tempdir)))
        output_dir = (tempdir / "output")
        (output_dir / "develop/api").mkdir(parents=True)
        os.chdir(tempdir)
        print("Starting to generate documentation...")
        template_overview(output_dir, device_list, board_list, template_path)
        print("... for {} devices, estimated memory footprint is {} MB".format(len(device_list), (len(device_list)*70)+2000))
        with multiprocessing.Pool(args.jobs) as pool:
            # We can only pass one argument to pool.map
            devices = ["{}|{}|{}||{}".format(modm_path, tempdir, dev, args.deduplicate) for dev in device_list]
            devices += ["{}|{}|{}|{}|{}".format(modm_path, tempdir, dev, brd, args.deduplicate) for (brd, dev) in board_list]
            results = pool.map(create_target, devices)
        # output_dir.rename(cwd / 'modm-api-docs')
        if args.compress:
            print("Zipping docs ...")
            # Zipping may take more than 10 minutes
            os.system("(cd {} && tar -czvf {} .)".format(str(output_dir), str(cwd / 'modm-api-docs.tar.gz')))
            # shutil.make_archive(str(cwd / 'modm-api-docs'), 'gztar', str(output_dir))
        else:
            final_output_dir = Path(args.output)
            if args.overwrite and final_output_dir.exists():
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
    modm_path, tempdir, device, board, deduplicate = argument.split("|")
    tempdir = Path(tempdir)
    output_dir = board if board else device
    try:
        print("Generating documentation for {} ...".format(output_dir))

        options = ["modm:target={0}".format(device)]
        if device.startswith("at"):
            options.append("modm:platform:core:f_cpu=16000000")
        builder = lbuild.api.Builder(options=options)
        builder.load([Path(modm_path) / "repo.lb", Path(modm_path) / "test/repo.lb"])
        modules = sorted(builder.parser.modules.keys())

        if board:
            chosen_board = "modm:board:{}".format(board)
        else:
            # Only allow the first board module to be built (they overwrite each others files)
            chosen_board = next((m for m in modules if ":board:" in m), None)
        modules = [m for m in modules if ":board" not in m or m == chosen_board]

        # Remove :tinyusb:host modules, they conflict with :tinyusb:device modules
        modules = [m for m in modules if ":tinyusb:host" not in m]

        # Remove :architecture modules. Only the :architecture modules for which actual implementations
        #  exist are include as dependencies of the :platform modules.
        modules = [m for m in modules if ":architecture" not in m]

        builder.build(output_dir, modules)

        print('Executing: (cd {}/modm/docs/ && doxypress doxypress.json)'.format(output_dir))
        os.system('(cd {}/modm/docs/ && doxypress doxypress.json > /dev/null 2>&1)'.format(output_dir))
        print("Finished generating documentation for device {}.".format(output_dir))

        srcdir = (tempdir / output_dir / "modm/docs/html")
        destdir = tempdir / 'output/develop/api' / output_dir

        if deduplicate == "True":
            print("Deduplicating files for {}...".format(device))
            symlinks = defaultdict(list)
            for file in (tempdir / 'output').rglob('*'):
                if file.is_dir() or file.is_symlink(): continue;
                key = file.relative_to(tempdir).parts[4:]
                if key:
                    symlinks[os.path.join(*key)].append(file)
            for file in srcdir.rglob('*'):
                if file.is_dir():
                    print(end="", flush=True)
                    continue
                key = str(file.relative_to(srcdir))
                if key in symlinks:
                    for kfile in symlinks[key]:
                        symlinks[hash(kfile.read_bytes())].append(kfile)
                    del symlinks[key]
                fhash = hash(file.read_bytes())
                if fhash in symlinks:
                    print(".", end="")
                    rpath = symlinks[fhash][0].relative_to(tempdir / 'output/develop/api')
                    lpath = os.path.relpath(srcdir, file.parent)
                    sympath = os.path.join("..", lpath, rpath)
                    # print("Linking {} -> {}".format(file.relative_to(srcdir), sympath))
                    file.unlink()
                    file.symlink_to(sympath)

        # Only move folder *after* deduplication to prevent race condition with file.unlink()
        srcdir.rename(destdir)
        print(end="", flush=True)
        return True
    except Exception as e:
        print("Error generating documentation for device {}: {}".format(output_dir, e))
        return False


def template_overview(output_dir, device_list, board_list, template_path):
    html = Environment(loader=FileSystemLoader(template_path)).get_template("docs_modm_io_index.html.in").render(
        devices=device_list,
        boards=[(b, rename_board(b), d) for (b,d) in board_list],
        date=datetime.datetime.now().strftime("%d.%m.%Y, %H:%M"),
        num_devices=len(device_list),
        num_boards=len(board_list))
    with open(str(output_dir) + "/index.html","w+") as f:
        f.write(html)
    with open(str(output_dir) + "/robots.txt","w+") as f:
        robots_txt = "User-agent: *\n"
        f.write(robots_txt)


if __name__ == "__main__":
    exit(main())
