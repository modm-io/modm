#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc_project_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

int
main()
{
	// Let's print some information that is provided in the xpcc_git_info.hpp
	XPCC_LOG_INFO << "Project Name: " << XPCC_PROJECT_NAME << xpcc::endl;
	XPCC_LOG_INFO << "Add more defines by editing project_info_header" << xpcc::endl;
	XPCC_LOG_INFO << "in scons/site_tools/xpcc.py." << xpcc::endl;

	return 0;
}
