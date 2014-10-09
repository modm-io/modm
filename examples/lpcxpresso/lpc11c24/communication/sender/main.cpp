
#include <xpcc/architecture.hpp>
#include <xpcc/communication.hpp>
#include <xpcc/communication/backend/can/can_connector.hpp>
#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

#include "component_sender/sender.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

// ----------------------------------------------------------------------------
// Logging

extern xpcc::lpc::Uart1 loggerUart;
xpcc::IODeviceWrapper<xpcc::lpc::Uart1> loggerDevice(loggerUart);

xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
// CAN communication

typedef xpcc::lpc::Can CanDevice;

static CanDevice device;
static xpcc::CanConnector< CanDevice > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Sender sender(robot::component::SENDER, &dispatcher);
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	SystemInit();

	xpcc::lpc::SysTickTimer::enable();

	xpcc::lpc::Uart1 loggerUart(115200);

	// Configure Can device and set the filters
	device.initialize(xpcc::can::BITRATE_125_KBPS);
//	device.setFilter(xpcc::accessor::asFlash(canFilter));
	
	component::sender.initialize();

	XPCC_LOG_INFO << "Welcome to the communication test at Sender!" << xpcc::endl;
	
	while (1)
	{
		// deliver received messages
		dispatcher.update();
		
		component::sender.update();
		
		xpcc::delayMicroseconds(100);
	}
}

/**
 * Provide empty while loops for every handler that is normally
 * handled by defaultHandler. 
 * This is for debugging purposes to see which exception occurred.
 */

extern "C" {
void NMI_Handler(){while(1);}
void HardFault_Handler(){while(1);}
void SVCall_Handler(){while(1);}
void PendSV_Handler(){while(1);}
//void SysTick_Handler(){while(1);}	// Implemented by SysTickTimer

//void CAN_IRQHandler(){while(1);}	// Implemented
void SSP1_IRQHandler(){while(1);}
void I2C_IRQHandler(){while(1);}
void TIMER16_0_IRQHandler(){while(1);}
void TIMER16_1_IRQHandler(){while(1);}
void TIMER32_0_IRQHandler(){while(1);}
void TIMER32_1_IRQHandler(){while(1);}
void SSP0_IRQHandler(){while(1);}
//void UART_IRQHandler(){while(1);}	// Implemented
void ADC_IRQHandler(){while(1);}
void WDT_IRQHandler(){while(1);}
void BOD_IRQHandler(){while(1);}
void PIOINT3_IRQHandler(){while(1);}
void PIOINT2_IRQHandler(){while(1);}
void PIOINT1_IRQHandler(){while(1);}
void PIOINT0_IRQHandler(){while(1);}
void WAKEUP_IRQHandler(){while(1);}
}

