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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__INTERFACE_HPP
#define XPCC__INTERFACE_HPP

namespace xpcc
{
	/**
	 * \brief	Interface class
	 * 
	 * This class defines no real functionality but acts as a base class for
	 * all classes describing the public interface of some hardware drivers.
	 * 
	 * As we use template parameters to configure drivers for different pin
	 * layouts etc. we have no way of describing the interface expected by
	 * the driver.
	 * This class and its subclasses should bridge this gap in the
	 * documentation as they show the expected interface.
	 * 
	 * Because they don't have any virtual methods and don't implement any
	 * of the methods they don't add any overhead to the system.
	 * 
	 * The user must implement all given methods and therewith shadowing the
	 * methods of the base classes. Otherwise the Compiler will try to use
	 * methods of the base class and this will lead to Linker errors because
	 * of the missing implementation.
	 * 
	 * \ingroup	driver
	 */
	class Interface
	{
	};
}

#endif // XPCC__INTERFACE_HPP
