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

#include "log_singleton.hpp"

// -----------------------------------------------------------------------------

xpcc::LogSingleton::LogSingleton() :
	device( 0 ),
	ownDevice( false ),
	level(xpcc::Log::FATAL)
{
};

// -----------------------------------------------------------------------------

xpcc::LogSingleton::~LogSingleton()
{
	// Delete the log device
	if( this->ownDevice ) {
		delete this->device;
	}
}

// -----------------------------------------------------------------------------

xpcc::LogSingleton&
xpcc::LogSingleton::instance()
{
	static LogSingleton loggerInstance;
	return loggerInstance;
};

// -----------------------------------------------------------------------------

void
xpcc::LogSingleton::setLogDevice(LogDevice* device)
{
	if( this->ownDevice ) {
		delete this->device;
	}
	this->device = device;
	this->ownDevice = true;
};

// -----------------------------------------------------------------------------

void
xpcc::LogSingleton::setLogDevice(LogDevice& device)
{
	if( this->ownDevice ) {
		delete this->device;
	}
	this->device = &device;
	this->ownDevice = false;
};

// ----------------------------------------------------------------------------

void
xpcc::LogSingleton::setLevel(Log::Level level)
{
	this->level = level;
}

// -----------------------------------------------------------------------------

void
xpcc::LogSingleton::put(char c)
{
	this->device->put(c);
}

// -----------------------------------------------------------------------------

void
xpcc::LogSingleton::put(const char* s)
{
	this->device->put(s);
}

// -----------------------------------------------------------------------------

void
xpcc::LogSingleton::flush()
{
	this->device->flush();
}

// -----------------------------------------------------------------------------

bool
xpcc::LogSingleton::get(char& value)
{
	(void) value;
	return false;
}
