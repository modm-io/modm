#include <xpcc/architecture/platform.hpp>

#define XPCC_CAN_MODULE_NAME "can"
#define XPCC_IOBUFFER_MODULE_NAME "iobuffer"
#define XPCC_UART_MODULE_NAME "uart"

using namespace Board;

static xpcc::Abandonment
test_assertion_handler(const char * module,
					   const char * /* location */,
					   const char * /* failure */,
					   uintptr_t /* context */)
{
	if (strcmp(module, XPCC_IOBUFFER_MODULE_NAME) == 0)
		return xpcc::Abandonment::Ignore;
	return xpcc::Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(test_assertion_handler);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	xpcc_assert(true, XPCC_CAN_MODULE_NAME, "init", "timeout");

	xpcc_assert_debug(false, XPCC_IOBUFFER_MODULE_NAME, "tx", "full");

	xpcc_assert(false, XPCC_UART_MODULE_NAME, "init", "mode");

	while(1)
	{
		LedRed::toggle();
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
