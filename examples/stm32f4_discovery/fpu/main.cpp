#include <xpcc/architecture/platform.hpp>
#include <xpcc/math.hpp>

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	while (1)
	{
		xpcc::Vector<float, 2> v(1.0f, 2.0f);
		v.getLength();

		xpcc::Quaternion<float> q;
		q.getLength();
	}
}
