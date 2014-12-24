#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

#include <xpcc/processing/timeout.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/driver/temperature/tmp102.hpp>

#include <xpcc/io/iostream.hpp>

xpcc::IODeviceWrapper< Usart2, xpcc::IODevice::BufferBehavior::BusyWait > device;
xpcc::IOStream stream(device);

typedef I2cMaster1 MyI2cMaster;


class ThreadOne : public xpcc::pt::Protothread
{
public:
	ThreadOne()
	:	temp(temperatureData, 0x48)
	{
	}

	bool
	update()
	{
		temp.update();

		PT_BEGIN();

		// ping the device until it responds
		while(true)
		{
			// we wait until the task started
			if (PT_CALL(temp.ping(this)))
				break;
			// otherwise, try again in 100ms
			this->timer.restart(100);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}


		PT_CALL(temp.setUpdateRate(this, 200));
		PT_CALL(temp.enableExtendedMode(this));

		PT_CALL(temp.configureAlertMode(this,
				xpcc::tmp102::ThermostatMode::Comparator,
				xpcc::tmp102::AlertPolarity::ActiveLow,
				xpcc::tmp102::FaultQueue::Faults6));
		PT_CALL(temp.writeLowerLimit(this, 28.f));
		PT_CALL(temp.writeUpperLimit(this, 30.f));

		while (true)
		{
			{
				PT_CALL(temp.readComparatorMode(this, result));
				float temperature = temperatureData.getTemperature();
				uint8_t tI = (int) temperature;
				uint16_t tP = (temperature - tI) * 10000;
				stream << "T= " << tI << ".";
				if (tP == 0)
				{
					stream << "0000 C";
				}
				else if (tP == 625)
				{
					stream << "0" << tP << " C";
				}
				else
				{
					stream << tP << " C";
				}
				stream << xpcc::endl;
				if (result) stream << "Heat me up!" << xpcc::endl;
			}
			this->timer.restart(200);
			PT_WAIT_UNTIL(this->timer.isExpired());
			LedRed::toggle();
		}

		PT_END();
	}

private:
	bool result;
	xpcc::Timeout<> timer;
    xpcc::tmp102::Data temperatureData;
	xpcc::Tmp102<MyI2cMaster> temp;
};

ThreadOne one;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<defaultSystemClock, xpcc::Uart::B38400>(10);

	GpioB7::connect(MyI2cMaster::Sda);
	GpioB8::connect(MyI2cMaster::Scl);
	GpioB7::configure(Gpio::InputType::PullUp);
	GpioB8::configure(Gpio::InputType::PullUp);
	MyI2cMaster::initialize<defaultSystemClock, 100000>();

	stream << "\n\nRESTART\n\n";

	while (1)
	{
		one.update();
		LedOrange::toggle();
	}

	return 0;
}
