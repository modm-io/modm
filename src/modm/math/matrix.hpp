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

#pragma once

#include <cmath>
#include <span>
#include <algorithm>
#include <stdint.h>

#include <modm/io/iostream.hpp>

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
	 * - The compiler knows how many ElementCount you have in your matrix and can
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
	template<typename T, std::size_t ROWS, std::size_t COLUMNS>
	class Matrix
	{
	public:
		static constexpr std::size_t RowCount = ROWS;
		static constexpr std::size_t ColumnCount = COLUMNS;
		static constexpr std::size_t ElementCount = RowCount * ColumnCount;

		T element[ElementCount]{};

		/**
		 * \brief	Default Constructor
		 *
		 * Creates a Matrix with uninitialized ElementCount. Use zeroMatrix() to
		 * create a matrix with all ElementCount set to zero.
		 */
		constexpr Matrix() = default;

		/**
		 * \brief	Create a matrix from an array
		 *
		 * Example:
		 * \code
		 *
		 * modm::Matrix<int16_t, 3, 2> a({
		 *     1, 2,
		 *     3, 4,
		 *     5, 6,
		 * });
		 * \endcode
		 */
		constexpr Matrix(std::span<const T, ElementCount> data) {
			std::ranges::copy(data, element);
		}

		/**
		 * \brief	Get a zero matrix
		 *
		 * Creates an internal zero matrix and returns a reference to
		 * that internal matrix.
		 */
		static const Matrix&
		zeroMatrix();

		/**
		 * \brief	Get a identity matrix
		 *
		 * Creates an internal identity matrix and returns a reference to
		 * that internal matrix.
		 */
		static const Matrix&
		identityMatrix();

		/**
		 * \brief	Create a new sub matrix
		 *
		 */
		template <std::size_t MR, std::size_t MC>
		Matrix<T, MR, MC>
		subMatrix(std::size_t row, std::size_t column) const;

		T* operator [] (std::size_t row);
		const T* operator [] (std::size_t row) const;

		std::size_t getNumberOfRows() const
		{ return RowCount; }

		std::size_t getNumberOfColumns() const
		{ return ColumnCount; }

		Matrix<T, 1, COLUMNS>
		getRow(std::size_t index) const;

		Matrix<T, ROWS, 1>
		getColumn(std::size_t index) const;

		[[deprecated("Access public member directly")]]
		T* ptr() { return element; }

		[[deprecated("Access public member directly")]]
		const T* ptr() const { return element;}

		bool operator== (const Matrix &m) const
		{ return std::equal(element, element + ElementCount, m.element); };

		constexpr Matrix operator- ();
		constexpr Matrix operator+ (const Matrix &rhs) const;
		constexpr Matrix operator- (const Matrix &rhs) const;
		constexpr Matrix operator* (T rhs) const;		///< Scalar multiplication
		constexpr Matrix operator/ (T rhs) const;		///< Scalar division

		Matrix& operator+= (const Matrix &rhs);
		Matrix& operator-= (const Matrix &rhs);
		Matrix& operator*= (T rhs);			///< Scalar multiplication
		Matrix& operator/= (T rhs);			///< Scalar division

		/// Matrix multiplication with matrices with the same size
		Matrix operator*= (const Matrix &rhs);

		/// Matrix multiplication with different size matrices
		template<std::size_t RHSCOL>
		constexpr Matrix<T, ROWS, RHSCOL>
		operator * (const Matrix<T, COLUMNS, RHSCOL> &rhs) const;

		Matrix<T, COLUMNS, ROWS>
		asTransposed() const;

		/**
		 * \brief	Transpose the matrix
		 *
		 * \warning	Will only work if the matrix is square!
		 */
		void
		transpose();

		/**
		 * \brief	Calculate the determinant
		 *
		 * Uses modm::determinant(*this);
		 */
		T
		determinant() const;

		// TODO Implement these
		/*void
		inverse();

		Matrix
		asInversed() const;*/

		bool hasNan() const;
		bool hasInf() const;

		/// Fill the matrix with the values in \p data
		template<typename U>
		Matrix&
		replace(const U *data);

		///
		template<std::size_t MW, std::size_t MH>
		Matrix&
		replace(std::size_t row, std::size_t column, const Matrix<T, MW, MH> &m);

		Matrix&
		replaceRow(std::size_t index, const Matrix<T, 1, COLUMNS> &m);

		Matrix&
		replaceColumn(std::size_t index, const Matrix<T, ROWS, 1> &m);


		Matrix<T, ROWS, COLUMNS+1>
		addColumn(std::size_t index, const Matrix<T, ROWS, 1> &c) const;

		Matrix<T, ROWS+1, COLUMNS>
		addRow(std::size_t index, const Matrix<T, 1, COLUMNS> &r) const;


		Matrix<T, ROWS, COLUMNS-1>
		removeColumn(std::size_t index) const;

		Matrix<T, ROWS-1, COLUMNS>
		removeRow(std::size_t index) const;

	private:
		std::size_t getSize() const
		{ return ElementCount * sizeof(T); }
	};

	template<typename T, std::size_t WIDTH, std::size_t HEIGHT>
	IOStream&
	operator << (IOStream&, const Matrix<T, WIDTH, HEIGHT>&);

	using Matrix1f = Matrix<float, 1, 1>;
	using Matrix2f = Matrix<float, 2, 2>;
	using Matrix3f = Matrix<float, 3, 3>;
	using Matrix4f = Matrix<float, 4, 4>;

	// ------------------------------------------------------------------------
	/// @cond
	template<typename T>
	T determinant(const modm::Matrix<T, 1, 1> &m);

	template<typename T>
	T determinant(const modm::Matrix<T, 2, 2> &m);

	/**
	 * \brief	Calculate the determinant
	 *
	 * \param	m	Matrix
	 * \ingroup	modm_math_matrix
	 */
	template<typename T, std::size_t N>
	T determinant(const modm::Matrix<T, N, N> &m);
}

#include "matrix_impl.hpp"