/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Kevin Läufer
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FIR_IMPL_HPP
#define MODM_FIR_IMPL_HPP

#include <modm/architecture/utils.hpp>

//#define FIR_DEBUG

#ifdef FIR_DEBUG
	#include <stdio.h>

	//#define FIR_DEBUG_APPEND
	#define FIR_DEBUG_UPDATE

	#ifdef FIR_DEBUG_UPDATE
	#define FIR_DEBUG_SUM(tap, coeff) printf("sum += %.3f * %.3f;\n", tap, coeff)
	#else
	#define FIR_DEBUG_SUM(tap, coeff)
	#endif /*FIR_DEBUG_UPDATE*/
#else
	#define FIR_DEBUG_SUM(tap, coeff)
#endif /*FIR_DEBUG*/



template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor>
modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor>::Fir(const float (&coeff)[N])
{
	setCoefficients(coeff);
	reset();
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor>
void
modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor>::setCoefficients(const float (&coeff)[N])
{
	for(int i = 0; i < N; i++){
		coefficients[i] = static_cast<T>(coeff[i] * ScaleFactor);
	}
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor>
void
modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor>::reset()
{
	// reset all to make debugging easier
	for(int i = 0; i < N + BLOCK_SIZE; i++){
		taps[i] = (T)0;
	}
	taps_index = BLOCK_SIZE;
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor>
void
modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor>::append(const T& input)
{
#ifdef FIR_DEBUG_APPEND
	printf("append(%f);\ttaps_index=%d\n", input, taps_index);
	printf("BEFORE: ");
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n");
#endif // FIR_DEBUG_APPEND
	if(modm_likely(taps_index > 0)){
		taps_index--;
	}
	else{
		for(int i = N + BLOCK_SIZE - 1; i > BLOCK_SIZE; i--)
			taps[i] = taps[i - BLOCK_SIZE - 1];
		taps_index = BLOCK_SIZE;
	}
	taps[taps_index] = input;

#ifdef FIR_DEBUG_APPEND
	printf(" AFTER: ");
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n\n");
#endif // FIR_DEBUG_APPEND
}

// -----------------------------------------------------------------------------
template<typename T, int N, int BLOCK_SIZE, signed int ScaleFactor>
void
modm::filter::Fir<T, N, BLOCK_SIZE, ScaleFactor>::update()
{
#ifdef FIR_DEBUG_UPDATE
	printf("update();\ttaps_index=%d\n", taps_index);
	for(int i = 0; i < N + BLOCK_SIZE; i++)
		if(i >= taps_index && i-taps_index < N) printf("n-%d:%.3f; ", i-taps_index, taps[i]);
		else printf("(%.3f)    ", taps[i]);
	printf("\n");
#endif // FIR_DEBUG_UPDATE

	T sum = (T)0;
	T *tap = taps + taps_index;
	for(int i = 0; i < (N - (N%4)); i++){
		FIR_DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		FIR_DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		FIR_DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i]; i++;
		FIR_DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i];
	}
	for(int i = (N - (N%4)); i < N; i++){
		FIR_DEBUG_SUM(tap[i], coefficients[i]);
		sum += tap[i]*coefficients[i];
	}
	output = sum / ScaleFactor;
#ifdef FIR_DEBUG_UPDATE
	printf("sum=%.3f\n", output);
#endif // FIR_DEBUG_UPDATE
}
#endif // MODM_FIR_IMPL_HPP
