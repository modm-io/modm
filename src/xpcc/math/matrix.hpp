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
/**
 * \ingroup		math
 * \defgroup	matrix	Matrix
 * \brief		Common matrix operations
 *
 */

#ifndef XPCC__MATRIX_HPP
#define XPCC__MATRIX_HPP

#include <cmath>
#include <stdint.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
{
	/**
	 * \brief	Class for handling common matrix operations
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/matrix.html">Homepage</a>
	 * 
	 * \ingroup	matrix
	 * \author Niklas Hauser
	 */
	template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
	class Matrix
	{
	public:
		typedef T Type;
		
	public:
		Matrix();
		Matrix(const T *data);
		Matrix(const Matrix<T, WIDTH, HEIGHT> &m);
		
		template<typename U>
		Matrix& operator = (const Matrix<U, WIDTH, HEIGHT> &m);
		
		static const Matrix& identityMatrix();
		static const Matrix& zeroMatrix();
		
		template <uint8_t MW, uint8_t MH>
		static Matrix subMatrix(uint8_t r, uint8_t c, const Matrix<T, MW, MH> &m);
		
		bool operator == (const Matrix &m) const;
		bool operator != (const Matrix &m) const;
		
		const T* operator [] (uint8_t row) const;
		T* operator [] (uint8_t row);
		
		inline uint8_t width() const;
		inline uint8_t height() const;
		
		const T* ptr() const;
		T* ptr();
		
		template<typename U>
		Matrix& copy(const U *data);
		
		Matrix operator + (const Matrix &rhs) const;
		Matrix& operator += (const Matrix &rhs);
		Matrix operator - (const Matrix &rhs) const;
		Matrix& operator -= (const Matrix &rhs);
		Matrix& operator - ();
		
		template<uint8_t _RHSWIDTH>
		Matrix<T, _RHSWIDTH, HEIGHT> operator * (const Matrix<T, _RHSWIDTH, WIDTH> &rhs) const;
		Matrix operator *= (const Matrix<T, WIDTH, HEIGHT> &rhs);
		
		Matrix operator * (const T &rhs) const;
		Matrix& operator *= (const T &rhs);
		
		Matrix operator / (const T &rhs) const;
		Matrix& operator /= (const T &rhs);
		
		Matrix<T, HEIGHT, WIDTH> transposed() const;
		
		//	will work only if the matrix is square
		inline void transpose();
		
#ifdef USE_MATRIXUTILS
		void inverse();
		Matrix inversed() const;
#endif // USE_MATRIXUTILS
		
		bool hasNan() const;
		bool hasInf() const;
		
//		static Matrix deSerialize(const std::string &str);
		
		template<uint8_t MW, uint8_t MH>
		Matrix<T, WIDTH, HEIGHT>& copy(uint8_t r, uint8_t c, const Matrix<T, MW, MH> &m);
		
		Matrix<T, WIDTH, HEIGHT>& copyRow(uint8_t index, const Matrix<T, WIDTH, 1> &m);
		Matrix<T, WIDTH, HEIGHT>& copyCol(uint8_t index, const Matrix<T, 1, HEIGHT> &m);
		
		Matrix<T, WIDTH, 1> row(uint8_t index) const;
		Matrix<T, 1, HEIGHT> col(uint8_t index) const;
		
		Matrix<T, WIDTH, HEIGHT+1> addBackRow(const Matrix<T, WIDTH, 1> &r ) const;
		Matrix<T, WIDTH+1, HEIGHT> addBackCol(const Matrix<T, 1, HEIGHT> &c) const;
		
		Matrix<T, WIDTH, HEIGHT+1> addFrontRow(const Matrix<T, WIDTH, 1> &r ) const;
		Matrix<T, WIDTH+1, HEIGHT> addFrontCol(const Matrix<T, 1, HEIGHT> &c) const;
		
		Matrix<T, WIDTH, HEIGHT+1> addRow(uint8_t index, const Matrix<T, WIDTH, 1> &r ) const;
		Matrix<T, WIDTH+1, HEIGHT> addCol(uint8_t index, const Matrix<T, 1, HEIGHT> &c) const;
		
		Matrix<T, WIDTH-1, HEIGHT> removeBackCol() const;
		Matrix<T, WIDTH-1, HEIGHT> removeFrontCol() const;
		Matrix<T, WIDTH-1, HEIGHT> removeCol(uint8_t index) const;
		
		Matrix<T, WIDTH, HEIGHT-1> removeBackRow() const;
		Matrix<T, WIDTH, HEIGHT-1> removeFrontRow() const;
		Matrix<T, WIDTH, HEIGHT-1> removeRow(uint8_t index) const;
		
	public:
		T element[WIDTH*HEIGHT];
		
	private:
		size_t size() const;
		uint8_t count() const;
	};
	
	template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
	IOStream&
	operator << (IOStream&, const Matrix<T, WIDTH, HEIGHT>&);
	
	typedef Matrix<float, 1, 1> Matrix1f;
	typedef Matrix<float, 2, 2> Matrix2f;
	typedef Matrix<float, 3, 3> Matrix3f;
	typedef Matrix<float, 4, 4> Matrix4f;
}

#include "matrix_impl.hpp"

#endif	// XPCC__MATRIX_HPP
