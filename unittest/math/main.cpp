
#include "angle_test.hpp"
#include "saturated_test.hpp"

int
main(void)
{
	// initialize stream
	
	// run tests
	AngleTest angleTest;
	
	angleTest.testArithmetic();
	angleTest.testGlobalFunctions();
	
	SaturatedTest saturatedTest;
	
	saturatedTest.test8BitSigned();
	saturatedTest.test8BitUnsigned();
	
	// print summary
	
	
	return 0;
}
