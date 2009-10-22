
#include <unittest/reporter.hpp>

#include "angle_test.hpp"
#include "saturated_test.hpp"

#include <xpcc/io/backplane/std/std_iodevice.hpp>

int
main(void)
{
	// initialize stream
	xpcc::StdIODevice iodevice;
	unittest::Reporter reporter(iodevice);
	
	
	// run tests
	AngleTest angleTest(reporter);
	
	angleTest.setUp();
	angleTest.testArithmetic();
	angleTest.tearDown();
	
	angleTest.setUp();
	angleTest.testGlobalFunctions();
	angleTest.tearDown();
	
	
	SaturatedTest saturatedTest(reporter);
	
	saturatedTest.setUp();
	saturatedTest.test8BitSigned();
	angleTest.tearDown();
	
	saturatedTest.setUp();
	saturatedTest.test8BitUnsigned();
	angleTest.tearDown();
	
	
	// print summary
	reporter.printSummary();
	
	return 0;
}
