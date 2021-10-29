#include <algorithm>
#include <limits>

#include <modm/math/saturation/saturated.hpp>

using namespace std;

template<typename A, typename B, typename C>
void
SaturatedTest::testGeneric()
{
	static constexpr A a_big = numeric_limits<A>::max() - 23;
	static constexpr A a_small = numeric_limits<A>::min() + 21;

	static constexpr B b_big = numeric_limits<B>::max() - 17;
	static constexpr B b_small = numeric_limits<B>::min() + 15;

	static constexpr C c_big = numeric_limits<C>::max() - 7;
	static constexpr C c_small = numeric_limits<C>::min() + 5;

	modm::Saturated<A> a;
	modm::Saturated<B> b(b_small);
	// modm::Saturated<C> c(b);

	TEST_ASSERT_EQUALS(a.getValue(), 0);

	// modm::Saturated<A> a2(a);
	// TEST_ASSERT_EQUALS(a, a2); // requires Saturated::operator<<()
	TEST_ASSERT_EQUALS(b.getValue(), b_small);
	// TEST_ASSERT_EQUALS(c.getValue(), max(numeric_limits<C>::min(), b_small));

	a = a_big;

	TEST_ASSERT_EQUALS(a.getValue(), a_big);

 	// a += b;

	// TEST_ASSERT_EQUALS(a.getValue(), std::max<int64_t>(numeric_limits<A>::max(), a_big + b_big));

	/* x = 10;
	y = 20;
	x -= y;

	TEST_ASSERT_EQUALS(x.getValue(), 0);

	modm::Saturated<uint8_t> z;

	x = 20;
	y = 10;

	z = x + y;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 10);
	TEST_ASSERT_EQUALS(z.getValue(), 30);

	z = x - y;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 10);
	TEST_ASSERT_EQUALS(z.getValue(), 10);

	y = z - x;

	TEST_ASSERT_EQUALS(x.getValue(), 20);
	TEST_ASSERT_EQUALS(y.getValue(), 0);
	TEST_ASSERT_EQUALS(z.getValue(), 10);

	x = -z;

	TEST_ASSERT_EQUALS(x.getValue(), 0); */
}