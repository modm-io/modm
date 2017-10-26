/**
 * Example demonstrating to subscribing and publishing to topics.
 *
 * Connect three LEDs to A0, A1, and A2 at the Arduino header of a
 * stm32nucleo board.
 */

#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing.hpp>

#include <std_msgs/Bool.h>

#include <ros/node_handle.h>
#include <xpcc/communication/ros.hpp>

#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DISABLED

// Define which UART is used for communication to and from ROS serial
// When using the STlink Uart on Nucleo boards logging must be disabled.
using RosSerialUart = Board::stlink::Uart;

namespace ampel
{
	using LedRed   = GpioOutputA0;
	using LedGreen = GpioOutputA1;
	using LedBlue  = GpioOutputA4;
}

namespace ros
{
	using xpccHardware = XpccHardware<RosSerialUart>;
	using XpccNodeHandle = NodeHandle_<xpccHardware>;
}

ros::XpccNodeHandle nh;

// For publisher
std_msgs::Bool button_msg;
ros::Publisher pub_button("/button", &button_msg);

// For subscriber
void
message_cb_red(const std_msgs::Bool& msg) {
	ampel::LedRed::set(msg.data);
}

void
message_cb_green(const std_msgs::Bool& msg) {
	ampel::LedGreen::set(msg.data);
}

void
message_cb_blue(const std_msgs::Bool& msg) {
	ampel::LedBlue::set(msg.data);
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Reinit onboard UART to 1 Mbps
	// Do not use it for logging because this will destroy ROS serial messages.
	Board::stlink::Uart::initialize<Board::systemClock, xpcc::Uart::Baudrate::MBps1>(12);

	Board::LedGreen::set();

	ampel::LedRed::setOutput();
	ampel::LedGreen::setOutput();
	ampel::LedBlue::setOutput();

	nh.initNode();

	ros::Subscriber<std_msgs::Bool> sub_red  ("/led/red",   &message_cb_red);
	ros::Subscriber<std_msgs::Bool> sub_green("/led/green", &message_cb_green);
	ros::Subscriber<std_msgs::Bool> sub_blue ("/led/blue",  &message_cb_blue);

	nh.subscribe(sub_red);
	nh.subscribe(sub_green);
	nh.subscribe(sub_blue);

	nh.advertise(pub_button);

	// Make sure to publish on boot
	bool button_state = not Board::Button::read();

	xpcc::ShortPeriodicTimer heartbeat(1000);
	while(true)
	{
		bool button_read = Board::Button::read();
		if (button_state != button_read)
		{
			// Store and publish
			button_msg.data = button_state = button_read;
			pub_button.publish(&button_msg);
		}

		nh.spinOnce();
		if (heartbeat.execute()) {
			Board::LedGreen::toggle();
		}
	}

	return 0;
}
