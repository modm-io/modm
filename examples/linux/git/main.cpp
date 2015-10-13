#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc_git_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

int
main()
{
	// Let's print some information that is provided in the xpcc_git_info.hpp
	XPCC_LOG_INFO << "Local Git User:" << xpcc::endl;
	XPCC_LOG_INFO << "Name:  " << XPCC_GIT_CONFIG_USER_NAME      << xpcc::endl;
	XPCC_LOG_INFO << "Email: " << XPCC_GIT_CONFIG_USER_EMAIL     << xpcc::endl;
	XPCC_LOG_INFO << "Last Commit:" << xpcc::endl;
	XPCC_LOG_INFO << "SHA:             " << XPCC_GIT_SHA      << xpcc::endl;
	XPCC_LOG_INFO << "Abbreviated SHA: " << XPCC_GIT_SHA_ABBR << xpcc::endl;
	XPCC_LOG_INFO << "Subject:         " << XPCC_GIT_SUBJECT  << xpcc::endl;
	XPCC_LOG_INFO << xpcc::endl << "Author:" << xpcc::endl;
	XPCC_LOG_INFO << "Name:      " << XPCC_GIT_AUTHOR                << xpcc::endl;
	XPCC_LOG_INFO << "Email:     " << XPCC_GIT_AUTHOR_EMAIL          << xpcc::endl;
	XPCC_LOG_INFO << "Date:      " << XPCC_GIT_AUTHOR_DATE           << xpcc::endl;
	XPCC_LOG_INFO << "Timestamp: " << XPCC_GIT_AUTHOR_DATE_TIMESTAMP << xpcc::endl;
	XPCC_LOG_INFO << xpcc::endl << "Committer:" << xpcc::endl;
	XPCC_LOG_INFO << "Name:      " << XPCC_GIT_COMMITTER                << xpcc::endl;
	XPCC_LOG_INFO << "Email:     " << XPCC_GIT_COMMITTER_EMAIL          << xpcc::endl;
	XPCC_LOG_INFO << "Date:      " << XPCC_GIT_COMMITTER_DATE           << xpcc::endl;
	XPCC_LOG_INFO << "Timestamp: " << XPCC_GIT_COMMITTER_DATE_TIMESTAMP << xpcc::endl;
	XPCC_LOG_INFO << xpcc::endl << "File Status:" << xpcc::endl;
	XPCC_LOG_INFO << "Modified:  " << XPCC_GIT_MODIFIED  << xpcc::endl;
	XPCC_LOG_INFO << "Added:     " << XPCC_GIT_ADDED     << xpcc::endl;
	XPCC_LOG_INFO << "Deleted:   " << XPCC_GIT_DELETED   << xpcc::endl;
	XPCC_LOG_INFO << "Renamed:   " << XPCC_GIT_RENAMED   << xpcc::endl;
	XPCC_LOG_INFO << "Copied:    " << XPCC_GIT_COPIED    << xpcc::endl;
	XPCC_LOG_INFO << "Untracked: " << XPCC_GIT_UNTRACKED << xpcc::endl;

	return 0;
}
