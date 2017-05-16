#ifndef XPCC_DS1302_HPP
#error "Do not include ds1302_impl.hpp. Only include ds1302.hpp"
#endif

template < class PinSet >
void
xpcc::Ds1302< PinSet >::initialize()
{
	Ce::reset();
	Sclk::reset();
	Io::setInput();
}

template< class PinSet >
void
xpcc::Ds1302< PinSet >::writeByte(uint8_t byte)
{
	xpcc::delayMicroseconds(delay_ce);
	Io::setOutput();

	for (uint8_t ii = 8; ii > 0; --ii)
	{
		xpcc::delayMicroseconds(delay_clk);
		Sclk::reset();
		Io::set(byte & 0x01);
		xpcc::delayMicroseconds(delay_clk);
		Sclk::set();

		byte >>= 1;
	}
	xpcc::delayMicroseconds(delay_clk);
}

template< class PinSet >
void
xpcc::Ds1302< PinSet >::write(const uint8_t addr, const uint8_t data)
{
	Ce::set();
	writeByte(addr);
	writeByte(data);

	// Cleanup
	Sclk::reset();
	xpcc::delayMicroseconds(delay_clk);
	Io::setInput();
	Ce::reset();
	xpcc::delayMicroseconds(delay_ce);
}

template< class PinSet >
uint8_t
xpcc::Ds1302< PinSet >::read(const uint8_t addr)
{
	Ce::set();
	writeByte(addr);

	uint8_t ret = 0;

	Io::setInput();
	xpcc::delayMicroseconds(delay_clk);
	for (uint8_t ii = 8; ii > 0; --ii)
	{
		bool rr = Io::read();
		ret >>= 1;
		ret |= (rr << 7);
		Sclk::set();
		xpcc::delayMicroseconds(delay_clk);
		Sclk::reset();
		xpcc::delayMicroseconds(delay_clk);
	}

	Ce::reset();

	return ret;
}


template< class PinSet >
void
xpcc::Ds1302< PinSet >::readRtc(ds1302::Data &storage)
{
	Ce::set();
	writeByte(0xbf); // Burst Read
	Io::setInput();

	// Falling edge of SCLK will trigger DS1302 output
	Sclk::reset();

	// Wait for stable output
	xpcc::delayMicroseconds(delay_ce);

	for (uint8_t jj = 0; jj < XPCC_ARRAY_SIZE(storage.data); ++jj)
	{
		uint8_t ret = 0;
		for (uint8_t ii = 8; ii > 0; --ii)
		{
			bool rr = Io::read();
			ret >>= 1;
			ret |= (rr << 7);
			Sclk::set();
			xpcc::delayMicroseconds(delay_clk);

			// Falling edge of SCLK will trigger DS1302 output
			Sclk::reset();
			xpcc::delayMicroseconds(delay_clk);
		}
		storage.data[jj] = ret;
	}

	Ce::reset();
}
