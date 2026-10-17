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

#ifndef MODM_MATRIX_HPP
#define MODM_MATRIX_HPP

#include <algorithm>
#include <cmath>
#include <string.h>		// for memset()
#include <stdint.h>

#include <modm/io/iostream.hpp>
#include <modm/math/matrix.hpp>

namespace modm
{
	/// @ingroup	modm_math_matrix
	/// @{

	/**
	 * \brief	Class for handling common matrix operations
	 *
	 * Having the width and height as template parameters has several
	 * advantages over the tradition dynamic matrix class:
	 *
	 * - The compiler knows how many elements you have in your matrix and can
	 *   unroll and optimize loops
	 * - You can ensure that you are not doing operations on matrices with
	 *   incompatible sizes (multiplication for example). The compiler will
	 *   tell you at compile time if you do.
	 * - When you receive a matrix as a function parameter, you don't need to
	 *   check that it is what you expect it to be. For example, if your
	 *   function expects a 4x4 matrix, you'll ask for a Matrix and you are
	 *   guaranteed to get what you asked for.
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/matrix.html">Homepage</a>
	 *
	 * \tparam	ROWS		Number of rows
	 * \tparam	COLUMNS		Number of columns
	 *
	 * \author	Niklas Hauser
	 * \author	Fabian Greif
	 */
	template<typename T, uint8_t ROWS, uint8_t COLUMNS>
	class Matrix
	{
	public:
		/**
		 * \brief	Default Constructor
		 *
		 * Creates a Matrix with uninitialized elements. Use zeroMatrix() to
		 * create a matrix with all elements set to zero.
		 */
		Matrix() = default;

		/**
		 * \brief	Create a matrix from an array
		 *
		 * Example:
		 * \code
		 * const int16_t m[6] = {
		 *     1, 2,
		 *     3, 4,
		 *     5, 6,
		 * };
		 *
		 * modm::Matrix<int16_t, 3, 2> a(m);
		 * \endcode
		 */
		constexpr Matrix(const T *data);

		/**
		 * \brief	Create a matrix from an initializer list
		 *
		 * Example:
		 * \code
		 * modm::Matrix<int16_t, 3, 2> a{1,2,3,4,5,6};
		 * \endcode
		 */
		template<typename... U>
		explicit constexpr Matrix(U... data) requires (std::is_convertible_v<U, T> && ...);

		/**
		 * \brief	Get a zero matrix
		 *
		 * Creates an internal zero matrix and returns a reference to
		 * that internal matrix.
		 */
		static constexpr const Matrix&
		zeroMatrix();

		/**
		 * \brief	Get a identity matrix
		 *
		 * Creates an internal identity matrix and returns a reference to
		 * that internal matrix.
		 */
		static constexpr const Matrix&
		identityMatrix();

		/**
		 * \brief	Create a new sub matrix
		 *
		 */
		template <uint8_t MR, uint8_t MC>
		constexpr Matrix<T, MR, MC>
		subMatrix(uint8_t row, uint8_t column) const;

		constexpr bool operator == (const Matrix &m) const;

		constexpr const T*
		operator [] (uint8_t row) const;

		constexpr T*
		operator [] (uint8_t row);

		constexpr uint8_t
		getNumberOfRows() const;

		constexpr uint8_t
		getNumberOfColumns() const;

		constexpr Matrix<T, 1, COLUMNS>
		getRow(uint8_t index) const;

		constexpr Matrix<T, ROWS, 1>
		getColumn(uint8_t index) const;

		// TODO remove these?
		constexpr const T* ptr() const;
		constexpr T* ptr();

		constexpr Matrix operator - ();
		constexpr Matrix operator + (const Matrix &rhs) const;
		constexpr Matrix operator - (const Matrix &rhs) const;
		constexpr Matrix operator * (const T &rhs) const;		///< Scalar multiplication
		constexpr Matrix operator / (const T &rhs) const;		///< Scalar division

		constexpr Matrix& operator += (const Matrix &rhs);
		constexpr Matrix& operator -= (const Matrix &rhs);
		constexpr Matrix& operator *= (const T &rhs);			///< Scalar multiplication
		constexpr Matrix& operator /= (const T &rhs);			///< Scalar division

		/// Matrix multiplication with matrices with the same size
		constexpr Matrix operator *= (const Matrix &rhs);

		/// Matrix multiplication with different size matrices
		template<uint8_t RHSCOL>
		constexpr Matrix<T, ROWS, RHSCOL>
		operator * (const Matrix<T, COLUMNS, RHSCOL> &rhs) const;

		constexpr Matrix<T, COLUMNS, ROWS>
		asTransposed() const;

		/**
		 * \brief	Transpose the matrix
		 *
		 * \warning	Will only work if the matrix is square!
		 */
		constexpr void
		transpose();

		/**
		 * \brief	Calculate the determinant
		 *
		 * Uses modm::determinant(*this);
		 */
		constexpr T
		determinant() const;

		// TODO Implement these
		/*void
		inverse();

		Matrix
		asInversed() const;*/

		constexpr bool hasNan() const;
		constexpr bool hasInf() const;

		/// Fill the matrix with the values in \p data
		template<typename U>
		constexpr Matrix&
		replace(const U *data);

		///
		template<uint8_t MW, uint8_t MH>
		constexpr Matrix&
		replace(uint8_t row, uint8_t column, const Matrix<T, MW, MH> &m);

		constexpr Matrix&
		replaceRow(uint8_t index, const Matrix<T, 1, COLUMNS> &m);

		constexpr Matrix&
		replaceColumn(uint8_t index, const Matrix<T, ROWS, 1> &m);


		constexpr Matrix<T, ROWS, COLUMNS+1>
		addColumn(uint8_t index, const Matrix<T, ROWS, 1> &c) const;

		constexpr Matrix<T, ROWS+1, COLUMNS>
		addRow(uint8_t index, const Matrix<T, 1, COLUMNS> &r) const;


		constexpr Matrix<T, ROWS, COLUMNS-1>
		removeColumn(uint8_t index) const;

		constexpr Matrix<T, ROWS-1, COLUMNS>
		removeRow(uint8_t index) const;

	public:
		T element[ROWS * COLUMNS];

	private:
		/// Size of the Matrix in Bytes
		constexpr size_t
		getSize() const;

		/// Number of elements in the Matrix (rows * columns)
		constexpr uint8_t
		getNumberOfElements() const;
	};

	template<typename T, uint8_t WIDTH, uint8_t HEIGHT>
	IOStream&
	operator << (IOStream&, const Matrix<T, WIDTH, HEIGHT>&);

	using Matrix1f = Matrix<float, 1, 1>;
	using Matrix2f = Matrix<float, 2, 2>;
	using Matrix3f = Matrix<float, 3, 3>;
	using Matrix4f = Matrix<float, 4, 4>;

	// ------------------------------------------------------------------------
	/// @cond
	template<typename T>
	constexpr T
	determinant(const modm::Matrix<T, 1, 1> &m);

	template<typename T>
	constexpr T
	determinant(const modm::Matrix<T, 2, 2> &m);
	/// @endcond

	/**
	 * \brief	Calculate the determinant
	 *
	 * \param	m	Matrix
	 * \ingroup	modm_math_matrix
	 */
	template<typename T, uint8_t N>
	constexpr T
	determinant(const modm::Matrix<T, N, N> &m);
	/// @}
}

#include "matrix_impl.hpp"

#endif	// MODM_MATRIX_HPP
