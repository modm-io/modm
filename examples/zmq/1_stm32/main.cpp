#include <xpcc/architecture/platform.hpp>

#include <xpcc/processing/timer/periodic_timer.hpp>

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/can.hpp>

#include "component_odometry/odometry.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

using namespace Board;

namespace Led
{
	using B = GpioOutputB12;
	using G = GpioOutputB13;
	using R = GpioOutputB14;
}

/*
 * Periodically publish an XPCC message on the CAN bus.
 * If it is longer than eight bytes it will fragmented and creates more than one CAN frame.
 */

static Can1 device;

// CanConnector does the fragmentation and defragmentation of xpcc messages to and from CAN frames.
static xpcc::CanConnector< Can1 > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Odometry odometry(robot::component::ODOMETRY, dispatcher);
}

xpcc::ShortPeriodicTimer tmr(50);

int
main()
{
	Board::initialize();

	Led::R::setOutput();
	Led::G::setOutput();
	Led::B::setOutput();

	using Timer = xpcc::stm32::Timer2;
	using ChannelA = GpioInputA0;
	using ChannelB = GpioInputA1;
	auto ChannelAInputType = Gpio::InputType::PullUp;
	auto ChannelBInputType = Gpio::InputType::PullUp;

	// Initialze the encoder
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter, Timer::SlaveMode::Encoder3);
	// Overflow must be 16bit because else a lot of our motor control code will break!
	Timer::setOverflow(0xffff);
	ChannelA::connect(Timer::Channel1, ChannelAInputType);
	ChannelB::connect(Timer::Channel2, ChannelBInputType);
	Timer::start();

	// Initialize Can1
	GpioInputA11::connect(Can1::Rx, Gpio::InputType::PullUp);
	GpioOutputA12::connect(Can1::Tx, Gpio::OutputType::PushPull);
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(9);
	CanFilter::setFilter(0, CanFilter::FIFO0,
                        CanFilter::ExtendedIdentifier(0),
                        CanFilter::ExtendedFilterMask(0));


	LedGreen::set();

	while (true)
	{
		dispatcher.update();
		component::odometry.update();

		if (tmr.execute()) {
			LedGreen::toggle();
		}

		xpcc::delayMicroseconds(100);
	}

	return 0;
}
