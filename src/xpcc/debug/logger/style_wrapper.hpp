/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LOG_STYLE_WRAPPER__HPP
#define XPCC_LOG_STYLE_WRAPPER__HPP

#include <xpcc/io/iodevice.hpp>
#include "style.hpp"

namespace xpcc
{
	namespace log
	{
		/**
		 * \brief 	Wrapper to use the Style as a IODevice in the Logger
		 *
		 * \ingroup logger
		 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
		 */
		template < typename STYLE >
		class StyleWrapper : public IODevice
		{
		public :
			StyleWrapper( STYLE inStyle );

			virtual
			~StyleWrapper();

			virtual void
			write(char c);

			virtual void
			write(const char* str);

			virtual void
			flush();

			virtual bool
			read(char&);

		private :
			StyleWrapper( const StyleWrapper& );

			StyleWrapper&
			operator=( const StyleWrapper& );

			STYLE style;
		};
	}
}

// -----------------------------------------------------------------------------

template < typename STYLE >
xpcc::log::StyleWrapper<STYLE>::StyleWrapper( STYLE inStyle ) :
	style( inStyle )
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
xpcc::log::StyleWrapper<STYLE>::~StyleWrapper()
{
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::StyleWrapper<STYLE>::write( char c )
{
	this->style.write( c );
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::StyleWrapper<STYLE>::write( const char* s )
{
	this->style.write( s );
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::StyleWrapper<STYLE>::flush()
{
	this->style.flush( );
}

// -----------------------------------------------------------------------------

template < typename STYLE >
bool
xpcc::log::StyleWrapper<STYLE>::read(char& c)
{
	(void) c;
	return false;
}


#endif // XPCC_LOG_STYLE_WRAPPER__HPP
