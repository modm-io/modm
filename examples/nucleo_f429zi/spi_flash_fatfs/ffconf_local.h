/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
 // ----------------------------------------------------------------------------

#ifndef FFCONF_DEF
#	error "Don't include this file directly, use 'ffconf.h' instead!"
#endif

// Enable directory filtering: f_findfirst(), f_findnext()
#define FF_USE_FIND 1

// Enable Volume Formatting: f_mkfs()
#define FF_USE_MKFS 1

// Enable Volume Label: f_setlabel(), f_getlabel()
#define FF_USE_LABEL 1

// Configure minium sector size
#define FF_MIN_SS 4096

// Configure maximum sector size
#define FF_MAX_SS 4096

// Enable tiny sector buffers
#define FF_FS_TINY 1
