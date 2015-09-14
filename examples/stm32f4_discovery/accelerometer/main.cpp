#include "../stm32f4_discovery.hpp"
#include <xpcc/driver/inertial/lis302dl.hpp>
#include <xpcc/processing.hpp>
#include <xpcc/math.hpp>


// ST changed the accelerometer in the C revision (MB997C)
// change this to `false`, if you have MB997A or MB997B!
#define REVISION_C true

// if you want to use Software I2c change this to `true`
#define USE_I2C false


namespace lis
{

#if USE_I2C
// I2c Transport Layer
typedef GpioA5 Scl;
typedef GpioA7 Sda;
typedef xpcc::SoftwareI2cMaster<Scl, Sda> I2cMaster;

typedef xpcc::Lis3TransportI2c< I2cMaster > Transport;
#else
typedef Board::lis3::Transport Transport;
#endif

} // namespace lis3

// Data and Driver object
#if REVISION_C
xpcc::lis3dsh::Data data;
xpcc::Lis3dsh< lis::Transport > accel(data);
#else
xpcc::lis302dl::Data data;
xpcc::Lis302dl< lis::Transport > accel(data);
#endif


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
			Board::LedOrange::set();
			PT_WAIT_UNTIL(this->timeout.isExpired());
			Board::LedOrange::reset();
		}

		// initialize with limited range of ~2.3G
		PT_CALL(accel.configure(accel.Scale::G2, accel.MeasurementRate::Hz400));

		while (true)
		{
			PT_CALL(accel.readAcceleration());

#if REVISION_C
			averageX.update(-accel.getData().getY());
			averageY.update(accel.getData().getX());
#else
			averageX.update(accel.getData().getX());
			averageY.update(accel.getData().getY());
#endif

			Board::LedOrange::set(averageX.getValue() < -0.2);
			Board::LedBlue::set(averageX.getValue() > 0.2);
			Board::LedGreen::set(averageY.getValue() < -0.2);
			Board::LedRed::set(averageY.getValue() > 0.2);

			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::filter::MovingAverage<float, 25> averageX;
	xpcc::filter::MovingAverage<float, 25> averageY;
};

ReaderThread reader;

MAIN_FUNCTION
{
	Board::initialize();

#if USE_I2C
	Board::lis3::Cs::setOutput(xpcc::Gpio::High);
	Board::lis3::Mosi::setOutput(xpcc::Gpio::High);

	lis::Scl::connect(lis::I2cMaster::Scl, Gpio::InputType::PullUp);
	lis::Sda::connect(lis::I2cMaster::Sda, Gpio::InputType::PullUp);

	lis::I2cMaster::initialize<systemClock, 400000>();
#else
	Board::initializeLis3();
#endif

	while (1)
	{
		reader.update();
	}

	return 0;
}
