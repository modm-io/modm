// FIXME: this file should be generated automatically

#include <unittest/reporter.hpp>
#include <xpcc/io/backplane/std/std_iodevice.hpp>

#include "angle_test.hpp"
#include "saturated_test.hpp"

namespace
{
	FLASH_STRING(angleTestName) = "angle_test";
	FLASH_STRING(saturatedTestName) = "saturated_test";
}

int
main(void)
{
	// initialize stream
	xpcc::StdIODevice iodevice;
	unittest::Reporter reporter(iodevice);
	
	unittest::Controller::instance().setReporter(reporter);
	
	// run tests
	unittest::Controller::instance().nextTestSuite(xpcc::Flash(angleTestName));
	{
		AngleTest angleTest;
		
		angleTest.setUp();
		angleTest.testArithmetic();
		angleTest.tearDown();
		
		angleTest.setUp();
		angleTest.testGlobalFunctions();
		angleTest.tearDown();
	}
	
	unittest::Controller::instance().nextTestSuite(xpcc::Flash(saturatedTestName));
	{
		SaturatedTest saturatedTest;
		
		saturatedTest.setUp();
		saturatedTest.testSigned();
		saturatedTest.tearDown();
		
		saturatedTest.setUp();
		saturatedTest.testUnsigned();
		saturatedTest.tearDown();
	}
	
	unittest::Controller::instance().getReporter().printSummary();
	
	return 0;
}
