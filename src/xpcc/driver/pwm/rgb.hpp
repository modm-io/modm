// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 * $Id: rgb.hpp 612 2011-10-01 21:27:09Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PWM_RGB_HPP
#define XPCC__PWM_RGB_HPP

#include "led.hpp"

namespace xpcc
{
	namespace pwm
	{
		
		enum Rgba
		{
			RED,
			GREEN,
			BLUE,
			ALPHA
		};
		
		/**
		 * \brief Group three PWM LED into a RGB controller.
		 *
		 * \see Led
		 *
		 * \author	Niklas Hauser
		 * \ingroup pwm
		 */
		class Rgb
		{
		private:
			Led* redLed;
			Led* greenLed;
			Led* blueLed;
		public:
			/**
			 * \param	red		a PWM Led module of a red color led
			 * \param	green	a PWM Led module of a green color led
			 * \param	blue	a PWM Led module of a blue color led
			 */
			Rgb(Led* red, Led* green, Led* blue);
			
			/// set the normalized brightness
			void
			setValues(float red, float green, float blue);
			
			/// get the normalized brightness of any led
			float
			getValue(Rgba color);
			
			/// \return \c if any led if fading
			bool
			isFading();
			
			/// fade to the normalized brightness within x ms
			void
			fadeTo(uint16_t time, float red, float green, float blue);
			
			/// Must be called periodically, preferably in the main while loop.
			void
			run();
		};
	}
}

#include "rgb_impl.hpp"

#endif	// XPCC__PWM_RGB_HPP
