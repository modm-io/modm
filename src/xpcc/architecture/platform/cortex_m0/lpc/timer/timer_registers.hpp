#ifndef XPCC_LPC11__TIMER_REGISTERS_HPP
#define XPCC_LPC11__TIMER_REGISTERS_HPP

namespace xpcc
{
	namespace lpc11
	{
		enum TMR_TCR
		{
			TCR_CEn 	= (1 << 0),		///< Counter Enable.
			TCR_CRst 	= (1 << 1),		///< Counter Reset.
		};


	}
}

#endif // XPCC_LPC11__TIMER_REGISTERS_HPP
