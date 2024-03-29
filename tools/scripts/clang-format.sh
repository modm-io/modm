#!/bin/bash
# Copyright (c) 2020, Erik Henriksson
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#----------------------------------------------------------------------------

git diff --name-only --diff-filter=A -C -M develop | grep -e "\.\(c\|h\|cc\|hh\|hpp\|cpp\)\$" | xargs -r clang-format -i "$@"
