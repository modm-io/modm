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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MATRIX_HPP
	#error	"Don't include this file directly, use 'matrix.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::Matrix()
{
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::Matrix(const T *data)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] = data[i];
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT> 
xpcc::Matrix<T, WIDTH, HEIGHT>::Matrix(const xpcc::Matrix<T, WIDTH, HEIGHT> &m)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] = m.element[i];
	}
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT> template<typename U>
xpcc::Matrix<T, WIDTH, HEIGHT>& 
xpcc::Matrix<T, WIDTH, HEIGHT>::operator = (const xpcc::Matrix<U, WIDTH, HEIGHT> &m)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] = m.element[i];
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
const xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::identityMatrix()
{
	static xpcc::Matrix<T, WIDTH, HEIGHT> sIdentityMatrix = zeroMatrix();
	static bool hasIdentityMatrix = false;
	if (!hasIdentityMatrix)
	{
		if (WIDTH < HEIGHT)
		{
			for (uint_fast8_t i = 0; i < WIDTH; ++i) {
				sIdentityMatrix[i][i] = 1;
			}
		}
		else
		{
			for (uint_fast8_t i = 0; i < HEIGHT; ++i) {
				sIdentityMatrix[i][i] = 1;
			}
		}
		
		hasIdentityMatrix = true;
	}
	
	return sIdentityMatrix;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
const xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::zeroMatrix()
{
	static xpcc::Matrix<T, WIDTH, HEIGHT> sZeroMatrix;
	static bool hasZeroMatrix = false;
	if (!hasZeroMatrix)
	{
		memset(sZeroMatrix.ptr(), 0, sZeroMatrix.size());
		hasZeroMatrix = true;
	}
	
	return sZeroMatrix;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::Matrix<T, WIDTH, HEIGHT>::operator == (const xpcc::Matrix<T, WIDTH, HEIGHT> &m) const
{
	return memcmp(element, m.element, getSize()) == 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::Matrix<T, WIDTH, HEIGHT>::operator != (const xpcc::Matrix<T, WIDTH, HEIGHT> &m) const
{
	return memcmp(element, m.element, getSize()) != 0;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
const T*
xpcc::Matrix<T, WIDTH, HEIGHT>::operator [] (uint8_t row) const
{
	return &element[row * WIDTH];
}

template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
T*
xpcc::Matrix<T, WIDTH, HEIGHT>::operator [] (uint8_t row)
{
	return &element[row * WIDTH];
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
uint8_t
xpcc::Matrix<T, WIDTH, HEIGHT>::width() const	
{
	return WIDTH;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
uint8_t
xpcc::Matrix<T, WIDTH, HEIGHT>::height() const	
{
	return HEIGHT;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
const T*
xpcc::Matrix<T, WIDTH, HEIGHT>::ptr() const
{
	return element;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
T*
xpcc::Matrix<T, WIDTH, HEIGHT>::ptr()
{
	return element;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT> template<typename U>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::copy(const U *data)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] = data[i];
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator + (const xpcc::Matrix<T, WIDTH, HEIGHT> &rhs) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT> m;
	
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		m.element[i] = element[i] + rhs.element[i];
	}
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::operator += (const xpcc::Matrix<T, WIDTH, HEIGHT> &rhs)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] += rhs.element[i];
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator - (const xpcc::Matrix<T, WIDTH, HEIGHT> &rhs) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT> m;
	
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		m.element[i] = element[i] - rhs.element[i];
	}
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::operator -= (const xpcc::Matrix<T, WIDTH, HEIGHT> &rhs)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] -= rhs.element[i];
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
//
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::operator - ()
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] = -element[i];
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT> template<uint8_t RHSWIDTH>
xpcc::Matrix<T, RHSWIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator * (const xpcc::Matrix<T, RHSWIDTH, WIDTH> &rhs) const
{
	xpcc::Matrix<T, RHSWIDTH, HEIGHT> m;
	
    for (uint_fast8_t j = 0; j < HEIGHT; ++j)
    {
    	for (uint_fast8_t i = 0; i < RHSWIDTH; ++i)
        {
			m[j][i] = element[j * WIDTH] * rhs[0][i];
        	for (uint_fast8_t x = 1; x < WIDTH; ++x)
        	{
				m[j][i] += element[j * WIDTH + x] * rhs[x][i];
			}
		}
	}
    return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator *= (const xpcc::Matrix<T, WIDTH, HEIGHT> &rhs)
{
	(*this) = (*this) * rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator * (const T &rhs) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT> m;
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		m.element[i] = element[i] * rhs;
	}
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::operator *= (const T &rhs)
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] *= rhs;
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::operator / (const T &rhs) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT> m;
	
	float oneOverRhs = 1.0f / rhs;
	
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		m.element[i] = element[i] * oneOverRhs;
	}
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::operator /= (const T &rhs)
{
	float oneOverRhs = 1.0f / rhs;
	
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		element[i] *= oneOverRhs;
	}
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
void
xpcc::Matrix<T, WIDTH, HEIGHT>::transpose()
{
	*this = asTransposed();
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, HEIGHT, WIDTH>
xpcc::Matrix<T, WIDTH, HEIGHT>::asTransposed() const
{
	xpcc::Matrix<T, HEIGHT, WIDTH> m;
	
	for (uint_fast8_t j = 0; j < HEIGHT; ++j) {
		for (uint_fast8_t i = 0; i < WIDTH; ++i) {
			m.element[i * HEIGHT + j] = element[j * WIDTH + i];
		}
	}
	
	return m;
}

// ----------------------------------------------------------------------------
/*
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::deSerialize(const std::string &str)
{
	char buf[2048];
	strcpy(buf, str.c_str());
	xpcc::Matrix<T, WIDTH, HEIGHT> m = xpcc::Matrix<T, WIDTH, HEIGHT>::zeroMatrix();
	
	char *value;
	value = strtok(buf, " \t\r\n{,}");
	
	uint8_t i = 0;
	while (value && i < WIDTH*HEIGHT)
	{
		double v;
		sscanf(value, "%lf", &v);
		m.element[i++] = v;
		value = strtok(NULL, " \t\r\n{,}");
	}
	
	return m;
}*/

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::Matrix<T, WIDTH, HEIGHT>::hasNan() const
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		if (isnan(element[i])) {
			return true;
		}
	}
	
	return false;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
bool
xpcc::Matrix<T, WIDTH, HEIGHT>::hasInf() const
{
	for (uint_fast8_t i = 0; i < getNumberOfElements(); ++i) {
		if (isinf(element[i])) {
			return true;
		}
	}
	
	return false;
}
 
// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
operator * (int8_t lhs, const xpcc::Matrix<T, WIDTH, HEIGHT> &m)
{
	return m * lhs;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
operator * (float lhs, const xpcc::Matrix<T, WIDTH, HEIGHT> &m)
{
	return m * lhs;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t N>
T
xpcc::determinant(const xpcc::Matrix<T, N, N> &m)
{
	// not the most efficient way, but should work for now...
	T determinant = 0;
	uint8_t factor = 1;
	for (uint_fast8_t i = 0; i < N; ++i)
	{
		T coeff = m[0][i];
		xpcc::Matrix<T, N-1, N-1> subM;
		
		for (uint_fast8_t x = 0; x < i; ++x) {
			for (uint_fast8_t y = 1; y < N; ++y) {
				subM[y-1][x] = m[y][x];
			}
		}
		
		for (uint_fast8_t x = i+1; x < N; ++x) {
			for (uint_fast8_t y = 1; y < N; ++y) {
				subM[y-1][x-1] = m[y][x];
			}
		}
		
		determinant += coeff * factor * Determinant(subM);
		factor *= -1;
	}
	
	return determinant;
}

template<typename T>
T
xpcc::determinant(const xpcc::Matrix<T, 1, 1> &m)
{
	return m[0][0];
}

#ifdef USE_MATRIXUTILS
// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
void
xpcc::Matrix<T, WIDTH, HEIGHT>::inverse()
{
	*this = inversed();
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::inversed() const
{
	xpcc::Matrix<T, WIDTH, HEIGHT> inverse;
	xpcc::MatrixUtils::luInverse(*this, &inverse, true);
	return inverse;
}
#endif // USE_MATRIXUTILS

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
size_t
xpcc::Matrix<T, WIDTH, HEIGHT>::getSize() const
{
	return getNumberOfElements() * sizeof(T);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
uint8_t
xpcc::Matrix<T, WIDTH, HEIGHT>::getNumberOfElements() const
{
	return WIDTH * HEIGHT;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
template <uint8_t MW, uint8_t MH>
xpcc::Matrix<T, WIDTH, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::subMatrix(uint8_t r, uint8_t c, const xpcc::Matrix<T, MW, MH> &m)
{
//	XPCC__STATIC_ASSERT(WIDTH + c <= MW);
//	XPCC__STATIC_ASSERT(HEIGHT + r <= MH);
	
	xpcc::Matrix<T, WIDTH, HEIGHT> sub;
	for (uint_fast8_t j = 0; j < HEIGHT; ++j) {
		for (uint_fast8_t i = 0; i < WIDTH; ++i) {
			sub[j][i] = m[r+j][c+i];
		}
	}
	
	return sub;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
template <uint8_t MW, uint8_t MH>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::copy(uint8_t r, uint8_t c, const xpcc::Matrix<T, MW, MH> &m)
{
	for (uint_fast8_t j = 0; j < MH && (j+r) < HEIGHT; ++j) {
		for (uint_fast8_t i = 0; i < MW && (i+c) < WIDTH; ++i) {
			element[(j+r)*WIDTH+(i+c)] = m[j][i];
		}
	}
	
	return *this;
}


// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::copyRow(uint8_t index, const xpcc::Matrix<T, WIDTH, 1> &m)
{
	return copy(index, 0, m);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>&
xpcc::Matrix<T, WIDTH, HEIGHT>::copyCol(uint8_t index, const xpcc::Matrix<T, 1, HEIGHT> &m)
{
	return copy(0, index, m);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, 1>
xpcc::Matrix<T, WIDTH, HEIGHT>::row(uint8_t index) const
{
	return xpcc::Matrix<T, WIDTH, 1>::subMatrix(index, 0, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, 1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::col(uint8_t index) const
{
	return xpcc::Matrix<T, 1, HEIGHT>::subMatrix(0, index, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT+1>
xpcc::Matrix<T, WIDTH, HEIGHT>::addBackRow(const xpcc::Matrix<T, WIDTH, 1> &r ) const
{
	return addRow(HEIGHT, r);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH+1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::addBackCol(const xpcc::Matrix<T, 1, HEIGHT> &c) const
{
	return addCol(WIDTH, c);
}


// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT+1>
xpcc::Matrix<T, WIDTH, HEIGHT>::addFrontRow(const xpcc::Matrix<T, WIDTH, 1> &r ) const
{
	return addRow(0, r);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH+1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::addFrontCol(const xpcc::Matrix<T, 1, HEIGHT> &c) const
{
	return addCol(0, c);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT+1>
xpcc::Matrix<T, WIDTH, HEIGHT>::addRow(uint8_t index, const xpcc::Matrix<T, WIDTH, 1> &r ) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT+1> m;
	uint_fast8_t i = 0, ri = 0;
	
	for (; i < index; ++i)	m.copyRow(ri++, row(i));
	m.copyRow(ri++, r);
	for (; i < index; ++i)	m.copyRow(ri++, row(i));
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH+1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::addCol(uint8_t index, const xpcc::Matrix<T, 1, HEIGHT> &c) const
{
	xpcc::Matrix<T, WIDTH+1, HEIGHT> m;
	uint_fast8_t i = 0, ci = 0;
	
	for (; i < index; ++i)	m.copyCol(ci++, col(i));
	m.copyCol(ci++, c);
	for (; i < index; ++i)	m.copyCol(ci++, col(i));
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT-1>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeBackRow() const
{
	return xpcc::Matrix<T, WIDTH, HEIGHT-1>::subMatrix(0, 0, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH-1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeBackCol() const
{
	return xpcc::Matrix<T, WIDTH-1, HEIGHT>::subMatrix(0, 0, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT-1>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeFrontRow() const
{
	return xpcc::Matrix<T, WIDTH, HEIGHT-1>::subMatrix(1, 0, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH-1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeFrontCol() const
{
	return xpcc::Matrix<T, WIDTH-1, HEIGHT>::subMatrix(0, 1, *this);
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT-1>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeRow(uint8_t index ) const
{
	xpcc::Matrix<T, WIDTH, HEIGHT-1> m;
	uint_fast8_t i = 0, ri = 0;
	
	for (; i < index; ++i)	m.copyRow(ri++, row(i));
	++i; // skip one row
	for (; i < index; ++i)	m.copyRow(ri++, row(i));
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::Matrix<T, WIDTH-1, HEIGHT>
xpcc::Matrix<T, WIDTH, HEIGHT>::removeCol(uint8_t index) const
{
	xpcc::Matrix<T, WIDTH-1, HEIGHT> m;
	uint_fast8_t i = 0, ci = 0;
	
	for (; i < index; ++i)	m.copyCol(ci++, col(i));
	++i; // skip one column
	for (; i < index; ++i)	m.copyCol(ci++, col(i));
	
	return m;
}

// ----------------------------------------------------------------------------
template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& os, const xpcc::Matrix<T, WIDTH, HEIGHT> &m)
{
	os << "{ ";
	
	for (uint_fast8_t j = 0; j < HEIGHT; ++j)
	{
		os << "{ ";
		for (uint_fast8_t i = 0; i < WIDTH; ++i)
		{
			os << m.element[j*WIDTH+i];
			if (i < WIDTH-1)
			{
				os << ", ";
			}
		}
		os << " }";
		if (j < HEIGHT-1)
		{
			os << ", \n\r";
		}
	}
	os << " }";
	return os;
}
