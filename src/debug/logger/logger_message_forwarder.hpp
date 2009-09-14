// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
#ifndef RCA__LOGGER_MESSAGE_FORWARDER_HPP
#define RCA__LOGGER_MESSAGE_FORWARDER_HPP

#include <string>

#include "logger.hpp"



 

#ifdef RCA_DISABLE_LOGGING
#define SUB_LOGGER_CREATE(OBJECT_NAME, LOCATION_NAME, FORWARD_LOG_LEVEL);
#define SUB_LOGGER_LOG(OBJECT_NAME, LOG_LEVEL, LOCATION)						\
			if(0) std::cout
#define SUB_LOGGER_LOG2(OBJECT_NAME, LOG_LEVEL)									\
			if(0) std::cout
#define SUB_LOGGER_SET_INSTANCE_NAME(OBJECT_NAME, INSTANCE_NAME) ;
#else
/**
 * @def		SUB_LOGGER_CREATE(OBJECT_NAME, FORWARD_LOG_LEVEL, LOCATION)
 * @brief 	Macro to create the LoggerMessageForwarder
 * 
 * Expanation of the following macros: Those macros are necessary to have the 
 * compiler statement for selecting logging or not just at one position in the 
 * code. So it is possible to compile the whole logging stuff out of the code 
 * if necessary - just by using a compiler flag (-D RCA_DISABLE_LOGGING). 
 * 
 * The first macro SUB_LOGGER_CREATE creates a static logger object: \n
 * For example we need a logger that logs every message from and above 
 * warning-level. So we pass Logger::WARNING and a filename the logger was 
 * used (e.g. "logger_test.cpp").\n
 * 
 * The result looks like:
 * \code
 * SUB_LOGGER_CREATE(loggerName, Logger::WARNING, "logger_test.cpp");
 * \endcode
 * 
 * It is replaced by:
 * \code
 * static LoggerMessageForwarder loggerName(Logger::WARNING, "logger_test.cpp");
 * \endcode
 * 
 * @ingroup logger
 * @version	$Id$
 * @since 	04 December 2006
 * @author	Christofer Hedbrand,
 * 			Carsten Schmidt,
 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
#define SUB_LOGGER_CREATE(OBJECT_NAME, FORWARD_LOG_LEVEL, LOCATION)				\
			static LoggerMessageForwarder OBJECT_NAME(FORWARD_LOG_LEVEL, LOCATION) ;

/**
 * @def		SUB_LOGGER_LOG(OBJECT_NAME, LOG_LEVEL, LOCATION) 
 * @brief 	Macro to access the LoggerMessageForwarder
 * 
 * The second macro SUB_LOGGER_LOG is used if the developer want output a 
 * message. One must provide a valid loggername (defined by the 
 * SUB_LOGGER_CREATE macro). The log level of the message and a method or 
 * functionname. \n
 * 
 * For example if the macro is called with:
 * \code
 * SUB_LOGGER_LOG(loggerName, Logger::FATAL, "methodname")	<< "CORBA ON AVR???" 
 * 															<< 123 
 * 															<< " OF COURSE O_o !";
 * \endcode
 * 
 * it is replaced by:
 * \code
 * loggerName.initializeMessage(Logger::FATAL, "methodname");
 * loggerName << "CORBA ON AVR???" << 123 << " OF COURSE O_o !";
 * \endcode
 * 
 * 
 * @ingroup logger
 * @version	$Id$
 * @since 	04 December 2006
 * @author	Christofer Hedbrand,
 * 			Carsten Schmidt,
 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
#define SUB_LOGGER_LOG(OBJECT_NAME, LOG_LEVEL, LOCATION)						\
			OBJECT_NAME.initializeMessage(LOG_LEVEL, LOCATION);					\
			OBJECT_NAME

/**
 * @def		SUB_LOGGER_LOG2(OBJECT_NAME, LOG_LEVEL) 
 * @brief 	Macro to access the LoggerMessageForwarder
 * 
 * The same as SUB_LOGGER_LOG(OBJECT_NAME, LOG_LEVEL, LOCATION) but the LOCATION
 * is replaced internal with the __FUNCTION__-compilervalue \n
 * 
 * For example if the macro is called with:
 * \code
 * SUB_LOGGER_LOG(loggerName, Logger::FATAL)	<< "CORBA ON AVR???" 
 * 												<< 123 
 * 												<< " OF COURSE O_o !";
 * \endcode
 * 
 * it is replaced by:
 * \code
 * loggerName.initializeMessage(Logger::FATAL, __FUNCTION__);
 * loggerName << "CORBA ON AVR???" << 123 << " OF COURSE O_o !";
 * \endcode
 * 
 * 
 * @ingroup logger
 * @version	$Id$
 * @since 	04 December 2006
 * @author	Christofer Hedbrand,
 * 			Carsten Schmidt,
 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
#define SUB_LOGGER_LOG2(OBJECT_NAME, LOG_LEVEL)									\
			OBJECT_NAME.initializeMessage(LOG_LEVEL, __FUNCTION__);				\
			OBJECT_NAME

#define SUB_LOGGER_SET_INSTANCE_NAME(OBJECT_NAME, INSTANCE_NAME)				\
			OBJECT_NAME.setInstanceName(INSTANCE_NAME) ;
#endif

/**
 * @class 	LoggerMessageForwarder 
 * @brief 	Interface to the Logger.
 * 
 * This class provides an interface to the logger singleton. It is used by the
 * macro defined below. This class overloads the << operator so that it is
 * possible to write different message types to the logger.
 * 
 * @ingroup logger
 * @version	$Id$
 * @since 	04 December 2006
 * @author	Christofer Hedbrand,
 * 			Carsten Schmidt,
 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
class LoggerMessageForwarder {
	public:
		LoggerMessageForwarder(	const Logger::PriorityLevel & loggerPriorityLevel, 
								const std::string & loggerLocationString);
		
		void 
		initializeMessage(	const Logger::PriorityLevel & messagePriorityLevel,
							const std::string & messageLocationString);

		template <class T> inline 
		LoggerMessageForwarder& 
		operator <<(const T& msg) {
			if(this->messagePriorityLevel_ >= this->loggerPriorityLevel_) {
				Logger::instance().log(msg);
		   }
		   return *this;
		};



	private:
		Logger::PriorityLevel	loggerPriorityLevel_;
		Logger::PriorityLevel	messagePriorityLevel_;
		std::string				loggerLocationString_;
};

#endif /*RCA__LOGGER_MESSAGE_FORWARDER_HPP*/
