
#include <xpcc/architecture.hpp>
#include <xpcc/architecture/board/cpuboard2/slave.hpp>

#include "../interconnect_structure.hpp"

typedef xpcc::CpuBoard2Slave<SlaveToMaster, MasterToSlave> Board;

// Timer overflow interrupt, every 1ms
ISR(TCD0_OVF_vect)
{
	xpcc::Clock::increment();
}

MAIN_FUNCTION
{
	Board::initialize();
	
	// Timer for the timebase
	// prescaler = 1
	// f_timer = 32MHz / 1
	// f_pwm = f_timer / 32000 = 1000 Hz 
	TCD0.PER = 32000;
	TCD0.CCA = 0;
	TCD0.INTCTRLA = TC_OVFINTLVL_HI_gc;
	TCD0.CTRLA = TC_CLKSEL_DIV1_gc;
	
	// enable all interrupt level
	PMIC_CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	
	// enable interrupts
	sei();
	
	while (1)
	{
		Board::Interconnect::makeTransaction();
		xpcc::delay_ms(100);
	}
}
