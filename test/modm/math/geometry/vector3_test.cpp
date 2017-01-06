/*
 * Copyright (c) 2011-2012, Niklas Hauser
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/vector.hpp>
#include "vector3_test.hpp"

void
Vector3Test::testConstructor()
{
	
	modm::Vector1i p1(3);
	modm::Vector2i p2(1, 2);
	
	int16_t array[3] = {-4, 5, 7};
	modm::Matrix<int16_t, 3, 1> m(array);
	
	modm::Vector3i a;
	TEST_ASSERT_EQUALS(a.x, 0);
	TEST_ASSERT_EQUALS(a.y, 0);
	TEST_ASSERT_EQUALS(a.z, 0);
	
	a.x = 100;
	a.y = 9;
	a.z = 4;
	TEST_ASSERT_EQUALS(a.x, 100);
	TEST_ASSERT_EQUALS(a.y, 9);
	TEST_ASSERT_EQUALS(a.z, 4);
	
	modm::Vector3i b(20,1,-4);
	TEST_ASSERT_EQUALS(b.x, 20);
	TEST_ASSERT_EQUALS(b.y, 1);
	TEST_ASSERT_EQUALS(b.z, -4);
	
	modm::Vector3i c(array);
	TEST_ASSERT_EQUALS(c.x, -4);
	TEST_ASSERT_EQUALS(c.y, 5);
	TEST_ASSERT_EQUALS(c.z, 7);
	
	modm::Vector3i d(4);
	TEST_ASSERT_EQUALS(d.x, 4);
	TEST_ASSERT_EQUALS(d.y, 4);
	TEST_ASSERT_EQUALS(d.z, 4);
	
	modm::Vector3i e(1,2,p1);
	TEST_ASSERT_EQUALS(e.x, 1);
	TEST_ASSERT_EQUALS(e.y, 2);
	TEST_ASSERT_EQUALS(e.z, 3);
	
	modm::Vector3i f(1,p1,2);
	TEST_ASSERT_EQUALS(f.x, 1);
	TEST_ASSERT_EQUALS(f.y, 3);
	TEST_ASSERT_EQUALS(f.z, 2);
	
	modm::Vector3i g(p1,2,1);
	TEST_ASSERT_EQUALS(g.x, 3);
	TEST_ASSERT_EQUALS(g.y, 2);
	TEST_ASSERT_EQUALS(g.z, 1);
	
	modm::Vector3i h(1,p1,p1);
	TEST_ASSERT_EQUALS(h.x, 1);
	TEST_ASSERT_EQUALS(h.y, 3);
	TEST_ASSERT_EQUALS(h.z, 3);
	
	modm::Vector3i i(p1,1,p1);
	TEST_ASSERT_EQUALS(i.x, 3);
	TEST_ASSERT_EQUALS(i.y, 1);
	TEST_ASSERT_EQUALS(i.z, 3);
	
	modm::Vector3i j(p1,p1,1);
	TEST_ASSERT_EQUALS(j.x, 3);
	TEST_ASSERT_EQUALS(j.y, 3);
	TEST_ASSERT_EQUALS(j.z, 1);
	
	modm::Vector3i k(p1,p1,p1);
	TEST_ASSERT_EQUALS(k.x, 3);
	TEST_ASSERT_EQUALS(k.y, 3);
	TEST_ASSERT_EQUALS(k.z, 3);
	
	modm::Vector3i l(2,p2);
	TEST_ASSERT_EQUALS(l.x, 2);
	TEST_ASSERT_EQUALS(l.y, 1);
	TEST_ASSERT_EQUALS(l.z, 2);
	
	modm::Vector3i r(p2,6);
	TEST_ASSERT_EQUALS(r.x, 1);
	TEST_ASSERT_EQUALS(r.y, 2);
	TEST_ASSERT_EQUALS(r.z, 6);
	
	modm::Vector3i n(p1,p2);
	TEST_ASSERT_EQUALS(n.x, 3);
	TEST_ASSERT_EQUALS(n.y, 1);
	TEST_ASSERT_EQUALS(n.z, 2);
	
	modm::Vector3i o(p2,p1);
	TEST_ASSERT_EQUALS(o.x, 1);
	TEST_ASSERT_EQUALS(o.y, 2);
	TEST_ASSERT_EQUALS(o.z, 3);
	
	modm::Vector3i p(a);
	TEST_ASSERT_EQUALS(p.x, 100);
	TEST_ASSERT_EQUALS(p.y, 9);
	TEST_ASSERT_EQUALS(p.z, 4);
	
	modm::Vector3i q(m);
	TEST_ASSERT_EQUALS(q.x, -4);
	TEST_ASSERT_EQUALS(q.y, 5);
	TEST_ASSERT_EQUALS(q.z, 7);
}

void
Vector3Test::testAssign()
{
	modm::Vector3i a(42,-4,3);
	
	int16_t array[3] = {-26,9,2};
	modm::Matrix<int16_t, 3, 1> m(array);
	
	modm::Vector3i b;
	
	b = a;
	TEST_ASSERT_EQUALS(b.x, 42);
	TEST_ASSERT_EQUALS(b.y, -4);
	TEST_ASSERT_EQUALS(b.z, 3);
	
	b = m;
	TEST_ASSERT_EQUALS(b.x, -26);
	TEST_ASSERT_EQUALS(b.y, 9);
	TEST_ASSERT_EQUALS(b.z, 2);
}

void
Vector3Test::testCompare()
{
	modm::Vector3i a(4,5,1);
	modm::Vector3i b(4,5,2);
	modm::Vector3i c(4,5,2);
	// ==
	TEST_ASSERT_TRUE(b == c);
	TEST_ASSERT_FALSE(a == c);
	// !=
	TEST_ASSERT_TRUE(a != c);
	TEST_ASSERT_FALSE(b != c);
	// <
	TEST_ASSERT_TRUE(a < c);
	TEST_ASSERT_FALSE(b < c);
	TEST_ASSERT_FALSE(c < a);
	// <=
	TEST_ASSERT_TRUE(a <= c);
	TEST_ASSERT_TRUE(b <= c);
	TEST_ASSERT_FALSE(c <= a);
	// >
	TEST_ASSERT_TRUE(c > a);
	TEST_ASSERT_FALSE(b > c);
	TEST_ASSERT_FALSE(a > c);
	// >=
	TEST_ASSERT_TRUE(c >= a);
	TEST_ASSERT_TRUE(b >= c);
	TEST_ASSERT_FALSE(a >= c);
}

void
Vector3Test::testRawDataAccess()
{
	modm::Vector3i a(0, 1, 2);
	int16_t *pointer = a.ptr();
	
	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 1);
	TEST_ASSERT_EQUALS(a[2], 2);
	TEST_ASSERT_EQUALS(pointer[0], 0);
	TEST_ASSERT_EQUALS(pointer[1], 1);
	TEST_ASSERT_EQUALS(pointer[2], 2);
}

void
Vector3Test::testOperators()
{
	modm::Vector3i a(1, 2, 3);
	modm::Vector3i b(4, 5, 6);
	
	TEST_ASSERT_EQUALS((a + b).x, 1+4);
	TEST_ASSERT_EQUALS((a + b).y, 2+5);
	TEST_ASSERT_EQUALS((a + b).z, 3+6);
	
	TEST_ASSERT_EQUALS((a - b).x, 1-4);
	TEST_ASSERT_EQUALS((a - b).y, 2-5);
	TEST_ASSERT_EQUALS((a - b).z, 3-6);
	
	TEST_ASSERT_EQUALS((a * b), 1*4+2*5+3*6);
	
	TEST_ASSERT_EQUALS((a ^ b).x, 2*6-3*5);
	TEST_ASSERT_EQUALS((a ^ b).y, 3*4-1*6);
	TEST_ASSERT_EQUALS((a ^ b).z, 1*5-2*4);
	
	TEST_ASSERT_EQUALS((a * 3).x, 1*3);
	TEST_ASSERT_EQUALS((a * 3).y, 2*3);
	TEST_ASSERT_EQUALS((a * 3).z, 3*3);
	TEST_ASSERT_EQUALS((3 * a).x, 3*1);
	TEST_ASSERT_EQUALS((3 * a).y, 3*2);
	TEST_ASSERT_EQUALS((3 * a).z, 3*3);
	
	TEST_ASSERT_EQUALS((b / 2).x, 4/2);
	TEST_ASSERT_EQUALS((b / 2).y, 5/2);
	TEST_ASSERT_EQUALS((b / 2).z, 6/2);
	
	-b;
	TEST_ASSERT_EQUALS(b.x, 4);
	TEST_ASSERT_EQUALS(b.y, 5);
	TEST_ASSERT_EQUALS(b.z, 6);
	b = -b;
	TEST_ASSERT_EQUALS(b.x, -4);
	TEST_ASSERT_EQUALS(b.y, -5);
	TEST_ASSERT_EQUALS(b.z, -6);
	a += b;
	TEST_ASSERT_EQUALS(a.x, 1-4);
	TEST_ASSERT_EQUALS(a.y, 2-5);
	TEST_ASSERT_EQUALS(a.z, 3-6);
	a -= b;
	TEST_ASSERT_EQUALS(a.x, 1);
	TEST_ASSERT_EQUALS(a.y, 2);
	TEST_ASSERT_EQUALS(a.z, 3);
	a *= 2;
	TEST_ASSERT_EQUALS(a.x, 1*2);
	TEST_ASSERT_EQUALS(a.y, 2*2);
	TEST_ASSERT_EQUALS(a.z, 3*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.x, -4/2);
	TEST_ASSERT_EQUALS(b.y, -5/2);
	TEST_ASSERT_EQUALS(b.z, -6/2);
}

void
Vector3Test::testLength()
{
	modm::Vector3f a(1.f,2.f,3.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f+2.f*2.f+3.f*3.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 3.741657387);
	
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).x, 0.5345224838);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).y, 1.0690449676);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).z, 1.6035674514);
	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.5345224838);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 1.0690449676);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 1.6035674514);
	
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().x, 0.2672612419);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().y, 0.5345224838);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().z, 0.8017837257);
	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.2672612419);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 0.5345224838);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 0.8017837257);
}

void
Vector3Test::testMathDefs()
{
	modm::Vector3i a(7,1,0);
	modm::Vector3f b(INFINITY,NAN,1);
	
	TEST_ASSERT_FALSE(a.hasNan());
	TEST_ASSERT_FALSE(a.hasInf());
	
	TEST_ASSERT_TRUE(b.hasNan());
	TEST_ASSERT_TRUE(b.hasInf());
}
