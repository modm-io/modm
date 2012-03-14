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

#ifndef XPCC__LU_DECOMPOSITION_HPP
	#error	"Don't include this file directly, use 'lu_decomposition.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
bool
xpcc::LUDecomposition::decompose(
		const xpcc::Matrix<T, SIZE, SIZE> &matrix, 
		xpcc::Matrix<T, SIZE, SIZE> *l, 
		xpcc::Matrix<T, SIZE, SIZE> *u)
{
	*u = matrix;
	*l = xpcc::Matrix<T, SIZE, SIZE>::identityMatrix();
	
	return LUSubDecomposition<T, 0, SIZE, SIZE>::decomposeRecur(u->ptr(), l->ptr());
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
bool
xpcc::LUDecomposition::decompose(
		const xpcc::Matrix<T, SIZE, SIZE> &matrix, 
		xpcc::Matrix<T, SIZE, SIZE> *l,
		xpcc::Matrix<T, SIZE, SIZE> *u,
		xpcc::Vector<int8_t, SIZE> *p)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		(*p)[i] = i;
	}
	*u = matrix;
	*l = xpcc::Matrix<T, SIZE, SIZE>::identityMatrix();
	
	return LUSubDecomposition<T, 0, SIZE, SIZE>::decomposeRecur(u->ptr(), l->ptr(), p->ptr());
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE, uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::solve(
		const xpcc::Matrix<T, SIZE, SIZE> &l, 
		const xpcc::Matrix<T, SIZE, SIZE> &u,
		xpcc::Matrix<T, BXWIDTH, SIZE> *xb)
{
	return LUSubDecomposition<T, 0, SIZE, SIZE>::solve(l, u, xb);
}

//=============================================================================
// PRIVATE CLASS xpcc::LUDecomposition::RowOperation
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
xpcc::LUDecomposition::RowOperation<T, SIZE>::multiply(T *intoRow, const T *row, const T &factor)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		intoRow[i] = row[i] * factor;
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
xpcc::LUDecomposition::RowOperation<T, SIZE>::addRowTimesFactor(T *intoRow, const T *srcRow, const T *addRow, const T &timesFactor)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		intoRow[i] = srcRow[i] + addRow[i] * timesFactor;
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
xpcc::LUDecomposition::RowOperation<T, SIZE>::swap(T *row1, T *row2)
{
	T tmp[SIZE];
	
	memcpy(tmp, row1, SIZE*sizeof(T));
	memcpy(row1, row2, SIZE*sizeof(T));
	memcpy(row2, tmp, SIZE*sizeof(T));
}

//=============================================================================
// PRIVATE CLASS xpcc::LUDecomposition::RowOperation<T, 0>
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::LUDecomposition::RowOperation<T, 0>::multiply(
		T *intoRow, const T *row, const T &factor)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::LUDecomposition::RowOperation<T, 0>::addRowTimesFactor(
		T *intoRow, const T *srcRow, const T *addRow, const T &timesFactor)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::LUDecomposition::RowOperation<T, 0>::swap(T *row1, T *row2)
{
}

//=============================================================================
// PRIVATE CLASS xpcc::LUDecomposition::LUSubDecomposition
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::decomposeRecur(T * u, T * l)
{
	if (!decompose(u, l))
		return false;
	
	return LUSubDecomposition<T, OFFSET+1, WIDTH, HEIGHT>::decomposeRecur(u, l);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::decompose(T * u, T * l)
{
	const uint8_t width = WIDTH;
	const uint8_t height = HEIGHT;

	// normalize the row
	T factor = u[OFFSET*width + OFFSET];
	l[OFFSET*width + OFFSET] = factor;
			
	u[OFFSET*width + OFFSET] = 1;
	RowOperation<T, WIDTH-OFFSET-1>::multiply(&u[OFFSET*width + OFFSET+1], &u[OFFSET*width + OFFSET+1], T(1.0)/factor);

	for (uint_fast8_t j = OFFSET+1; j < height; ++j)
	{
		factor = u[j*width + OFFSET];
		l[j*width + OFFSET] = factor;
		u[j*width + OFFSET] = 0;
		RowOperation<T, WIDTH-OFFSET-1>::addRowTimesFactor(&u[j*width + OFFSET+1], &u[j*width + OFFSET+1], &u[OFFSET*width + OFFSET+1], -factor);
	}
	
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::decomposeRecur(T *u, T *l, int8_t *p)
{
	if (!decompose(u, l, p))
		return false;

	return LUSubDecomposition<T, OFFSET+1, WIDTH, HEIGHT>::decomposeRecur(u, l, p);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::decompose(T *u, T *l, int8_t *p)
{
	const uint8_t width = WIDTH;
	const uint8_t height = HEIGHT;

	//	swap with a lower row so that we have the highest value factor
	// FIXME: we need to use something other than fabs here
	T max = fabs(u[OFFSET*width + OFFSET]);
	uint8_t maxRow = OFFSET;
	for (uint_fast8_t j = OFFSET+1; j < height; ++j)
	{
		T v = fabs(u[j*width + OFFSET]);
		if (v > max)
		{
			max = v;
			maxRow = j;
		}
	}

	if (maxRow != OFFSET)
	{
		uint16_t temp = p[OFFSET];
		p[OFFSET] = p[maxRow];
		p[maxRow] = temp;
		RowOperation<T, WIDTH-OFFSET>::swap(&u[maxRow*width+OFFSET], &u[OFFSET*width+OFFSET]);
		RowOperation<T, OFFSET>::swap(&l[maxRow*width], &l[OFFSET*width]);

	}

	return decompose(u, l);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT> template<uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::solveLyEqualsB(
		xpcc::Matrix<T, WIDTH, HEIGHT> *l,
		xpcc::Matrix<T, BXWIDTH, HEIGHT> *bx)
{
	T factor = (*l)[OFFSET][OFFSET];
	RowOperation<T, BXWIDTH>::multiply((*bx)[OFFSET], (*bx)[OFFSET], 1.0/factor);

	// make sure there is a row under us
	if (OFFSET >= HEIGHT-1)	{
		return true;
	}
	
	// substract the row so that all lower rows have a 0 at OFFSET
	for (uint_fast8_t j = OFFSET+1; j < HEIGHT; ++j)
	{
		T factor = (*l)[j][OFFSET];
		RowOperation<T, BXWIDTH>::addRowTimesFactor((*bx)[j], (*bx)[j], (*bx)[OFFSET], -factor);
		(*l)[j][OFFSET] = 0;
	}
	// solve the problem for SIZE-1
	LUSubDecomposition<T, OFFSET+1, WIDTH, HEIGHT>::solveLyEqualsB(l, bx);

	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT> template<uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::solveUxEqualsY(
		xpcc::Matrix<T, WIDTH, HEIGHT> *u,
		xpcc::Matrix<T, BXWIDTH, HEIGHT> *bx)
{
	// make sure there is a row under us
	if (OFFSET >= HEIGHT-1)	{
		return true;
	}
	
	// solve the problem for SIZE-1
	LUSubDecomposition<T, OFFSET+1, WIDTH, HEIGHT>::solveUxEqualsY(u, bx);

	// substract the row so that all upper rows have a 0 at OFFSET
	for (uint8_t j = 0; j < OFFSET+1; ++j)
	{
		T factor = (*u)[j][OFFSET+1];
		RowOperation<T, BXWIDTH>::addRowTimesFactor((*bx)[j], (*bx)[j], (*bx)[OFFSET+1], -factor);
		(*u)[j][OFFSET+1] = 0;
	}

	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT> template<uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, HEIGHT>::solve(
		const xpcc::Matrix<T, WIDTH, HEIGHT> &l, 
		const xpcc::Matrix<T, WIDTH, HEIGHT> &u,
		xpcc::Matrix<T, BXWIDTH, HEIGHT> *bx)
{
	// we want to solve Ax = b where A = LU
	// we can write LUx = b
	// subsituing Ux = y, we have:
	//				Ly = b
	//
	// Ux = y is easy to solve, and Ly = b is also easy to solve

	// start solving Ly = b

	xpcc::Matrix<T, WIDTH, HEIGHT> lCopy(l);
	if (!solveLyEqualsB(&lCopy, bx)) {
		return false;
	}

	xpcc::Matrix<T, WIDTH, HEIGHT> uCopy(u);
	if (!solveUxEqualsY(&uCopy, bx)) {
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::decomposeRecur(T *u, T *l, int8_t *p)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::decomposeRecur(T *u, T *l)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::solveUxEqualsY(
		xpcc::Matrix<T, WIDTH, OFFSET> *u,
		xpcc::Matrix<T, BXWIDTH, OFFSET> *bx)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
xpcc::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::solveLyEqualsB(
		xpcc::Matrix<T, WIDTH, OFFSET> *l,
		xpcc::Matrix<T, BXWIDTH, OFFSET> *bx)
{	
	return true;
}

