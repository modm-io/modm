
#include "timer_0.hpp"
#include "interrupt_avr32.h"

void
xpcc::avr32::Timer0::initWaveform(const timer::waveform *opt)
{
  // GENERATE SIGNALS: Waveform operating mode.
	AVR32_TC.channel[0].cmr =
		static_cast<uint8_t>(opt->bswtrg) << AVR32_TC_BSWTRG_OFFSET |
		static_cast<uint8_t>(opt->beevt) << AVR32_TC_BEEVT_OFFSET |
		static_cast<uint8_t>(opt->bcpc) << AVR32_TC_BCPC_OFFSET |
		static_cast<uint8_t>(opt->bcpb) << AVR32_TC_BCPB_OFFSET |
		static_cast<uint8_t>(opt->aswtrg) << AVR32_TC_ASWTRG_OFFSET |
		static_cast<uint8_t>(opt->aeevt) << AVR32_TC_AEEVT_OFFSET |
		static_cast<uint8_t>(opt->acpc) << AVR32_TC_ACPC_OFFSET |
		static_cast<uint8_t>(opt->acpa) << AVR32_TC_ACPA_OFFSET |
					  1 << AVR32_TC_WAVE_OFFSET |
		static_cast<uint8_t>(opt->wavsel) << AVR32_TC_WAVSEL_OFFSET |
		static_cast<uint8_t>(opt->enetrg) << AVR32_TC_ENETRG_OFFSET |
		static_cast<uint8_t>(opt->eevt) << AVR32_TC_EEVT_OFFSET |
		static_cast<uint8_t>(opt->eevtedg) << AVR32_TC_EEVTEDG_OFFSET |
		static_cast<uint8_t>(opt->cpcdis) << AVR32_TC_CPCDIS_OFFSET |
		static_cast<uint8_t>(opt->cpcstop) << AVR32_TC_CPCSTOP_OFFSET |
		static_cast<uint8_t>(opt->burst) << AVR32_TC_BURST_OFFSET |
		static_cast<uint8_t>(opt->clki) << AVR32_TC_CLKI_OFFSET |
		static_cast<uint8_t>(opt->tcclks) << AVR32_TC_TCCLKS_OFFSET;
}

void
xpcc::avr32::Timer0::configureInterrupts(const timer::interrupt *bitfield)
{
	// Disable global interrupts.
	bool global_interrupt_enabled = cpu_irq_is_enabled();

	const uint8_t channel = 0;

	// Enable the appropriate interrupts.
	AVR32_TC.channel[channel].ier =
			bitfield->etrgs	<< AVR32_TC_ETRGS_OFFSET |
			bitfield->ldrbs	<< AVR32_TC_LDRBS_OFFSET |
			bitfield->ldras	<< AVR32_TC_LDRAS_OFFSET |
			bitfield->cpcs	<< AVR32_TC_CPCS_OFFSET |
			bitfield->cpbs	<< AVR32_TC_CPBS_OFFSET |
			bitfield->cpas	<< AVR32_TC_CPAS_OFFSET |
			bitfield->lovrs << AVR32_TC_LOVRS_OFFSET |
			bitfield->covfs	<< AVR32_TC_COVFS_OFFSET;

	// Disable the appropriate interrupts.
	if (global_interrupt_enabled) cpu_irq_disable();
	AVR32_TC.channel[channel].idr =
			(~bitfield->etrgs & 1)	<< AVR32_TC_ETRGS_OFFSET |
			(~bitfield->ldrbs & 1)	<< AVR32_TC_LDRBS_OFFSET |
			(~bitfield->ldras & 1)	<< AVR32_TC_LDRAS_OFFSET |
			(~bitfield->cpcs & 1)	<< AVR32_TC_CPCS_OFFSET |
			(~bitfield->cpbs & 1)	<< AVR32_TC_CPBS_OFFSET |
			(~bitfield->cpas & 1)	<< AVR32_TC_CPAS_OFFSET |
			(~bitfield->lovrs & 1)	<< AVR32_TC_LOVRS_OFFSET |
			(~bitfield->covfs & 1)	<< AVR32_TC_COVFS_OFFSET;

	// read Status register to clear interrupts
	AVR32_TC.channel[channel].sr;

	// Enable global interrupts again if they were enabled.
	if (global_interrupt_enabled) cpu_irq_enable();
}
