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
#ifndef XPCC_LOG_STYLE_WRAPPER__HPP
#define XPCC_LOG_STYLE_WRAPPER__HPP

#include <xpcc/communication/io/iodevice.hpp>
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
				StyleWrapper( STYLE style );

				virtual
				~StyleWrapper();

				virtual void
				put(char c);

				virtual void
				put(const char* str);

				virtual void
				flush();

				virtual bool
				get(char&);

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
xpcc::log::StyleWrapper<STYLE>::StyleWrapper( STYLE style ) :
	style( style )
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
xpcc::log::StyleWrapper<STYLE>::put( char c )
{
	this->style.put( c );
}

// -----------------------------------------------------------------------------

template < typename STYLE >
void
xpcc::log::StyleWrapper<STYLE>::put( const char* s )
{
	this->style.put( s );
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
xpcc::log::StyleWrapper<STYLE>::get(char& c)
{
	(void) c;
	return false;
}


#endif // XPCC_LOG_STYLE_WRAPPER__HPP
