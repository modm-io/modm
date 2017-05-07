#include <xpcc/architecture/platform.hpp>

using namespace Board;

static xpcc::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */,
					   uintptr_t /* context */)
{
	if (!strcmp(module, "iobuffer")) {
		XPCC_LOG_ERROR << "Ignoring iobuffer full!" << xpcc::endl;
		return xpcc::Abandonment::Ignore;
	}
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler);

static xpcc::Abandonment
core_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * failure,
					   uintptr_t context)
{
	if (!memcmp(module, "core\0nvic\0undefined", 19)) {
		XPCC_LOG_ERROR.printf("Ignoring undefined IRQ handler %d!\n", context);
		return xpcc::Abandonment::Ignore;
	}
	if (!memcmp(module, "core\0heap", 9)) {
		XPCC_LOG_ERROR.printf("Ignoring 'core.heap.%s' of size 0x%x!\n", failure, context);
		return xpcc::Abandonment::Ignore;
	}
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(core_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// trigger an IRQ with undefined handler
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPendingIRQ(RTC_Alarm_IRQn);

	// trigger an out of memory
	// we definitely don't have 32MB RAM on this board
	// returns NULL, asserts in debug mode
	volatile void * ptr = malloc(1 << 25);
	// returns NULL, asserts in debug mode
	ptr = new (std::nothrow) uint8_t[1 << 25];
	// always asserts
	ptr = new uint8_t[1 << 25];
	(void) ptr;

	// does not fail, should not be optimized away
	volatile bool true_condition = true;
	xpcc_assert(true_condition, "can", "init", "timeout");

	// only fails for debug builds, but is ignored anyways
	xpcc_assert_debug(false, "iobuffer", "tx", "full");

	// "accidentally" return from main, without even returning properly!
	// This should be cought by the debug assert core.main.exit!
	// while(1) ;
	// return 0;
}
