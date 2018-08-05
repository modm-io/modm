#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2017-2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from collections import defaultdict, OrderedDict
import subprocess
import sys
import re
import os
from configparser import ConfigParser
from pathlib import Path
from jinja2 import Environment

lbuild_template = \
r"""<?xml version='1.0' encoding='UTF-8'?>
<library>
{%- if extends is defined %}
  <extends>{{extends}}</extends>
{%- else %}
  <repositories>
    <repository><path>../modm/repo.lb</path></repository>
  </repositories>
{%- endif %}

  <options>
{%- for name, value in options.items() %}
    <option name="{{name}}">{{value}}</option>
{%- endfor %}
  </options>

  <modules>
{%- for name in modules %}
    <module>{{name}}</module>
{%- endfor %}
  </modules>
</library>
"""

board_map = {
    "al_avreb_can": "al_avreb_can",
    "arduino_uno": "arduino_uno",
    "lpcxpresso_lpc11c24": None,
    "nucleo_f031k6": "nucleo_f031k6",
    "nucleo_f103rb": "nucleo_f103rb",
    "nucleo_f303k8": "nucleo_f303k8",
    "nucleo_f401re": "nucleo_f401re",
    "nucleo_f411re": "nucleo_f411re",
    "nucleo_f429zi": "nucleo_f429zi",
    "nucleo_l432kc": "nucleo_l432kc",
    "nucleo_l476rg": "nucleo_l476rg",
    "olimexino_stm32": "olimexino_stm32",
    "stm32f030f4p6_demo_board": "stm32f030f4p6_demo",
    "stm32f072_discovery": "disco_f072rb",
    "stm32f0_discovery": "disco_f051r8",
    "stm32f103c8t6_black_pill": "black_pill",
    "stm32f103c8t6_blue_pill": "blue_pill",
    "stm32f1_discovery": "disco_f100rb",
    "stm32f3_discovery": "disco_f303vc",
    "stm32f429_discovery": "disco_f429zi",
    "stm32f469_discovery": "disco_f469ni",
    "stm32f4_discovery": "disco_f407vg",
    "stm32f746g_discovery": "disco_f746ng",
    "stm32f769i_discovery": "disco_f769ni",
    "stm32l476_discovery": "disco_l476vg",
}

file_ext = [
    ".c",
    ".h",
    ".cpp",
    ".hpp",
]

diff_mapping = [
    (r"xpcc", "modm"),
    (r"XPCC", "MODM"),
    (r"(\w)__(\w)", r"\1_\2"),
    # Some header files moved
    (r"modm/architecture\.hpp", "modm/platform.hpp"),
    (r"modm/architecture/platform", "modm/platform"),
    (r"modm/architecture/driver/atomic/lock", "modm/architecture/interface/atomic_lock"),
    (r"modm/architecture/driver/", "modm/architecture/interface/"),
    (r"modm/architecture/interface/atomic\.hpp", "modm/architecture/driver/atomic.hpp"),
    (r"modm/communication\.hpp", "modm/communication/xpcc.hpp"),
    (r"modm/communication/modm\.hpp", "modm/communication/xpcc.hpp"),
    (r"modm/architecture/interface/accessor/flash\.hpp", "modm/architecture/interface/accessor_flash.hpp"),
    # specific classes and namespaces moved
    (r"modm::SoftwareI2cMaster", "BitBangI2cMaster"),
    (r"modm::SoftwareSpiMaster", "BitBangSpiMaster"),
    (r"modm::SoftwareOneWireMaster", "BitBangOneWireMaster"),
    (r"modm::SoftwareGpioPort", "SoftwareGpioPort"),
    (r"modm::GpioUnused", "GpioUnused"),
    (r"modm::GpioInverted", "GpioInverted"),
    (r"modm::stm32", "modm::platform"),
    (r"modm::atmega", "modm::platform"),
    (r"modm::attiny", "modm::platform"),
    (r"modm::avr", "modm::platform"),
    (r"modm::at90", "modm::platform"),
    (r"MAIN_FUNCTION\w*", "int main()"),
    (r"(?<!MODM_)STRINGIFY", "MODM_STRINGIFY"),
    (r"(?<!MODM_)CONCAT", "MODM_CONCAT"),
    (r"ATTRIBUTE_UNUSED", "modm_unused"),
    # connect function changed its API
    (r"([a-zA-Z0-9:]+?)::connect\( *?(\w+?)::(\w+?) *?(, ?(.*?))?\);", r"\2::connect<\1::\3>(\5);"),
    (r"([a-zA-Z0-9:]+?)::setChannel\( *?(\w+?)::Adc\dChannel *?(, ?(.*?))?\);", r"\1::setPinChannel<\2>(\4);"),
    # undo xpcc->modm rename for XPCC classes and include paths
    (r"modm/communication/modm/", "modm/communication/xpcc/"),
    (r"modm::AbstractComponent", "xpcc::AbstractComponent"),
    (r"modm::ActionResult", "xpcc::ActionResult"),
    (r"modm::ActionResponse", "xpcc::ActionResponse"),
    (r"modm::CanConnector", "xpcc::CanConnector"),
    (r"modm::CanConnectorBase", "xpcc::CanConnectorBase"),
    (r"modm::CommunicatableTask", "xpcc::CommunicatableTask"),
    (r"modm::Communicator", "xpcc::Communicator"),
    (r"modm::Dispatcher", "xpcc::Dispatcher"),
    (r"modm::DynamicPostman", "xpcc::DynamicPostman"),
    (r"modm::Header", "xpcc::Header"),
    (r"modm::Postman", "xpcc::Postman"),
    (r"modm::ResponseHandle", "xpcc::ResponseHandle"),
    (r"modm::TipcConnector", "xpcc::TipcConnector"),
    (r"modm::ResponseCallback", "xpcc::ResponseCallback"),
    (r"modm::ZeroMQ", "xpcc::ZeroMQ"),
    (r"modm::Response", "xpcc::Response"),
    (r"MODM_CAN_PACKET_", "XPCC_CAN_PACKET_"),
]

# modm_modules = [ ... long list of tuples ... ]
exec((Path(os.path.realpath(__file__)).parent / "modm_modules.py").read_text())

def get_files(paths):
    files = []
    for path in paths:
        if path.is_file():
            files.append(path)
        else:
            for ext in file_ext:
                glob = path.glob("**/*{}".format(ext))
                glob = [g for g in glob if g.parts[0] != "modm"]
                files.extend(glob)
    return files

def map_diff(content):
    for (inp, out) in diff_mapping:
        content = re.sub(inp, out, content)
    return content

def get_includes(content):
    incs = re.findall(r"#include +?[<\"](.*?)[>\"]", content)
    modm_inc = [i for i in incs if i.startswith("modm/")]
    local_inc = [i for i in incs if not i.startswith("modm/")]
    return (modm_inc, local_inc)

def get_modules(content):
    modules = []
    for (pattern, module) in modm_modules:
        for match in re.findall(pattern, content):
            mod = re.sub(pattern, module, match[0] if isinstance(match, tuple) else match)
            # print(match, "->", mod, pattern)
            modules.append(mod)

    return modules

project_file = Path(sys.argv[1])
if project_file.is_file(): project_file = project_file.parent;
project_file = project_file / "project.cfg"
lbuild_project_file = Path(project_file.parent / "project.xml")
if project_file.is_file():
    config = ConfigParser()
    config.read(project_file)
else:
    project_file = None

files = get_files([Path(p) for p in sys.argv[1:] if not p.startswith("-")])
modules = ["modm:build:scons", "modm:platform:core"]

for file in files:
    content = file.read_text()
    content = map_diff(content)
    file.write_text(content)
    modules.extend(get_modules(content))
modules = sorted(list(set(modules)))

if project_file:
    # gather the build data from old config
    subs = {"options": {
        "modm:build:build.path": config["build"].get("buildpath", "build/")\
            .replace("${name}", config["build"].get("name", ""))\
            .replace("${xpccpath}", "."),
        "modm:build:scons:include_sconstruct": False,
    }}
    if config["build"].get("board", None):
        board = config["build"]["board"]
        if board not in board_map:
            print("Unsupported board!", board)
        elif board_map[board]:
            subs["extends"] = "../modm/src/modm/board/{}/board.xml".format(board_map[board])
        else:
            print("Board support has been dropped in modm!", board)
    elif config["build"].get("device", False):
        device = config["build"]["device"]
        subs["options"]["modm:target"] = device
        print("Please add the package to the device identifier!", device)

    if "parameters" in config:
        for key in config["parameters"]:
            k = key.split(".")
            if k[0] in ["can", "uart"]:
                name = "buffer." + k[3][:2]
                subs["options"]["modm:platform:{}:{}:{}".format(k[0], k[2], name)] = config["parameters"][key]
            if k[0] in ["i2c"]:
                subs["options"]["modm:platform:i2c:{}:transaction_buffer".format(k[2])] = config["parameters"][key]
            if k[0] in ["core"]:
                if k[3] == "vector_table_in_ram":
                    subs["options"]["modm:platform:core:vector_table_location"] = \
                        "ram" if config["parameters"].getboolean(key) else "fastest"
                if k[3] == "main_stack_size":
                    subs["options"]["modm:platform:core:main_stack_size"] = config["parameters"][key]

    subs["options"] = OrderedDict(sorted(subs["options"].items(), key=lambda t: t[0]))
    subs["modules"] = modules
    lbuild_config = Environment().from_string(lbuild_template).render(subs)
    if not lbuild_project_file.is_file() or "-f" in sys.argv:
        lbuild_project_file.write_text(lbuild_config)
else:
    print("\n".join(modules))

