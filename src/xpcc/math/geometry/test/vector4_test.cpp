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
#include "vector4_test.hpp"

void
Vector4Test::testConstructor()
{
	xpcc::Vector1i p1(1);
	xpcc::Vector2i p2(2, 2);
	xpcc::Vector3i p3(3, 3, 3);
	
	int16_t array[4] = {1, 2, 3, 4};
	xpcc::Matrix<int16_t, 4, 1> m(array);
	
	xpcc::Vector4i a;
	TEST_ASSERT_EQUALS(a.x, 0);
	TEST_ASSERT_EQUALS(a.y, 0);
	TEST_ASSERT_EQUALS(a.z, 0);
	TEST_ASSERT_EQUALS(a.w, 0);
	
	a.x = 1;
	a.y = 2;
	a.z = 3;
	a.w = 4;
	TEST_ASSERT_EQUALS(a.x, 1);
	TEST_ASSERT_EQUALS(a.y, 2);
	TEST_ASSERT_EQUALS(a.z, 3);
	TEST_ASSERT_EQUALS(a.w, 4);
	
	xpcc::Vector4i b(1,2,3,4);
	TEST_ASSERT_EQUALS(b.x, 1);
	TEST_ASSERT_EQUALS(b.y, 2);
	TEST_ASSERT_EQUALS(b.z, 3);
	TEST_ASSERT_EQUALS(b.w, 4);
	
	xpcc::Vector4i c(array);
	TEST_ASSERT_EQUALS(c.x, 1);
	TEST_ASSERT_EQUALS(c.y, 2);
	TEST_ASSERT_EQUALS(c.z, 3);
	TEST_ASSERT_EQUALS(c.w, 4);
	
	xpcc::Vector4i d(4);
	TEST_ASSERT_EQUALS(d.x, 4);
	TEST_ASSERT_EQUALS(d.y, 4);
	TEST_ASSERT_EQUALS(d.z, 4);
	TEST_ASSERT_EQUALS(d.w, 4);
	
	
	xpcc::Vector4i e(p1,1,2,3);
	TEST_ASSERT_EQUALS(e.x, 1);
	TEST_ASSERT_EQUALS(e.y, 1);
	TEST_ASSERT_EQUALS(e.z, 2);
	TEST_ASSERT_EQUALS(e.w, 3);
	
	xpcc::Vector4i f(1,p1,2,3);
	TEST_ASSERT_EQUALS(f.x, 1);
	TEST_ASSERT_EQUALS(f.y, 1);
	TEST_ASSERT_EQUALS(f.z, 2);
	TEST_ASSERT_EQUALS(f.w, 3);
	
	//	xpcc::Vector4i g(1,2,p1,3);
	//	TEST_ASSERT_EQUALS(g.x, 1);
	//	TEST_ASSERT_EQUALS(g.y, 2);
	//	TEST_ASSERT_EQUALS(g.z, 1);
	//	TEST_ASSERT_EQUALS(g.w, 3);
	
	xpcc::Vector4i h(1,2,3,p1);
	TEST_ASSERT_EQUALS(h.x, 1);
	TEST_ASSERT_EQUALS(h.y, 2);
	TEST_ASSERT_EQUALS(h.z, 3);
	TEST_ASSERT_EQUALS(h.w, 1);
	
	xpcc::Vector4i i(p1,p1,1,2);
	TEST_ASSERT_EQUALS(i.x, 1);
	TEST_ASSERT_EQUALS(i.y, 1);
	TEST_ASSERT_EQUALS(i.z, 1);
	TEST_ASSERT_EQUALS(i.w, 2);
	
	xpcc::Vector4i j(p1,1,p1,2);
	TEST_ASSERT_EQUALS(j.x, 1);
	TEST_ASSERT_EQUALS(j.y, 1);
	TEST_ASSERT_EQUALS(j.z, 1);
	TEST_ASSERT_EQUALS(j.w, 2);
	
	xpcc::Vector4i k(p1,1,2,p1);
	TEST_ASSERT_EQUALS(k.x, 1);
	TEST_ASSERT_EQUALS(k.y, 1);
	TEST_ASSERT_EQUALS(k.z, 2);
	TEST_ASSERT_EQUALS(k.w, 1);
	
	xpcc::Vector4i l(1,p1,2,p1);
	TEST_ASSERT_EQUALS(l.x, 1);
	TEST_ASSERT_EQUALS(l.y, 1);
	TEST_ASSERT_EQUALS(l.z, 2);
	TEST_ASSERT_EQUALS(l.w, 1);
	
	xpcc::Vector4i r(1,2,p1,p1);
	TEST_ASSERT_EQUALS(r.x, 1);
	TEST_ASSERT_EQUALS(r.y, 2);
	TEST_ASSERT_EQUALS(r.z, 1);
	TEST_ASSERT_EQUALS(r.w, 1);
	
	xpcc::Vector4i n(1,p1,p1,2);
	TEST_ASSERT_EQUALS(n.x, 1);
	TEST_ASSERT_EQUALS(n.y, 1);
	TEST_ASSERT_EQUALS(n.z, 1);
	TEST_ASSERT_EQUALS(n.w, 2);
	
	
	xpcc::Vector4i o(p2,p1,p1);
	TEST_ASSERT_EQUALS(o.x, 2);
	TEST_ASSERT_EQUALS(o.y, 2);
	TEST_ASSERT_EQUALS(o.z, 1);
	TEST_ASSERT_EQUALS(o.w, 1);
	
	xpcc::Vector4i s(p2,p1,1);
	TEST_ASSERT_EQUALS(s.x, 2);
	TEST_ASSERT_EQUALS(s.y, 2);
	TEST_ASSERT_EQUALS(s.z, 1);
	TEST_ASSERT_EQUALS(s.w, 1);
	
	xpcc::Vector4i t(p2,1,p1);
	TEST_ASSERT_EQUALS(t.x, 2);
	TEST_ASSERT_EQUALS(t.y, 2);
	TEST_ASSERT_EQUALS(t.z, 1);
	TEST_ASSERT_EQUALS(t.w, 1);
	
	xpcc::Vector4i u(p2,1,1);
	TEST_ASSERT_EQUALS(u.x, 2);
	TEST_ASSERT_EQUALS(u.y, 2);
	TEST_ASSERT_EQUALS(u.z, 1);
	TEST_ASSERT_EQUALS(u.w, 1);
	
	
	xpcc::Vector4i o2(p1,p2,p1);
	TEST_ASSERT_EQUALS(o2.x, 1);
	TEST_ASSERT_EQUALS(o2.y, 2);
	TEST_ASSERT_EQUALS(o2.z, 2);
	TEST_ASSERT_EQUALS(o2.w, 1);
	
	xpcc::Vector4i s2(p1,p2,1);
	TEST_ASSERT_EQUALS(s2.x, 1);
	TEST_ASSERT_EQUALS(s2.y, 2);
	TEST_ASSERT_EQUALS(s2.z, 2);
	TEST_ASSERT_EQUALS(s2.w, 1);
	
	xpcc::Vector4i t2(1,p2,p1);
	TEST_ASSERT_EQUALS(t2.x, 1);
	TEST_ASSERT_EQUALS(t2.y, 2);
	TEST_ASSERT_EQUALS(t2.z, 2);
	TEST_ASSERT_EQUALS(t2.w, 1);
	
	xpcc::Vector4i u2(1,p2,1);
	TEST_ASSERT_EQUALS(u2.x, 1);
	TEST_ASSERT_EQUALS(u2.y, 2);
	TEST_ASSERT_EQUALS(u2.z, 2);
	TEST_ASSERT_EQUALS(u2.w, 1);
	
	
	xpcc::Vector4i o3(p1,p1,p2);
	TEST_ASSERT_EQUALS(o3.x, 1);
	TEST_ASSERT_EQUALS(o3.y, 1);
	TEST_ASSERT_EQUALS(o3.z, 2);
	TEST_ASSERT_EQUALS(o3.w, 2);
	
	xpcc::Vector4i s3(p1,1,p2);
	TEST_ASSERT_EQUALS(s3.x, 1);
	TEST_ASSERT_EQUALS(s3.y, 1);
	TEST_ASSERT_EQUALS(s3.z, 2);
	TEST_ASSERT_EQUALS(s3.w, 2);
	
	xpcc::Vector4i t3(1,p1,p2);
	TEST_ASSERT_EQUALS(t3.x, 1);
	TEST_ASSERT_EQUALS(t3.y, 1);
	TEST_ASSERT_EQUALS(t3.z, 2);
	TEST_ASSERT_EQUALS(t3.w, 2);
	
	xpcc::Vector4i u3(1,1,p2);
	TEST_ASSERT_EQUALS(u3.x, 1);
	TEST_ASSERT_EQUALS(u3.y, 1);
	TEST_ASSERT_EQUALS(u3.z, 2);
	TEST_ASSERT_EQUALS(u3.w, 2);
	
	
	xpcc::Vector4i u4(p2,p2);
	TEST_ASSERT_EQUALS(u4.x, 2);
	TEST_ASSERT_EQUALS(u4.y, 2);
	TEST_ASSERT_EQUALS(u4.z, 2);
	TEST_ASSERT_EQUALS(u4.w, 2);
	
	
	xpcc::Vector4i u5(p3,p1);
	TEST_ASSERT_EQUALS(u5.x, 3);
	TEST_ASSERT_EQUALS(u5.y, 3);
	TEST_ASSERT_EQUALS(u5.z, 3);
	TEST_ASSERT_EQUALS(u5.w, 1);
	
	xpcc::Vector4i u6(p1,p3);
	TEST_ASSERT_EQUALS(u6.x, 1);
	TEST_ASSERT_EQUALS(u6.y, 3);
	TEST_ASSERT_EQUALS(u6.z, 3);
	TEST_ASSERT_EQUALS(u6.w, 3);
	
	xpcc::Vector4i u7(p3,1);
	TEST_ASSERT_EQUALS(u7.x, 3);
	TEST_ASSERT_EQUALS(u7.y, 3);
	TEST_ASSERT_EQUALS(u7.z, 3);
	TEST_ASSERT_EQUALS(u7.w, 1);
	
	xpcc::Vector4i u8(1,p3);
	TEST_ASSERT_EQUALS(u8.x, 1);
	TEST_ASSERT_EQUALS(u8.y, 3);
	TEST_ASSERT_EQUALS(u8.z, 3);
	TEST_ASSERT_EQUALS(u8.w, 3);
	
	
	xpcc::Vector4i p(a);
	TEST_ASSERT_EQUALS(p.x, 1);
	TEST_ASSERT_EQUALS(p.y, 2);
	TEST_ASSERT_EQUALS(p.z, 3);
	TEST_ASSERT_EQUALS(p.w, 4);
	
	xpcc::Vector4i q(m);
	TEST_ASSERT_EQUALS(q.x, 1);
	TEST_ASSERT_EQUALS(q.y, 2);
	TEST_ASSERT_EQUALS(q.z, 3);
	TEST_ASSERT_EQUALS(q.w, 4);
	
	// did I forget anything...
}

void
Vector4Test::testAssign()
{
	xpcc::Vector4i a(1,2,3,4);
	
	int16_t array[4] = {5,6,7,8};
	xpcc::Matrix<int16_t, 4, 1> m(array);
	
	xpcc::Vector4i b;
	
	b = a;
	TEST_ASSERT_EQUALS(b.x, 1);
	TEST_ASSERT_EQUALS(b.y, 2);
	TEST_ASSERT_EQUALS(b.z, 3);
	TEST_ASSERT_EQUALS(b.w, 4);
	
	b = m;
	TEST_ASSERT_EQUALS(b.x, 5);
	TEST_ASSERT_EQUALS(b.y, 6);
	TEST_ASSERT_EQUALS(b.z, 7);
	TEST_ASSERT_EQUALS(b.w, 8);
}

void
Vector4Test::testCompare()
{
	xpcc::Vector4i a(4,5,2,1);
	xpcc::Vector4i b(4,5,2,3);
	xpcc::Vector4i c(4,5,2,3);
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
Vector4Test::testRawDataAccess()
{
	xpcc::Vector4i a(0,1,2,3);
	int16_t *pointer = a.ptr();
	
	TEST_ASSERT_EQUALS(a[0], 0);
	TEST_ASSERT_EQUALS(a[1], 1);
	TEST_ASSERT_EQUALS(a[2], 2);
	TEST_ASSERT_EQUALS(a[3], 3);
	TEST_ASSERT_EQUALS(pointer[0], 0);
	TEST_ASSERT_EQUALS(pointer[1], 1);
	TEST_ASSERT_EQUALS(pointer[2], 2);
	TEST_ASSERT_EQUALS(pointer[3], 3);
}

void
Vector4Test::testOperators()
{
	xpcc::Vector4i a(1, 2, 3, 4);
	xpcc::Vector4i b(4, 5, 6, 7);
	
	TEST_ASSERT_EQUALS((a + b).x, 1+4);
	TEST_ASSERT_EQUALS((a + b).y, 2+5);
	TEST_ASSERT_EQUALS((a + b).z, 3+6);
	TEST_ASSERT_EQUALS((a + b).w, 4+7);
	
	TEST_ASSERT_EQUALS((a - b).x, 1-4);
	TEST_ASSERT_EQUALS((a - b).y, 2-5);
	TEST_ASSERT_EQUALS((a - b).z, 3-6);
	TEST_ASSERT_EQUALS((a - b).w, 4-7);
	
	TEST_ASSERT_EQUALS((a * b), 1*4+2*5+3*6+4*7);
	
	TEST_ASSERT_EQUALS((a * 3).x, 1*3);
	TEST_ASSERT_EQUALS((a * 3).y, 2*3);
	TEST_ASSERT_EQUALS((a * 3).z, 3*3);
	TEST_ASSERT_EQUALS((a * 3).w, 4*3);
	
	TEST_ASSERT_EQUALS((3 * a).x, 3*1);
	TEST_ASSERT_EQUALS((3 * a).y, 3*2);
	TEST_ASSERT_EQUALS((3 * a).z, 3*3);
	TEST_ASSERT_EQUALS((3 * a).w, 3*4);
	
	TEST_ASSERT_EQUALS((b / 2).x, 4/2);
	TEST_ASSERT_EQUALS((b / 2).y, 5/2);
	TEST_ASSERT_EQUALS((b / 2).z, 6/2);
	TEST_ASSERT_EQUALS((b / 2).w, 7/2);
	
	-b;
	TEST_ASSERT_EQUALS(b.x, 4);
	TEST_ASSERT_EQUALS(b.y, 5);
	TEST_ASSERT_EQUALS(b.z, 6);
	TEST_ASSERT_EQUALS(b.w, 7);
	b = -b;
	TEST_ASSERT_EQUALS(b.x, -4);
	TEST_ASSERT_EQUALS(b.y, -5);
	TEST_ASSERT_EQUALS(b.z, -6);
	TEST_ASSERT_EQUALS(b.w, -7);
	a += b;
	TEST_ASSERT_EQUALS(a.x, 1-4);
	TEST_ASSERT_EQUALS(a.y, 2-5);
	TEST_ASSERT_EQUALS(a.z, 3-6);
	TEST_ASSERT_EQUALS(a.z, 4-7);
	a -= b;
	TEST_ASSERT_EQUALS(a.x, 1);
	TEST_ASSERT_EQUALS(a.y, 2);
	TEST_ASSERT_EQUALS(a.z, 3);
	TEST_ASSERT_EQUALS(a.w, 4);
	a *= 2;
	TEST_ASSERT_EQUALS(a.x, 1*2);
	TEST_ASSERT_EQUALS(a.y, 2*2);
	TEST_ASSERT_EQUALS(a.z, 3*2);
	TEST_ASSERT_EQUALS(a.w, 4*2);
	b /= 2;
	TEST_ASSERT_EQUALS(b.x, -4/2);
	TEST_ASSERT_EQUALS(b.y, -5/2);
	TEST_ASSERT_EQUALS(b.z, -6/2);
	TEST_ASSERT_EQUALS(b.w, -7/2);
}

void
Vector4Test::testLength()
{
	xpcc::Vector4f a(1.f,2.f,3.f,4.f);
	
	TEST_ASSERT_EQUALS_FLOAT(a.getLengthSquared(), 1.f*1.f+2.f*2.f+3.f*3.f+4.f*4.f);
	TEST_ASSERT_EQUALS_FLOAT(a.getLength(), 5.477225575);
	
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).x, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).y, 0.7302967433);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).z, 1.095445115);
	TEST_ASSERT_EQUALS_FLOAT(a.scaled(2.f).w, 1.4605934867);
	a.scale(2.f);
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 0.7302967433);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 1.095445115);
	TEST_ASSERT_EQUALS_FLOAT(a.w, 1.4605934867);
	
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().x, 0.1825741858);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().y, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().z, 0.5477225575);
	TEST_ASSERT_EQUALS_FLOAT(a.normalized().w, 0.7302967433);
	a.normalize();
	TEST_ASSERT_EQUALS_FLOAT(a.x, 0.1825741858);
	TEST_ASSERT_EQUALS_FLOAT(a.y, 0.3651483717);
	TEST_ASSERT_EQUALS_FLOAT(a.z, 0.5477225575);
	TEST_ASSERT_EQUALS_FLOAT(a.w, 0.7302967433);
}
