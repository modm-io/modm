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
import json
import gzip
import shutil
import hashlib
import tempfile
import argparse
import datetime
import platform
import subprocess
from multiprocessing.pool import ThreadPool
from threading import Lock
from pathlib import Path
from jinja2 import Environment, FileSystemLoader
from collections import defaultdict

is_running_on_macos = "macOS" in platform.platform()
def repopath(path):
    return Path(__file__).absolute().parents[2] / path
def relpath(path):
    return os.path.relpath(path, str(repopath(".")))
def rename_board(name):
    return name.replace("_", "-").replace(".", "-").replace(":", "-").upper()\
               .replace("BLUE-PILL", "Blue Pill") \
               .replace("BLACK-PILL", "Black Pill") \
               .replace("ARDUINO-UNO", "Arduino UNO") \
               .replace("ARDUINO-NANO", "Arduino NANO")

sys.path.append(str(repopath("ext/modm-devices")))
from modm_devices.device_identifier import *

# sys.path.insert(0, str(repopath("../lbuild")))
import lbuild

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

        # invalidate id cache due to lack of proper API
        short_id.set("platform", target.platform)
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
    test_group.add_argument("--test", "-t", action="store_true", help="Test mode: generate only a few targets. List includes targets with multiple board modules.")
    test_group.add_argument("--test2", "-t2", action="store_true", help="Test mode: generate only a few targets. List has targets from the real target list.")
    parser.add_argument("--jobs", "-j", type=int, default=2, help="Number of parallel doxygen processes")
    parser.add_argument("--local-temp", "-l", action="store_true", help="Create temporary directory inside current working directory")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--compress", "-c", action="store_true", help="Compress output into gzip archive")
    group.add_argument("--output", "-o", type=str, help="Output directory")
    parser.add_argument("--overwrite", "-f", action="store_true", help="Overwrite existing data in output directory (Removes all files from output directory.)")
    parser.add_argument("--deduplicate", "-d", action="store_true", help="Deduplicate identical files with symlinks.")
    parser.add_argument("--target-job", help="Create a single target from job string.")
    args = parser.parse_args()

    # Called by the thread pool below as a workaround for buggy multiprocessing
    if args.target_job:
        exit(0 if create_target(args.target_job) else -1)

    device_list, board_list, family_list = get_targets()
    if args.test:
        device_list = ["stm32f103c8t6", "stm32f417zgt6"] + family_list
        board_list = [("arduino-nano", "atmega328p-au"), ("arduino-uno", "atmega328p-au"), ("nucleo-g474re", "stm32g474ret6"),
                      ("blue-pill", "stm32f103c8t6"), ("feather-m0", "samd21g18a-uu"), ("disco-f469ni", "stm32f469nih6")]
    elif args.test2:
        device_list = ["hosted-linux", "atmega328p-pu", "stm32f103zgt7"]
        board_list = [("nucleo-g474re", "stm32g474ret6")]
    device_list = list(set(device_list))

    final_output_dir = Path(args.output).absolute() if args.output else "."
    template_path = os.path.realpath(os.path.dirname(sys.argv[0]))
    cwd = Path().cwd()
    filepath = os.path.abspath(__file__)
    if args.local_temp:
        temp_dir = str(cwd)
    else:
        temp_dir = None
    with tempfile.TemporaryDirectory(dir=temp_dir) as tempdir:
        tempdir = Path(tempdir)
        modm_path = os.path.abspath(os.path.dirname(sys.argv[0]) + "/../..")
        print(f"Modm Path: {modm_path}")
        print(f"Temporary directory: {tempdir}")
        output_dir = (tempdir / "output")
        (output_dir / "develop/api").mkdir(parents=True)
        os.chdir(tempdir)
        print("Starting to generate documentation...")
        template_overview(output_dir, device_list, board_list, template_path)
        print(f"... for {len(device_list) + len(board_list)} devices, estimated memory footprint is {len(device_list)*70+2000} MB")
        # We can only pass one argument to pool.map
        jobs = [(str(dev), f'python3 {filepath} --target-job "{modm_path}|{tempdir}|{dev}||{args.deduplicate}|{args.compress}"') for dev in device_list]
        jobs += [(brd, f'python3 {filepath} --target-job "{modm_path}|{tempdir}|{dev}|{brd}|{args.deduplicate}|{args.compress}"') for (brd, dev) in board_list]
        jobs = sorted(set(jobs))
        # The targets are generated in parallel, but deduplicated one after the other,
        # otherwise targets finishing at the same time cannot see each others files
        hashdb = {}
        hashdb_lock = Lock()
        def run_job(job):
            output_dir, command = job
            if subprocess.call(command, shell=True) != 0:
                return 1
            with hashdb_lock:
                move_target(tempdir, output_dir, hashdb)
            return 0
        with ThreadPool(args.jobs) as pool:
            results = pool.map(run_job, jobs)
        if args.compress:
            print("Zipping docs ...")
            # Zipping is *much* faster via command line than via python!
            tar = "gtar" if is_running_on_macos else "tar"
            zip_cmd = f"(cd {str(output_dir)} && {tar} --checkpoint=.100 -czf {str(cwd)}/modm-api-docs.tar.gz .)"
            subprocess.call(zip_cmd, shell=True)
        else:
            if args.overwrite and final_output_dir.exists():
                for i in final_output_dir.iterdir():
                    print(f"Removing {i}")
                    if i.is_dir():
                        shutil.rmtree(i)
                    else:
                        os.remove(i)
            print(f"Moving {output_dir} -> {final_output_dir}")
            output_dir.rename(final_output_dir)
        return len(results) - results.count(0)


def create_target(argument):
    modm_path, tempdir, device, board, deduplicate, compress = argument.split("|")
    tempdir = Path(tempdir)
    output_dir = board if board else device
    try:
        print(f"Generating documentation for {output_dir}...", flush=True)

        options = [f"modm:target={device}"]
        if device.startswith("at"):
            options.append("modm:platform:core:f_cpu=16000000")
        builder = lbuild.api.Builder(options=options)
        builder.load([Path(modm_path) / "repo.lb", Path(modm_path) / "test/repo.lb"])
        modules = sorted(builder.parser.modules.keys())

        if board:
            chosen_board = f"modm:board:{board}"
        else:
            # Only allow the first board module to be built (they overwrite each others files)
            chosen_board = next((m for m in modules if ":board:" in m), None)
        modules = [m for m in modules if ":board" not in m or m == chosen_board]

        # Remove :architecture modules. Only the :architecture modules for which actual implementations
        #  exist are include as dependencies of the :platform modules.
        modules = [m for m in modules if ":architecture" not in m]

        builder.build(output_dir, modules)

        print(f"Executing: (cd {output_dir}/modm/docs/ && doxypress doxypress.json)", flush=True)
        retval = subprocess.call(f"(cd {output_dir}/modm/docs/ && doxypress doxypress.json > /dev/null 2>&1)", shell=True)
        # retval = subprocess.call(f"(cd {output_dir}/modm/docs/ && doxygen doxyfile.cfg > /dev/null 2>&1)", shell=True)
        if retval != 0:
            print(f"Error {retval} generating documentation for device {output_dir}.", flush=True)
            return False
        print(f"Finished generating documentation for device {output_dir}.", flush=True)

        srcdir = (tempdir / output_dir / "modm/docs/html")
        destdir = tempdir / "output/develop/api" / output_dir

        if deduplicate == "True":
            print(f"Hashing files for {device}...", flush=True)
            # Only compress and hash the files here, the symlinks are created by move_target()
            hashes = []
            for file in sorted(srcdir.rglob("*")):
                if file.is_dir():
                    continue
                file_bytes = file.read_bytes()
                if compress == "True":
                    cfile = file.with_suffix(file.suffix + ".gz")
                    file_bytes = gzip.compress(file_bytes, mtime=0)
                    cfile.write_bytes(file_bytes)
                    file.unlink()
                    file = cfile
                hashes.append(f"{hashlib.md5(file_bytes).hexdigest()} {file.relative_to(srcdir)}")
            (tempdir / f"{output_dir}.hash").write_text("\n".join(hashes))
        return True
    except Exception as e:
        print(f"Error generating documentation for device {output_dir}: {e}", flush=True)
        return False


def move_target(tempdir, output_dir, hashdb):
    srcdir = tempdir / output_dir / "modm/docs/html"
    destdir = tempdir / "output/develop/api" / output_dir
    hashfile = tempdir / f"{output_dir}.hash"
    if hashfile.exists():
        for line in hashfile.read_text().splitlines():
            fhash, relpath = line.split(" ", 1)
            if (rpath := hashdb.get(fhash)) is not None:
                # Previously seen file can be symlinked
                file = srcdir / relpath
                file.unlink()
                file.symlink_to(os.path.relpath(rpath, (Path(output_dir) / relpath).parent))
            else:
                # This is a new file
                hashdb[fhash] = os.path.join(output_dir, relpath)
        hashfile.unlink()
    print(f"Moving {srcdir.relative_to(tempdir)} -> {destdir.relative_to(tempdir)}", flush=True)
    srcdir.rename(destdir)


def template_overview(output_dir, device_list, board_list, template_path):
    html = Environment(loader=FileSystemLoader(template_path)).get_template("docs_modm_io_index.html.in").render(
        devices=device_list,
        boards=[(b, rename_board(b), d) for (b,d) in board_list],
        date=datetime.datetime.now().strftime("%d.%m.%Y, %H:%M"),
        num_devices=len(device_list),
        num_boards=len(board_list))
    (output_dir / "index.html").write_text(html)
    json_data = {
        "devices": [str(d).upper() for d in device_list] + [rename_board(b) for (b,_) in board_list],
        "name2board": [{rename_board(b): b} for (b,_) in board_list],
    }
    with (output_dir / "develop/targets.json").open("w+", encoding="UTF-8") as outfile:
        json.dump(json_data, outfile)
    (output_dir / "robots.txt").write_text("User-agent: *\n")

if __name__ == "__main__":
    exit(main())
