#include <xpcc/io/iostream.hpp>
#include <xpcc/architecture/interface/i2c.hpp>

namespace xpcc
{

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::I2c::Operation op)
{
	switch (op)
	{
		case xpcc::I2c::Operation::Stop:    s << "Stop";    break;
		case xpcc::I2c::Operation::Restart: s << "Restart"; break;
		case xpcc::I2c::Operation::Write:   s << "Write";   break;
		case xpcc::I2c::Operation::Read:    s << "Read";    break;
	}
	return s;
}

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::I2c::OperationAfterStart op)
{
	s << static_cast<xpcc::I2c::Operation>(op);
	return s;
}

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::I2c::OperationAfterRead op)
{
	s << static_cast<xpcc::I2c::Operation>(op);
	return s;
}

xpcc::IOStream&
operator << (xpcc::IOStream& s, const xpcc::I2c::OperationAfterWrite op)
{
	s << static_cast<xpcc::I2c::Operation>(op);
	return s;
}

}
