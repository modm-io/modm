#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

/**! \file main.cpp
 * @author David Hebbeker
 */

using namespace xpcc::stm32;

#define __DISCOVERY

static bool initClock()
{
#ifdef __DISCOVERY
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HSE_CRYSTAL)) {
		return false;
	}

	Clock::enablePll(Clock::PLL_HSE, 4, 168);
	return Clock::switchToPll();
#else
	// For using an external oscillator with 8 MHz use:
	using namespace Clock;
	if (enableHSE(HSE_BYPASS))
	 {
	     enablePll(PLL_HSE, PLL_MUL_9);
	     switchToPll();
	 }
#endif
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

#define ADC_CHANNEL_NUM (2)

uint16_t ADC_buffer[ADC_CHANNEL_NUM];

void enableInterruptVector(bool enable, uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(ADC_IRQn, priority);

		// register IRQ at the NVIC
		NVIC_EnableIRQ(ADC_IRQn);
	}
	else {
		NVIC_DisableIRQ(ADC_IRQn);
	}
}


extern "C" void ADC_IRQHandler(void)
{
	static uint8_t index=0;
	ADC_buffer[index++] = ADC1->DR & 0xFFF;
	if(index == ADC_CHANNEL_NUM) index=0;
	ADC1->SR &= ~ADC_SR_EOC;
}

extern "C" void ADC1_2_IRQHandler(void)
{
	ADC1->SR &= ~ADC_SR_EOC;

	XPCC_LOG_INFO << "ADC_INT1" << xpcc::endl;
}

static inline void enableInterrupt()
{
	ADC1->CR1 |= ADC_CR1_EOCIE;
}

MAIN_FUNCTION
{
	initClock();

	uart.configurePins(uart.REMAP_PC12_PD2);

	const uint8_t adc_channel = 1;

	XPCC_LOG_INFO    << xpcc::endl << "This is an ADC Test program by David."    << xpcc::endl;

	// --------------------------------------------------
	// Here some (pseudo) code to use the ADC:
	// --------------------------------------------------


	GPIOA->MODER |= GPIO_MODER_MODER1;
	GPIOA->MODER |= GPIO_MODER_MODER2;
	GPIOA->MODER |= GPIO_MODER_MODER3;
	GPIOA->MODER |= GPIO_MODER_MODER4;
	GPIOA->MODER |= GPIO_MODER_MODER5;
	GPIOA->MODER |= GPIO_MODER_MODER6;
	GPIOA->MODER |= GPIO_MODER_MODER7;

	// Initialize ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // start ADC Clock


	ADC1->CR2 |= ADC_CR2_ADON;		// switch on ADC

	// enable Update Interrupt
		enableInterruptVector(true, 15);
		enableInterrupt();


	ADC1->CR1 |= ADC_CR1_SCAN;		// set to scan mode
	ADC1->CR2 |= ADC_CR2_CONT;		// set to continuous mode

	// configure sequence
	ADC1->SQR1 &= 0;		// clear number of conversions in the sequence
	ADC1->SQR1 |= ((ADC_CHANNEL_NUM-1) << 20);		// set number of conversions to 1
	//@TODO 	ADC1->SQR1 |= (3 << 20);		// set number of conversions to 1



	ADC1->SQR3 |= adc_channel | (3 << 5);	// set the first conversion channel to adc_channel

	ADC1->SMPR2 |= 0b111; // set high sample time

	ADC1->CR2 |= ADC_CR2_EOCS; //!< Select, that an interrupt is called after each conversion and not only after the sequence



	ADC1->CR2 |= ADC_CR2_SWSTART;	// starts single conversion for the regular group

	// The regular channels and their order in the conversion sequence must be
	// selected in the ADC_SQRx registers. The total number of conversions in
	// the regular group must be written in the L[3:0] bits in the ADC_SQR1
	// register.

	while(1)
	{
		for(uint32_t t1=0; t1<16200000; t1+=2) asm volatile ("NOP"); // knowing, that there is probably a better function for this (@see http://xpcc.sourceforge.net/api/classxpcc_1_1stm32_1_1_general_purpose_timer.html)
		//while( (ADC1->SR & ADC_SR_EOC)==0 ) { XPCC_LOG_INFO << "ADC Status is " << xpcc::hex << (uint16_t) ADC1->SR << xpcc::ascii << "| Control is: " << xpcc::hex << (uint16_t) ADC1->CR2 << xpcc::endl; } //wait for a conversion
		XPCC_LOG_INFO << xpcc::ascii << "PINA1=" << ADC_buffer[0] << " PINA3=" << ADC_buffer[1] << xpcc::endl; // send the value
		//XPCC_LOG_INFO << "The status registers are ADC1_SR_EOC=" << xpcc::hex << ADC1->SR & ADC_SR_EOC << xpcc::ascii << " and ADC_CCR_EOC1=" << xpcc::hex << ADC->CCR & ADC_CCR_EOC1 << xpcc::endl;
	}

	// Shut down ADC
	ADC1->CR2 &= ~(ADC_CR2_ADON);				// switch off ADC

	while (1)
	{
	}
}
