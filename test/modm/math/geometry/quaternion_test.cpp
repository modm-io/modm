/*
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/vector.hpp>
#include <modm/math/geometry/quaternion.hpp>
#include <modm/math/geometry/angle.hpp>

#include "quaternion_test.hpp"

void
QuaternionTest::testConstructor()
{
	modm::Vector<float,3> v(1,2,3);
	float mData[] = {1,2,3,4};
	modm::Matrix<float, 4, 1> m(mData);

	modm::Quaternion<float> q0;
	modm::Quaternion<float> q1(1,2,3,4);
	modm::Quaternion<float> q2(v, M_PI/4);
	modm::Quaternion<float> q3(m);
	modm::Quaternion<float> q4(q1);

	TEST_ASSERT_EQUALS(q0.w, 0);
	TEST_ASSERT_EQUALS(q0.x, 0);
	TEST_ASSERT_EQUALS(q0.y, 0);
	TEST_ASSERT_EQUALS(q0.z, 0);

	TEST_ASSERT_EQUALS(q1.w, 1);
	TEST_ASSERT_EQUALS(q1.x, 2);
	TEST_ASSERT_EQUALS(q1.y, 3);
	TEST_ASSERT_EQUALS(q1.z, 4);

	TEST_ASSERT_EQUALS_FLOAT(q2.w, cos(M_PI/8));
	TEST_ASSERT_EQUALS_FLOAT(q2.x, 1*sin(M_PI/8));
	TEST_ASSERT_EQUALS_FLOAT(q2.y, 2*sin(M_PI/8));
	TEST_ASSERT_EQUALS_FLOAT(q2.z, 3*sin(M_PI/8));

	TEST_ASSERT_EQUALS(q3.w, 1);
	TEST_ASSERT_EQUALS(q3.x, 2);
	TEST_ASSERT_EQUALS(q3.y, 3);
	TEST_ASSERT_EQUALS(q3.z, 4);

	TEST_ASSERT_EQUALS(q4.w, 1);
	TEST_ASSERT_EQUALS(q4.x, 2);
	TEST_ASSERT_EQUALS(q4.y, 3);
	TEST_ASSERT_EQUALS(q4.z, 4);

	q0.w = 1;
	q0.x = 2;
	q0.y = 3;
	q0.z = 4;
	TEST_ASSERT_EQUALS(q0.w, 1);
	TEST_ASSERT_EQUALS(q0.x, 2);
	TEST_ASSERT_EQUALS(q0.y, 3);
	TEST_ASSERT_EQUALS(q0.z, 4);

}

void
QuaternionTest::testAssign()
{
	modm::Quaternion<float> q1(1,2,3,4);
	modm::Quaternion<float> q5;
	q5 = q1;
	TEST_ASSERT_EQUALS(q5.w, 1);
	TEST_ASSERT_EQUALS(q5.x, 2);
	TEST_ASSERT_EQUALS(q5.y, 3);
	TEST_ASSERT_EQUALS(q5.z, 4);


	float mData[] = {1,2,3,4};
	modm::Matrix<float, 4, 1> m(mData);
	modm::Quaternion<float> q6;
	q6 = m;
	TEST_ASSERT_EQUALS(q6.w, 1);
	TEST_ASSERT_EQUALS(q6.x, 2);
	TEST_ASSERT_EQUALS(q6.y, 3);
	TEST_ASSERT_EQUALS(q6.z, 4);
}

void
QuaternionTest::testRawDataAccess()
{
	modm::Quaternion<float> a(1,2,3,4);
	float *pointer = a.ptr();

	TEST_ASSERT_EQUALS(a[0], 1);
	TEST_ASSERT_EQUALS(a[1], 2);
	TEST_ASSERT_EQUALS(a[2], 3);
	TEST_ASSERT_EQUALS(a[3], 4);
	TEST_ASSERT_EQUALS(pointer[0], 1);
	TEST_ASSERT_EQUALS(pointer[1], 2);
	TEST_ASSERT_EQUALS(pointer[2], 3);
	TEST_ASSERT_EQUALS(pointer[3], 4);
}

void
QuaternionTest::testOperators()
{
	// there are +q, -q, *q, *T, T*, /T, -, +=q, -=q, *=T, /=T
	modm::Quaternion<float> a(1, 2, 3, 4);
	modm::Quaternion<float> b(4, 5, 6, 7);

	TEST_ASSERT_EQUALS((a + b).w, 1+4);
	TEST_ASSERT_EQUALS((a + b).x, 2+5);
	TEST_ASSERT_EQUALS((a + b).y, 3+6);
	TEST_ASSERT_EQUALS((a + b).z, 4+7);

	TEST_ASSERT_EQUALS((a - b).w, 1-4);
	TEST_ASSERT_EQUALS((a - b).x, 2-5);
	TEST_ASSERT_EQUALS((a - b).y, 3-6);
	TEST_ASSERT_EQUALS((a - b).z, 4-7);

	modm::Quaternion<float> product(a*b);
	TEST_ASSERT_EQUALS(product.w, -52);
	TEST_ASSERT_EQUALS(product.x, 10);
	TEST_ASSERT_EQUALS(product.y, 24);
	TEST_ASSERT_EQUALS(product.z, 20);

	TEST_ASSERT_EQUALS((a * 3).w, 1*3);
	TEST_ASSERT_EQUALS((a * 3).x, 2*3);
	TEST_ASSERT_EQUALS((a * 3).y, 3*3);
	TEST_ASSERT_EQUALS((a * 3).z, 4*3);

	TEST_ASSERT_EQUALS((3.f * a).w, 3*1);
	TEST_ASSERT_EQUALS((3.f * a).x, 3*2);
	TEST_ASSERT_EQUALS((3.f * a).y, 3*3);
	TEST_ASSERT_EQUALS((3.f * a).z, 3*4);

	TEST_ASSERT_EQUALS((b / 2).w, 4/2.f);
	TEST_ASSERT_EQUALS((b / 2).x, 5/2.f);
	TEST_ASSERT_EQUALS((b / 2).y, 6/2.f);
	TEST_ASSERT_EQUALS((b / 2).z, 7/2.f);

	-b;
	TEST_ASSERT_EQUALS(b.w, 4);
	TEST_ASSERT_EQUALS(b.x, 5);
	TEST_ASSERT_EQUALS(b.y, 6);
	TEST_ASSERT_EQUALS(b.z, 7);
	b = -b;
	TEST_ASSERT_EQUALS(b.w, -4);
	TEST_ASSERT_EQUALS(b.x, -5);
	TEST_ASSERT_EQUALS(b.y, -6);
	TEST_ASSERT_EQUALS(b.z, -7);
	a += b;
	TEST_ASSERT_EQUALS(a.w, 1-4);
	TEST_ASSERT_EQUALS(a.x, 2-5);
	TEST_ASSERT_EQUALS(a.y, 3-6);
	TEST_ASSERT_EQUALS(a.z, 4-7);
	a -= b;
	TEST_ASSERT_EQUALS(a.w, 1);
	TEST_ASSERT_EQUALS(a.x, 2);
	TEST_ASSERT_EQUALS(a.y, 3);
	TEST_ASSERT_EQUALS(a.z, 4);
	a *= 2;
	TEST_ASSERT_EQUALS(a.w, 1*2);
	TEST_ASSERT_EQUALS(a.x, 2*2);
	TEST_ASSERT_EQUALS(a.y, 3*2);
	TEST_ASSERT_EQUALS(a.z, 4*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.w, -4/2.f);
	TEST_ASSERT_EQUALS(b.x, -5/2.f);
	TEST_ASSERT_EQUALS(b.y, -6/2.f);
	TEST_ASSERT_EQUALS(b.z, -7/2.f);
}

void
QuaternionTest::testLength()
{
	modm::Quaternion<float> a(1.f,2.f,3.f,4.f);

	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f+2.f*2.f+3.f*3.f+4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.477225575);

	TEST_ASSERT_EQUALS_FLOAT(a.conjugated().w, 1);
	TEST_ASSERT_EQUALS_FLOAT(a.conjugated().x, -2);
	TEST_ASSERT_EQUALS_FLOAT(a.conjugated().y, -3);
	TEST_ASSERT_EQUALS_FLOAT(a.conjugated().z, -4);
	a.conjugate();
	TEST_ASSERT_EQUALS_FLOAT(a.w, 1);
	TEST_ASSERT_EQUALS_FLOAT(a.x, -2);
	TEST_ASSERT_EQUALS_FLOAT(a.y, -3);
	TEST_ASSERT_EQUALS_FLOAT(a.z, -4);

	a.conjugate();
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).w, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).x, 0.7302967433);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).y, 1.095445115);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).z, 1.4605934867);
	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.w, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.7302967433);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 1.095445115);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 1.4605934867);

	TEST_ASSERT_EQUALS_FLOAT(a.normalized().w, 0.1825741858);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().x, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().y, 0.5477225575);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().z, 0.7302967433);
	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.w, 0.1825741858);
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 0.5477225575);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 0.7302967433);
}
