/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, 2013, Martin Rosekeit
 * Copyright (c) 2010, 2013, Georgi Grinshpun
 * Copyright (c) 2012, 2016, Niklas Hauser
 * Copyright (c) 2013, 2015, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LOGGER_HPP
#define MODM_LOGGER_HPP

#include <modm/architecture/utils.hpp>
#include <modm/io/iostream.hpp>

#include "level.hpp"
#include "style.hpp"
#include "style_wrapper.hpp"
#include "style/prefix.hpp"

namespace modm
{
	/**
	 * \ingroup	modm_debug
	 * \brief	Logger
	 */
	namespace log
	{
		/**
		 * \brief 	Interface to the Logger.
		 *
		 * It is used by the macros defined below. This class overloads
		 * the shift-operator so that it is possible to write different
		 * message types to the logger.
		 *
		 * \ingroup modm_debug
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		class Logger : public ::modm::IOStream
		{
			public:
				inline Logger(::modm::IODevice& outputDevice) :
					IOStream(outputDevice)
				{
				}

			private:
				Logger(const Logger&);

				Logger&
				operator = (const Logger&);
		};

		/**
		 * \name	Output streams
		 *
		 * Don't use this instances directly! Prefer an access through the
		 * MODM_LOG_DEBUG, MODM_LOG_INFO, MODM_LOG_WARNING and
		 * MODM_LOG_ERROR macros.
		 *
		 * \ingroup modm_debug
		 */
		//\{
		extern Logger debug;	///< log device to take messages on DEBUG level
		extern Logger info;		///< log device to take messages on INFO level
		extern Logger warning;	///< log device to take messages on WARNING level
		extern Logger error;	///< log device to take messages on ERROR level
		//\}
	}
}

// these macros are defined like this to avoid the dangling else problem.
// if (condition)
// 		MODM_LOG_DEBUG << "string";
// else
//		expression;

/**
 * \brief	Turn off messages print
 * \ingroup modm_debug
 */
#define MODM_LOG_OFF \
	if ( true ){}	\
	else modm::log::debug

/**
 * \brief	Output stream for debug messages
 * \ingroup modm_debug
 */
#define MODM_LOG_DEBUG \
	if (MODM_LOG_LEVEL > modm::log::DEBUG){} \
	else modm::log::debug

/**
 * \brief	Output stream for info messages
 * \ingroup modm_debug
 */
#define MODM_LOG_INFO \
	if (MODM_LOG_LEVEL > modm::log::INFO){}	\
	else modm::log::info

/**
 * \brief	Output stream for warnings
 * \ingroup modm_debug
 */
#define MODM_LOG_WARNING \
	if (MODM_LOG_LEVEL > modm::log::WARNING){}	\
	else modm::log::warning

/**
 * \brief	Output stream for error messages
 * \ingroup modm_debug
 */
#define MODM_LOG_ERROR \
	if (MODM_LOG_LEVEL > modm::log::ERROR){}	\
	else modm::log::error

#ifdef __DOXYGEN__

/**
 * \brief	Filename of the current file
 *
 * In contrast to \c __FILE__ the path to the file is omitted (if this feature
 * is available, otherwise this macro will resolve to \c __FILE__).
 *
 * \ingroup	modm_debug
 */
#define FILENAME

/**
 * \brief	String containing the filename and the current line
 *
 * Will generate something like:
 * \code
 * "file.cpp (123) >> "
 * \endcode
 *
 * This can be very useful the track the origin of log-messages:
 * \code
 * MODM_LOG_DEBUG   << MODM_FILE_INFO
 *                  << "... something has happened ..."
 *                  << modm::endl;
 * \endcode
 *
 * This will result into:
 * \code
 * file.cpp (123) >> ... something has happened ...
 * \endcode
 *
 * \ingroup	modm_debug
 */
#define	MODM_FILE_INFO

#else	// !__DOXYGEN__

#ifndef	BASENAME
#	ifndef __BASE_FILE__
#		define FILENAME	__FILE__
#	else
#		define FILENAME __BASE_FILE__
#	endif
#else
#	define	FILENAME	MODM_STRINGIFY(BASENAME)
#endif

#define	MODM_FILE_INFO		"[" FILENAME "(" MODM_STRINGIFY(__LINE__) ")] "

#endif	// __DOXYGEN__
#endif // MODM_LOGGER_HPP
