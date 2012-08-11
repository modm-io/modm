
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#include "../gpio.hpp"
#include "spi_1.hpp"

void
xpcc::lpc::SpiMaster1::configurePins(MappingSck mapping, bool useSsel)
{
	// Deassert Reset
	LPC_SYSCON->PRESETCTRL 		|= PRESETCTRL_SSP1_RST_N;

	// Enable peripheral clock
	LPC_SYSCON->SYSAHBCLKCTRL	|= SYSAHBCLKCTRL_SSP1;

	// Divide peripheral clock by 1
	LPC_SYSCON->SSP1CLKDIV = 0x01;

	// MISO1 at PIO2_2 and MOSI1 at PIO2_3
	LPC_IOCON->PIO2_2			&= ~0x07;
	LPC_IOCON->PIO2_2			|=  0x02;	/* SSP MISO */
	LPC_IOCON->PIO2_3			&= ~0x07;
	LPC_IOCON->PIO2_3			|=  0x02;	/* SSP MOSI */

	// For SPI1 SCK1 is fixed at PIO2_1
	LPC_IOCON->PIO2_1 = 0x02;
	(void) mapping; // unused

	if (useSsel) {
	// SSEL1 fixed at PIO2_0
		LPC_IOCON->PIO2_0 &= ~0x07;
		LPC_IOCON->PIO2_0 |=  0x02;
	}

}

void
xpcc::lpc::SpiMaster1::initialize(
		Mode mode,
		Presacler prescaler,
		uint8_t serialClockRate,
		TransferSize transferSize,
		FrameFormat frameFormat)
{
	// Control register 0
	LPC_SSP1->CR0 = (serialClockRate << 8) |
			 (static_cast<uint16_t>(mode)) |
			((static_cast<uint16_t>(frameFormat)) << 4) |
			((static_cast<uint16_t>(transferSize)) << 0);

	// Clock prescale register
	LPC_SSP1->CPSR = static_cast<uint8_t>(prescaler);

	for (uint8_t ii = 0; ii < fifoSize; ++ii)
	{
	  uint16_t Dummy = LPC_SSP1->DR;		/* clear the RxFIFO */
	  (void)Dummy; // unused
	}

	/* TODO Enable the SSP Interrupt */

	// Enable SPI1 in master mode
	LPC_SSP1->CR1 = SPI_CR1_SSE;
}

void
xpcc::lpc::SpiMaster1::write(uint8_t data)
{
	/* Move on only if TX FIFO not full. */
	while (!(LPC_SSP1->SR & SPI_SRn_TNF));

	/* Put data into FIFO */
	LPC_SSP1->DR = data;
}

void
xpcc::lpc::SpiMaster1::write(uint8_t * data, uint8_t len)
{
	while (len-- > 0)
	{
		while (!(LPC_SSP1->SR & SPI_SRn_TNF));
		LPC_SSP1->DR = *data;
		++data;
	}
}

bool
xpcc::lpc::SpiMaster1::isBusy()
{
	return (LPC_SSP1->SR & SPI_SRn_BSY);
}