/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <sys/stat.h>
#include <modm/architecture/interface/assert.h>

// ------------------------------------------------------------------------
modm_weak void _exit(int status)
{
	modm_assert(false, "libc.exit",
			"The libc exit(status) function was called!", status);
}

modm_weak void abort(void)
{
	modm_assert(false, "libc.abort",
			"The libc abort() function was called!");
}

modm_weak int atexit(void (*fn)(void))
{
	(void) fn;
	return 0;
}

// ------------------------------------------------------------------------
// Minimal stubs of Newlib system calls.
// See https://sourceware.org/newlib/libc.html#Syscalls

modm_weak int _close(int file)
{
	(void) file;
	return -1;
}

modm_weak int _fstat(int file, struct stat *st)
{
	(void) file;
	st->st_mode = S_IFCHR;
	return 0;
}

modm_weak int _isatty(int file)
{
	(void) file;
	return 1;
}

modm_weak int _lseek(int file, int ptr, int dir)
{
	(void) file;
	(void) ptr;
	(void) dir;
	return 0;
}

modm_weak void _kill(int pid, int sig)
{
	(void) pid;
	(void) sig;
}

modm_weak int _getpid(void)
{
	return -1;
}

modm_weak int _write(int file, char *ptr, int len)
{
	(void) file;
	(void) ptr;
	(void) len;
	return -1;
}

modm_weak int _read(int file, char *ptr, int len)
{
	(void) file;
	(void) ptr;
	(void) len;
	return -1;
}

modm_weak int _gettimeofday(void *tp, void *tzp)
{
	(void) tp;
	(void) tzp;
	return -1;
}
