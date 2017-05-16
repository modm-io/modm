#include <modm/io/iostream.hpp>
#include <modm/architecture/interface/i2c.hpp>

namespace modm
{

modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::Operation op)
{
	switch (op)
	{
		case modm::I2c::Operation::Stop:    s << "Stop";    break;
		case modm::I2c::Operation::Restart: s << "Restart"; break;
		case modm::I2c::Operation::Write:   s << "Write";   break;
		case modm::I2c::Operation::Read:    s << "Read";    break;
	}
	return s;
}

modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterStart op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterRead op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

modm::IOStream&
operator << (modm::IOStream& s, const modm::I2c::OperationAfterWrite op)
{
	s << static_cast<modm::I2c::Operation>(op);
	return s;
}

}
