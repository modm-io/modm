#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

/**! \file main.cpp
 * @author David Hebbeker
 */

using namespace xpcc::stm32;

static bool initClock()
{
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HSE_CRYSTAL)) {
		return false;
	}

	Clock::enablePll(Clock::PLL_HSE, 4, 168);
	return Clock::switchToPll();
}

xpcc::stm32::Uart5 uart(115200);
xpcc::IODeviceWrapper<xpcc::stm32::Uart5> loggerDevice(uart); // Create a new UART object and configure it to a baudrate of 115200

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

MAIN_FUNCTION
{
	initClock();

	uart.configurePins(uart.REMAP_PC12_PD2);

	const uint8_t adc_channel = 1;

	XPCC_LOG_INFO    << xpcc::endl << "This is an ADC Test program by David."    << xpcc::endl;


	GPIOA->MODER |= GPIO_MODER_MODER1;

	// --------------------------------------------------
	// Here some (pseudo) code to use the ADC:
	// --------------------------------------------------

	// Initialize ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // start ADC Clock

	ADC1->CR2 |= ADC_CR2_ADON;		// switch on ADC
	ADC1->CR2 |= ADC_CR2_CONT;		// set to continuous mode

	// configure sequence
	ADC1->SQR1 &= ~(0xF << 20);		// clear number of conversions in the sequence
	ADC1->SQR1 |= ~(1 << 20);		// set number of conversions to 1

	ADC1->SQR3 |= adc_channel;	// set the first conversion channel to adc_channel

	ADC1->SMPR2 |= 0b111; // set high sample time


	ADC->CCR |= ADC_CCR_TSVREFE; // wake up temperature sensor
	ADC->CCR |= ADC_CCR_VBATE;	// enable Vbat measurements


	ADC1->CR2 |= ADC_CR2_SWSTART;	// starts single conversion for the regular group

	// The regular channels and their order in the conversion sequence must be
	// selected in the ADC_SQRx registers. The total number of conversions in
	// the regular group must be written in the L[3:0] bits in the ADC_SQR1
	// register.

	while(1)
	{
		XPCC_LOG_INFO << "Channel " << adc_channel << " = " ;	// set the first conversion channel to adc_channel
		for(uint32_t t1=0; t1<16200000; t1++) asm volatile ("NOP"); // knowing, that there is probably a better function for this (@see http://xpcc.sourceforge.net/api/classxpcc_1_1stm32_1_1_general_purpose_timer.html)
		while( (ADC1->SR & ADC_SR_EOC)==0 ) { XPCC_LOG_INFO << "ADC Status is " << xpcc::hex << (uint16_t) ADC1->SR << xpcc::ascii << "| Control is: " << xpcc::hex << (uint16_t) ADC1->CR2 << xpcc::endl; } ///*asm volatile("NOP");*/ }	//wait for a conversion
		XPCC_LOG_INFO << xpcc::ascii << "[" << (ADC1->DR & 0xFFFF) << "]" << xpcc::endl; // send the value
	}

	// Shut down ADC
	ADC1->CR2 &= ~(ADC_CR2_ADON);				// switch off ADC

	while (1)
	{
	}
}
