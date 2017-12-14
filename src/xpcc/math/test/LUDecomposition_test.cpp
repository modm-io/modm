#include <xpcc/math/matrix.hpp>
#include <xpcc/math/lu_decomposition.hpp>
#include "LUDecomposition_test.hpp"

void
LUDecompositionTest::testLUD()
{
	// https://www.wolframalpha.com/input/?i=LU+Decomposition+of+%7B%7B1,2,3%7D,%7B0,1,2%7D,%7B3,4,6%7D%7D

	const float m[] = {
		1.f, 2.f, 3.f,
		0.f, 1.f, 2.f,
		3.f, 4.f, 6.f
	};

	xpcc::Matrix<float, 3, 3> A(m);
	xpcc::Matrix<float, 3, 3> l;
	xpcc::Matrix<float, 3, 3> u;
	TEST_ASSERT_TRUE(xpcc::LUDecomposition::decompose(A, &l, &u));
	TEST_ASSERT_EQUALS(l[0][0],  1);
	TEST_ASSERT_EQUALS(l[0][1],  0);
	TEST_ASSERT_EQUALS(l[0][2],  0);
	TEST_ASSERT_EQUALS(l[1][0],  0);
	TEST_ASSERT_EQUALS(l[1][1],  1);
	TEST_ASSERT_EQUALS(l[1][2],  0);
	TEST_ASSERT_EQUALS(l[2][0],  3);
	TEST_ASSERT_EQUALS(l[2][1], -2);
	TEST_ASSERT_EQUALS(l[2][2],  1);

	TEST_ASSERT_EQUALS(u[0][0], 1);
	TEST_ASSERT_EQUALS(u[0][1], 2);
	TEST_ASSERT_EQUALS(u[0][2], 3);
	TEST_ASSERT_EQUALS(u[1][0], 0);
	TEST_ASSERT_EQUALS(u[1][1], 1);
	TEST_ASSERT_EQUALS(u[1][2], 2);
	TEST_ASSERT_EQUALS(u[2][0], 0);
	TEST_ASSERT_EQUALS(u[2][1], 0);
	TEST_ASSERT_EQUALS(u[2][2], 1);

	// test if the inverse is calculated
	xpcc::Matrix<float, 3, 3> AI = xpcc::Matrix<float, 3, 3>::identityMatrix();
	TEST_ASSERT_TRUE(xpcc::LUDecomposition::solve(l, u, &AI));

	// test if (inverse of A) * (A) = E
	TEST_ASSERT_TRUE(A * AI == (xpcc::Matrix<float, 3, 3>::identityMatrix()));

	// test Ax=b
	// https://www.wolframalpha.com/input/?i=solve%7B%7B1,2,3%7D,%7B0,1,2%7D,%7B3,4,6%7D%7D+%7B%7Bx%7D,%7By%7D,%7Bz%7D%7D%3D+%7B%7B0%7D,%7B1%7D,%7B2%7D%7D
	const float n[] = {
		0.f,
		1.f,
		2.f
	};
	xpcc::Matrix<float, 3, 1> b(n);
	TEST_ASSERT_TRUE(xpcc::LUDecomposition::solve(l, u, &b));
	TEST_ASSERT_EQUALS(b[0][0],  2);
	TEST_ASSERT_EQUALS(b[1][0], -7);
	TEST_ASSERT_EQUALS(b[2][0],  4);

	b = xpcc::Matrix<float, 3, 1>(n);
	TEST_ASSERT_TRUE(xpcc::LUDecomposition::solve(A, &b));
	TEST_ASSERT_EQUALS(b[0][0],  2.f);
	TEST_ASSERT_EQUALS(b[1][0], -7.f);
	TEST_ASSERT_EQUALS(b[2][0],  4.f);
}

