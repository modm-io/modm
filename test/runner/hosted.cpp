/*
 * Copyright (c) 2009-2011, 2017, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2014, 2016, 2018, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/reporter.hpp>

#include <modm/debug/logger.hpp>
#include <modm/platform.hpp>
#include <modm/driver/io/terminal.hpp>

#include <info_git.h>
#include <info_build.h>

modm::Terminal outputDevice;
namespace unittest
{
	Reporter reporter(outputDevice);
}

modm::log::StyleWrapper< modm::log::Prefix< char[10] > > loggerDeviceDebug( \
		modm::log::Prefix< char[10] >("Debug  : ", outputDevice ));
modm::log::Logger modm::log::debug( loggerDeviceDebug );

modm::log::StyleWrapper< modm::log::Prefix< char[10] > > loggerDeviceInfo( \
		modm::log::Prefix< char[10] >("Info   : ", outputDevice ));
modm::log::Logger modm::log::info( loggerDeviceInfo );

modm::log::StyleWrapper< modm::log::Prefix< char[10] > > loggerDeviceWarning( \
		modm::log::Prefix< char[10] >("Warning: ", outputDevice ));
modm::log::Logger modm::log::warning(loggerDeviceWarning);

modm::log::StyleWrapper< modm::log::Prefix< char[10] > > loggerDeviceError( \
		modm::log::Prefix< char[10] >("Error  : ", outputDevice ));
modm::log::Logger modm::log::error(loggerDeviceError);

int main()
{
	MODM_LOG_INFO << "Machine:  " << MODM_BUILD_MACHINE  << modm::endl;
	MODM_LOG_INFO << "User:     " << MODM_BUILD_USER     << modm::endl;
	MODM_LOG_INFO << "Os:       " << MODM_BUILD_OS       << modm::endl;
	MODM_LOG_INFO << "Compiler: " << MODM_BUILD_COMPILER << modm::endl;

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

	extern int run_modm_unit_test();
	return run_modm_unit_test();
}
