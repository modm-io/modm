/*
 * Copyright (c) 2014-2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/debug/logger.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

int
main()
{
	// Let's print some information that is provided in the modm_git_info.hpp
	MODM_LOG_INFO << "Local Git User:" << modm::endl;
	MODM_LOG_INFO << "Name:  " << MODM_GIT_CONFIG_USER_NAME      << modm::endl;
	MODM_LOG_INFO << "Email: " << MODM_GIT_CONFIG_USER_EMAIL     << modm::endl;
	MODM_LOG_INFO << "Last Commit:" << modm::endl;
	MODM_LOG_INFO << "SHA:             " << MODM_GIT_SHA      << modm::endl;
	MODM_LOG_INFO << "Abbreviated SHA: " << MODM_GIT_SHA_ABBR << modm::endl;
	MODM_LOG_INFO << "Subject:         " << MODM_GIT_SUBJECT  << modm::endl;
	MODM_LOG_INFO << modm::endl << "Author:" << modm::endl;
	MODM_LOG_INFO << "Name:      " << MODM_GIT_AUTHOR                << modm::endl;
	MODM_LOG_INFO << "Email:     " << MODM_GIT_AUTHOR_EMAIL          << modm::endl;
	MODM_LOG_INFO << "Date:      " << MODM_GIT_AUTHOR_DATE           << modm::endl;
	MODM_LOG_INFO << "Timestamp: " << MODM_GIT_AUTHOR_DATE_TIMESTAMP << modm::endl;
	MODM_LOG_INFO << modm::endl << "Committer:" << modm::endl;
	MODM_LOG_INFO << "Name:      " << MODM_GIT_COMMITTER                << modm::endl;
	MODM_LOG_INFO << "Email:     " << MODM_GIT_COMMITTER_EMAIL          << modm::endl;
	MODM_LOG_INFO << "Date:      " << MODM_GIT_COMMITTER_DATE           << modm::endl;
	MODM_LOG_INFO << "Timestamp: " << MODM_GIT_COMMITTER_DATE_TIMESTAMP << modm::endl;
	MODM_LOG_INFO << modm::endl << "File Status:" << modm::endl;
	MODM_LOG_INFO << "Modified:  " << MODM_GIT_MODIFIED  << modm::endl;
	MODM_LOG_INFO << "Added:     " << MODM_GIT_ADDED     << modm::endl;
	MODM_LOG_INFO << "Deleted:   " << MODM_GIT_DELETED   << modm::endl;
	MODM_LOG_INFO << "Renamed:   " << MODM_GIT_RENAMED   << modm::endl;
	MODM_LOG_INFO << "Copied:    " << MODM_GIT_COPIED    << modm::endl;
	MODM_LOG_INFO << "Untracked: " << MODM_GIT_UNTRACKED << modm::endl;

	return 0;
}
