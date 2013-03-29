#ifndef	XPCC__CLOCK_DUMMY_HPP
#define XPCC__CLOCK_DUMMY_HPP

#include <xpcc/workflow/timestamp.hpp>

namespace xpcc
{
	/**
	 * \brief	Dummy implementation of a clock where the user can control the time.
	 *
	 * This is useful in unittests where a precise control of the system time is necessary.
	 */
	class ClockDummy
	{
	public:
		static xpcc::Timestamp
		now()
		{
			return time;
		}

		static void
		setTime(const Timestamp::Type time)
		{
			ClockDummy::time = time;
		}

	private:
		static Timestamp::Type time;
	};
}

#endif // XPCC__CLOCK_DUMMY_HPP
