
#ifndef XILINX_SPARTAN3_IMPL_HPP_
#define XILINX_SPARTAN3_IMPL_HPP_

#ifndef XPCC__XILINX_SPARTAN_3
#error "Please include xilinx_spartan3.hpp instead."
#endif


template <	typename Cclk,
			typename Din,
			typename ProgB,
			typename InitB,
			typename Done,
			typename DataSource >
bool
xpcc::XilinxSpartan3<Cclk, Din, ProgB, InitB, Done, DataSource>::configure()
{
	return true;
}

#endif /* XILINX_SPARTAN3_IMPL_HPP_ */
