#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, 2016-2017, German Aerospace Center (DLR)
# Copyright (c) 2018, 2021, 2023, Niklas Hauser
# Copyright (c) 2018, Fabian Greif
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2013-2014, 2016-2017, Fabian Greif (DLR RY-AVS)
# - 2018, 2021, Niklas Hauser
# - 2018, Fabian Greif

import sys
import os

from SCons.Script import *

# ─────-------------------------------------------------------
def generate(env, **kw):
    colors = {
        'cyan':         '\033[;0;36m',
        'purple':       '\033[;0;35m',
        'blue':         '\033[;0;34m',
        'green':        '\033[;0;32m',
        'boldgreen':    '\033[;1;32m',
        'lightgreen':   '\033[;0;92m',
        'yellow':       '\033[;0;33m',
        'boldyellow':   '\033[;1;33m',
        'lightyellow':  '\033[;0;93m',
        'red':          '\033[;0;31m',
        'boldred':      '\033[;1;31m',
        'end':          '\033[;0;0m',
    }

    # If the output is not a terminal, remove the colors
    if not sys.stdout.isatty():
        for key in colors:
            colors[key] = ''

    default = (colors['green'], colors['yellow'], colors['end'])
    library = (colors['boldgreen'], colors['yellow'], colors['end'])
    linking = (colors['boldgreen'], colors['boldyellow'], colors['end'])
    install = (colors['green'], colors['yellow'], colors['green'], colors['boldyellow'], colors['end'])
    template = install

    # build messages
    if ARGUMENTS.get('verbose') != '1':
        # Warning: Due to an inconsistency in SCons these ASCII-art arrow are
        #          necessary to keep the indentation. Spaces would be removed.
        #
        # See also:
        # http://scons.tigris.org/ds/viewMessage.do?dsForumId=1268&dsMessageId=2425232

        # Relative path is enough
        env['CCCOMSTR'] =       "%sCompiling C···· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['CXXCOMSTR'] =      "%sCompiling C++·· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['ASCOMSTR'] =       "%sAssembling····· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['ASPPCOMSTR'] =     env['ASCOMSTR']
        env['RANLIBCOMSTR'] =   "%sIndexing······· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['ARCOMSTR'] =       "%sArchiving······ %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % library
        env['STRIPCOMSTR'] =    "%sStripping······ %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % linking
        env['SYMBOLSCOMSTR'] =  "%sSymbols········ %s${str(SOURCE).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        # Full path for information
        env['SIZECOMSTR'] =     "%sMemory usage··· %s$SOURCE%s" % default
        env['LINKCOMSTR'] =     "%sLinking········ %s$TARGET%s" % linking
        env['HEXCOMSTR'] =      "%sHex File······· %s$TARGET%s" % default
        env['BINCOMSTR'] =      "%sBinary File···· %s$TARGET%s" % default
        env['LSSCOMSTR'] =      "%sListing········ %s$TARGET%s" % default
        env['UF2COMSTR'] =      "%sUF2 File······· %s$TARGET%s" % default

        # modm tools format strings
        env['BITMAPCOMSTR'] =   "%sBitmap········· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['FONTCOMSTR'] =     "%sFont··········· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default
        env['UNITTESTCOMSTR'] = "%sUnittest······· %s${str(TARGET).replace(BUILDPATH,CONFIG_PROFILE)}%s" % default

        # modm tools templating
        env["JINJA_TEMPLATE_COMSTR"] =   "%s╭────Jinja2──── %s$SOURCE\n" \
                                         "%s╰───Template──> %s$TARGET%s" % template

        env["XPCC_PACKETS_COMSTR"] =     "%s╭─────XPCC───── %s$SOURCE\n" \
                                         "%s╰───Packets───> %s$TARGET%s" % template

        env["XPCC_IDENTIFIER_COMSTR"] =  "%s╭─────XPCC───── %s$SOURCE\n" \
                                         "%s╰-Identifier──> %s$TARGET%s" % template

        env["XPCC_POSTMAN_COMSTR"] =     "%s╭─────XPCC───── %s$SOURCE\n" \
                                         "%s╰───Postman───> %s$TARGET%s" % template

        env["XPCC_COMM_STUBS_COMSTR"] =  "%s╭─────XPCC───── %s$SOURCE\n" \
                                         "%s╰─Comm─Stubs──> %s$TARGET%s" % template

        env["XPCC_TASK_CALLER_COMSTR"] = "%s╭─────XPCC───── %s$SOURCE\n" \
                                         "%s╰─Task─Caller─> %s$TARGET%s" % template

        env["NANOPB_MESSAGES_COMSTR"] =  "%s╭────Nanopb──── %s$SOURCE\n" \
                                         "%s╰───Messages──> %s$TARGET%s" % template

        # modm tools copying
        env["ARTIFACT_COMSTR"] =         "%s╭───Artifact─── %s$SOURCE\n" \
                                         "%s╰────Cache────> %s$ARTIFACT_FILEPATH%s" % install

        # Debug probes running
        env["RUN_OPENOCD_COMSTR"] =      "%s╭────────────── %s\n" \
                                         "%s╰───OpenOCD───> %s$CONFIG_DEVICE_NAME%s" % install

        env["RUN_JLINK_COMSTR"] =        "%s╭────────────── %s\n" \
                                         "%s╰────JLink────> %s$CONFIG_DEVICE_NAME%s" % install

        # Debug probes debugging
        env["DEBUG_OPENOCD_COMSTR"] =    "%s╭─────GDB─────> %s$SOURCE\n" \
                                         "%s╰───OpenOCD───> %s$CONFIG_DEVICE_NAME%s" % install

        env["DEBUG_BMP_COMSTR"] =        "%s╭─────GDB─────> %s$SOURCE\n" \
                                         "%s╰─────BMP─────> %s$CONFIG_DEVICE_NAME%s" % install

        env["DEBUG_JLINK_COMSTR"] =      "%s╭─────GDB─────> %s$SOURCE\n" \
                                         "%s╰────JLink────> %s$CONFIG_DEVICE_NAME%s" % install

        env["DEBUG_REMOTE_COMSTR"] =     "%s╭─────GDB─────> %s$SOURCE\n" \
                                         "%s╰─Rem─OpenOCD─> %s$CONFIG_DEVICE_NAME%s" % install

        env["DEBUG_COREDUMP_COMSTR"] =   "%s╭─────GDB─────> %s$SOURCE\n" \
                                         "%s╰───Coredump──> %s$CONFIG_DEVICE_NAME ($COREDUMP_FILE)%s" % install

        # Debug probes programming
        env["PROGRAM_OPENOCD_COMSTR"] =  "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰───OpenOCD───> %s$CONFIG_DEVICE_NAME%s" % install

        env["PROGRAM_BMP_COMSTR"] =      "%s╭─Black─Magic── %s$SOURCE\n" \
                                         "%s╰────Probe────> %s$CONFIG_DEVICE_NAME%s" % install

        env["PROGRAM_JLINK_COMSTR"] =    "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰────JLink────> %s$CONFIG_DEVICE_NAME%s" % install

        env["PROGRAM_AVRDUDE_COMSTR"] =  "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰───Avrdude───> %s$CONFIG_DEVICE_NAME%s" % install

        env["PROGRAM_BOSSAC_COMSTR"] =   "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰────BOSSAc───> %s$CONFIG_DEVICE_NAME%s" % install

        env['PROGRAM_DFU_COMSTR'] =      "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰─────DFU─────> %s$CONFIG_DEVICE_NAME%s" % install

        env["PROGRAM_REMOTE_COMSTR"] =   "%s╭────────────── %s$SOURCE\n" \
                                         "%s╰─Rem─OpenOCD─> %s$CONFIG_DEVICE_NAME%s" % install

        # Debug probes resetting
        env["RESET_OPENOCD_COMSTR"] =    "%s╭────Reset───── %s\n" \
                                         "%s╰───OpenOCD───> %s$CONFIG_DEVICE_NAME%s" % install

        env["RESET_BMP_COMSTR"] =        "%s╭────Reset───── %s\n" \
                                         "%s╰─────BMP─────> %s$CONFIG_DEVICE_NAME%s" % install

        env["RESET_JLINK_COMSTR"] =      "%s╭────Reset───── %s\n" \
                                         "%s╰────JLink────> %s$CONFIG_DEVICE_NAME%s" % install

        env["RESET_REMOTE_COMSTR"] =     "%s╭────Reset───── %s\n" \
                                         "%s╰─Remote─GDB──> %s$CONFIG_DEVICE_NAME%s" % install

        # Debug probes coredumping
        env["COREDUMP_OPENOCD_COMSTR"] = "%s╭───Coredump──> %s$COREDUMP_FILE\n" \
                                         "%s╰───OpenOCD──── %s$CONFIG_DEVICE_NAME%s" % install

        env["COREDUMP_JLINK_COMSTR"] =   "%s╭───Coredump──> %s$COREDUMP_FILE\n" \
                                         "%s╰────JLink───── %s$CONFIG_DEVICE_NAME%s" % install

        # Debug probes logging
        env["ITM_OPENOCD_COMSTR"] =      "%s╭───OpenOCD───> %sSingle Wire Viewer\n" \
                                         "%s╰─────SWO────── %s$CONFIG_DEVICE_NAME%s" % install

        env["ITM_JLINK_COMSTR"] =        "%s╭────JLink────> %sSingle Wire Viewer\n" \
                                         "%s╰─────SWO────── %s$CONFIG_DEVICE_NAME%s" % install

        env["RTT_OPENOCD_COMSTR"] =      "%s╭───OpenOCD───> %sReal Time Transfer\n" \
                                         "%s╰─────RTT────── %s$CONFIG_DEVICE_NAME%s" % install

        env["RTT_JLINK_COMSTR"] =        "%s╭────JLink────> %sReal Time Transfer\n" \
                                         "%s╰─────RTT────── %s$CONFIG_DEVICE_NAME%s" % install

def exists(env):
    return True
