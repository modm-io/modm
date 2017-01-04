/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG_LEVEL_HPP
#define XPCC_LOG_LEVEL_HPP

#pragma push_macro("ERROR") // avoid collision with ERROR defined macro in winsock.h
#undef ERROR

namespace xpcc
{
	namespace log
	{
		/**
		 * \brief	Log levels
		 *
		 * DEBUG < INFO < WARNING < ERROR < DISABLED
		 *
		 * \ingroup logger
		 */
		enum Level
		{
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			DISABLED
		};
	}
}

#ifndef XPCC_LOG_LEVEL
	/**
	 * \brief	Default log level
	 * 
	 * Define to set the level of the logging to a defined value for each file.
	 * To change the logging level in a source file use \c \#undef
	 * 
	 * \code
	 * #undef  XPCC_LOG_LEVEL
	 * #define XPCC_LOG_LEVEL xpcc::log::INFO
	 * \endcode
	 * 
	 * DEBUG < INFO < WARNING < ERROR < DISABLED
	 * 
	 * \ingroup logger
	 */
	#define XPCC_LOG_LEVEL xpcc::log::DEBUG
#endif // XPCC_LOG_LEVEL

#pragma pop_macro("ERROR")

#endif // XPCC_LOG_LEVEL_HPP
