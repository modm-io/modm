#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2023, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Logging via Single-Wire Output (SWO)

Logging using the SWO protocol is supported by the `modm:platform:itm` module.
You can use OpenOCD to receive the output, but you must manually supply the
CPU frequency:

```sh
python3 -m modm_tools.itm openocd -f modm/openocd.cfg --fcpu 48000000
```

(\* *only ARM Cortex-M targets*)
"""

from . import openocd

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='SWO output via ITM.')
    parser.add_argument(
            "-b", "--baudrate",
            dest="baudrate",
            type=int,
            default=None,
            help="Set the baudrate of the ITM connection.")

    subparsers = parser.add_subparsers(title="Backend", dest="backend")

    # Add backends
    parser_openocd = openocd.add_subparser(subparsers)
    parser_openocd.add_argument(
            "--fcpu",
            dest="fcpu",
            type=int,
            help="The devices' CPU/HCLK frequency.")

    args = parser.parse_args()
    backend = args.backend(args)

    if isinstance(backend, openocd.OpenOcdBackend):
        openocd.itm(backend, fcpu=args.fcpu, baudrate=args.baudrate)
