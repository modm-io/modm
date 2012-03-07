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
 * $Id: lu_decomposition.hpp 606 2011-09-13 17:43:01Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__LU_DECOMPOSITION_HPP
#define XPCC__LU_DECOMPOSITION_HPP

#include "matrix.hpp"
#include "geometry/vector.hpp"

namespace xpcc
{
	// forward declaration
	template <class T, uint8_t ROWS, uint8_t COLUMNS> class Matrix;
	template <class T, uint8_t N> class Vector;

	/**
	 * \brief	Class for decomposing matrices
	 * 
	 * Factorise a matrix A into an L(ower) and U(pper) matrix such that
	 * A = L*U or P*A = L*U where P is a pivot matrix (changes the row order).
	 * 
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 * 
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/ludecomposition.html">Homepage</a>
	 * 
	 * \ingroup	matrix
	 * \author	Niklas Hauser
	 */
	class LUDecomposition
	{
	public:
		template <typename T, uint8_t N>
		static bool
		decompose(const Matrix<T, N, N> &matrix,
				Matrix<T, N, N> *l,
				Matrix<T, N, N> *u);

		template <typename T, uint8_t N>
		static bool
		decompose(const Matrix<T, N, N> &matrix,
				Matrix<T, N, N> *l,
				Matrix<T, N, N> *u,
				Vector<int8_t, N> *p);

		template <typename T, uint8_t N, uint8_t BXWIDTH>
		static bool
		solve(const Matrix<T, N, N> &l,
				const Matrix<T, N, N> &u,
				Matrix<T, BXWIDTH, N> *xb);
		
	private:
		template<typename T, uint8_t OFFSET, uint8_t WIDTH, uint8_t HEIGHT>
		class LUSubDecomposition
		{
		public:
			static bool
			decomposeRecur(T * u, T * l);
			
			static bool
			decompose(T * u, T * l);
			
			static bool
			decomposeRecur(T *u, T *l, int8_t *p);
			
			static bool
			decompose(T *u, T *l, int8_t *p);

			template<uint8_t BXWIDTH>
			static bool
			solveLyEqualsB(
					Matrix<T, WIDTH, HEIGHT> *l,
					Matrix<T, BXWIDTH, HEIGHT> *bx);

			template<uint8_t BXWIDTH>
			static bool
			solveUxEqualsY(
					Matrix<T, WIDTH, HEIGHT> *u,
					Matrix<T, BXWIDTH, HEIGHT> *bx);

			template<uint8_t BXWIDTH>
			static bool
			solve(
					const Matrix<T, WIDTH, HEIGHT> &l, 
					const Matrix<T, WIDTH, HEIGHT> &u,
					Matrix<T, BXWIDTH, HEIGHT> *bx);
		};

		template<typename T, uint8_t HEIGHT>
		class RowOperation
		{
		public:
			static void
			multiply(T *intoRow, const T *row, const T &factor);

			static void
			addRowTimesFactor(
					T *intoRow, 
					const T *srcRow, 
					const T *addRow, 
					const T &timesFactor);
			
			static void
			swap(T *row1, T *row2);
		};

		template<typename T>
		class RowOperation<T, 0>
		{
		public:
			static void
			multiply(T *intoRow, const T *row, const T &factor);

			static void
			addRowTimesFactor(
					T *intoRow, 
					const T *srcRow, 
					const T *addRow, 
					const T &timesFactor);

			static void
			swap(T *row1, T *row2);
		};

		template<typename T, uint8_t OFFSET, uint8_t WIDTH>
		class LUSubDecomposition<T, OFFSET, WIDTH, OFFSET>
		{
		public:
			static bool
			decomposeRecur(T *u, T *l, int8_t *p);
			
			static bool
			decomposeRecur(T *u, T *l);
			
			template<uint8_t BXWIDTH>
			static bool
			solveUxEqualsY(
					Matrix<T, WIDTH, OFFSET> *u, 
					Matrix<T, BXWIDTH, OFFSET> *bx);
			
			template<uint8_t BXWIDTH>
			static bool
			solveLyEqualsB(
					Matrix<T, WIDTH, OFFSET> *l, 
					Matrix<T, BXWIDTH, OFFSET> *bx);
		};
	};
}
#include "lu_decomposition_impl.hpp"

#endif // XPCC__LU_DECOMPOSITION_HPP
