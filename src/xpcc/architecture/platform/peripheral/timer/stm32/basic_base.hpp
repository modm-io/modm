// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
#ifndef XPCC_STM32_TIMER_BASIC_BASE_HPP
#define XPCC_STM32_TIMER_BASIC_BASE_HPP

#include <stdint.h>
#include "../../../device.hpp"

namespace xpcc
{

namespace stm32
{

class BasicTimer
{
public:
	enum class Mode : uint32_t
	{
		UpCounter = 0,
		OneShotUpCounter = TIM_CR1_OPM,
	};

	enum class Interrupt : uint32_t
	{
		Update = TIM_DIER_UIE,
	};

	enum class InterruptFlag : uint32_t
	{
		Update = TIM_SR_UIF,
	};

public:
	/**
	 * Enables the clock for the timer and resets all settings
	 *
	 * Has to be called before calling any other function from this
	 * class! Otherwise the settings won't have any effect.
	 */
	static void
	enable();

	/**
	 * Disable clock.
	 *
	 * All settings are ignored in this mode and the timer is
	 * shut down. Calling any function other than enable() won't
	 * have any effect.
	 */
	static void
	disable();

	/**
	 * Pause timer operation
	 *
	 * All settings are stored but the timer operation is suspend until
	 * start() is called.
	 */
	static void
	pause();

	/**
	 * Re-enable timer operations
	 *
	 * Has to called after the initial configuration to start timer
	 * or after pause() to restart the timer.
	 */
	static void
	start();

	/**
	 * Set operation mode of the timer
	 */
	static void
	setMode(Mode mode);

	/**
	 * Set new prescaler
	 *
	 * The prescaler can divide the counter clock frequency by any
	 * factor between 1 and 65536. The new prescaler ratio is taken
	 * into account at the next update event.
	 *
	 * @see		applyAndReset()
	 */
	static inline void
	setPrescaler(uint16_t prescaler);

	/**
	 * Set overflow.
	 *
	 * This sets the maximum counter value of the timer.
	 * The timer is blocked if \p overflow is set to zero.
	 *
	 * Takes effect at next update event.
	 *
	 * @see		applyAndReset()
	 */
	static inline void
	setOverflow(uint16_t overflow);

	/**
	 * Set period in microseconds
	 *
	 * Changes prescaler and overflow values.
	 * Takes effect at next update event.
	 *
	 * @param	microseconds	Requested period in microseconds
	 * @param	autoApply		Update the new value immediately and
	 * 							reset the counter value.
	 *
	 * @return	New overflow value.
	 *
	 * @see		applyAndReset()
	 */
	static uint16_t
	setPeriod(uint32_t microseconds, bool autoApply = true);

	/**
	 * @brief	Reset the counter, and update the prescaler and
	 * 			overflow values.
	 *
	 * Generates an Update-Event without generating an
	 * Update-Interrupt.
	 *
	 * This will reset the counter to 0 in up-counting mode (the
	 * default) or to the maximal value in down-counting mode. It will
	 * also update the timer's prescaler and overflow values if you
	 * have set them up to be changed using setPrescaler() or
	 * setOverflow() (or setPeriod()).
	 *
	 * An Update-Event is also generated when the timer reaches its
	 * maximal (up-counting) or minimal (down-counting) value. The
	 * settings for Prescaler, Overflow and Compare values are applied
	 * then without calling this function.
	 */
	static inline void
	applyAndReset();

	/**
	 * Get the counter value
	 */
	static inline uint16_t
	getValue();

	/**
	 * Set a new counter value
	 */
	static inline void
	setValue(uint16_t value);
	
	/**
	 * Enables or disables the Interrupt Vector.
	 * 
	 * \param	enable
	 * 				Enable/disable the interrupt vector
	 * \param	priority
	 * 				Priority of the interrupt vector
	 * 				(0=highest to 15=lowest).
	 */
	static void
	enableInterruptVector(bool enable, uint8_t priority);
	
	/**
	 * Enables interrupts. Don't forget to enable the Interrupt Vector.
	 * 
	 * \param	interrupt	
	 * 				Interrupts to enable
	 * 
	 * \see		enableInterruptVector()
	 */
	static void
	enableInterrupt(Interrupt interrupt);

	/**
	 * Disables interrupts.
	 *
	 * \param	interrupt
	 * 				Interrupts to disable
	 */
	static void
	disableInterrupt(Interrupt interrupt);

	/**
	 * Returns a bitmap of the enum StateFlag. Use this method while
	 * executing an interrupt or in other situations where you want to
	 * know which of the flags are set.
	 *
	 * If a flag is a cause of an enabled interrupt (and the Interrupt
	 * Vector is enabled) then interrupt will be triggered as long
	 * the flag is set.
	 *
	 * \warning		You cannot use a switch statement for the returned
	 * 				value, since multiple flags may be set!
	 *
	 * \code
	 * InterruptFlag flags = Timer{{ id }}::getInterruptFlags()
	 * if (flags & Timer{{ id }}::FLAG_XX){
	 *     // Flag was set, clear the flag since flags are set by
	 *     // hardware, but have to be cleared by software
	 *     Timer{{ id }}::resetInterruptFlags(TIM{{ id }}::FLAG_XX)
	 * }
	 * \endcode
	 */
	static InterruptFlag
	getInterruptFlags();
	
	/**
	 * Clears one or multiple flags.
	 *
	 * \param flags		Bitmap of StateFlag
	 */
	static void
	acknowledgeInterruptFlag(InterruptFlag flags);
};

ENUM_CLASS_FLAG(BasicTimer::Interrupt)
ENUM_CLASS_FLAG(BasicTimer::InterruptFlag)

} // namespace stm32

} // namespace xpcc

#endif // XPCC_STM32_TIMER_BASIC_BASE_HPP
