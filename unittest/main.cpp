// FIXME: this file should be generated automatically

#include <unittest/reporter.hpp>
#include <unittest/controller.hpp>

#include <xpcc/io/iodevice.hpp>
#include <xpcc/io/backplane/std/std_iodevice.hpp>

#include "math/angle_test.hpp"
#include "math/saturated_test.hpp"

#include "io/io_stream_test.hpp"

namespace
{
	FLASH_STRING(angleTestName) = "angle";
	FLASH_STRING(saturatedTestName) = "saturated";
	FLASH_STRING(ioStreamTestName) = "io_stream";
}

class DummyDevice : public xpcc::IODevice
{
	virtual void
	put(char) {};

	using IODevice::put;

	virtual void
	flush() {};

	virtual bool
	get(char&) { return false; };
};

int
main(void)
{
	// initialize stream
	xpcc::StdIODevice device;
	//DummyDevice device;
	unittest::Reporter reporter(device);
	
	unittest::Controller::instance().setReporter(reporter);
	
	// run tests
	unittest::Controller::instance().nextTestSuite(xpcc::Flash(angleTestName));
	{
		AngleTest angleTest;
		
		angleTest.setUp();
		angleTest.testConstructor();
		angleTest.tearDown();
		
		angleTest.setUp();
		angleTest.testAssignment();
		angleTest.tearDown();
		
		angleTest.setUp();
		angleTest.testNormalize();
		angleTest.tearDown();
		
		angleTest.setUp();
		angleTest.testReverse();
		angleTest.tearDown();
		
		angleTest.setUp();
		angleTest.testGlobalFunctions();
		angleTest.tearDown();
	}
	
	unittest::Controller::instance().nextTestSuite(xpcc::Flash(ioStreamTestName));
	{
		IoStreamTest ioStreamTest;
		
		ioStreamTest.setUp();
		ioStreamTest.testString();
		ioStreamTest.tearDown();
		
		ioStreamTest.setUp();
		ioStreamTest.testFlashString();
		ioStreamTest.tearDown();
		
		ioStreamTest.setUp();
		ioStreamTest.testShortInteger();
		ioStreamTest.tearDown();
		
		ioStreamTest.setUp();
		ioStreamTest.testInteger();
		ioStreamTest.tearDown();
		
		ioStreamTest.setUp();
		ioStreamTest.testFloat();
		ioStreamTest.tearDown();
	}
	
	/*unittest::Controller::instance().nextTestSuite(xpcc::Flash(saturatedTestName));
	{
		SaturatedTest saturatedTest;
		
		saturatedTest.setUp();
		saturatedTest.testSigned();
		saturatedTest.tearDown();
		
		saturatedTest.setUp();
		saturatedTest.testUnsigned();
		saturatedTest.tearDown();
	}*/
	
	unittest::Controller::instance().getReporter().printSummary();
	
	return 0;
}
