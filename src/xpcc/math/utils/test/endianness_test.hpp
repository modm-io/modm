
#include <unittest/testsuite.hpp>

class EndiannessTest : public unittest::TestSuite
{
public:
	void
	testToBigEndian();

	void
	testToLittleEndian();

	void
	testFromBigEndian();

	void
	testFromLittleEndian();
};
