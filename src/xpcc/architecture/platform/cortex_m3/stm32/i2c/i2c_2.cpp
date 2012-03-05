// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#include "i2c_2.hpp"

#include <xpcc_config.hpp>
#include "../gpio.hpp"

#if !defined(STM32F10X_LD)

namespace
{
	GPIO__IO(SclB10, B, 10);
	GPIO__IO(SdaB11, B, 11);
	
#if defined(STM32F2XX) || defined(STM32F4XX)
	GPIO__IO(SclF1, F, 1);
	GPIO__IO(SdaF0, F, 0);
	
	GPIO__IO(SclH4, H, 4);
	GPIO__IO(SdaH5, H, 5);
#endif
}

namespace
{
	// parameter advice
	static bool reading;
	static bool restartAfterReading;
	static bool restartAfterWriting;
	static uint8_t twiAddress;
	
	// buffer management
	static const uint8_t *twiWriteBuffer;
	static uint8_t *twiReadBuffer;
	static uint8_t twiReadCounter;
	static uint8_t twiWriteCounter;
	static uint8_t twiReadBufferSize;
	static uint8_t twiWriteBufferSize;
	
	// state
	static xpcc::i2c::BusState busState = xpcc::i2c::BUS_RESET;
	static xpcc::i2c::ErrorState errorState = xpcc::i2c::NO_ERROR;
	static bool occupied = false;
//	static bool startConditionGenerated =  false;
	
	// delegating
	static xpcc::i2c::Delegate *delegate = 0;
}

// ----------------------------------------------------------------------------
extern "C" void
I2C2_EV_IRQHandler(void)
{
	uint16_t sr1 = I2C2->SR1;

	if (sr1 & I2C_SR1_SB)
	{	// EV5: SB=1, cleared by reading SR1 register 
		// followed by writing DR register with Address.
		I2C2->DR = twiAddress | reading;
		
		return;
	}

	if (sr1 & I2C_SR1_ADDR)
	{
		// In case a single byte has to be received, the Acknowledge disable is
		// made during EV6 (before ADDR flag is cleared) and the STOP condition
		// generation is made after EV6.
		
		// NACK is set in operation setup, no need to clear it again
		
		// EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
		uint16_t sr2 = I2C2->SR2;
		(void) sr2;
		
		// EV8_1 will be generated automatically.
		
		// Generate STOP condition
		if (twiReadCounter == 1)
			I2C2->CR1 &= ~I2C_CR1_STOP;
		return;
	}
	
	if (sr1 & I2C_SR1_TXE)
	{
		// EV8_2: TxE=1, BTF = 1, Program Stop request. TxE and BTF are
		// cleared by hardware by the Stop condition
		if (sr1 & I2C_SR1_BTF) {
			// Transfer finished
			if (twiReadCounter == 0)
			{
				if (restartAfterWriting) {
					I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
					xpcc::stm32::I2c2::restart(twiAddress);
					// the bus is now reserved for further operations
					busState = xpcc::i2c::BUS_WRITE;
				}
				else xpcc::stm32::I2c2::stop();
				
				if (delegate) {
					xpcc::i2c::Delegate *bufferedDelegate = delegate;
					delegate = 0;
					bufferedDelegate->twiCompletion(twiWriteBuffer-twiWriteBufferSize, twiWriteBufferSize, xpcc::i2c::WRITE);
				}
			}
			else xpcc::stm32::I2c2::restart(twiAddress);
			
			return;
		}
		
		// we have nothing more to send, create BTF condition
		if (!twiWriteCounter) return;
		
		// EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
		// EV8: TxE=1, shift register not empty, daua register empty, cleared by writing DR register
		I2C2->DR = *twiWriteBuffer++;
		twiWriteCounter--;
		twiWriteBufferSize++;
		
		return;
	}
	
	if (sr1 & I2C_SR1_RXNE)
	{
		// EV7: RxNE = 1, cleared by reading DR register
		*twiReadBuffer++ = I2C2->DR & 0xff;
		twiReadCounter--;
		twiReadBufferSize++;
		
		// ACK is set by default, no need to set it again
		if (twiReadCounter > 1)
			return;
		
		// EV7_1: RxNE = 1, cleared by reading DR register programming ACK = 0 and STOP request
		if (twiReadCounter == 1) {
			I2C2->CR1 &= ~I2C_CR1_ACK;
			// Disable only the event interrupt
			I2C2->CR2 &= ~(I2C_CR2_ITEVTEN);
			if (restartAfterReading) xpcc::stm32::I2c2::restart(twiAddress);
			else I2C2->CR1 |= I2C_CR1_STOP;
			
			return;
		}
		
		if (delegate) {
			xpcc::i2c::Delegate *bufferedDelegate = delegate;
			delegate = 0;
			bufferedDelegate->twiCompletion(twiReadBuffer-twiReadBufferSize, twiReadBufferSize, xpcc::i2c::READ);
		}
		busState = restartAfterReading ? xpcc::i2c::BUS_HOLD : xpcc::i2c::BUS_STOPPED;
		occupied = restartAfterReading;
		// Disable all interrupts
		I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	}
}

// ----------------------------------------------------------------------------
extern "C" void
I2C2_ER_IRQHandler(void)
{
	uint16_t sr1 = I2C2->SR1;
	
	twiWriteCounter = 0;
	twiReadCounter = 0;
	
	if (sr1 & I2C_SR1_BERR) {
		I2C2->CR1 |= I2C_CR1_STOP;
		errorState = xpcc::i2c::BUS_ERROR;
	}
	else if (sr1 & I2C_SR1_AF) {			// acknowledge fail
		I2C2->CR1 |= I2C_CR1_STOP;
		errorState = xpcc::i2c::DATA_NACK;
	}
	else if (sr1 & I2C_SR1_ARLO) {		// arbitration lost
		errorState = xpcc::i2c::ARBITRATION_LOST;
	}
	// Overrun error is not handled here
	if (!errorState) errorState = xpcc::i2c::UNKNOWN_ERROR;
	
	if (delegate) {
		xpcc::i2c::Delegate *bufferedDelegate = delegate;
		delegate = 0;
		bufferedDelegate->twiError(errorState);
	}
	
	busState = xpcc::i2c::BUS_RESET;
	occupied = false;
	
	// Clear flags
	I2C2->SR1 = 0;
	// Clear interrupts
	I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::I2c2::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PB10_PB11) {
		SclB10::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
		SdaB11::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
	}
	if (mapping == REMAP_PF1_PF0) {
		SclF1::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
		SdaF0::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
	}
	else {
		SclH4::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
		SdaH5::setAlternateFunction(AF_I2C2, xpcc::stm32::OPEN_DRAIN);
	}
#else
	(void) mapping;		// avoid compiler warning
	
	SclB10::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
	SdaB11::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::I2c2::initialize(uint16_t ccrPrescaler)
{
	reset();
	
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	I2C2->CR1 = I2C_CR1_SWRST; 		// reset module
	I2C2->CR1 = 0;

	NVIC_EnableIRQ(I2C2_ER_IRQn);
	NVIC_EnableIRQ(I2C2_EV_IRQn);

	I2C2->CCR = ccrPrescaler; 	// baudrate (prescaler), only while i2c disabled, PE = 0
	// not enable interrupts | Value of peripheral clock (Unknown effect of this value yet, since Module is always running on APB frequency)
	I2C2->CR2 = STM32_APB1_FREQUENCY/1000000;
//	I2C2->TRISE = // rise time

	I2C2->CR1 |= I2C_CR1_PE; // Enable peripheral
}

void
xpcc::stm32::I2c2::reset()
{
	twiReadCounter = 0;
	twiWriteCounter = 0;
	busState = xpcc::i2c::BUS_RESET;
	occupied = false;
	errorState = xpcc::i2c::NO_ERROR;
	delegate = 0;
}

// MARK: - ownership
bool
xpcc::stm32::I2c2::start(uint8_t slaveAddress)
{
	if (getBusyState() == xpcc::i2c::FREE)
	{
		twiAddress = slaveAddress & 0xfe;
		busState = xpcc::i2c::BUS_STANDBY;
		errorState = xpcc::i2c::NO_ERROR;
		occupied = true;
		
		// generate a start condition
		I2C2->CR1 |= I2C_CR1_START;
		
		return true;
	}
	return false;
}

void
xpcc::stm32::I2c2::restart(uint8_t slaveAddress)
{
	// if start condition has not been generated
	if (!(I2C2->SR1 & I2C_SR1_SB))
	{
		twiAddress = slaveAddress & 0xfe;
		busState = xpcc::i2c::BUS_STANDBY;
		occupied = true;
		
		// generate a start condition
		I2C2->CR1 |= I2C_CR1_START;
	}
}

void
xpcc::stm32::I2c2::stop()
{
	if (busState != xpcc::i2c::BUS_STOPPED && busState != xpcc::i2c::BUS_RESET)
	{
		I2C2->CR1 |= I2C_CR1_STOP;
		busState = xpcc::i2c::BUS_STOPPED;
		occupied = false;
		// Clear interrupts
		I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	}
}

// MARK: - operations
void
xpcc::stm32::I2c2::read(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param)
{
	writeRead(data, 0, size, false, (param == xpcc::i2c::READ_RESTART));
}

void
xpcc::stm32::I2c2::write(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param)
{
	writeRead(data, size, 0, (param == xpcc::i2c::WRITE_RESTART), false);
}

void
xpcc::stm32::I2c2::writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, xpcc::i2c::OperationParameter param)
{
	writeRead(data, writeSize, readSize, false, (param == xpcc::i2c::READ_RESTART));
}

// MARK: - delegate
void
xpcc::stm32::I2c2::attachDelegate(xpcc::i2c::Delegate *object)
{
	delegate = object;
}

// MARK: - status
xpcc::i2c::ErrorState
xpcc::stm32::I2c2::getErrorState()
{
	return errorState;
}

xpcc::i2c::BusyState
xpcc::stm32::I2c2::getBusyState()
{
	// Reading I2C_SR2 after reading I2C_SR1 clears the ADDR flag, even if the 
	// ADDR flag was set after reading I2C_SR1. Consequently, I2C_SR2 must be read
	// only when ADDR is found set in I2C_SR1 or when the STOPF bit is cleared.
	uint16_t sr1 = I2C2->SR1;
	
	if (occupied || !((sr1 & I2C_SR1_ADDR) || !(sr1 & I2C_SR1_STOPF)))
		return xpcc::i2c::OCCUPIED;
	
	uint16_t sr2 = I2C2->SR2;
	
	if (sr2 & I2C_SR2_BUSY)
		return xpcc::i2c::BUSY;
	
	return xpcc::i2c::FREE;
}

xpcc::i2c::BusState
xpcc::stm32::I2c2::getBusState()
{
	return busState;
}

// MARK: - private
void
xpcc::stm32::I2c2::writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, bool restartW, bool restartR)
{	
	// Save pointer to data and number of bytes to send
	twiWriteBuffer = data;
	twiReadBuffer = data;
	twiWriteCounter = writeSize;
	twiReadCounter = readSize;
	twiWriteBufferSize = 0;
	twiReadBufferSize = 0;
	
	restartAfterWriting = restartW;
	restartAfterReading = restartR;
	reading = writeSize ? false : true;

	// Default ACK;
	if (twiReadCounter > 2) I2C2->CR1 |= I2C_CR1_ACK;
	else I2C2->CR1 &= ~I2C_CR1_ACK;
	
	// sent stop condition, if we do not want to write or read anything.
	if (!twiWriteCounter && !twiReadCounter)
		I2C2->CR1 &= ~I2C_CR1_STOP;
	
	// Enable interrupts again
	I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;
}


#endif
