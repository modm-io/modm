// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC_LPC__PLL_HPP
#define XPCC_LPC__PLL_HPP

#include "../common/common_clock.hpp"

using namespace xpcc::clock;

namespace xpcc
{
	namespace lpc
	{

		template<int64_t InputFrequency, int64_t SystemFrequency>
		class
		Lpc11PllSettings
		{
		private:
			static constexpr int64_t InputMin  = MHz10;
			static constexpr int64_t InputMax  = MHz25;
			// Pll Constant Range
			static constexpr int64_t Mmin =   1;
			static constexpr int64_t Mmax =  32;

//------------------------------- PllM -----------------------------------------
			static constexpr int64_t
			checkM(int64_t m)
			{
				return (calculatePllP(m) >= 0);
			}

			static constexpr int64_t
			calculatePllM(int64_t m = Mmin)
			{
				return checkM(m)? m : ((m < Mmax)? calculatePllM(m + 1) : -1);
			}

//------------------------------- PllP -----------------------------------------
			static constexpr int64_t
			pllP(int64_t m)
			{
				// SystemFrequency = InputFrequency * PllM / PllP
				// => PllP = InputFrequency * PllM * / SystemFrequency
				return InputFrequency * m / SystemFrequency;
			}

			static constexpr int64_t
			checkP(int64_t m, int64_t p)
			{
				// SystemFrequency = InputFrequency * PllM / PllP
				return ((p == 1 || p == 2 || p == 4 || p == 8) &&
						(InputFrequency * m / p) == SystemFrequency);
			}

			static constexpr int64_t
			calculatePllP(int64_t m)
			{
				return checkP(m, pllP(m))? pllP(m) : -1;
			}

			// Internal Pll Constants Representation
			static constexpr int64_t _PllM = calculatePllM(Mmin);		// TODO: remove default
			static constexpr int64_t _PllP = calculatePllP(_PllM);
		public:
			// Pll Constants casted to the correct datatype
			static constexpr uint8_t  PllM = (_PllM > 0)? static_cast<uint8_t>(_PllM)  : 0xff;
			static constexpr uint8_t  PllP = (_PllP > 0)? static_cast<uint8_t>(_PllP)  : 0xff;
			// Resulting Frequencies
			static constexpr int64_t SystemClock = InputFrequency * PllM / PllP;
		private:
			// Static Asserts
			// Check Ranges
			static_assert(PllM >= Mmin && PllM <= Mmax, "MSEL is out of range!");
			static_assert(PllP == 1 || PllP == 2 || PllP == 4 || PllP == 8,
					"PSEL is an invalid value (possible values are 1,2,4,8)!");
			// Check that InputFrequency is between 10 and 25 MHz
			static_assert(InputFrequency >= InputMin && InputFrequency <= InputMax,
				"InputFrequency needs to be between 10MHz and 25MHz! "
				"Please consult your LPC11 Reference Manual.");
			// Check if desired clock frequency is met
			static_assert(SystemClock == SystemFrequency,
				"Desired Output Frequency could not be met."
				"Please consult your LPC11 Reference Manual.");
		};
	}
}

#endif	//  XPCC_LPC__PLL_HPP
