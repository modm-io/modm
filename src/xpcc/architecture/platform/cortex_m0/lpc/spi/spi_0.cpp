#include "../gpio.hpp"
#include "spi_0.hpp"

void
xpcc::lpc::SpiMaster0::configurePins(MappingSck mapping, bool useSsel)
{
	// Deassert Reset
	LPC_SYSCON->PRESETCTRL 		|= PRESETCTRL_SSP0_RST_N;

	// Enable peripheral clock
	LPC_SYSCON->SYSAHBCLKCTRL	|= SYSAHBCLKCTRL_SSP0;

	// Divide peripheral clock by 1
	LPC_SYSCON->SSP0CLKDIV = 0x01;

	// MISO at PIO0_8 and MOSI at PIO0_9
	LPC_IOCON->PIO0_8			&= ~0x07;
	LPC_IOCON->PIO0_8			|=  0x01;	/* SSP MISO */
	LPC_IOCON->PIO0_9			&= ~0x07;
	LPC_IOCON->PIO0_9			|=  0x01;	/* SSP MOSI */

	// SCK at PIO0_6, PIO0_10 or PIO2_11
	switch (mapping)
	{
	case MappingSck::PIO0_6:
		LPC_IOCON->SCK_LOC = 0x02;
		LPC_IOCON->PIO0_6  = 0x02;	/* P0.6 function 2 is SSP clock, need to
									combined with IOCONSCKLOC register setting */
	break;
	case MappingSck::PIO0_10:
		/* This is only available if SWD/JTAG is disabled */
		LPC_IOCON->SCK_LOC = 0x00;
		LPC_IOCON->SWCLK_PIO0_10 &= ~0x07;
		LPC_IOCON->SWCLK_PIO0_10 |=  0x02;		/* SSP CLK */
	break;
	case MappingSck::PIO2_11:
		LPC_IOCON->SCK_LOC = 0x01;
		LPC_IOCON->PIO2_11 = 0x01;	/* P2.11 function 1 is SSP clock, need to
										combined with IOCONSCKLOC register setting */
	break;
	}

	if (useSsel)
	{
		LPC_IOCON->PIO0_2 &= ~0x07;
		LPC_IOCON->PIO0_2 |=  0x01;		/* SSP SSEL */
	}
}

void
xpcc::lpc::SpiMaster0::initialize(
		Mode mode,
		Presacler prescaler,
		uint8_t serialClockRate,
		TransferSize transferSize,
		FrameFormat frameFormat)
{
	// Control register 0
	LPC_SSP0->CR0 = (serialClockRate << 8) | (static_cast<uint16_t>(mode)) | ((static_cast<uint16_t>(frameFormat)) << 4) | ((static_cast<uint16_t>(transferSize)) << 0); // 0x0707;

	// Clock prescale register
	LPC_SSP0->CPSR = static_cast<uint8_t>(prescaler);

	for (uint8_t ii = 0; ii < fifoSize; ++ii)
	{
	  uint16_t Dummy = LPC_SSP0->DR;		/* clear the RxFIFO */
	  (void)Dummy; // unused
	}

	/* TODO Enable the SSP Interrupt */

	// Enable SPI0 in master mode
	LPC_SSP0->CR1 = SPI_CR1_SSE;
}

void
xpcc::lpc::SpiMaster0::write(uint8_t data)
{
	/* Move on only if TX FIFO not full. */
	while (!(LPC_SSP0->SR & SPI_SRn_TNF));

	/* Put data into FIFO */
	LPC_SSP0->DR = data;
}

void
xpcc::lpc::SpiMaster0::write(uint8_t * data, uint8_t len)
{
	while (len-- > 0)
	{
		while (!(LPC_SSP0->SR & SPI_SRn_TNF));
		LPC_SSP0->DR = *data;
		++data;
	}
}

bool
xpcc::lpc::SpiMaster0::isBusy()
{
	return (LPC_SSP0->SR & SPI_SRn_BSY);
}
