/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "atomics_test.hpp"
#include <atomic>
#include <modm/architecture/detect.hpp>

// ============================= atomic integrals =============================
template< typename T >
static void
test(std::atomic<T> &a)
{
	// we are only testing the API, not the actual atomic properties
	// note: these memory order make no sense, they are only defined for testing
	TEST_ASSERT_EQUALS(a.load(), T(0));
	a.store(1, std::memory_order_relaxed);
	TEST_ASSERT_EQUALS(a.load(std::memory_order_relaxed), T(1));

	TEST_ASSERT_EQUALS(a.exchange(T(2), std::memory_order_acquire), T(1));
	TEST_ASSERT_EQUALS(a.exchange(T(3), std::memory_order_release), T(2));

	T value{2};
	TEST_ASSERT_FALSE(a.compare_exchange_weak(value, T(4)));

	value = T(3);
	TEST_ASSERT_TRUE(a.compare_exchange_weak(value, T(4),
					 std::memory_order_seq_cst, std::memory_order_consume));
	value = T(4);
	TEST_ASSERT_TRUE(a.compare_exchange_strong(value, T(5), std::memory_order_seq_cst));

	TEST_ASSERT_EQUALS(a.fetch_add(T(2), std::memory_order_relaxed), T(5));
	TEST_ASSERT_EQUALS(a.load(std::memory_order_relaxed), T(7));
	TEST_ASSERT_EQUALS(++a, T(8));

	TEST_ASSERT_EQUALS(a.fetch_sub(T(2), std::memory_order_relaxed), T(8));
	TEST_ASSERT_EQUALS(a.load(std::memory_order_relaxed), T(6));
	TEST_ASSERT_EQUALS(--a, T(5));

	TEST_ASSERT_EQUALS(a.fetch_and(T(0b1110), std::memory_order_relaxed), T(5));
	TEST_ASSERT_EQUALS(a.fetch_or(T(0b1000), std::memory_order_relaxed), T(4));
	TEST_ASSERT_EQUALS(a.fetch_xor(T(0b1000), std::memory_order_relaxed), T(0b1100));
	TEST_ASSERT_EQUALS(a.load(std::memory_order_relaxed), T(0b0100));
}

static std::atomic<uint8_t> a8{};
void
AtomicsTest::testAtomic8()
{
	TEST_ASSERT_TRUE(a8.is_lock_free());
	test(a8);
}

static std::atomic<uint16_t> a16{};
void
AtomicsTest::testAtomic16()
{
#ifdef MODM_CPU_AVR
	TEST_ASSERT_FALSE(a16.is_lock_free());
#else
	TEST_ASSERT_TRUE(a16.is_lock_free());
#endif
	test(a16);
}

static std::atomic<uint32_t> a32{};
void
AtomicsTest::testAtomic32()
{
#ifdef MODM_CPU_AVR
	TEST_ASSERT_FALSE(a32.is_lock_free());
#else
	TEST_ASSERT_TRUE(a32.is_lock_free());
#endif
	test(a32);
}

static std::atomic<uint64_t> a64{};
void
AtomicsTest::testAtomic64()
{
#ifdef MODM_OS_HOSTED
	TEST_ASSERT_TRUE(a64.is_lock_free());
#else
	TEST_ASSERT_FALSE(a64.is_lock_free());
#endif
	test(a64);
}

// ============================== atomic arrays ===============================
struct Array3
{
	uint8_t v[3];
};
static std::atomic<Array3> array3{};
void
AtomicsTest::testAtomicArray3()
{
	using T = Array3;
	constexpr size_t size = sizeof(T);

#ifndef _LIBCPP_VERSION // libc++ uses lock-free instructions here
	TEST_ASSERT_FALSE(array3.is_lock_free());
#endif

	TEST_ASSERT_EQUALS_ARRAY(array3.load().v, T{}.v, size);
	array3.store(T{1,2,3}, std::memory_order_relaxed);
	TEST_ASSERT_EQUALS_ARRAY(array3.load(std::memory_order_relaxed).v, (T{1,2,3}).v, size);

	TEST_ASSERT_EQUALS_ARRAY(array3.exchange(T{2,3,4}, std::memory_order_acquire).v,
							 (T{1,2,3}).v, size);
	TEST_ASSERT_EQUALS_ARRAY(array3.exchange(T{3,4,5}, std::memory_order_release).v,
							 (T{2,3,4}).v, size);

	T value{1,2,3};
	TEST_ASSERT_FALSE(array3.compare_exchange_weak(value, T{4,5,6}));

	value = T{3,4,5};
	TEST_ASSERT_TRUE(array3.compare_exchange_weak(value, T{4,5,6},
					 std::memory_order_seq_cst, std::memory_order_consume));
	value = T{4,5,6};
	TEST_ASSERT_TRUE(array3.compare_exchange_strong(value, T{5,6,7},
													std::memory_order_seq_cst));
}

struct Array
{
	uint8_t v[15];
};
static std::atomic<Array> array{};
void
AtomicsTest::testAtomicArray()
{
	using T = Array;
	constexpr size_t size = sizeof(T);

#ifndef _LIBCPP_VERSION // libc++ uses lock-free instructions here
	TEST_ASSERT_FALSE(array.is_lock_free());
#endif

	TEST_ASSERT_EQUALS_ARRAY(array.load().v, T{}.v, size);
	array.store(T{1,2,3,4,5,6,7,8}, std::memory_order_relaxed);
	TEST_ASSERT_EQUALS_ARRAY(array.load(std::memory_order_relaxed).v, (T{1,2,3,4,5,6,7,8}).v, size);

	TEST_ASSERT_EQUALS_ARRAY(array.exchange(T{2,3,4,5,6,7,8,9}, std::memory_order_acquire).v,
							 (T{1,2,3,4,5,6,7,8}).v, size);
	TEST_ASSERT_EQUALS_ARRAY(array.exchange(T{3,4,5,6,7,8,9,10}, std::memory_order_release).v,
							 (T{2,3,4,5,6,7,8,9}).v, size);

	T value{1,2,3,4,5,6,7,8};
	TEST_ASSERT_FALSE(array.compare_exchange_weak(value, T{4,5,6,7,8,9,10,11}));

	value = T{3,4,5,6,7,8,9,10};
	TEST_ASSERT_TRUE(array.compare_exchange_weak(value, T{4,5,6,7,8,9,10,11},
					 std::memory_order_seq_cst, std::memory_order_consume));
	value = T{4,5,6,7,8,9,10,11};
	TEST_ASSERT_TRUE(array.compare_exchange_strong(value, T{5,6,7,8,9,10,11,12},
												   std::memory_order_seq_cst));
}

// =============================== atomic flags ===============================
static std::atomic_flag af{};
void
AtomicsTest::testAtomicFlag()
{
	TEST_ASSERT_FALSE(af.test_and_set());
	TEST_ASSERT_TRUE(af.test());

	TEST_ASSERT_TRUE(af.test_and_set());
	af.clear();
	TEST_ASSERT_FALSE(af.test());
	TEST_ASSERT_FALSE(af.test_and_set());

	TEST_ASSERT_TRUE(af.test());
}
