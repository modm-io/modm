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
#include "logger.hpp"

#include <sys/sysinfo.h>
#include <iomanip>
// -----------------------------------------------------------------------------

Logger::Logger() {
	// Assign loglevel names to ids
	this->logLevelIdNameMap_[Logger::DEBUG]		=	"DEBUG";
	this->logLevelIdNameMap_[Logger::INFO]		=	"INFO";
	this->logLevelIdNameMap_[Logger::WARNING]	=	"WARNING";
	this->logLevelIdNameMap_[Logger::ERROR]		=	"ERROR";
	this->logLevelIdNameMap_[Logger::FATAL]		=	"FATAL";
	
	// Create the default Output-Writer
	this->defaultOutputWriter_	= new ConsoleOutputWriter("DefaultConsoleOutputWriter");

	// Set output writer to default output writer (call of public method allowed,
	// because constructor is private --> no FBC)
	this->setOutputWriter( *this->defaultOutputWriter_ );
};

// -----------------------------------------------------------------------------
Logger::~Logger() {
	this->outputWriter_->getOutputStream() << std::endl;
	// Delete the default output-Writer
	delete this->defaultOutputWriter_;
}
// -----------------------------------------------------------------------------

void
Logger::setDefaultOutputWriter() {
	this->outputWriter_ = this->defaultOutputWriter_;
};

// -----------------------------------------------------------------------------

void
Logger::setOutputWriter(AbstractOutputWriter& outputWriter) {
	this->outputWriter_ = &outputWriter;

	// DEBUG
	this->setMessageHeader( "logger",
							"setOutputWriter",
							INFO);

	this->outputWriter_->getOutputStream()	<< "Output-Writer set to: '"
											<< this->outputWriter_->getIdentifier()
											<< "'"
											<< std::flush;
};

// -----------------------------------------------------------------------------

void
Logger::setMessageHeader(	const std::string & loggerLocationString,
							const std::string & messageLocationString,
							const Logger::PriorityLevel & messagePriorityLevel) {


	// The output format of the message header is set here!!!!! The used
	// OutputWriter is not responsible for the format of the log message!
	AbstractOutputWriter::TColors color;
	if ( messagePriorityLevel <= DEBUG ) {
		color = AbstractOutputWriter::WHITE;
	}
	else if ( messagePriorityLevel <= DEBUG ) {
		color = AbstractOutputWriter::WHITE;
	}
	else if ( messagePriorityLevel <= INFO ) {
		color = AbstractOutputWriter::GREEN;
	}
	else if ( messagePriorityLevel <= WARNING ) {
		color = AbstractOutputWriter::YELLOW;
	}
	else  {
		color = AbstractOutputWriter::RED;
	}

	struct sysinfo si;
	sysinfo(&si);
	if(sysinfo(&si) == 0) {
		//int precision = std::cout.precision();

		this->outputWriter_->getOutputStream()
				<< "\n"
				<< "[" << si.uptime << "] ";
	}
	else {
		this->outputWriter_->getOutputStream()
				<< "\n";
	}

	this->outputWriter_->getOutputStream(color)	<< std::setw(8)
												<< std::left
												<< this->logLevelIdNameMap_[messagePriorityLevel];
	this->outputWriter_->getOutputStream()	<< ": "
											<< std::setw(15)
											<< std::left
											<< loggerLocationString
											<< " > "
											<< std::setw(15)
											<< std::left
											<< messageLocationString
											<< " > "
											<< std::flush;
};

// -----------------------------------------------------------------------------

Logger&
Logger::instance() {
	static Logger loggerInstance;
	return loggerInstance;
};

// -----------------------------------------------------------------------------

