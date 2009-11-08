
extern "C"
{
	/**
	 * @brief Pure-virtual workaround.
	 *
	 * The avr-libc does not support a default implementation for handling 
	 * possible pure-virtual calls. This is a short and empty workaround for this.
	 */
	void
	__cxa_pure_virtual()
	{
		// put error handling here
	}
	
	__extension__ typedef int __guard __attribute__((mode (__DI__)));
	
	int
	__cxa_guard_acquire(__guard *g) {
		return !*(char *)(g);
	}
	
	void
	__cxa_guard_release (__guard *g) {
		*(char *)g = 1;
	}
	
	void
	__cxa_guard_abort (__guard *) {
	}
	
	int
	__cxa_atexit (void (* destructor)(void *), void *object, void *dso_handle) {
		// TODO!
		// http://sourceware.org/ml/ecos-discuss/2005-02/msg00056.html
		// http://lists.debian.org/debian-gcc/2003/07/msg00057.html
		// http://gcc.gnu.org/ml/gcc-patches/2004-09/msg01650.html
		(void) destructor;
		(void) object;
		(void) dso_handle;
		
		return 0;
	}
	
	// TODO!
	void* __dso_handle = (void*) &__dso_handle;
}
