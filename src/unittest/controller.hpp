/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	UNITTEST_CONTROLLER_HPP
#define	UNITTEST_CONTROLLER_HPP

#include <modm/io/iostream.hpp>
#include <modm/architecture/driver/accessor/flash.hpp>

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

#endif	// UNITTEST_CONTROLLER_HPP
