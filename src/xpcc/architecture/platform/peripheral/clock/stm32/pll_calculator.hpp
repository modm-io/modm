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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__PLL_HPP
#define XPCC_STM32__PLL_HPP

#include "../common/common_clock.hpp"

using namespace xpcc::clock;

namespace xpcc
{
	namespace stm32
	{

		/*
		 *
		 * For Stm32F2 VCOOutputMinimum needs to be MHz64
		 * For Stm32F4 VCOOutputMinimum needs to be MHz192
		 */
		template<int VCOOutputMinimum, int InputFrequency,
					int SystemFrequency, int USBFrequency>
		class
		Stm32F2F4PllSettings
		{
		private:
			// Processor Specific Values
			static constexpr int VCOInputMin  = MHz1;
			static constexpr int VCOInputMax  = MHz2;
			static constexpr int VCOOutputMin =  VCOOutputMinimum;
			static constexpr int VCOOutputMax = MHz432;
			// Pll Constant Range
			static constexpr int Mmin =   2;
			static constexpr int Mmax =  63;
			static constexpr int Nmin =  64;
			static constexpr int Nmax = 432;
			static constexpr int Qmin =   2;
			static constexpr int Qmax =  15;


//------------------------------- PllM -----------------------------------------
			static constexpr int
			checkM(int m)
			{
				return ((InputFrequency / m) <= VCOInputMax &&
						(InputFrequency / m) >= VCOInputMin &&
						(calculatePllN(m) >= 0));
			}

			static constexpr int
			calculatePllM(int m = Mmin)
			{
				return checkM(m)? m : ((m < Mmax)? calculatePllM(m + 1) : -1);
			}

//------------------------------- PllN -----------------------------------------
			static constexpr int
			checkN(int m, int n)
			{
				return ((InputFrequency / m * n) <= VCOOutputMax &&
						(InputFrequency / m * n) >= VCOOutputMin &&
						(calculatePllP(m, n) >= 0));// &&
						// (calculatePllQ(m, n) >= 0));
			}

			static constexpr int
			calculatePllN(int m, int n = Nmax)
			{
				return checkN(m, n)? n : ((n > Nmin)? calculatePllN(m, n - 1) : -1);
			}

//------------------------------- PllP -----------------------------------------
			static constexpr int
			pllP(int m, int n)
			{
				// SystemFrequency = InputFrequency / PllM * PllN / PllP
				// => PllP = InputFrequency / PllM * PllN / SystemFrequency
				return InputFrequency * n / m / SystemFrequency;
			}

			static constexpr int
			checkP(int m, int n, int p)
			{
				// SystemFrequency = InputFrequency / PllM * PllN / PllP
				return ((p == 2 || p == 4 || p == 6 || p == 8) &&
						(InputFrequency / m * n / p) == SystemFrequency);
			}

			static constexpr int
			calculatePllP(int m, int n)
			{
				return checkP(m, n, pllP(m, n))? pllP(m, n) : -1;
			}

//------------------------------- PllQ -----------------------------------------
			static constexpr int
			pllQ(int m, int n)
			{
				// USBFrequency = InputFrequency / PllM * PllN / PllQ
				// => PllQ = InputFrequency / PllM * PllN / USBFrequency
				return InputFrequency * n / m / USBFrequency;
			}

			static constexpr int
			checkQ(int m, int n, int q)
			{
				// USBFrequency = InputFrequency / PllM * PllN / PllQ
				return (q >= Qmin && q <= Qmax &&
						(InputFrequency / m * n / q) == USBFrequency);
			}

			static constexpr int
			calculatePllQ(int m, int n)
			{
				return checkQ(m, n, pllQ(m, n))? pllQ(m, n) : -1;
			}


			static constexpr int PllMVCOMHz1 = InputFrequency / MHz1;
			static constexpr int PllMVCOMHz2 = InputFrequency / MHz2;
		public:
			// Pll Constants
			static constexpr int PllM = calculatePllM(Mmin);		// TODO: remove default
			static constexpr int PllN = calculatePllN(PllM, Nmax);	// TODO: remove default
			static constexpr int PllP = calculatePllP(PllM, PllN);
			static constexpr int PllQ = calculatePllQ(PllM, PllN);
			// Resulting Frequencies
			static constexpr int VCOInput    = InputFrequency / PllM;
			static constexpr int VCOOutput   = VCOInput * PllN;
			static constexpr int SystemClock = VCOOutput / PllP;
			static constexpr int USBClock    = VCOOutput / PllQ;
		private:
			// Static Asserts
			// Check Ranges
			static_assert(PllM >= Mmin && PllM <= Mmax, "PllM is out of range!");
			static_assert(PllN >= Nmin && PllN <= Nmax, "PllQ is out of range!");
			static_assert(PllQ >= Qmin && PllP <= Qmax, "PllQ is out of range!");
			static_assert(PllP == 2 || PllP == 4 || PllP == 6 || PllP == 8,
							"PllP is an invalid value (possible values are 2,4,6,8)!");
			// Check that VCOInput is between 1 and 2 MHz
			static_assert(VCOInput >= MHz1 && VCOInput <= MHz2,
				"VCOInput Frequency needs to be between 1MHz and 2MHz! "
				"'VCOInput = InputFrequency / PllM' "
				"Probably no good value for PllM could be found. "
				"Please consult your STM32's Reference Manual page.");
			// Check that VCOOutput is between 192 and 432 MHz
			static_assert(VCOOutput >= VCOOutputMin && VCOOutput <= VCOOutputMax,
				"VCOOutput Frequency needs to be in range! "
				"'VCOOutput = VCOInput * PllN' "
				"Probably no good value for PllN could be found. "
				"Please consult your STM32's Reference Manual page.");
			// Check if desired clock frequency is met
			static_assert(SystemClock == SystemFrequency,
				"Desired Output Frequency could not be met."
				"Please consult your STM32's Reference Manual page.");
			// Check if desired usb frequency is met
			static_assert(USBClock == USBFrequency,
				"Desired Output Frequency could not be met."
				"Please consult your STM32's Reference Manual page.");
		};
	}
}

#endif	//  XPCC_STM32__PLL_HPP
