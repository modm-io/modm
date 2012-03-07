// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: scripto_narrow.hpp 693 2012-01-14 01:11:14Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FONT__SCRIPTO_NARROW_HPP
#define	XPCC_FONT__SCRIPTO_NARROW_HPP

#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace font
	{
		/**
		 * \brief	Scripto Narrow
		 * 
		 * - preferred width : 3
		 * - height          : 7
		 * - hspace          : 0
		 * - vspace          : 1
		 * - first char      : 32
		 * - last char       : 127
		 * - number of chars : 95
		 * - size in bytes   : 382
		 * 
		 * \ingroup	font
		 */
		EXTERN_FLASH_STORAGE(uint8_t ScriptoNarrow[]);
	}
}

#endif	// XPCC_FONT__SCRIPTO_NARROW_HPP

