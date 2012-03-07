// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
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
 * $Id: reporter.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	UNITTEST__REPORTER_HPP
#define	UNITTEST__REPORTER_HPP

#include <stdint.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/driver/accessor/flash.hpp>

namespace unittest
{
	/**
	 * \brief	%Reporter
	 *
	 * Used to generate the visible output.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	unittest
	 */
	class Reporter
	{
	public:
		/**
		 * \brief	Constructor
		 *
		 * \param	device	IODevice used for printing
		 */
		Reporter(xpcc::IODevice& device);
		
		/**
		 * \brief	Switch to the next test suite
		 * 
		 * \param	name	Name of the test suite
		 */
		void
		nextTestSuite(xpcc::accessor::Flash<char> name);
		
		/**
		 * \brief	Report a passed test
		 * 
		 * Doesn't generate any output, but increments the number of
		 * passed tests
		 */
		void
		reportPass();
		
		/**
		 * \brief	Reported a failed test
		 *
		 * Generates a basic failure message, the returned stream can then
		 * be used to write some more specific information about the failure.
		 */
		xpcc::IOStream&
		reportFailure(unsigned int lineNumber);
		
		/**
		 * \brief	Writes a summary of all the tests
		 * 
		 * Basically the total number of failed and passed tests and then 
		 * 'OK' if there was no failure or 'FAIL' otherwise.
		 */
		void
		printSummary();
		
	private:
		xpcc::IOStream outputStream;
		xpcc::accessor::Flash<char> testName;
		
		int_fast16_t testsPassed;
		int_fast16_t testsFailed;
	};
}

#endif	// UNITTEST__REPORTER_HPP
