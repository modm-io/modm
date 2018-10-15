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
#define MODM_LU_DECOMPOSITION_HPP

#include "matrix.hpp"
#include "geometry/vector.hpp"

namespace modm
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
	 * \ingroup	modm_math_matrix
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
				Matrix<T, N, N> *p);

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
				Matrix<T, N, BXWIDTH> *xb);

		template <typename T, uint8_t N, uint8_t BXWIDTH>
		static bool
		solve(const Matrix<T, N, N> &A,
				Matrix<T, N, BXWIDTH> *xb);


	private:
		template<typename T, uint8_t OFFSET, uint8_t HEIGHT, uint8_t WIDTH>
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
					Matrix<T, HEIGHT, WIDTH> *l,
					Matrix<T, HEIGHT, BXWIDTH> *bx);

			template<uint8_t BXWIDTH>
			static bool
			solveUxEqualsY(
					Matrix<T, HEIGHT, WIDTH> *u,
					Matrix<T, HEIGHT, BXWIDTH> *bx);

			template<uint8_t BXWIDTH>
			static bool
			solve(
					const Matrix<T, HEIGHT, WIDTH> &l,
					const Matrix<T, HEIGHT, WIDTH> &u,
					Matrix<T, HEIGHT, BXWIDTH> *bx);
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
					Matrix<T, OFFSET, BXWIDTH> *bx);

			template<uint8_t BXWIDTH>
			static bool
			solveLyEqualsB(
					Matrix<T, WIDTH, OFFSET> *l,
					Matrix<T, OFFSET, BXWIDTH> *bx);
		};
	};
}
#include "lu_decomposition_impl.hpp"

#endif // MODM_LU_DECOMPOSITION_HPP
