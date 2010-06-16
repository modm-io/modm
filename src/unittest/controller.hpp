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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	UNITTEST__CONTROLLER_HPP
#define	UNITTEST__CONTROLLER_HPP

#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/general/accessor/flash.hpp>

#include "reporter.hpp"

namespace unittest
{
	/**
	 * \brief	%Controller singleton
	 *
	 * Used to forward information from the test suites to the reporters.
	 * All macros form 'harness.hpp' use this class to determine the
	 * active reporter.
	 *
	 * \author	Fabian Greif 
	 * \ingroup	unittest
	 */
	class Controller
	{
	public:
		/// Get instance
		static inline Controller&
		instance() {
			static Controller controller;
			return controller;
		}
		
		/// Set a new reporter
		void
		setReporter(Reporter& reporter);
		
		/// Get currently active reporter
		Reporter&
		getReporter() const;
		
		/**
		 * \brief	Switch to the next test suite
		 *
		 * \param	name	Name of the test suite, used by the reporter
		 *					to generate messages
		 */
		void
		nextTestSuite(xpcc::accessor::Flash<char> name) const;
	
	private:
		Controller();
		
		Controller(const Controller&);
		
		Controller&
		operator =(const Controller&);
		
		Reporter *reporter;		//!< active reporter
	};
}

#endif	// UNITTEST__CONTROLLER_HPP
