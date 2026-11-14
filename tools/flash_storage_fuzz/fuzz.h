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

// Emulated flash with power loss and the fuzzing harness, see README.md.
// The storage library file defines the hooks below and includes this header.
#pragma once
#include <errno.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------- the model
enum cut_model { CUT_UNIT, CUT_PREFIX, CUT_BITS, CUT_STM32 }; // what an interrupted write leaves
enum erase_model { ERASE_ATOMIC, ERASE_BITS, ERASE_U5, ERASE_H7 }; // what an interrupted erase leaves
enum eccd_read { ECCD_RAW, ECCD_GARBAGE };           // what reading a word with ECCD returns
enum prog_model { PROG_NOR, PROG_CONTENT, PROG_TOUCHED, PROG_STM32 }; // when programming a unit fails
enum when_model { WHEN_OPS, WHEN_TIME };             // when the power fails

static struct
{
	enum cut_model cut;
	enum erase_model erase;
	enum prog_model prog;
	enum when_model when;
	size_t unit;      // program unit in bytes
	size_t sector;    // erase unit in bytes
	size_t size;      // flash size in bytes
	uint32_t write_us, erase_us; // duration per unit and per sector
	int skipff;       // the driver does not program units of all ones
	// CUT_STM32: percent of interrupted units that are still erased, complete,
	// or wrong with ECCC (corrected, reads consistently), the rest has ECCD
	uint32_t p_erased, p_complete, p_eccc;
	enum eccd_read eccd;
	int eccmap;       // the port reads ECCD units as zeros (not erased, invalid)
} model = {CUT_PREFIX, ERASE_BITS, PROG_NOR, WHEN_OPS, 8, 2048, 8192, 80, 20000, 0, 20, 50, 0, ECCD_RAW, 0};

/// Measured on the hardware (modm flash storage.md: "Interrupted program on hardware"
/// and "Interrupted erase, multi-unit programs, timings, ECC reads")
static const struct family
{
	const char *name;
	size_t unit;
	uint32_t write_us, erase_us;
	uint32_t p_erased, p_complete, p_eccc;
	enum cut_model cut;
	enum erase_model erase;
	enum prog_model prog;
	enum eccd_read eccd;
} families[] = {
	// L4/L5/G0/G4: a reset never interrupted the erase (unmeasured for power loss)
	{"l4", 8, 92, 22100, 19, 56, 0, CUT_STM32, ERASE_ATOMIC, PROG_STM32, ECCD_RAW},
	// U5/U3: the erase drifts bits towards one, PROGERR after programming all ones
	{"u5", 16, 125, 1540, 10, 83, 2, CUT_STM32, ERASE_U5, PROG_STM32, ECCD_RAW},
	{"h5", 16, 36, 1550, 26, 60, 12, CUT_STM32, ERASE_ATOMIC, PROG_CONTENT, ECCD_RAW},
	// H7: the erase first programs the sector to zero, ECCD reads return garbage
	{"h7", 32, 86, 930000, 7, 18, 0, CUT_STM32, ERASE_H7, PROG_CONTENT, ECCD_GARBAGE},
	// F2/F4/F7: no ECC, a word is programmed or not, the erase was not interrupted
	{"f4", 1, 4, 190000, 50, 50, 0, CUT_STM32, ERASE_ATOMIC, PROG_NOR, ECCD_RAW},
	// L0/L1: one high voltage phase per word, 3.2ms, interrupted program unmeasured
	{"l0", 4, 3200, 3300, 20, 50, 0, CUT_STM32, ERASE_ATOMIC, PROG_CONTENT, ECCD_RAW},
	{0},
};
static void set_family(const struct family *f)
{
	model.unit = f->unit; model.write_us = f->write_us; model.erase_us = f->erase_us;
	model.p_erased = f->p_erased; model.p_complete = f->p_complete; model.p_eccc = f->p_eccc;
	model.cut = f->cut; model.erase = f->erase; model.prog = f->prog; model.eccd = f->eccd;
	model.when = WHEN_TIME; model.skipff = 1; model.eccmap = 1;
}

// ---------------------------------------------------------------- the flash
#define FLASH_MAX (64 * 1024)
static uint8_t flash[FLASH_MAX];
static uint8_t touched[FLASH_MAX];     // unit programmed since erase (PROG_TOUCHED)
static uint8_t interrupted[FLASH_MAX]; // unit touched by an interrupted operation
static uint8_t ecc_flag[FLASH_MAX];    // unit reads with ECCC (1) or ECCD (2), cannot be programmed
static uint8_t ffprog[FLASH_MAX];      // unit programmed with all ones, cannot be programmed (PROG_STM32)
static long power_left = -1;
static uint32_t cut_permille;  // how far the interrupted operation got
static int trace;           // operations or microseconds until power loss
static jmp_buf power_loss;
static struct
{
	unsigned long erases, bytes, double_writes, residue_writes, bad_reads;
	const char *op;
	uint32_t addr;
} stats;

static uint32_t rng = 1;
static uint32_t xorshift(uint32_t *x) { *x ^= *x << 13; *x ^= *x >> 17; *x ^= *x << 5; return *x; }
#ifdef LIBFUZZER
// The fuzzer input decides the structure: operations, values, when the power
// fails and the kind of damage. The damaged bits themselves come from drnd(),
// so that the fuzzer cannot construct CRC collisions byte by byte.
static const uint8_t *input;
static size_t input_left;
static int input_end;
static uint32_t drng = 1;
static uint32_t rnd(uint32_t max)
{
	if (!max) return 0;
	const size_t bytes = max <= 0x100 ? 1 : max <= 0x10000 ? 2 : 4;
	if (input_left < bytes) { input_end = 1; input_left = 0; return 0; }
	uint32_t v = 0;
	for (size_t i = 0; i < bytes; i++) v = v << 8 | *input++;
	input_left -= bytes;
	return v % max;
}
static uint32_t drnd(uint32_t max) { return xorshift(&drng) % max; }
#else
static uint32_t rnd(uint32_t max) { return max ? xorshift(&rng) % max : 0; }
#define drnd rnd
#endif

static int all_ones(const uint8_t *b)
{
	for (size_t i = 0; i < model.unit; i++) if (b[i] != 0xff) return 0;
	return 1;
}

static int unit_erased(uint32_t u)
{
	for (size_t i = 0; i < model.unit; i++) if (flash[u * model.unit + i] != 0xff) return 0;
	return 1;
}

/// @return the number of bytes (or 1 for erases) before the power fails, or -1
static long power_cut(size_t units, uint32_t us_per_unit)
{
	if (power_left < 0) return -1;
	if (model.when == WHEN_OPS) {
		if (power_left-- == 0) { cut_permille = rnd(1000); return rnd(units); }
		return -1;
	}
	long duration = (long)units * us_per_unit;
	if (power_left < duration) {
		const long cut = power_left / us_per_unit;
		cut_permille = (uint64_t)(power_left % us_per_unit) * 1000 / us_per_unit;
		power_left = -1;
		return cut;
	}
	power_left -= duration;
	return -1;
}

/// Out of range reads fail like the range check of a port. Words with ECCC read
/// back as their corrected (wrong) data, words with ECCD as their raw bytes or
/// garbage, or as zeros if the port maps them (eccmap).
static int emu_read(uint32_t a, void *b, size_t n)
{
	if (a > model.size || n > model.size - a) { stats.bad_reads++; return -EINVAL; }
	memcpy(b, flash + a, n);
	if (n) for (uint32_t u = a / model.unit; u <= (a + n - 1) / model.unit; u++) {
		if (ecc_flag[u] != 2) continue;
		for (uint32_t i = u * model.unit; i < (u + 1) * model.unit; i++)
			if (i >= a && i < a + n)
				((uint8_t *)b)[i - a] = model.eccmap ? 0 : model.eccd == ECCD_GARBAGE ? drnd(256) : flash[i];
	}
	return 0;
}

static int emu_write(uint32_t a, const void *data, size_t n)
{
	const uint8_t *b = data;
	if (a % model.unit || n % model.unit || a + n > model.size) {
		printf("FATAL: unaligned write 0x%x+%zu\n", (unsigned)a, n); exit(2);
	}
	for (uint32_t u = a / model.unit; u < (a + n) / model.unit; u++) {
		if (model.skipff && all_ones(b + (u * model.unit - a))) continue; // not programmed
		const int fails = (model.prog == PROG_CONTENT && (!unit_erased(u) || ecc_flag[u])) ||
						  (model.prog == PROG_TOUCHED && touched[u]) ||
						  (model.prog == PROG_STM32 && (!unit_erased(u) || ecc_flag[u] || ffprog[u]));
		if (fails) {
			if (trace) printf("    write 0x%04x+%zu FAILED at 0x%04x%s\n", (unsigned)a, n, (unsigned)(u * model.unit), interrupted[u] ? " (residue)" : "");
			if (interrupted[u]) stats.residue_writes++; else stats.double_writes++;
			return -EIO;
		}
	}
	const long cut = power_cut(n / model.unit, model.write_us); // in units
	if (trace) printf("    write 0x%04x+%zu%s\n", (unsigned)a, n, cut < 0 ? "" : " CUT");
	const size_t whole = cut < 0 ? n : (size_t)cut * model.unit;
	for (size_t i = 0; i < whole; i++) flash[a + i] &= b[i];
	for (size_t u = a / model.unit; u < (a + whole) / model.unit; u++) {
		const int ones = all_ones(b + (u * model.unit - a));
		if (model.skipff && ones) continue;
		touched[u] = 1;
		ffprog[u] |= ones; // L4/U5: PROGERR on the next program, although it reads erased
	}
	stats.bytes += whole;
	if (cut < 0) return 0;
	// the interrupted unit
	const uint32_t u = (a + whole) / model.unit, base = u * model.unit;
	if (model.skipff && all_ones(b + whole)) { // the driver did not program it
		stats.op = "write"; stats.addr = base;
		longjmp(power_loss, 1);
	}
	switch (model.cut) {
	case CUT_UNIT: break; // nothing of this unit was programmed
	case CUT_PREFIX: { // the first bytes, like the Zephyr flash simulator
		size_t k = rnd(model.unit);
		for (size_t i = 0; i < k; i++) flash[base + i] &= b[whole + i];
		touched[u] = k > 0;
		break;
	}
	case CUT_STM32: { // measured: erased, complete or flagged by the ECC
		const uint32_t r = rnd(100);
		if (r < model.p_erased) break;
		const int damaged = r >= model.p_erased + model.p_complete;
		for (size_t i = 0; i < model.unit; i++)
			flash[base + i] &= b[whole + i] | (damaged ? (uint8_t)drnd(256) : 0);
		touched[u] = 1;
		// a word that reads as erased had no ECC flag on the hardware and could be programmed
		if (damaged && !unit_erased(u))
			ecc_flag[u] = r < model.p_erased + model.p_complete + model.p_eccc ? 1 : 2;
		break;
	}
	case CUT_BITS: // a random subset of the bits that should be cleared
		for (size_t i = 0; i < model.unit; i++) flash[base + i] &= b[whole + i] | (uint8_t)drnd(256);
		touched[u] = 1; // the program pulse was applied
		break;
	}
	// L4/U5: only an all ones program interrupted early stays programmable
	if (all_ones(b + whole)) ffprog[u] = model.cut == CUT_STM32 ? rnd(100) >= 20 : 1;
	interrupted[u] = 1;
	stats.op = "write"; stats.addr = base;
	longjmp(power_loss, 1);
}

static int emu_erase(uint32_t a, size_t n)
{
	if (a % model.sector || n % model.sector) { printf("FATAL: unaligned erase\n"); exit(2); }
	const long cut = power_cut(n / model.sector, model.erase_us); // in sectors
	if (trace) printf("    erase 0x%04x+%zu%s\n", (unsigned)a, n, cut < 0 ? "" : " CUT");
	const size_t whole = cut < 0 ? n : (size_t)cut * model.sector;
	memset(flash + a, 0xff, whole);
	memset(touched + a / model.unit, 0, whole / model.unit);
	memset(interrupted + a / model.unit, 0, whole / model.unit);
	memset(ecc_flag + a / model.unit, 0, whole / model.unit);
	memset(ffprog + a / model.unit, 0, whole / model.unit);
	stats.erases += whole / model.sector;
	if (cut < 0) return 0;
	const uint32_t s = a + whole;
	const uint32_t f = cut_permille, words = model.sector / model.unit;
	switch (model.erase) {
	case ERASE_ATOMIC: break;
	case ERASE_BITS:
		for (size_t i = 0; i < model.sector; i++) flash[s + i] |= drnd(256);
		for (size_t u = s / model.unit; u < (s + model.sector) / model.unit; u++)
			interrupted[u] = 1, ecc_flag[u] = unit_erased(u) ? 0 : 1 + drnd(2); // partially erased words fail ECC
		break;
	case ERASE_U5: // bits drift towards one, erased words scattered from 12%, clean from 60%
		if (f >= 600) { memset(flash + s, 0xff, model.sector); break; }
		for (size_t u = s / model.unit; u < s / model.unit + words; u++) {
			uint8_t *w = flash + u * model.unit;
			if (f >= 120 && drnd(480) < f - 120) memset(w, 0xff, model.unit);
			else for (size_t i = 0; i < model.unit; i++)
				for (int bit = 0; bit < 8; bit++) if (drnd(600) < f) w[i] |= 1 << bit;
			interrupted[u] = 1;
			ecc_flag[u] = unit_erased(u) ? 0 : 1 + drnd(2);
		}
		break;
	case ERASE_H7: // programs the sector to zero in order (0-15%), then garbage, ECCD, clean from 50%
		for (size_t u = s / model.unit; u < s / model.unit + words; u++) {
			uint8_t *w = flash + u * model.unit;
			const size_t index = u - s / model.unit;
			interrupted[u] = 1;
			if (f < 150) { if (index * 150 < (size_t)f * words) memset(w, 0, model.unit); }
			else if (f < 360) for (size_t i = 0; i < model.unit; i++) w[i] = drnd(256);
			else if (f < 500 && drnd(140) >= f - 360) {
				for (size_t i = 0; i < model.unit; i++) w[i] = drnd(256);
				ecc_flag[u] = 2;
			}
			else memset(w, 0xff, model.unit);
			if (unit_erased(u)) ecc_flag[u] = 0;
		}
		break;
	}
	stats.op = "erase"; stats.addr = s;
	longjmp(power_loss, 1);
}

// ---------------------------------------------------------------- the hooks
static void lib_format(void);       // erase and initialize the storage
static int lib_mount(void);         // after a power loss, 0 on success
static void lib_step(void);         // one random operation, updates the model
static void lib_verify(void);       // compares all data with the model
static unsigned long lib_updates(void);

static char failure[256];
#define FAIL(...) do { if (!failure[0]) snprintf(failure, sizeof failure, __VA_ARGS__); } while (0)

// ---------------------------------------------------------------- the harness
static int parse(const char *arg, const char *name, const char *const *values, int *out)
{
	size_t len = strlen(name);
	if (strncmp(arg, name, len) || arg[len] != '=') return 0;
	for (int i = 0; values[i]; i++) if (!strcmp(arg + len + 1, values[i])) { *out = i; return 1; }
	printf("unknown %s\n", arg); exit(2);
}

static const char *const cuts[] = {"unit", "prefix", "bits", "stm32", 0}, *const erases[] = {"atomic", "bits", "u5", "h7", 0},
	*const progs[] = {"nor", "content", "touched", "stm32", 0}, *const whens[] = {"ops", "time", 0};
static unsigned long nseeds = 10, first_seed = 1, cap = 1000, wear = 0;
static int nopower;

/// @return 0 for an unknown option
static int option(const char *arg)
{
	int v;
	if (parse(arg, "cut", cuts, &v)) model.cut = v;
	else if (parse(arg, "erase", erases, &v)) model.erase = v;
	else if (parse(arg, "prog", progs, &v)) model.prog = v;
	else if (parse(arg, "when", whens, &v)) model.when = v;
	else if (!strncmp(arg, "unit=", 5)) model.unit = strtoul(arg + 5, 0, 0);
	else if (!strncmp(arg, "seeds=", 6)) nseeds = strtoul(arg + 6, 0, 0);
	else if (!strncmp(arg, "seed=", 5)) first_seed = strtoul(arg + 5, 0, 0), nseeds = 1;
	else if (!strncmp(arg, "cap=", 4)) cap = strtoul(arg + 4, 0, 0);
	else if (!strncmp(arg, "trace=", 6)) trace = strtoul(arg + 6, 0, 0);
	else if (!strncmp(arg, "skipff=", 7)) model.skipff = strtoul(arg + 7, 0, 0);
	else if (!strncmp(arg, "wear=", 5)) wear = strtoul(arg + 5, 0, 0);
	else if (!strncmp(arg, "nopower=", 8)) nopower = strtoul(arg + 8, 0, 0);
	else if (!strncmp(arg, "eccmap=", 7)) model.eccmap = strtoul(arg + 7, 0, 0);
	else if (!strncmp(arg, "family=", 7)) {
		const struct family *f = families;
		while (f->name && strcmp(f->name, arg + 7)) f++;
		if (!f->name) { printf("unknown %s\n", arg); exit(2); }
		set_family(f);
	}
	else return 0;
	return 1;
}

static void print_model(void)
{
	printf("%s cut=%s erase=%s prog=%s when=%s unit=%zu%s%s%s%s: ", LIB_NAME, cuts[model.cut], erases[model.erase],
		   progs[model.prog], whens[model.when], model.unit, model.skipff ? " skipff=1" : "",
		   model.eccmap ? " eccmap=1" : "", model.eccd == ECCD_GARBAGE ? " eccd=garbage" : "",
		   nopower ? " nopower=1" : "");
}

static void flash_reset(void)
{
	memset(flash, 0xff, sizeof flash); memset(touched, 0, sizeof touched);
	memset(interrupted, 0, sizeof interrupted); memset(&stats, 0, sizeof stats);
	memset(ecc_flag, 0, sizeof ecc_flag); memset(ffprog, 0, sizeof ffprog);
	failure[0] = 0; power_left = -1;
}

#ifdef LIBFUZZER
// ---------------------------------------------------------------- libFuzzer
// The model comes from the FUZZ_MODEL environment variable, e.g.
// FUZZ_MODEL="cut=stm32 erase=bits prog=stm32 when=time unit=8 skipff=1".
// Every input is one run from erased flash with up to 32 power losses.
static int run_input(const uint8_t *data, size_t size, uint32_t damage_seed)
{
	input = data; input_left = size; input_end = 0; drng = damage_seed * 2654435761u | 1;
	flash_reset();
	lib_format();
	volatile unsigned resets = 0;
	while (!failure[0] && !input_end && resets < 32) {
		if (setjmp(power_loss) == 0) {
			const uint32_t choice = rnd(256);
			power_left = (nopower || choice < 32) ? -1 :
						 model.when == WHEN_OPS ? (long)rnd(200) : (long)rnd(2000000);
			while (!failure[0] && !input_end) lib_step();
			power_left = -1;
			break;
		}
		power_left = -1;
		resets++;
		if (trace) printf("  --- reset %u\n", resets);
		if (lib_mount()) break;
		lib_verify();
	}
	// Everything must also survive a regular reboot
	if (!failure[0]) {
		if (trace) printf("  --- reboot\n");
		if (!lib_mount()) lib_verify();
	}
	return failure[0] != 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc; (void)argv;
	char buf[512];
	const char *env = getenv("FUZZ_MODEL");
	snprintf(buf, sizeof buf, "%s", env ? env : "");
	for (char *arg = strtok(buf, " "); arg; arg = strtok(NULL, " "))
		if (!option(arg)) { printf("unknown %s\n", arg); exit(2); }
	setvbuf(stdout, NULL, _IONBF, 0);
	print_model();
	printf("\n");
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	if (!run_input(data, size, 1)) return 0;
	char first_failure[sizeof failure];
	memcpy(first_failure, failure, sizeof failure);
	// A failure that needs particular damaged bits, like a CRC collision, does
	// not repeat with other damage: only report the robust ones.
	int fails = 1;
	for (uint32_t seed = 2; seed <= 8; seed++) fails += run_input(data, size, seed);
	if (fails < 6) return 0;
	printf("FAILURE with %d of 8 damage seeds: %s\n", fails, first_failure);
	abort(); // libFuzzer saves the input on SIGABRT, not on SIGTRAP
}
#else
int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) if (!option(argv[i])) { printf("unknown %s\n", argv[i]); return 2; }
	setvbuf(stdout, NULL, _IONBF, 0);
	print_model();

	if (wear) {
		// Without power loss: wear and double writes of the library itself
		rng = 1; memset(flash, 0xff, sizeof flash); memset(touched, 0, sizeof touched);
		lib_format();
		memset(&stats, 0, sizeof stats);
		unsigned long u0 = lib_updates();
		for (unsigned long i = 0; i < wear && !failure[0]; i++) lib_step();
		const double k = 1000.0 / (lib_updates() - u0);
		printf("per 1000 updates %.1f erases, %.0f bytes, %lu double writes%s%s\n", stats.erases * k,
			   stats.bytes * k, stats.double_writes, failure[0] ? ", FAILED: " : "", failure);
		return failure[0] != 0;
	}

	unsigned long survived[1000], capped = 0;
	for (unsigned long seed = first_seed; seed < first_seed + nseeds && seed - first_seed < 1000; seed++) {
		rng = seed * 2654435761u;
		flash_reset();
		lib_format();
		unsigned long resets = 0;
		while (!failure[0] && resets < cap) {
			if (setjmp(power_loss) == 0) {
				power_left = model.when == WHEN_OPS ? (long)rnd(200) : (long)rnd(2000000);
				while (!failure[0]) lib_step();
				break; // a failure without power loss
			}
			power_left = -1;
			resets++;
			if (trace) printf("  --- reset %lu\n", resets);
			if (lib_mount()) break;
			lib_verify();
		}
		survived[seed - first_seed] = resets;
		if (!failure[0]) capped++;
		else printf("\n  seed %lu after %lu resets (%s 0x%x, %lu double, %lu residue): %s", seed, resets,
					stats.op ? stats.op : "-", (unsigned)stats.addr, stats.double_writes,
					stats.residue_writes, failure);
	}
	unsigned long min = cap;
	for (unsigned long s = 0; s < nseeds; s++) if (survived[s] < min) min = survived[s];
	printf("\n  => %lu of %lu seeds survived %lu resets, first failure after %lu resets\n",
		   capped, nseeds, cap, capped == nseeds ? cap : min);
	return capped != nseeds;
}
#endif
