#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Logging

Usually logging is done simply via a serial port and only a USB-Serial adapter
is necessary to read the device output. However, some forms of logging require
host support or additional processing steps.


#### ITM

Logging via the SWO trace pin is supported via OpenOCD and the
`modm:platform:itm` module:

```sh
python3 modm/modm_tools/log.py itm openocd -f modm/openocd.cfg --fcpu 64000000
```

#### RTT

Logging via the RTT protocol is supported via OpenOCD and the
`modm:platform:rtt` module:

```sh
python3 modm/modm_tools/log.py rtt openocd -f modm/openocd.cfg --channel 0
```

(\* *only ARM Cortex-M targets*)
"""

# -----------------------------------------------------------------------------
import os

if __name__ == "__main__":
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

    import argparse
    from modm_tools import openocd

    parser = argparse.ArgumentParser(description='Host-side logging post-processing.')
    parser.add_argument(
            dest="type",
            choices=["itm", "rtt"],
            help="The type of log connection.")

    subparsers = parser.add_subparsers(title="Backend", dest="backend")

    # Add backends
    parser_openocd = openocd.add_subparser(subparsers)
    parser_openocd.add_argument(
            "--fcpu",
            dest="fcpu",
            type=int,
            help="The devices' CPU/HCLK frequency (ITM only).")
    parser_openocd.add_argument(
            "-b", "--baudrate",
            dest="baudrate",
            type=int,
            help="Set the baudrate of the log connection (ITM only).")
    parser_openocd.add_argument(
            "--channel",
            dest="channel",
            type=int,
            default=0,
            help="The RTT channel to display (RTT only).")

    args = parser.parse_args()
    # FIXME: Currently hardcoded to the OpenOCD backend
    if args.type == "itm":
        openocd.log_itm(backend=args.backend(args), fcpu=args.fcpu, baudrate=args.baudrate)
    else:
        openocd.log_rtt(backend=args.backend(args), channel=args.channel)
