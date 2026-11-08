/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
 * Stress test, timing and fragmentation benchmark of the heap allocator.
 *
 * Every block is filled with a pattern derived from its slot and generation,
 * which is verified before it is freed or reallocated, so overlapping blocks
 * and corruption by the allocator are found. Timing uses the cycle counter
 * with interrupts disabled. Fragmentation is measured as the part of the
 * memory that a workload can use before the first allocation fails.
 *
 * Lines starting with "R " are results for the runner script, lines starting
 * with "E " are errors. Any error fails the test.
 */

#include <modm/board.hpp>
#include <modm/architecture/interface/memory.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/driver/time/cycle_counter.hpp>
#include <modm/platform/core/heap_table.hpp>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <new>
#include <algorithm>

#include "../integration_test.hpp"

extern "C" void* malloc_traits(size_t size, uint32_t traits);

#ifndef HEAP_TEST_SLOTS
#define HEAP_TEST_SLOTS 256
#endif
#ifndef HEAP_TEST_OPS
#define HEAP_TEST_OPS 20000
#endif

#define LOG(...) do { MODM_LOG_INFO.printf(__VA_ARGS__); MODM_LOG_INFO.flush(); } while(0)

using modm::MemoryTraits;
using modm::MemoryTrait;

// ----------------------------------------------------------------------------
static uint32_t errors{0};
#define ERROR(...) do { errors++; if (errors < 40) { LOG("E " __VA_ARGS__); LOG("\n"); } } while(0)

// Keeps GCC from removing malloc/free pairs whose memory is not used
static inline void* escape(void *p) { asm volatile("" : "+r"(p) :: "memory"); return p; }
#define malloc(s) escape(malloc(s))

static uint32_t rng_state{0x12345678};
static uint32_t rnd()
{
	uint32_t x = rng_state;
	x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	return rng_state = x;
}
static uint32_t rnd(uint32_t lo, uint32_t hi) // [lo, hi]
{ return lo + (hi > lo ? rnd() % (hi - lo + 1) : 0); }

// ----------------------------------------------------------------------------
struct Region { uint16_t traits; const uint8_t *start, *end; size_t size; size_t used; };
static Region regions[16];
static size_t region_count{0};
static size_t heap_total{0};
static size_t default_size{0}; // all regions with the default traits

static Region* find_region(const void *p)
{
	for (size_t ii = 0; ii < region_count; ii++)
		if (regions[ii].start <= (const uint8_t*)p and (const uint8_t*)p < regions[ii].end)
			return &regions[ii];
	return nullptr;
}

static uint32_t crossings{0}; // blocks that span two contiguous regions
static bool in_heap(const void *p, size_t size)
{
	const Region *r = find_region(p);
	if (not r) return false;
	const uint8_t *end = (const uint8_t*)p + size;
	if (end < (const uint8_t*)p) return false;
	if (end > r->end) crossings++;
	while (r and end > r->end) r = find_region(r->end);
	return r;
}

// ----------------------------------------------------------------------------
static modm::CycleCounter counter;

struct Timing
{
	const char *name;
	uint32_t count{0}, min{UINT32_MAX}, max{0};
	uint64_t sum{0};
	uint16_t buckets[64]{};

	void add(uint32_t c)
	{
		count++; sum += c;
		if (c < min) min = c;
		if (c > max) max = c;
		// 4 buckets per power of two
		uint32_t b = 0;
		if (c >= 4) {
			const uint32_t l2 = 31 - __builtin_clz(c);
			b = (l2 - 1) * 4 + ((c >> (l2 - 2)) & 3);
		} else b = c;
		buckets[std::min<uint32_t>(b, 63)]++;
	}
	uint32_t percentile(uint32_t pm) const // per mille
	{
		uint32_t target = uint64_t(count) * pm / 1000, acc = 0;
		for (uint32_t b = 0; b < 64; b++) {
			acc += buckets[b];
			if (acc > target) {
				if (b < 4) return b;
				// upper bound of the bucket
				const uint32_t l2 = b / 4 + 1, sub = b % 4;
				return ((4 + sub + 1) << (l2 - 2)) - 1;
			}
		}
		return max;
	}
	void print() const
	{
		if (not count) return;
		LOG("R time %s n=%lu min=%lu avg=%lu p50=%lu p99=%lu max=%lu\n", name, count, min,
			uint32_t(sum / count), percentile(500), percentile(990), max);
	}
	void reset() { *this = Timing{name}; }
};
static Timing t_malloc{"malloc"}, t_free{"free"}, t_realloc{"realloc"}, t_calloc{"calloc"};

template<typename F>
static auto timed(Timing &t, F&& f)
{
	modm::atomic::Lock lock;
	counter.start();
	auto r = f();
	counter.stop();
	t.add(counter.cycles());
	return r;
}

// ----------------------------------------------------------------------------
// Pattern helpers: byte wise, so that misaligned blocks can still be checked.
static inline uint8_t pattern(uint32_t seed, size_t i)
{ return uint8_t(seed ^ (i * 0x9d) ^ (i >> 7) ^ (seed >> 11)); }

// Large blocks only get their first and last 512 bytes patterned: allocator
// corruption shows up at the block edges, and SDRAM is slow.
static void fill(void *p, size_t size, uint32_t seed)
{
	uint8_t *b = (uint8_t*)p;
	for (size_t i = 0; i < size; i = (i == 511 and size > 1024) ? size - 512 : i + 1) b[i] = pattern(seed, i);
}
static bool check(const void *p, size_t size, uint32_t seed)
{
	const uint8_t *b = (const uint8_t*)p;
	for (size_t i = 0; i < size; i = (i == 511 and size > 1024) ? size - 512 : i + 1)
		if (b[i] != pattern(seed, i)) return false;
	return true;
}

static bool check_block(const void *p, size_t size, const char *what)
{
	if (not p) return true;
	bool ok = true;
	static uint32_t misaligned{0};
	if (uintptr_t(p) & 7) {
		if (not misaligned++) ERROR("%s %p (%u) is not 8-byte aligned", what, p, size);
		ok = false;
	}
	if (not in_heap(p, size)) { ERROR("%s %p (%u) is outside the heap", what, p, size); ok = false; }
	return ok;
}

// ----------------------------------------------------------------------------
struct Slot { uint8_t *p; uint32_t size; uint32_t seed; };
static Slot slots[HEAP_TEST_SLOTS];
static size_t live_bytes{0};
static uint32_t generation{1};

static void slot_free(Slot &s, bool timing = true)
{
	if (not s.p) return;
	if (not check(s.p, s.size, s.seed))
		ERROR("corrupted block %p (%lu) before free", s.p, s.size);
	if (timing) timed(t_free, [&]{ free(s.p); return 0; });
	else free(s.p);
	live_bytes -= s.size;
	s.p = nullptr;
}
static void free_all()
{
	for (auto &s : slots) slot_free(s, false);
	live_bytes = 0;
}

static bool slot_alloc(Slot &s, size_t size, bool zeroed = false)
{
	uint8_t *p;
	if (zeroed) {
		const size_t n = rnd(1, 7), m = (size + n - 1) / n;
		size = n * m;
		p = (uint8_t*) timed(t_calloc, [&]{ return calloc(n, m); });
		if (p) {
			for (size_t i = 0; i < size; i++)
				if (p[i]) { ERROR("calloc(%u, %u) %p not zero at %u", n, m, p, i); break; }
		}
	}
	else p = (uint8_t*) timed(t_malloc, [&]{ return malloc(size); });
	if (not p) return false;
	check_block(p, size, zeroed ? "calloc" : "malloc");
	s = {p, uint32_t(size), generation++ * 0x9e3779b9u};
	fill(p, size, s.seed);
	live_bytes += size;
	return true;
}

static bool slot_realloc(Slot &s, size_t size)
{
	if (not check(s.p, s.size, s.seed))
		ERROR("corrupted block %p (%lu) before realloc", s.p, s.size);
	uint8_t *p = (uint8_t*) timed(t_realloc, [&]{ return realloc(s.p, size); });
	if (not p) {
		// old block must stay valid
		if (not check(s.p, s.size, s.seed))
			ERROR("failed realloc(%p, %u) destroyed the block", s.p, size);
		return false;
	}
	check_block(p, size, "realloc");
	// a shrunk large block keeps only its first 512 patterned bytes
	if (not check(p, size >= s.size ? s.size : std::min<size_t>(size, 512), s.seed))
		ERROR("realloc(%p, %u) -> %p lost the content (old %lu)", s.p, size, p, s.size);
	live_bytes += size; live_bytes -= s.size;
	s.p = p; s.size = size;
	fill(p, size, s.seed);
	return true;
}

// ----------------------------------------------------------------------------
// Intrusive list through allocated blocks: allows filling the whole heap
// without a table. The next pointer is copied byte wise (alignment!).
static void* fill_list(size_t size, uint32_t traits, size_t &count, size_t &bytes, bool by_region=false)
{
	void *head = nullptr;
	count = bytes = 0;
	while (true)
	{
		void *p = traits ? malloc_traits(size, traits) : malloc(size);
		if (not p) break;
		check_block(p, size, "fill");
		if (not in_heap(p, size)) break;
		if (by_region) if (auto *r = find_region(p); r) r->used += size;
		memcpy(p, &head, sizeof(void*));
		head = p;
		count++; bytes += size;
		if (count > 10'000'000) break;
	}
	return head;
}
static void free_list(void *head)
{
	while (head) { void *next; memcpy(&next, head, sizeof(void*)); free(head); head = next; }
}

// ----------------------------------------------------------------------------
static size_t capacity{0}; // bytes of 256B blocks that malloc() can hand out

static void test_info()
{
	for (const auto [traits, start, end, size] : modm::platform::HeapTable())
	{
		if (region_count < 16) regions[region_count++] = {traits.value, start, end, size, 0};
		heap_total += size;
		if ((traits & modm::MemoryDefault) == modm::MemoryDefault) default_size += size;
		LOG("R region %u traits=0x%04x start=%p size=%u\n", region_count-1, traits.value, start, size);
	}
	LOG("R heap_total %u default %u\n", heap_total, default_size);
}

static void test_capacity()
{
	size_t count, bytes;
	void *list = fill_list(256, 0, count, bytes);
	capacity = bytes;
	free_list(list);
	LOG("R capacity256 %u %u%%\n", capacity, unsigned(uint64_t(capacity) * 100 / default_size));
	// After freeing everything the same amount must fit again (leaks, coalescing)
	list = fill_list(256, 0, count, bytes);
	if (bytes + 1024 < capacity)
		ERROR("capacity shrank after free: %u -> %u", capacity, bytes);
	free_list(list);

	// overhead per allocation with small blocks
	for (size_t sz : {1u, 8u, 16u, 24u, 64u})
	{
		list = fill_list(sz, 0, count, bytes);
		free_list(list);
		LOG("R small%u count=%u bytes_per_alloc=%u util=%u%%\n", sz, count,
			count ? default_size / count : 0, unsigned(uint64_t(bytes) * 100 / default_size));
	}
}

static void test_edge_cases()
{
	// all of these must fail cleanly
	const size_t huge[] = {SIZE_MAX, SIZE_MAX - 1, SIZE_MAX - 7, SIZE_MAX - 255, SIZE_MAX / 2 + 1,
						   0x80000000u, heap_total + 1};
	for (size_t s : huge)
	{
		void *p = malloc(s);
		if (p) { ERROR("malloc(%u) returned %p", s, p); free(p); }
	}
	{
		// volatile hides the overflow from -Walloc-size-larger-than
		volatile size_t n = SIZE_MAX / 2 + 2, m = 0x10001;
		void *p = calloc(n, 2);
		if (p) { ERROR("calloc(SIZE_MAX/2+2, 2) overflowed and returned %p", p); free(p); }
		p = calloc(m, m);
		if (p) { ERROR("calloc(0x10001, 0x10001) overflowed and returned %p", p); free(p); }
	}
	free(nullptr);
	// malloc(0) may return null or a unique pointer
	if (void *p = malloc(0); p) { check_block(p, 0, "malloc(0)"); free(p); }

	// calloc must allocate n*m and zero it
	{
		void *dirty = malloc(2048);
		if (dirty) { memset(dirty, 0xa5, 2048); free(dirty); }
		uint8_t *p = (uint8_t*)calloc(37, 29);
		uint8_t *q = (uint8_t*)malloc(512);
		if (p and q)
		{
			check_block(p, 37*29, "calloc");
			for (size_t i = 0; i < 37*29; i++) if (p[i]) { ERROR("calloc(37, 29) not zero at %u", i); break; }
			fill(q, 512, 42);
			memset(p, 0x5a, 37*29);
			if (not check(q, 512, 42)) ERROR("calloc(37, 29) %p allocated less than 1073 bytes, overlaps %p", p, q);
		}
		else if (default_size >= 8192) ERROR("calloc(37, 29)/malloc(512) failed");
		else LOG("R calloc_test skipped, heap too small\n");
		free(q); free(p);
	}

	// sizes around powers of two, also near the largest block
	size_t max_size = 0;
	for (uint32_t k = 4; k < 31; k++)
	{
		for (int32_t d : {-24, -9, -8, -4, -1, 0, 1, 4, 8, 24})
		{
			const size_t s = (size_t(1) << k) + d;
			uint8_t *p = (uint8_t*) malloc(s);
			if (p) {
				check_block(p, s, "malloc2^k");
				// touch first and last byte
				p[0] = 0x11; p[s-1] = 0x22;
				max_size = std::max(max_size, s);
				free(p);
			}
		}
	}
	// largest single block with binary search
	size_t lo = 0, hi = heap_total;
	while (lo < hi) {
		const size_t mid = lo + (hi - lo + 1) / 2;
		if (void *p = malloc(mid); p) { free(p); lo = mid; } else hi = mid - 1;
	}
	LOG("R max_block %u %u%%\n", lo, unsigned(uint64_t(lo) * 100 / default_size));
}

static void test_realloc()
{
	Slot s{};
	if (not slot_alloc(s, 100)) { ERROR("malloc(100) failed"); return; }
	for (size_t size : {200u, 1000u, 50u, 4000u, 8u, 3000u, 1u, 100u})
		if (not slot_realloc(s, size) and size <= default_size / 8) ERROR("realloc(%u) failed", size);
	// realloc(nullptr, n) is malloc
	void *p = realloc(nullptr, 64);
	if (not p) ERROR("realloc(nullptr, 64) failed");
	free(p);
	// realloc way too large must fail and keep the block
	slot_realloc(s, SIZE_MAX - 16);
	slot_realloc(s, heap_total + 1);
	slot_free(s, false);
	live_bytes = 0;
}

static void test_traits()
{
	// which region does each trait combination get?
	const MemoryTraits requests[] = {modm::MemoryDefault, modm::MemoryFastCode, modm::MemoryFastData,
		modm::MemoryDMA, modm::MemoryDMA2D, modm::MemoryMDMA, modm::MemoryExternal, modm::MemoryBackup};
	const char* names[] = {"Default", "FastCode", "FastData", "DMA", "DMA2D", "MDMA", "External", "Backup"};
	uint32_t violations{0};
	for (size_t ii = 0; ii < std::size(requests); ii++)
	{
		const uint16_t t = requests[ii].value;
		bool exists = false;
		for (size_t r = 0; r < region_count; r++) exists |= ((regions[r].traits & t) == t);
		void *p = malloc_traits(64, t);
		const Region *r = p ? find_region(p) : nullptr;
		const int idx = r ? int(r - regions) : -1;
		// a region with exactly these traits exists, it must be chosen
		const bool ok = not exists or (r and (r->traits & t) == t);
		if (not ok) violations++;
		LOG("R trait %s 0x%04x exists=%d region=%d %s\n", names[ii], t, exists, idx, ok ? "ok" : "WRONG");
		free(p);
	}
	LOG("R trait_violations %lu\n", violations);

	// all memory that is reachable with any traits
	void *lists[16]{};
	size_t total{0}, count, bytes;
	for (size_t r = 0; r < region_count; r++)
	{
		lists[r] = fill_list(256, regions[r].traits, count, bytes, true);
		total += bytes;
	}
	for (size_t r = 0; r < region_count; r++)
	{
		LOG("R region_used %u %u%%\n", r, unsigned(uint64_t(regions[r].used) * 100 / std::max<size_t>(regions[r].size, 1)));
		free_list(lists[r]);
	}
	LOG("R coverage %u %u%%\n", total, unsigned(uint64_t(total) * 100 / heap_total));
}

// Allocate with occasional frees until the first failure
template<typename Dist>
static void fragmentation(const char *name, Dist&& dist, uint32_t free_percent)
{
	free_all();
	uint32_t ops{0};
	size_t at_fail{0};
	while (ops++ < 200'000)
	{
		Slot &s = slots[rnd() % HEAP_TEST_SLOTS];
		if (s.p and (rnd() % 100) < free_percent) { slot_free(s, false); continue; }
		if (s.p) slot_free(s, false);
		const size_t size = dist();
		if (not slot_alloc(s, size)) { at_fail = size; break; }
	}
	LOG("R frag %s util=%u%% live=%u fail_size=%u ops=%lu\n", name,
		unsigned(uint64_t(live_bytes) * 100 / default_size), live_bytes, at_fail, ops);
	free_all();
}

static void test_fragmentation()
{
	const size_t unit = std::max<size_t>(default_size / HEAP_TEST_SLOTS, 16);
	fragmentation("uniform", [&]{ return rnd(unit / 4, unit * 2); }, 30);
	fragmentation("bimodal", [&]{ return (rnd() % 10) ? rnd(8, 64) : rnd(unit * 4, unit * 16); }, 30);
	fragmentation("large", [&]{ return rnd(unit * 2, unit * 12); }, 40);
	fragmentation("growing", [&]{ static size_t s{8}; s = (s * 9 / 8) + 1; if (s > unit * 8) s = 8; return s; }, 20);
}

// Random steady-state churn around 70% occupancy with timing
static void test_stress()
{
	free_all();
	t_malloc.reset(); t_free.reset(); t_realloc.reset(); t_calloc.reset();
	const size_t unit = std::max<size_t>(default_size / HEAP_TEST_SLOTS, 16);
	const size_t target = capacity * 7 / 10;
	uint32_t oom{0}, ops{0};
	for (; ops < HEAP_TEST_OPS; ops++)
	{
		Slot &s = slots[rnd() % HEAP_TEST_SLOTS];
		const uint32_t op = rnd() % 100;
		size_t size;
		switch (rnd() % 8) {
			case 0: case 1: case 2: size = rnd(1, 64); break;
			case 3: case 4: size = rnd(1, 512); break;
			case 5: case 6: size = rnd(unit / 2, unit * 2); break;
			default: size = rnd(unit, unit * 6); break;
		}
		if (s.p)
		{
			if (op < 25) { if (not slot_realloc(s, size)) oom++; }
			else if (op < 70 or live_bytes > target) slot_free(s);
		}
		else if (live_bytes < target or op < 20)
		{
			if (not slot_alloc(s, size, op < 10)) oom++;
		}
	}
	LOG("R stress ops=%lu oom=%lu live=%u\n", ops, oom, live_bytes);
	t_malloc.print(); t_free.print(); t_realloc.print(); t_calloc.print();

	// largest block left in the churned heap
	size_t lo = 0, hi = heap_total;
	while (lo < hi) {
		const size_t mid = lo + (hi - lo + 1) / 2;
		if (void *p = malloc(mid); p) { free(p); lo = mid; } else hi = mid - 1;
	}
	LOG("R churn_max_block %u free~%u\n", lo, capacity - live_bytes);
	free_all();
}

// Deterministic timing of fixed sizes in an empty and in a fragmented heap
static void test_timing_fixed()
{
	free_all();
	Timing tm{"m"}, tf{"f"};
	for (int frag = 0; frag < 2; frag++)
	{
		if (frag) {
			// every second block of a full table stays allocated
			const size_t unit = std::max<size_t>(capacity / HEAP_TEST_SLOTS / 2, 16);
			for (auto &s : slots) slot_alloc(s, rnd(8, unit));
			for (size_t ii = 0; ii < HEAP_TEST_SLOTS; ii += 2) slot_free(slots[ii], false);
		}
		for (size_t size : {16u, 256u, 4096u})
		{
			tm.reset(); tf.reset();
			for (int ii = 0; ii < 50; ii++)
			{
				void *p = timed(tm, [&]{ return malloc(size); });
				if (not p) break;
				timed(tf, [&]{ free(p); return 0; });
			}
			LOG("R fixed %s size=%u malloc_max=%lu malloc_avg=%lu free_max=%lu\n", frag ? "frag" : "empty",
				size, tm.max, tm.count ? uint32_t(tm.sum / tm.count) : 0, tf.max);
		}
	}
	free_all();
}

struct alignas(64) Aligned64 { uint8_t data[100]; };

static void test_aligned()
{
	uint32_t unsupported{0};
	for (size_t align : {16u, 32u, 64u, 256u, 1024u})
	{
		void *ptrs[4]{};
		for (auto &p : ptrs)
		{
			p = aligned_alloc(align, 100 + align);
			if (not p) { unsupported++; continue; }
			if (uintptr_t(p) & (align - 1)) ERROR("aligned_alloc(%u) returned %p", align, p);
			fill(p, 100 + align, align);
		}
		for (auto &p : ptrs) {
			if (p and not check(p, 100 + align, align)) ERROR("aligned_alloc(%u) %p corrupted", align, p);
			free(p);
		}
		void *p = memalign(align, 50);
		if (p and (uintptr_t(p) & (align - 1))) ERROR("memalign(%u) returned %p", align, p);
		free(p);
	}
	auto *a = new (std::nothrow) Aligned64;
	if (a and (uintptr_t(a) & 63)) ERROR("new alignas(64) returned %p", a);
	delete a;
	LOG("R aligned unsupported=%lu\n", unsupported);

	// the heap must still be intact afterwards
	size_t count, bytes;
	void *list = fill_list(256, 0, count, bytes);
	free_list(list);
	if (bytes + 2048 < capacity) ERROR("capacity shrank after aligned allocations: %u -> %u", capacity, bytes);
}

// Report crashes instead of rebooting, so that the runner sees them
void modm_abandon(const modm::AssertionInfo &info)
{
	MODM_LOG_INFO.printf("\nASSERTION '%s' context=%lx\n", info.name, uint32_t(info.context));
	MODM_LOG_INFO.flush();
	printTestResult(false);
	while(true) {}
}
extern "C" void HardFault_Handler()
{
	// halt here for fault_dump, the logger cannot run anymore
	asm volatile ("bkpt #0");
	while(true) {}
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	counter.initialize();
	LOG("\nHEAP TEST slots=%u ops=%u\n", HEAP_TEST_SLOTS, HEAP_TEST_OPS);

	test_info();
	test_capacity();
	test_edge_cases();
	test_realloc();
	test_traits();
	test_fragmentation();
	test_stress();
	test_timing_fixed();
	test_aligned();
	// a second round of the stress test after the aligned allocations
	rng_state = 0xcafe1234;
	test_stress();

	LOG("R crossings %lu\n", crossings);
	LOG("R errors %lu\n", errors);
	return finishTest(errors == 0);
}
