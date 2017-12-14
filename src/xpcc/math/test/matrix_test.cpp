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

#include <xpcc/math/matrix.hpp>

#include "matrix_test.hpp"

void
MatrixTest::testConstruction()
{
	const int16_t m[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	xpcc::Matrix<int16_t, 3, 3> a(m);
	
	TEST_ASSERT_EQUALS(a[0][0], 1);
	TEST_ASSERT_EQUALS(a[0][1], 2);
	TEST_ASSERT_EQUALS(a[0][2], 3);
	TEST_ASSERT_EQUALS(a[1][0], 4);
	TEST_ASSERT_EQUALS(a[1][1], 5);
	TEST_ASSERT_EQUALS(a[1][2], 6);
	TEST_ASSERT_EQUALS(a[2][0], 7);
	TEST_ASSERT_EQUALS(a[2][1], 8);
	TEST_ASSERT_EQUALS(a[2][2], 9);
	
	const int16_t n[] = {
		1, 2,
		3, 4,
		5, 6,
	};
	
	xpcc::Matrix<int16_t, 3, 2> b(n);
	
	TEST_ASSERT_EQUALS(b.getNumberOfRows(), 3);
	TEST_ASSERT_EQUALS(b.getNumberOfColumns(), 2);
	
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 2);
	TEST_ASSERT_EQUALS(b[1][0], 3);
	TEST_ASSERT_EQUALS(b[1][1], 4);
	TEST_ASSERT_EQUALS(b[2][0], 5);
	TEST_ASSERT_EQUALS(b[2][1], 6);
}

void
MatrixTest::testAssign()
{
	xpcc::Matrix<int16_t, 3, 2> a;
	
	a[0][0] = 1;
	a[0][1] = 2;
	a[1][0] = 3;
	a[1][1] = 4;
	a[2][0] = 5;
	a[2][1] = 6;
	
	TEST_ASSERT_EQUALS(a[0][0], 1);
	TEST_ASSERT_EQUALS(a[0][1], 2);
	TEST_ASSERT_EQUALS(a[1][0], 3);
	TEST_ASSERT_EQUALS(a[1][1], 4);
	TEST_ASSERT_EQUALS(a[2][0], 5);
	TEST_ASSERT_EQUALS(a[2][1], 6);
}

void
MatrixTest::testIdentityMatrix()
{
	xpcc::Matrix<int16_t, 3, 3> a = xpcc::Matrix<int16_t, 3, 3>::identityMatrix();
	
	for (uint_fast8_t i = 0; i < 3; ++i)
	{
		for (uint_fast8_t j = 0; j < 3; ++j)
		{
			if (i == j) {
				TEST_ASSERT_EQUALS(a[i][j], 1);
			}
			else {
				TEST_ASSERT_EQUALS(a[i][j], 0);
			}
		}
	}
}

void
MatrixTest::testZeroMatrix()
{
	xpcc::Matrix<int16_t, 3, 3> a = xpcc::Matrix<int16_t, 3, 3>::zeroMatrix();
	
	for (uint_fast8_t i = 0; i < 3; ++i)
	{
		for (uint_fast8_t j = 0; j < 3; ++j)
		{
			TEST_ASSERT_EQUALS(a[i][j], 0);
		}
	}
}

void
MatrixTest::testSubMatrix()
{
	const int16_t m[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	xpcc::Matrix<int16_t, 3, 3> a(m);
	
	xpcc::Matrix<int16_t, 2, 2> b = a.subMatrix<2, 2>(1, 1);
	
	TEST_ASSERT_EQUALS(b[0][0], 5);
	TEST_ASSERT_EQUALS(b[0][1], 6);
	TEST_ASSERT_EQUALS(b[1][0], 8);
	TEST_ASSERT_EQUALS(b[1][1], 9);
	
	xpcc::Matrix<int16_t, 2, 2> c = a.subMatrix<2, 2>(0, 0);
	
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 2);
	TEST_ASSERT_EQUALS(c[1][0], 4);
	TEST_ASSERT_EQUALS(c[1][1], 5);
	
	xpcc::Matrix<int16_t, 3, 2> d = a.subMatrix<3, 2>(0, 1);
	
	TEST_ASSERT_EQUALS(d[0][0], 2);
	TEST_ASSERT_EQUALS(d[0][1], 3);
	TEST_ASSERT_EQUALS(d[1][0], 5);
	TEST_ASSERT_EQUALS(d[1][1], 6);
	TEST_ASSERT_EQUALS(d[2][0], 8);
	TEST_ASSERT_EQUALS(d[2][1], 9);
}

void
MatrixTest::testAccess()
{
	const int16_t m[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	xpcc::Matrix<int16_t, 3, 3> a(m);
	
	xpcc::Matrix<int16_t, 3, 1> b = a.getColumn(1);
	
	TEST_ASSERT_EQUALS(b[0][0], 2);
	TEST_ASSERT_EQUALS(b[1][0], 5);
	TEST_ASSERT_EQUALS(b[2][0], 8);
	
	xpcc::Matrix<int16_t, 1, 3> c = a.getRow(2);
	
	TEST_ASSERT_EQUALS(c[0][0], 7);
	TEST_ASSERT_EQUALS(c[0][1], 8);
	TEST_ASSERT_EQUALS(c[0][2], 9);
}

void
MatrixTest::testCompare()
{
	const int16_t m[] = {
		1, 2,
		3, 4,
	};
	
	xpcc::Matrix<int16_t, 2, 2> a(m);
	xpcc::Matrix<int16_t, 2, 2> b(a);
	
	TEST_ASSERT_TRUE(a == b);
	TEST_ASSERT_FALSE(a != b);
	
	b[1][1] = 5;
	
	TEST_ASSERT_FALSE(a == b);
	TEST_ASSERT_TRUE(a != b);
	
	b[1][1] = 4;
	
	TEST_ASSERT_TRUE(a == b);
	TEST_ASSERT_FALSE(a != b);
}

void
MatrixTest::testArithmetics()
{
	const int16_t m[] = {
		1, 2,
		3, 4,
	};
	
	xpcc::Matrix<int16_t, 2, 2> a(m);
	xpcc::Matrix<int16_t, 2, 2> b(a);
	
	-a;
	TEST_ASSERT_EQUALS(a, b);
	a = -a;
	TEST_ASSERT_EQUALS(a[0][0], -1);
	TEST_ASSERT_EQUALS(a[0][1], -2);
	TEST_ASSERT_EQUALS(a[1][0], -3);
	TEST_ASSERT_EQUALS(a[1][1], -4);
	
	xpcc::Matrix<int16_t, 2, 2> c = b - a;
	TEST_ASSERT_EQUALS(c[0][0], 2);
	TEST_ASSERT_EQUALS(c[0][1], 4);
	TEST_ASSERT_EQUALS(c[1][0], 6);
	TEST_ASSERT_EQUALS(c[1][1], 8);
	
	c = c + a;
	TEST_ASSERT_EQUALS(c, b);
	
	a = c * 2;
	TEST_ASSERT_EQUALS(a[0][0], 2);
	TEST_ASSERT_EQUALS(a[0][1], 4);
	TEST_ASSERT_EQUALS(a[1][0], 6);
	TEST_ASSERT_EQUALS(a[1][1], 8);
	
	c = a / 2;
	TEST_ASSERT_EQUALS(c, b);
	
	c *= 2;
	TEST_ASSERT_EQUALS(c, a);
	
	c /= 2;
	TEST_ASSERT_EQUALS(c, b);
	
	c += b;
	TEST_ASSERT_EQUALS(c, a);
	
	c -= b;
	TEST_ASSERT_EQUALS(c, b);
}

void
MatrixTest::testMatrixMultiplicationInPlace()
{
	const int16_t m[] = {
		1, 2,
		3, 4,
	};
	
	const int16_t n[] = {
		1, 2,
		1, 2,
	};
	
	xpcc::Matrix<int16_t, 2, 2> a(m);
	xpcc::Matrix<int16_t, 2, 2> b(n);
	
	a *= b;
	TEST_ASSERT_EQUALS(a[0][0], 3);
	TEST_ASSERT_EQUALS(a[0][1], 6);
	TEST_ASSERT_EQUALS(a[1][0], 7);
	TEST_ASSERT_EQUALS(a[1][1], 14);
}

void
MatrixTest::testMatrixMultiplication()
{
	const int16_t m[] = {
		1,
		2,
		3,
	};
	
	const xpcc::Matrix<int16_t, 3, 1> a(m);
	xpcc::Matrix<int16_t, 1, 3> b(m);
	
	xpcc::Matrix<int16_t, 3, 3> c = a * b;
	
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 2);
	TEST_ASSERT_EQUALS(c[0][2], 3);
	TEST_ASSERT_EQUALS(c[1][0], 2);
	TEST_ASSERT_EQUALS(c[1][1], 4);
	TEST_ASSERT_EQUALS(c[1][2], 6);
	TEST_ASSERT_EQUALS(c[2][0], 3);
	TEST_ASSERT_EQUALS(c[2][1], 6);
	TEST_ASSERT_EQUALS(c[2][2], 9);
	
	xpcc::Matrix<int16_t, 1, 1> d = b * a;
	
	TEST_ASSERT_EQUALS(d[0][0], 14);
	
	const int16_t n[] = {
		1, 2,
		3, 4,
		5, 6,
	};
	
	xpcc::Matrix<int16_t, 3, 2> e(n);
	xpcc::Matrix<int16_t, 2, 3> f(n);
	
	c = e * f;
	
	TEST_ASSERT_EQUALS(c[0][0], 9);
	TEST_ASSERT_EQUALS(c[0][1], 12);
	TEST_ASSERT_EQUALS(c[0][2], 15);
	TEST_ASSERT_EQUALS(c[1][0], 19);
	TEST_ASSERT_EQUALS(c[1][1], 26);
	TEST_ASSERT_EQUALS(c[1][2], 33);
	TEST_ASSERT_EQUALS(c[2][0], 29);
	TEST_ASSERT_EQUALS(c[2][1], 40);
	TEST_ASSERT_EQUALS(c[2][2], 51);
	
	xpcc::Matrix<int16_t, 2, 2> g = f * e;
	
	TEST_ASSERT_EQUALS(g[0][0], 22);
	TEST_ASSERT_EQUALS(g[0][1], 28);
	TEST_ASSERT_EQUALS(g[1][0], 49);
	TEST_ASSERT_EQUALS(g[1][1], 64);


	// Testing non-square matrices
	// https://www.wolframalpha.com/input/?i=%7B%7B1,2,3%7D,%7B4,5,6%7D,%7B7,8,9%7D%7D*%7B%7B1%7D,%7B2%7D,%7B3%7D%7D
	const int16_t o[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};

	xpcc::Matrix<int16_t, 3, 3> h(o);
	xpcc::Matrix<int16_t, 3, 1> i = h * a;
	TEST_ASSERT_EQUALS(i[0][0], 14);
	TEST_ASSERT_EQUALS(i[1][0], 32);
	TEST_ASSERT_EQUALS(i[2][0], 50);

	// https://www.wolframalpha.com/input/?i=%7B%7B1,2,3%7D,%7B4,5,6%7D,%7B7,8,9%7D%7D*%7B%7B1,2%7D,%7B3,4%7D,%7B5,6%7D%7D
	const int16_t p[] = {
		1, 2,
		3, 4,
		5, 6,
	};

	xpcc::Matrix<int16_t, 3, 2> j(p);
	xpcc::Matrix<int16_t, 3, 2> k = h * j;

	TEST_ASSERT_EQUALS(k[0][0],  22);
	TEST_ASSERT_EQUALS(k[0][1],  28);
	TEST_ASSERT_EQUALS(k[1][0],  49);
	TEST_ASSERT_EQUALS(k[1][1],  64);
	TEST_ASSERT_EQUALS(k[2][0],  76);
	TEST_ASSERT_EQUALS(k[2][1], 100);

	// https://www.wolframalpha.com/input/?i=%7B%7B1,2,3,4,5%7D,%7B6,7,8,9,10%7D%7D*%7B%7B1,2,3%7D,%7B4,5,6%7D,%7B7,8,9%7D,%7B10,11,12%7D,%7B13,14,15%7D%7D
	const int16_t q[] = {
		 1,  2,  3,  4,  5,
		 6,  7,  8,  9, 10,
		11, 12, 13, 14, 15
	};
	xpcc::Matrix<int16_t, 2, 5> l(q);
	xpcc::Matrix<int16_t, 5, 3> ll(q);
	xpcc::Matrix<int16_t, 2, 3> aa = l * ll;

	TEST_ASSERT_EQUALS(aa[0][0], 135);
	TEST_ASSERT_EQUALS(aa[0][1], 150);
	TEST_ASSERT_EQUALS(aa[0][2], 165);
	TEST_ASSERT_EQUALS(aa[1][0], 310);
	TEST_ASSERT_EQUALS(aa[1][1], 350);
	TEST_ASSERT_EQUALS(aa[1][2], 390);
}

void
MatrixTest::testTranspose()
{
	const int16_t m[] = {
		1, 2,
		3, 4,
		5, 6,
	};
	
	xpcc::Matrix<int16_t, 3, 2> a(m);
	xpcc::Matrix<int16_t, 2, 3> b(a.asTransposed());
	
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 3);
	TEST_ASSERT_EQUALS(b[0][2], 5);
	TEST_ASSERT_EQUALS(b[1][0], 2);
	TEST_ASSERT_EQUALS(b[1][1], 4);
	TEST_ASSERT_EQUALS(b[1][2], 6);
	
	const int16_t n[] = {
		1, 2,
		3, 4,
	};
	
	xpcc::Matrix<int16_t, 2, 2> c(n);
	
	c.transpose();
	
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 3);
	TEST_ASSERT_EQUALS(b[1][0], 2);
	TEST_ASSERT_EQUALS(b[1][1], 4);
}

void
MatrixTest::testReplace()
{
	const int16_t m[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	xpcc::Matrix<int16_t, 3, 3> a(m);
	xpcc::Matrix<int16_t, 3, 3> b;
	
	b.replace(m);
	TEST_ASSERT_EQUALS(a, b);
	
	b = xpcc::Matrix<int16_t, 3, 3>::zeroMatrix();
	b.replace(0, 0, a);
	TEST_ASSERT_EQUALS(a, b);
	
	xpcc::Matrix<int16_t, 2, 2> c = a.subMatrix<2, 2>(1, 0);
	
	a.replace(0, 1, c);
	TEST_ASSERT_EQUALS(a[0][0], 1);
	TEST_ASSERT_EQUALS(a[0][1], 4);
	TEST_ASSERT_EQUALS(a[0][2], 5);
	TEST_ASSERT_EQUALS(a[1][0], 4);
	TEST_ASSERT_EQUALS(a[1][1], 7);
	TEST_ASSERT_EQUALS(a[1][2], 8);
	TEST_ASSERT_EQUALS(a[2][0], 7);
	TEST_ASSERT_EQUALS(a[2][1], 8);
	TEST_ASSERT_EQUALS(a[2][2], 9);
	
	const int16_t n[] = { 1, 2 };
	xpcc::Matrix<int16_t, 2, 1> column(n);
	
	c.replaceColumn(1, column);
	
	TEST_ASSERT_EQUALS(c[0][0], 4);
	TEST_ASSERT_EQUALS(c[0][1], 1);
	TEST_ASSERT_EQUALS(c[1][0], 7);
	TEST_ASSERT_EQUALS(c[1][1], 2);
	
	const int16_t o[2] = { 10, 12 };
	xpcc::Matrix<int16_t, 1, 2> row(o);
	
	c.replaceRow(0, row);
	
	TEST_ASSERT_EQUALS(c[0][0], 10);
	TEST_ASSERT_EQUALS(c[0][1], 12);
	TEST_ASSERT_EQUALS(c[1][0], 7);
	TEST_ASSERT_EQUALS(c[1][1], 2);
}

void
MatrixTest::testAddRowColumn()
{
	const int16_t in[] = {
		1, 2,
		3, 4,
	};
	const int16_t in2[] = {
		5, 6,
	};
	
	xpcc::Matrix<int16_t, 2, 2> a(in);
	
	xpcc::Matrix<int16_t, 1, 2> row2(in2);
	xpcc::Matrix<int16_t, 3, 2> b = a.addRow(0, row2);
	TEST_ASSERT_EQUALS(b[0][0], 5);
	TEST_ASSERT_EQUALS(b[0][1], 6);
	TEST_ASSERT_EQUALS(b[1][0], 1);
	TEST_ASSERT_EQUALS(b[1][1], 2);
	TEST_ASSERT_EQUALS(b[2][0], 3);
	TEST_ASSERT_EQUALS(b[2][1], 4);
	
	b = a.addRow(1, row2);
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 2);
	TEST_ASSERT_EQUALS(b[1][0], 5);
	TEST_ASSERT_EQUALS(b[1][1], 6);
	TEST_ASSERT_EQUALS(b[2][0], 3);
	TEST_ASSERT_EQUALS(b[2][1], 4);
	
	b = a.addRow(2, row2);
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 2);
	TEST_ASSERT_EQUALS(b[1][0], 3);
	TEST_ASSERT_EQUALS(b[1][1], 4);
	TEST_ASSERT_EQUALS(b[2][0], 5);
	TEST_ASSERT_EQUALS(b[2][1], 6);
	
	const int16_t in3[] = {
		7, 8, 9,
	};
	
	xpcc::Matrix<int16_t, 3, 1> colum3(in3);
	xpcc::Matrix<int16_t, 3, 3> c = b.addColumn(0, colum3);
	TEST_ASSERT_EQUALS(c[0][0], 7);
	TEST_ASSERT_EQUALS(c[0][1], 1);
	TEST_ASSERT_EQUALS(c[0][2], 2);
	TEST_ASSERT_EQUALS(c[1][0], 8);
	TEST_ASSERT_EQUALS(c[1][1], 3);
	TEST_ASSERT_EQUALS(c[1][2], 4);
	TEST_ASSERT_EQUALS(c[2][0], 9);
	TEST_ASSERT_EQUALS(c[2][1], 5);
	TEST_ASSERT_EQUALS(c[2][2], 6);
	
	c = b.addColumn(1, colum3);
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 7);
	TEST_ASSERT_EQUALS(c[0][2], 2);
	TEST_ASSERT_EQUALS(c[1][0], 3);
	TEST_ASSERT_EQUALS(c[1][1], 8);
	TEST_ASSERT_EQUALS(c[1][2], 4);
	TEST_ASSERT_EQUALS(c[2][0], 5);
	TEST_ASSERT_EQUALS(c[2][1], 9);
	TEST_ASSERT_EQUALS(c[2][2], 6);
	
	c = b.addColumn(2, colum3);
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 2);
	TEST_ASSERT_EQUALS(c[0][2], 7);
	TEST_ASSERT_EQUALS(c[1][0], 3);
	TEST_ASSERT_EQUALS(c[1][1], 4);
	TEST_ASSERT_EQUALS(c[1][2], 8);
	TEST_ASSERT_EQUALS(c[2][0], 5);
	TEST_ASSERT_EQUALS(c[2][1], 6);
	TEST_ASSERT_EQUALS(c[2][2], 9);
}

void
MatrixTest::testRemoveRowColumn()
{
	const int16_t m[] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	xpcc::Matrix<int16_t, 3, 3> a(m);
	xpcc::Matrix<int16_t, 3, 2> b = a.removeColumn(0);
	TEST_ASSERT_EQUALS(b[0][0], 2);
	TEST_ASSERT_EQUALS(b[0][1], 3);
	TEST_ASSERT_EQUALS(b[1][0], 5);
	TEST_ASSERT_EQUALS(b[1][1], 6);
	TEST_ASSERT_EQUALS(b[2][0], 8);
	TEST_ASSERT_EQUALS(b[2][1], 9);
	
	b = a.removeColumn(1);
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 3);
	TEST_ASSERT_EQUALS(b[1][0], 4);
	TEST_ASSERT_EQUALS(b[1][1], 6);
	TEST_ASSERT_EQUALS(b[2][0], 7);
	TEST_ASSERT_EQUALS(b[2][1], 9);
	
	b = a.removeColumn(2);
	TEST_ASSERT_EQUALS(b[0][0], 1);
	TEST_ASSERT_EQUALS(b[0][1], 2);
	TEST_ASSERT_EQUALS(b[1][0], 4);
	TEST_ASSERT_EQUALS(b[1][1], 5);
	TEST_ASSERT_EQUALS(b[2][0], 7);
	TEST_ASSERT_EQUALS(b[2][1], 8);
	
	xpcc::Matrix<int16_t, 2, 3> c = a.removeRow(0);
	TEST_ASSERT_EQUALS(c[0][0], 4);
	TEST_ASSERT_EQUALS(c[0][1], 5);
	TEST_ASSERT_EQUALS(c[0][2], 6);
	TEST_ASSERT_EQUALS(c[1][0], 7);
	TEST_ASSERT_EQUALS(c[1][1], 8);
	TEST_ASSERT_EQUALS(c[1][2], 9);
	
	c = a.removeRow(1);
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 2);
	TEST_ASSERT_EQUALS(c[0][2], 3);
	TEST_ASSERT_EQUALS(c[1][0], 7);
	TEST_ASSERT_EQUALS(c[1][1], 8);
	TEST_ASSERT_EQUALS(c[1][2], 9);
	
	c = a.removeRow(2);
	TEST_ASSERT_EQUALS(c[0][0], 1);
	TEST_ASSERT_EQUALS(c[0][1], 2);
	TEST_ASSERT_EQUALS(c[0][2], 3);
	TEST_ASSERT_EQUALS(c[1][0], 4);
	TEST_ASSERT_EQUALS(c[1][1], 5);
	TEST_ASSERT_EQUALS(c[1][2], 6);
}

void
MatrixTest::testDeterminant()
{
	const int16_t m[9] = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9
	};
	
	// determinate 3x3
	xpcc::Matrix<int16_t, 3, 3> a(m);
	TEST_ASSERT_EQUALS(a.determinant(), 0);
	
	a[2][2] = 10;
	TEST_ASSERT_EQUALS(a.determinant(), -3);
	
	// determinate 2x2
	xpcc::Matrix<int16_t, 2, 2> b = a.subMatrix<2, 2>(0, 0);
	TEST_ASSERT_EQUALS(b.determinant(), -3);
	
	xpcc::Matrix<int16_t, 2, 2> c = a.subMatrix<2, 2>(1, 1);
	TEST_ASSERT_EQUALS(c.determinant(), 2);
	
	// determinate 1x1
	xpcc::Matrix<int16_t, 1, 1> d = a.subMatrix<1, 1>(1, 1);
	TEST_ASSERT_EQUALS(d.determinant(), 5);
}
