/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef NAMESPACE__FILENAME_HPP
#define NAMESPACE__FILENAME_HPP

namespace my_namespace
{
	/**
	 * \brief 	Brief description
	 *
	 * Detailed description ...
	 * 
	 * \ingroup group
	 */
	class MyClass
	{
	public:
		MyClass();
		
		virtual void
		function(const Type& argument);
		
		bool
		anotherFunction();
		
		inline bool
		simpleInlineFunction() const
		{
			return this->myValue;
		}
	
	private:
		bool myValue;
	};
}

#endif // NAMESPACE_FILENAME_HPP
