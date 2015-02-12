#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

// ST changed the accelerometer in the C revision (MB997C)
// change this to `false`, if you have MB997A or MB997B!
#define REVISION_C true

// if you want to use Software I2c change this to `true`
#define USE_I2C false

#if REVISION_C
#	include <xpcc/driver/inertial/lis3dsh.hpp>
#else
#	include <xpcc/driver/inertial/lis302dl.hpp>
#endif

namespace lis
{

// Spi Transport Layer
typedef GpioOutputA5 Sck;
typedef GpioInputA6 Miso;
typedef GpioOutputA7 Mosi;
typedef GpioOutputE3 Cs;
typedef SpiMaster1 SpiMaster;

// I2c Transport Layer
typedef GpioA5 Scl;
typedef GpioA7 Sda;
typedef xpcc::SoftwareI2cMaster<Scl, Sda, 400000> I2cMaster;

// Interrupt lines
typedef GpioInputE0 Int1;
typedef GpioInputE1 Int2;

#if USE_I2C
typedef xpcc::Lis3TransportI2c< I2cMaster > Transport;
#else
typedef xpcc::Lis3TransportSpi< SpiMaster, Cs > Transport;
#endif

} // namespace lis

// Data and Driver object
#if REVISION_C
xpcc::lis3dsh::Data data;
xpcc::Lis3dsh< lis::Transport > accel(data);
#else
xpcc::lis302dl::Data data;
xpcc::Lis302dl< lis::Transport > accel(data);
#endif

#include <xpcc/processing.hpp>
#include <xpcc/math.hpp>

class ReaderThread : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(accel.ping()))
				break;
			// otherwise, try again in 100ms
			this->timeout.restart(100);
			LedOrange::set();
			PT_WAIT_UNTIL(this->timeout.isExpired());
			LedOrange::reset();
		}

		// initialize with limited range of ~2.3G
		PT_CALL(accel.configure(accel.Scale::G2, accel.MeasurementRate::Hz400));

		while (true)
		{
			PT_CALL(accel.readAcceleration());

#if REVISION_C
			averageX.update(-accel.data.getY());
			averageY.update(accel.data.getX());
#else
			averageX.update(accel.data.getX());
			averageY.update(accel.data.getY());
#endif

			LedOrange::set(averageX.getValue() < -0.2);
			LedBlue::set(averageX.getValue() > 0.2);
			LedGreen::set(averageY.getValue() < -0.2);
			LedRed::set(averageY.getValue() > 0.2);

			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::MovingAverage<float, 25> averageX;
	xpcc::MovingAverage<float, 25> averageY;
};

ReaderThread reader;

MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedOrange::setOutput(xpcc::Gpio::Low);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);

#if USE_I2C
	lis::Cs::setOutput(xpcc::Gpio::High);
	lis::Mosi::setOutput(xpcc::Gpio::High);

	lis::Scl::connect(lis::I2cMaster::Scl);
	lis::Sda::connect(lis::I2cMaster::Sda);
	lis::Scl::configure(Gpio::InputType::PullUp);
	lis::Sda::configure(Gpio::InputType::PullUp);

	lis::I2cMaster::initialize<defaultSystemClock, 400000>();
#else
	lis::Sck::connect(lis::SpiMaster::Sck);
	lis::Miso::connect(lis::SpiMaster::Miso);
	lis::Mosi::connect(lis::SpiMaster::Mosi);

	lis::SpiMaster::initialize<defaultSystemClock, MHz10>();
	lis::SpiMaster::setDataMode(lis::SpiMaster::DataMode::Mode3);
#endif

	while (1)
	{
		reader.update();
	}

	return 0;
}
