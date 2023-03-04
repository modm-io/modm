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
### Logging via Real-Time Transport (RTT)

Logging using the RTT protocol is supported by the `modm:platform:rtt` module.
You can use OpenOCD to send and receive on a channel of your choice using the
built-in Python telnet client:

```sh
python3 -m modm_tools.rtt --channel 0 openocd -f modm/openocd.cfg
```

JLink is also supported and may have faster transfer rates:

```sh
python3 -m modm_tools.rtt --channel 0 jlink -device STM32F469NI
```

(\* *only ARM Cortex-M targets*)
"""

from . import openocd, jlink

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Real-Time Transfer (RTT) logging.")
    parser.add_argument(
            "--channel",
            dest="channel",
            type=int,
            default=0,
            help="The RTT channel to display.")

    # Add backends
    subparsers = parser.add_subparsers(title="Backend", dest="backend")
    openocd.add_subparser(subparsers)
    jlink.add_subparser(subparsers)

    args = parser.parse_args()
    backend = args.backend(args)

    if isinstance(backend, openocd.OpenOcdBackend):
        openocd.rtt(backend, args.channel)
    elif isinstance(backend, jlink.JLinkBackend):
        jlink.rtt(backend, args.channel)
