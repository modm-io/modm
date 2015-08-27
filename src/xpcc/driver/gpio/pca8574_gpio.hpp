// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA8574_GPIO_HPP
#define XPCC_PCA8574_GPIO_HPP

#include <xpcc/architecture/interface/gpio.hpp>
#include <xpcc/processing/protothread.hpp>

namespace xpcc
{

/**
 * Storage of pin state for a port expander.
 *
 * For pin operations like set() / reset() the state of all other
 * pins at the port expander must be known because all eight bits
 * are written at once in a single I2C transaction.
 * All pins on the same gpioExpander must share the same storage.
 */
struct pca8574GpioStore {
	uint8_t pinState;
};

/**
 * Create an xpcc GPIO compatible interface from a PCA8574 port expander.
 *
 * @code
 * typedef xpcc::Pca8574<lcd::MyI2cMaster> GpioExpander;
 * GpioExpander gpioExpander;
 *
 * xpcc::pca8574GpioStore store;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 0 > Rs;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 1 > Rw;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 2 > E;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 3 > Backlight;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 4 > Pin4;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 5 > Pin5;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 6 > Pin6;
 * typedef xpcc::Pca8574Gpio<store, GpioExpander, gpioExpander, 7 > Pin7;
 *
 * typedef xpcc::SoftwareGpioPort<Pin7, Pin6, Pin5, Pin4> Data4BitGpio;
 *
 * Backlight::set();
 *
 * @author: strongly-typed
 */
template <
	pca8574GpioStore & store,
	typename gpioExpanderType,
	gpioExpanderType & gpioExpander,
	uint8_t Pin
	>
class Pca8574Gpio : public xpcc::GpioIO
{
public:
	static void
	setOutput() {
		// Do nothing because PCA8574 is always in output mode.
	}

	static void
	setOutput(bool value) {
		set(value);
	}

	static void
	set() {
		store.pinState |= (1 << Pin);
		RF_CALL_BLOCKING(gpioExpander.write(store.pinState));
	}

	static void
	set(bool value) {
		if (value) {
			store.pinState |= (1 << Pin);
		} else {
			store.pinState &= ~(1 << Pin);
		}
		RF_CALL_BLOCKING(gpioExpander.write(store.pinState));
	}

	static void
	reset() {
		store.pinState &= ~(1 << Pin);
		RF_CALL_BLOCKING(gpioExpander.write(store.pinState));
	}

	static void
	toggle() {
		store.pinState ^= (1 << Pin);
		RF_CALL_BLOCKING(gpioExpander.write(store.pinState));
	}

	static bool
	isSet() {
		return (store.pinState & (1 << Pin));
	}

	static void
	setInput() {
		// PCA 8574 is always in output mode. Pin must be driven HIGH to use as input.
		set();
	}

	static bool
	read() {
		uint8_t readback = 0;
		RF_CALL_BLOCKING(gpioExpander.read(readback));

		return (readback & (1 << Pin));
	}

};

} // xpcc namespace

#endif // XPCC_PCA8574_GPIO_HPP