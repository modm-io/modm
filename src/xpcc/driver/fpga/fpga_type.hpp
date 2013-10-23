#ifndef XPCC__FPGA_TYPE
#define XPCC__FPGA_TYPE

namespace xpcc
{

struct Xilinx
{
	/*
	 * Device  | Configuration | Configuration
	 *         |  Size [Bits]  |  Size [Bytes]
	 * ------- | ------------- | --------------
	 * XC3S50  |     439,264   |     54,908
	 * XC3S200 |   1,047,616   |    130,952
	 * XC3S400 |   1,699,136   |    212,392
	 * 6SLX9   |   2,742,528   |    342,816
	 *
	 */
	enum class
	FpgaType : uint32_t
	{
		Spartan3XC3S50  =  53908,
		Spartan3XC3S200 = 130952,
		Spartan3XC3S400 = 212392,
		Spartan6SLX9 	= 342816,
	};
};

}

#endif
