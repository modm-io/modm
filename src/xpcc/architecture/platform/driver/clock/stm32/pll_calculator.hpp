// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32_PLL_HPP
#define XPCC_STM32_PLL_HPP

#include "../generic/common_clock.hpp"

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
template<int64_t VCOOutputMinimum, int64_t InputFrequency,
			int64_t SystemFrequency, int64_t USBFrequency>
class
Stm32F2F4PllSettings
{
private:
	// Processor Specific Values
	static constexpr int64_t VCOInputMin  = MHz1;
	static constexpr int64_t VCOInputMax  = MHz2;
	static constexpr int64_t VCOOutputMin =  VCOOutputMinimum;
	static constexpr int64_t VCOOutputMax = MHz432;
	// Pll Constant Range
	static constexpr int64_t Mmin =   2;
	static constexpr int64_t Mmax =  63;
	static constexpr int64_t Nmin =  64;
	static constexpr int64_t Nmax = 432;
	static constexpr int64_t Qmin =   2;
	static constexpr int64_t Qmax =  15;


//------------------------------- PllM -----------------------------------------
	static constexpr int64_t
	checkM(int64_t m)
	{
		return ((InputFrequency / m) <= VCOInputMax &&
				(InputFrequency / m) >= VCOInputMin &&
				(calculatePllN(m) >= 0));
	}

	static constexpr int64_t
	calculatePllM(int64_t m = Mmin)
	{
		return checkM(m)? m : ((m < Mmax)? calculatePllM(m + 1) : -1);
	}

//------------------------------- PllN -----------------------------------------
	static constexpr int64_t
	checkN(int64_t m, int64_t n)
	{
		return ((InputFrequency / m * n) <= VCOOutputMax &&
				(InputFrequency / m * n) >= VCOOutputMin &&
				(calculatePllP(m, n) >= 0));// &&
				// (calculatePllQ(m, n) >= 0));
	}

	static constexpr int64_t
	calculatePllN(int64_t m, int64_t n = Nmax)
	{
		return checkN(m, n)? n : ((n > Nmin)? calculatePllN(m, n - 1) : -1);
	}

//------------------------------- PllP -----------------------------------------
	static constexpr int64_t
	pllP(int64_t m, int64_t n)
	{
		// SystemFrequency = InputFrequency / PllM * PllN / PllP
		// => PllP = InputFrequency / PllM * PllN / SystemFrequency
		return InputFrequency * n / m / SystemFrequency;
	}

	static constexpr int64_t
	checkP(int64_t m, int64_t n, int64_t p)
	{
		// SystemFrequency = InputFrequency / PllM * PllN / PllP
		return ((p == 2 || p == 4 || p == 6 || p == 8) &&
				(InputFrequency / m * n / p) == SystemFrequency);
	}

	static constexpr int64_t
	calculatePllP(int64_t m, int64_t n)
	{
		return checkP(m, n, pllP(m, n))? pllP(m, n) : -1;
	}

//------------------------------- PllQ -----------------------------------------
	static constexpr int64_t
	pllQ(int64_t m, int64_t n)
	{
		// USBFrequency = InputFrequency / PllM * PllN / PllQ
		// => PllQ = InputFrequency / PllM * PllN / USBFrequency
		return InputFrequency * n / m / USBFrequency;
	}

	static constexpr int64_t
	checkQ(int64_t m, int64_t n, int64_t q)
	{
		// USBFrequency = InputFrequency / PllM * PllN / PllQ
		return (q >= Qmin && q <= Qmax &&
				(InputFrequency / m * n / q) == USBFrequency);
	}

	static constexpr int64_t
	calculatePllQ(int64_t m, int64_t n)
	{
		return checkQ(m, n, pllQ(m, n))? pllQ(m, n) : -1;
	}


	// Internal Pll Constants Representation
	static constexpr int64_t _PllM = calculatePllM(Mmin);		// TODO: remove default
	static constexpr int64_t _PllN = calculatePllN(_PllM, Nmax);	// TODO: remove default
	static constexpr int64_t _PllP = calculatePllP(_PllM, _PllN);
	static constexpr int64_t _PllQ = calculatePllQ(_PllM, _PllN);
public:
	// Pll Constants casted to the correct datatype
	static constexpr uint8_t  PllM = (_PllM > 0)? static_cast<uint8_t>(_PllM)  : 0xff;
	static constexpr uint16_t PllN = (_PllN > 0)? static_cast<uint16_t>(_PllN) : 0xffff;
	static constexpr uint8_t  PllP = (_PllP > 0)? static_cast<uint8_t>(_PllP)  : 0xff;
	static constexpr uint8_t  PllQ = (_PllQ > 0)? static_cast<uint8_t>(_PllQ)  : 0xff;
	// Resulting Frequencies
	static constexpr int64_t VCOInput    = InputFrequency / PllM;
	static constexpr int64_t VCOOutput   = VCOInput * PllN;
	static constexpr int64_t SystemClock = VCOOutput / PllP;
	static constexpr int64_t USBClock    = VCOOutput / PllQ;
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

template<int64_t InputFrequency, int64_t SystemFrequency,
		 bool FixedDivideBy2>
class
Stm32F3PllSettings
{
private:
	// Pll Constant Range
	static constexpr int64_t Pmin =   1;	///< PreDivider Max
	static constexpr int64_t Pmax =  16;	///< PreDivider Min
	static constexpr int64_t Mmin =   2;	///< Multiplier Max
	static constexpr int64_t Mmax =  16;	///< Multiplier Min
	// Max/Min Pll Output Frequency
	static constexpr int64_t PllOutputMin = MHz8;
	static constexpr int64_t PllOutputMax = MHz72;

	static constexpr int64_t Input = FixedDivideBy2 ? InputFrequency / 2 : InputFrequency;

//------------------------------- PllP(redivider) ------------------------------
	static constexpr int64_t
	checkP(int64_t p)
	{
		return (/* (InputFrequency / p) <= VCOInputMax && */ // TODO:
				/* (InputFrequency / p) >= VCOInputMin && */ // are there any constraints?
				(calculatePllM(p) >= 0));
	}

	static constexpr int64_t
	calculatePllP(int64_t p = Pmin)
	{
		return (FixedDivideBy2) ? (checkP(p)? p : -1) : (checkP(p)? p : ((p < Pmax)? calculatePllP(p + 1) : -1));
	}

//------------------------------- PllM -----------------------------------------
	static constexpr int64_t
	pllM(int64_t p)
	{
		// SystemFrequency = m * InputFrequency / p
		return SystemFrequency * p / Input;
	}

	static constexpr int64_t
	checkM(int64_t p, int64_t m)
	{
		// SystemFrequency = m * InputFrequency / divisor
		return ((m >= Mmin && m <= Mmax) and (Input * m / p) == SystemFrequency);
	}

	static constexpr int64_t
	calculatePllM(int64_t p)
	{
		return checkM(p, pllM(p))? pllM(p) : -1;
	}

//------------------------------- Divisor --------------------------------------
	static constexpr int64_t
	calculateDivisor(int64_t m)
	{
		// SystemFrequency = m * InputFrequency / divisor
		return SystemFrequency / m / Input;
	}

	/// Internal Pll Constants Representation
	static constexpr int64_t _PllP = calculatePllP(Pmin);
	static constexpr int64_t _PllM = calculatePllM(_PllP);
public:
	/// Pll Constants casted to the correct datatype
	static constexpr uint8_t PllMul = (_PllM > 0)? static_cast<uint8_t>(_PllM)  : 0xff;
	static constexpr uint8_t PllPrediv = (_PllP > 0)? static_cast<uint8_t>(_PllP)  : 0xff;
	static constexpr uint8_t PllPrediv2 = 1;
	/// Resulting Frequencies
	static constexpr int64_t SystemClock = _PllM * Input / _PllP;
	/// USB needs 48 MHz clock input.
	static constexpr bool CanUseUSB =
			((SystemFrequency == MHz48) || (SystemFrequency == MHz72));
	/// USB Clock Divisor. Only valid if CanUseUSB is _true_.
	static constexpr bool DivideUSBClock = (SystemFrequency == MHz72);
private:
	// Static Asserts
	// Check SystemFrequency range (see STM32F302xx Reference Manual p. 101)
	static_assert(SystemFrequency >= PllOutputMin and SystemFrequency <= PllOutputMax,
		"Pll Output Freqeuncy needs to be between 16MHz and 72MHz."
		"Please consult your STM32's Reference Manual page.");
	// Check Ranges
	static_assert(PllPrediv >= Pmin and PllPrediv <= Pmax,
		"PllPREDIV is out of range!");
	static_assert(PllMul >= Mmin and PllMul <= Mmax,
		"PllM is out of range!");
	// Check if desired clock frequency is met
	static_assert(SystemClock == SystemFrequency,
		"Desired Output Frequency could not be met."
		"Please consult your STM32's Reference Manual page.");
};

template<int64_t InputFrequency, int64_t SystemFrequency,
		 bool FixedDivideBy2>
class
Stm32F100PllSettings
{
private:
	// Pll Constant Range
	static constexpr int64_t Pmin =   1;	///< PreDivider Max
	static constexpr int64_t Pmax =  16;	///< PreDivider Min
	static constexpr int64_t Mmin =   2;	///< Multiplier Max
	static constexpr int64_t Mmax =  16;	///< Multiplier Min
	// Max/Min Pll Output Frequency
	static constexpr int64_t PllOutputMin = MHz8;
	static constexpr int64_t PllOutputMax = MHz24;

	static constexpr int64_t Input = FixedDivideBy2 ? InputFrequency / 2 : InputFrequency;

//------------------------------- PllP(redivider) ------------------------------
	static constexpr int64_t
	checkP(int64_t p)
	{
		return (/* (InputFrequency / p) <= VCOInputMax && */ // TODO:
				/* (InputFrequency / p) >= VCOInputMin && */ // are there any constraints?
				(calculatePllM(p) >= 0));
	}

	static constexpr int64_t
	calculatePllP(int64_t p = Pmin)
	{
		return (FixedDivideBy2) ? (checkP(p)? p : -1) : (checkP(p)? p : ((p < Pmax)? calculatePllP(p + 1) : -1));
	}

//------------------------------- PllM -----------------------------------------
	static constexpr int64_t
	pllM(int64_t p)
	{
		// SystemFrequency = m * InputFrequency / p
		return SystemFrequency * p / Input;
	}

	static constexpr int64_t
	checkM(int64_t p, int64_t m)
	{
		// SystemFrequency = m * InputFrequency / divisor
		return ((m >= Mmin and m <= Mmax) and (Input * m / p) == SystemFrequency);
	}

	static constexpr int64_t
	calculatePllM(int64_t p)
	{
		return checkM(p, pllM(p))? pllM(p) : -1;
	}

//------------------------------- Divisor --------------------------------------
	static constexpr int64_t
	calculateDivisor(int64_t m)
	{
		// SystemFrequency = m * InputFrequency / divisor
		return SystemFrequency / m / Input;
	}

	/// Internal Pll Constants Representation
	static constexpr int64_t _PllP = calculatePllP(Pmin);
	static constexpr int64_t _PllM = calculatePllM(_PllP);
public:
	/// Pll Constants casted to the correct datatype
	static constexpr uint8_t PllMul = (_PllM > 0)? static_cast<uint8_t>(_PllM)  : 0xff;
	static constexpr uint8_t PllPrediv = (_PllP > 0)? static_cast<uint8_t>(_PllP)  : 0xff;
	/// Resulting Frequencies
	static constexpr int64_t SystemClock = _PllM * Input / _PllP;
private:
	// Static Asserts
	// Check SystemFrequency range (see STM32F100xx Reference Manual p. 101)
	static_assert(SystemFrequency >= PllOutputMin and SystemFrequency <= PllOutputMax,
		"Pll Output Freqeuncy needs to be between 8MHz and 24MHz."
		"Please consult your STM32's Reference Manual page.");
	// Check Ranges
	static_assert(PllPrediv >= Pmin and PllPrediv <= Pmax,
		"PllPREDIV is out of range!");
	static_assert(PllMul >= Mmin and PllMul <= Mmax,
		"PllM is out of range!");
	// Check if desired clock frequency is met
	static_assert(SystemClock == SystemFrequency,
		"Desired Output Frequency could not be met."
		"Please consult your STM32's Reference Manual page.");
};

}   // namespace stm32

}   // namespace xpcc

#endif	//  XPCC_STM32_PLL_HPP
