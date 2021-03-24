/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char*		MODM_GIT_SHA;
extern const char*		MODM_GIT_SHA_ABBR;
extern const char*		MODM_GIT_AUTHOR;
extern const char*		MODM_GIT_AUTHOR_EMAIL;
extern const char*		MODM_GIT_AUTHOR_DATE;
extern const uint32_t	MODM_GIT_AUTHOR_DATE_TIMESTAMP;
extern const char*		MODM_GIT_COMMITTER;
extern const char*		MODM_GIT_COMMITTER_EMAIL;
extern const char*		MODM_GIT_COMMITTER_DATE;
extern const uint32_t	MODM_GIT_COMMITTER_DATE_TIMESTAMP;
extern const char*		MODM_GIT_SUBJECT;
extern const char*		MODM_GIT_CONFIG_USER_NAME;
extern const char*		MODM_GIT_CONFIG_USER_EMAIL;

#if defined(MODM_GIT_STATUS) && MODM_GIT_STATUS
extern const int16_t	MODM_GIT_MODIFIED;
extern const int16_t	MODM_GIT_ADDED;
extern const int16_t	MODM_GIT_DELETED;
extern const int16_t	MODM_GIT_RENAMED;
extern const int16_t	MODM_GIT_COPIED;
extern const int16_t	MODM_GIT_UNTRACKED;
#endif

#ifdef __cplusplus
}
#endif
