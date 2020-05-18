#ifndef MODM_STM32_DAC_HPP
#	error 	"Don't include this file directly, use 'dac.hpp' instead!"
#endif

#include <modm/platform/clock/rcc.hpp>

void modm::platform::Dac::initialize() {
	Rcc::enable<Peripheral::Dac>();
}

void modm::platform::Dac::enableChannel(Channel chan) {
	switch(chan) {
	case Channel::Channel1:
		DAC1->CR |= DAC_CR_EN1_Msk;
		break;
	case Channel::Channel2:
		DAC1->CR |= DAC_CR_EN2_Msk;
		break;
	}
}

void modm::platform::Dac::disableChannel(Channel chan) {
	switch(chan) {
	case Channel::Channel1:
		DAC1->CR &= ~DAC_CR_EN1_Msk;
		break;
	case Channel::Channel2:
		DAC1->CR &= ~DAC_CR_EN2_Msk;
		break;
	}
}

void modm::platform::Dac::enableOutputBuffer(Channel chan, bool enable) {
	uint32_t chanShift = 0;
	switch(chan) {
		case Channel::Channel1:
			chanShift = 0;
			break;
		case Channel::Channel2:
			chanShift = 16;
			break;
	};

	if(enable) {
		DAC1->CR &= ~(DAC_CR_BOFF1_Msk << chanShift);
	} else {
		DAC1->CR |= DAC_CR_BOFF1_Msk << chanShift;
	}
}

void modm::platform::Dac::setOutput(Channel chan, uint16_t value) {
	switch(chan) {
		case Channel::Channel1:
			DAC1->DHR12R1 = value & DAC_DHR12R1_DACC1DHR_Msk;
			break;
		case Channel::Channel2:
			DAC1->DHR12R2 = value & DAC_DHR12R2_DACC2DHR_Msk;
			break;
	}
}

void modm::platform::Dac::setOutput1(uint16_t value) {
	setOutput(Channel::Channel1, value);
}

void modm::platform::Dac::setOutput2(uint16_t value) {
	setOutput(Channel::Channel2, value);
}