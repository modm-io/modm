/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
 * Copyright (c) 2017, Marten Junga
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LU_DECOMPOSITION_HPP
	#error	"Don't include this file directly, use 'lu_decomposition.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
bool
modm::LUDecomposition::decompose(
		const modm::Matrix<T, SIZE, SIZE> &matrix,
		modm::Matrix<T, SIZE, SIZE> *l,
		modm::Matrix<T, SIZE, SIZE> *u)
{
	*u = matrix;
	*l = modm::Matrix<T, SIZE, SIZE>::identityMatrix();

	return LUSubDecomposition<T, 0, SIZE, SIZE>::decomposeRecur(u->ptr(), l->ptr());
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
bool
modm::LUDecomposition::decompose(
		const modm::Matrix<T, SIZE, SIZE> &matrix,
		modm::Matrix<T, SIZE, SIZE> *l,
		modm::Matrix<T, SIZE, SIZE> *u,
		modm::Vector<int8_t, SIZE> *p)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		(*p)[i] = i;
	}
	*u = matrix;
	*l = modm::Matrix<T, SIZE, SIZE>::identityMatrix();

	return LUSubDecomposition<T, 0, SIZE, SIZE>::decomposeRecur(u->ptr(), l->ptr(), p->ptr());
}
// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
bool
modm::LUDecomposition::decompose(
		const modm::Matrix<T, SIZE, SIZE> &matrix,
		modm::Matrix<T, SIZE, SIZE> *l,
		modm::Matrix<T, SIZE, SIZE> *u,
		modm::Matrix<T, SIZE, SIZE> *p)
{
	modm::Vector<int8_t, SIZE> pv;
	if (not(decompose(matrix, l, u, &pv))){
		return false;}
	*p = modm::Matrix<T, SIZE, SIZE>::zeroMatrix();
	for (int i=0; i<SIZE; i++){
	(*p)[i][pv[i]] = 1;}
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE, uint8_t BXWIDTH>
bool
modm::LUDecomposition::solve(
		const modm::Matrix<T, SIZE, SIZE> &l,
		const modm::Matrix<T, SIZE, SIZE> &u,
		modm::Matrix<T, SIZE, BXWIDTH> *xb)
{
	return LUSubDecomposition<T, 0, SIZE, SIZE>::solve(l, u, xb);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE, uint8_t BXWIDTH>
bool
modm::LUDecomposition::solve(
		const modm::Matrix<T, SIZE, SIZE> &A,
		modm::Matrix<T, SIZE, BXWIDTH> *xb)
{
	modm::Matrix<T, SIZE, SIZE> l;
	modm::Matrix<T, SIZE, SIZE> u;
	modm::Matrix<T, SIZE, SIZE> p;
	if(not(decompose(A, &l, &u, &p))){
		return false;
	}
	*xb = (p * (*xb));
	if(not(solve(l, u, xb))){
		return false;}
	return true;
}

//=============================================================================
// PRIVATE CLASS modm::LUDecomposition::RowOperation
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
modm::LUDecomposition::RowOperation<T, SIZE>::multiply(T *intoRow, const T *row, const T &factor)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		intoRow[i] = row[i] * factor;
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
modm::LUDecomposition::RowOperation<T, SIZE>::addRowTimesFactor(T *intoRow, const T *srcRow, const T *addRow, const T &timesFactor)
{
	for (uint_fast8_t i = 0; i < SIZE; ++i)	{
		intoRow[i] = srcRow[i] + addRow[i] * timesFactor;
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t SIZE>
void
modm::LUDecomposition::RowOperation<T, SIZE>::swap(T *row1, T *row2)
{
	T tmp[SIZE];

	memcpy(tmp, row1, SIZE*sizeof(T));
	memcpy(row1, row2, SIZE*sizeof(T));
	memcpy(row2, tmp, SIZE*sizeof(T));
}

//=============================================================================
// PRIVATE CLASS modm::LUDecomposition::RowOperation<T, 0>
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T>
void
modm::LUDecomposition::RowOperation<T, 0>::multiply(
		T * /* intoRow */, const T * /* row */, const T & /* factor */)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::LUDecomposition::RowOperation<T, 0>::addRowTimesFactor(
		T * /* intoRow */, const T * /* srcRow */, const T * /* addRow */, const T & /* timesFactor */)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::LUDecomposition::RowOperation<T, 0>::swap(T * /* row1 */, T * /* row2 */)
{
}

//=============================================================================
// PRIVATE CLASS modm::LUDecomposition::LUSubDecomposition
//=============================================================================

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::decomposeRecur(T * u, T * l)
{
	if (!decompose(u, l))
		return false;

	return LUSubDecomposition<T, OFFSET+1, HEIGHT, WIDTH>::decomposeRecur(u, l);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::decompose(T * u, T * l)
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
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::decomposeRecur(T *u, T *l, int8_t *p)
{
	if (!decompose(u, l, p))
		return false;

	return LUSubDecomposition<T, OFFSET+1, HEIGHT, WIDTH>::decomposeRecur(u, l, p);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::decompose(T *u, T *l, int8_t *p)
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
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::solveLyEqualsB(
		modm::Matrix<T, HEIGHT, WIDTH> *l,
		modm::Matrix<T, HEIGHT, BXWIDTH> *bx)
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
	LUSubDecomposition<T, OFFSET+1, HEIGHT, WIDTH>::solveLyEqualsB(l, bx);

	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::solveUxEqualsY(
		modm::Matrix<T, HEIGHT, WIDTH> *u,
		modm::Matrix<T, HEIGHT, BXWIDTH> *bx)
{
	// make sure there is a row under us
	if (OFFSET >= HEIGHT-1)	{
		return true;
	}

	// solve the problem for SIZE-1
	LUSubDecomposition<T, OFFSET+1, HEIGHT, WIDTH>::solveUxEqualsY(u, bx);

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
template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, HEIGHT, WIDTH>::solve(
		const modm::Matrix<T, HEIGHT, WIDTH> &l,
		const modm::Matrix<T, HEIGHT, WIDTH> &u,
		modm::Matrix<T, HEIGHT, BXWIDTH> *bx)
{
	// we want to solve Ax = b where A = LU
	// we can write LUx = b
	// subsituing Ux = y, we have:
	//				Ly = b
	//
	// Ux = y is easy to solve, and Ly = b is also easy to solve

	// start solving Ly = b

	modm::Matrix<T, HEIGHT, WIDTH> lCopy(l);
	if (!solveLyEqualsB(&lCopy, bx)) {
		return false;
	}

	modm::Matrix<T, HEIGHT, WIDTH> uCopy(u);
	if (!solveUxEqualsY(&uCopy, bx)) {
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::decomposeRecur(T * /* u */, T * /* l */, int8_t * /* p */)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::decomposeRecur(T * /* u */, T * /* l */)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::solveUxEqualsY(
		modm::Matrix<T, WIDTH, OFFSET> * /* u */,
		modm::Matrix<T, OFFSET, BXWIDTH> * /* bx */)
{
	return true;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t OFFSET, uint8_t WIDTH> template<uint8_t BXWIDTH>
bool
modm::LUDecomposition::LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>::solveLyEqualsB(
		modm::Matrix<T, WIDTH, OFFSET> * /* l */,
		modm::Matrix<T, OFFSET, BXWIDTH> * /* bx */)
{
	return true;
}

