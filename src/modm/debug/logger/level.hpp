/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2016, Tarik TIRE
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOG_LEVEL_HPP
#define MODM_LOG_LEVEL_HPP

#undef ERROR // avoid collision with ERROR defined macro in winsock.h

namespace modm
{
	namespace log
	{
		/**
		 * \brief	Log levels
		 *
		 * DEBUG < INFO < WARNING < ERROR < DISABLED
		 *
		 * \ingroup modm_debug
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

#ifndef MODM_LOG_LEVEL
	/**
	 * \brief	Default log level
	 *
	 * Define to set the level of the logging to a defined value for each file.
	 * To change the logging level in a source file use \c \#undef
	 *
	 * \code
	 * #undef  MODM_LOG_LEVEL
	 * #define MODM_LOG_LEVEL modm::log::INFO
	 * \endcode
	 *
	 * DEBUG < INFO < WARNING < ERROR < DISABLED
	 *
	 * \ingroup modm_debug
	 */
	#define MODM_LOG_LEVEL modm::log::DEBUG
#endif // MODM_LOG_LEVEL

#endif // MODM_LOG_LEVEL_HPP
