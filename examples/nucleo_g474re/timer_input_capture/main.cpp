/*
 * Copyright (c) 2022, E. Steven Macías Uriña
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * ---------------------- Input Capture Example -----------------------------
 *
 * 	Connect a jumper cable between PB7 and PC2 in your Nucleo G474RE.
 * 	PB7 will generate a PWM signal that will be monitored by PC2.
 * 	The main goal of this example is to calculate the period of the input signal.
 *
 * --------------------------------------------------------------------------
 */

#include <modm/board.hpp>

using namespace Board;

// Input Capture Timer Configuration
constexpr uint16_t 	input_capture_overflow 		= 0xFFFF;
constexpr float 	input_capture_freq_hz 		= 3000.00;	// Hz
constexpr uint16_t 	input_capture_prescaler 	= SystemClock::Frequency / input_capture_freq_hz;
constexpr float 	input_capture_ms_per_tick 	= ( 1.0 / input_capture_freq_hz ) * 1000.0;

// PWM Generator Timer Configuration
constexpr uint16_t 	pwm_gen_overflow 		= 0xFFFF;
constexpr float 	pwm_gen_frequency_hz 	= 50.00; 	// Hz
constexpr float 	pwm_gen_pulse_width_ms 	= 1.5f;		// Milliseconds

// -------------------- PWM Generator Timer Calculations --------------------
// Desired PWM Freq = Clock Freq / Timer Prescaler / Overflow Value
// Timer Prescaler = Clock Freq / (Desired PWM Freq * Overflow Value)
constexpr uint16_t 	pwm_gen_prescaler 			= SystemClock::Frequency / ( pwm_gen_frequency_hz * pwm_gen_overflow );
constexpr float 	pwm_gen_period_ms			= ( 1.0 / pwm_gen_frequency_hz ) * 1000;
constexpr uint16_t 	pwm_gen_ticks_per_period 	= pwm_gen_overflow / pwm_gen_period_ms;
constexpr uint16_t 	pwm_pulse_width_in_ticks 	= pwm_gen_ticks_per_period * pwm_gen_pulse_width_ms;
// --------------------------------------------------------------------------

// Variables used to determine the period of the input signal
uint16_t latest_input_timer_value 	= 0;
uint16_t old_input_timer_value 		= 0;
uint16_t ticks_between_interrupts 	= 0;

// Interrupt Configuration
Timer20::InputCapturePrescaler interrupt_prescaler = Timer20::InputCapturePrescaler::Div8;
uint32_t interrupt_prescaler_value 	= 1 << (uint32_t) interrupt_prescaler;	// 2^interrupt_prescaler
uint8_t interrupt_priority 			= 10u;

/**
 * @brief Function executed when a Capute Compare interruption is triggered.
 * Every time there is a rising edge in the input timer pin this code
 * will be executed (If InputCapturePrescaler is set to Div1).
 * By saving the value of the capture compare register between interruptions
 * the period of the signal can be calculated.
 */
MODM_ISR(TIM20_CC)
{
	uint16_t temp_ticks_between_interrups = 0;
	Timer20::acknowledgeInterruptFlags(Timer20::InterruptFlag::CaptureCompare2);
	LedD13::toggle();	// Visual Feedback of the interruption being triggered

	old_input_timer_value = latest_input_timer_value;
	latest_input_timer_value = Timer20::getCompareValue<GpioC2::Ch2>();
	temp_ticks_between_interrups = (latest_input_timer_value-old_input_timer_value);

	// Ingore negative values (overflow of counter register)
	if ( temp_ticks_between_interrups > 0)
	{
		ticks_between_interrupts = temp_ticks_between_interrups/interrupt_prescaler_value;
	}
}

/**
 * @brief Get the period of the input signal in milliseconds.
 *
 * @return float
 */
float getPeriodOfSignalInMs()
{
	return ticks_between_interrupts * input_capture_ms_per_tick;
}

/**
 * @brief Setup of the timer used to generate the PWM Signal.
 *
 */
void generatePwm()
{
	Timer4::connect<GpioB7::Ch2>();
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);
	Timer4::setPrescaler(pwm_gen_prescaler);
	Timer4::setOverflow(pwm_gen_overflow);
	Timer4::configureOutputChannel<GpioB7::Ch2>(Timer4::OutputCompareMode::Pwm, pwm_pulse_width_in_ticks);
	Timer4::applyAndReset();
	Timer4::start();
}

/**
 * @brief Setup of the timer used to measure an input signal.
 *
 */
void inputTimerConfig()
{
	Timer20::connect<GpioC2::Ch2>();
	Timer20::enable();
	Timer20::setMode(Timer20::Mode::UpCounter);
	Timer20::setPrescaler(input_capture_prescaler);
	Timer20::setOverflow(input_capture_overflow);
	Timer20::configureInputChannel<GpioC2::Ch2>(Timer20::InputCaptureMapping::InputOwn,
									interrupt_prescaler, Timer20::InputCapturePolarity::Rising, 0, false);
	Timer20::enableInterruptVector(Timer20::Interrupt::CaptureCompare2, true, interrupt_priority);
	Timer20::enableInterrupt(Timer20::Interrupt::CaptureCompare2);
	Timer20::applyAndReset();
	Timer20::start();
}

int main()
{
	Board::initialize();
	LedD13::setOutput();

	// Configure timers
	generatePwm();
	inputTimerConfig();

	// Print Relevant Information
	MODM_LOG_INFO << modm::endl << "-----------------------------";
	MODM_LOG_INFO << modm::endl << "--- Input Capture Example ---";
	MODM_LOG_INFO << modm::endl << "-----------------------------";
	MODM_LOG_INFO << modm::endl << modm::endl;

	MODM_LOG_INFO << "-- Setup Information --" << modm::endl;
	MODM_LOG_INFO << "Clock Frequency: " << SystemClock::Frequency << "Hz" << modm::endl;
	MODM_LOG_INFO << modm::endl;

	MODM_LOG_INFO << "-- PWM Generator Info --" << modm::endl;
	MODM_LOG_INFO.printf("PWM Gen Frequency: %.2fHz\n", (double) pwm_gen_frequency_hz);
	MODM_LOG_INFO.printf("PWM Gen Pulse Width: %.2fms\n", (double) pwm_gen_pulse_width_ms);
	MODM_LOG_INFO << "PWM Gen Overflow: " << pwm_gen_overflow << modm::endl;
	MODM_LOG_INFO << "PWM Gen Prescaler: " << pwm_gen_prescaler << modm::endl;
	MODM_LOG_INFO << modm::endl;

	MODM_LOG_INFO << "-- Input Capture Info --" << modm::endl;
	MODM_LOG_INFO.printf("Timer Frequency: %.2fHz\n", (double) input_capture_freq_hz);
	MODM_LOG_INFO.printf("Timer Period: %.2fms\n", (double) input_capture_ms_per_tick);
	MODM_LOG_INFO << "Timer Overflow: " << input_capture_overflow << modm::endl;
	MODM_LOG_INFO << "Timer Prescaler: " << input_capture_prescaler << modm::endl;
	MODM_LOG_INFO << modm::endl;

	MODM_LOG_INFO << "-- Interrupt Info --" << modm::endl ;
	MODM_LOG_INFO << "Interrupt Prescaler: " << interrupt_prescaler_value << modm::endl;
	MODM_LOG_INFO << modm::endl;


	while (true)
	{
		modm::delay(100ms);

		// If USER button is pressed:
		if (!Button::read())
		{
			float period_of_signal_ms = getPeriodOfSignalInMs();
			MODM_LOG_INFO << "Current Timer Value: " << Timer20::getValue() << modm::endl;
			MODM_LOG_INFO << "Capture Compare Value: " << Timer20::getCompareValue<GpioC2::Ch2>() << modm::endl;
			MODM_LOG_INFO << "Ticks between interrupts: " << ticks_between_interrupts << modm::endl;
			MODM_LOG_INFO.printf("Period of the signal in ms: %.2f\n", (double) period_of_signal_ms);
			MODM_LOG_INFO << "-----------------------------" << modm::endl ;
		}
	}

	return 0;
}