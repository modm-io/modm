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

#ifndef XPCC__EA_DOG_HPP
#define XPCC__EA_DOG_HPP

#include "st7565.hpp"
#include "st7036.hpp"

namespace xpcc
{
	/**
	 * \brief	EA DOGM132x-5
	 * 
	 * \see		<a href="http://www.lcd-module.de/eng/pdf/grafik/dogm132-5e.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename A0, typename Reset>
	class DogM132 : public St7565<SPI, CS, A0, Reset, 132, 32>
	{
	};
	
	/**
	 * \brief	EA DOGM128x-6
	 * 
	 * \see		<a href="http://www.lcd-module.de/pdf/grafik/dogm128.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename A0, typename Reset>
	class DogM128 : public St7565<SPI, CS, A0, Reset, 128, 64>
	{
	};
	
	/**
	 * \brief	EA DOGL128x-6
	 * 
	 * \see		<a href="http://www.lcd-module.de/pdf/grafik/dogl128-6.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename A0, typename Reset>
	class DogL128 : public St7565<SPI, CS, A0, Reset, 128, 64>
	{
	};
	
	// TODO
	// EA DOGXL160x-7
	// <a href="www.lcd-module.com/eng/pdf/grafik/dogxl160-7e.pdf">Datasheet</a>
	
	/**
	 * \brief	EA DOGM081x-A
	 * 
	 * \todo	Initialization
	 * \see		<a href="http://www.lcd-module.de/eng/pdf/doma/dog-me.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename RS>
	class DogM081 : public St7036<SPI, CS, RS, 8, 1>
	{
	};
	
	/**
	 * \brief	EA DOGM162x-A
	 * 
	 * \todo	Initialization
	 * \see		<a href="http://www.lcd-module.de/eng/pdf/doma/dog-me.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename RS>
	class DogM162 : public St7036<SPI, CS, RS, 16, 2>
	{
	};
	
	/**
	 * \brief	EA DOGM163x-A
	 * 
	 * \todo	Initialization
	 * \see		<a href="http://www.lcd-module.de/eng/pdf/doma/dog-me.pdf">Datasheet</a>
	 * \ingroup	lcd
	 */
	template <typename SPI, typename CS, typename RS>
	class DogM163 : public St7036<SPI, CS, RS, 16, 3>
	{
	};
}

#endif // XPCC__EA_DOG_HPP
