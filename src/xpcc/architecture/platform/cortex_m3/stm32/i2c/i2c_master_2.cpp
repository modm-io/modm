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

#include <xpcc_config.hpp>

//#if DRIVER_CONNECTIVITY_I2C_DEBUG
//// some header magic
//// replace the release delegate definition by the debug version
//#include <xpcc/driver/connectivity/i2c/delegate_debug.hpp>
//#define DEBUG(code) if (delegate) delegate->event(xpcc::stm32::I2cMaster2::code)
//#else
//#define DEBUG(code)
//#endif

// uncomment to debug your driver using simple uart
/*
#include "../uart/uart_5.hpp"
typedef xpcc::stm32::BufferedUart5 DebugUart;
#include <xpcc/io/iostream.hpp>
extern xpcc::IOStream stream;
#define DEBUG_STREAM(x) stream << x << xpcc::endl
#define DEBUG(x) xpcc::stm32::BufferedUart5::write(x)
/*/
#define DEBUG_STREAM(x)
#define DEBUG(x)
//*/

#include "i2c_master_2.hpp"

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
	static xpcc::i2c::Delegate::NextOperation nextOperation;
	enum CheckNextOperation
	{
		CHECK_NEXT_OPERATION_NO,
		CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF,
		CHECK_NEXT_OPERATION_YES,
		CHECK_NEXT_OPERATION_YES_NO_STOP_BIT,
		
	};
	CheckNextOperation checkNextOperation = CHECK_NEXT_OPERATION_NO;
	
	// buffer management
	static uint8_t *readPointer;
	static const uint8_t *writePointer;
	static std::size_t readBytesLeft = 0;
	static std::size_t writeBytesLeft = 0;
	
	// delegating
	static xpcc::i2c::Delegate *delegate = 0;
	static xpcc::stm32::I2cMaster2::ErrorState errorState(xpcc::stm32::I2cMaster2::NO_ERROR);
	
	static void
	initializeWrite(xpcc::i2c::Delegate::Writing w)
	{
		writePointer = w.buffer;
		writeBytesLeft = w.size;
		readBytesLeft = 0;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(w.next);
	}
	
	static void
	initializeRead(xpcc::i2c::Delegate::Reading r)
	{
		readPointer = r.buffer;
		readBytesLeft = r.size;
		writeBytesLeft = 0;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(r.next);
	}
	
	static void
	initializeStopAfterAddress()
	{
		writeBytesLeft = 0;
		readBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::STOP_OP;
	}
	
	static void
	initializeRestartAfterAddress()
	{
		writeBytesLeft = 0;
		readBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::RESTART_OP;
	}
	
	void
	callStarting()
	{
		DEBUG_STREAM("callStarting");
		
		I2C2->CR1 &= ~I2C_CR1_POS;
		I2C2->SR1 = 0;
		I2C2->SR2 = 0;
		
		// generate startcondition
		I2C2->CR1 |= I2C_CR1_START;
		checkNextOperation = CHECK_NEXT_OPERATION_NO;
		
		// and enable interrupts
		DEBUG_STREAM("enable interrupts");
		I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
	}
}
	
// ----------------------------------------------------------------------------
extern "C" void
I2C2_EV_IRQHandler(void)
{
	DEBUG_STREAM("\n--- interrupt ---");
	
	uint16_t sr1 = I2C2->SR1;
	
	if (sr1 & I2C_SR1_SB)
	{
		DEBUG_STREAM("startbit set");
		
		xpcc::i2c::Delegate::Starting s = delegate->started();
		uint8_t address;
		
		switch (s.next)
		{
			case xpcc::i2c::Delegate::READ_OP:
				address = (s.address & 0xfe) | xpcc::i2c::READ;
				initializeRead(delegate->reading());
				if (readBytesLeft <= 1)
				{
					DEBUG_STREAM("NACK");
					I2C2->CR1 &= ~I2C_CR1_ACK;
				}
				else
				{
					DEBUG_STREAM("ACK");
					I2C2->CR1 |= I2C_CR1_ACK;
				}
				DEBUG_STREAM("read op: reading=" << readBytesLeft);
				break;
				
			case xpcc::i2c::Delegate::WRITE_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeWrite(delegate->writing());
				DEBUG_STREAM("write op: writing=" << writeBytesLeft);
				break;
				
			case xpcc::i2c::Delegate::RESTART_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeRestartAfterAddress();
				DEBUG_STREAM("restart op");
				break;
				
			default:
			case xpcc::i2c::Delegate::STOP_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeStopAfterAddress();
				DEBUG_STREAM("stop op");
				break;
		}
		
		I2C2->DR = address;
	}
	
	
	
	else if (sr1 & I2C_SR1_ADDR)
	{
		DEBUG_STREAM("address sent");
		DEBUG_STREAM("writeBytesLeft=" << writeBytesLeft);
		DEBUG_STREAM("readBytesLeft=" << readBytesLeft);
		
		if (writeBytesLeft > 0 || readBytesLeft > 2)
		{
			DEBUG_STREAM("enable buffers");
			I2C2->CR2 |= I2C_CR2_ITBUFEN;
		}
		if (!readBytesLeft && !writeBytesLeft)
			checkNextOperation = CHECK_NEXT_OPERATION_YES;
		
		if (readBytesLeft == 2)
		{
			DEBUG_STREAM("NACK | POS");
			I2C2->CR1 &= ~I2C_CR1_ACK;
			I2C2->CR1 |= I2C_CR1_POS;
		}
		
		
		DEBUG_STREAM("clearing ADDR");
		uint16_t sr2 = I2C2->SR2;
		(void) sr2;
		
		
		if (readBytesLeft == 1)
		{
			DEBUG_STREAM("STOP");
			I2C2->CR1 |= I2C_CR1_STOP;
			
//			DEBUG_STREAM("waiting for stop");
//			while (I2C2->CR1 & I2C_CR1_STOP)
//				;
			
			uint16_t dr = I2C2->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft = 0;
			checkNextOperation = CHECK_NEXT_OPERATION_YES_NO_STOP_BIT;
		}
	}
	
	
	
	else if (sr1 & I2C_SR1_TXE)
	{
		// EV8
		if (writeBytesLeft > 0)
		{
			DEBUG_STREAM("tx more bytes");
			I2C2->DR = *writePointer++; // write data
			writeBytesLeft--;
			
			DEBUG_STREAM("TXE: writeBytesLeft=" << writeBytesLeft);
			
			checkNextOperation = CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF;
		}
		// no else!
		if (writeBytesLeft == 0)
		{
			// disable TXE, and wait for BTF
			DEBUG_STREAM("last byte transmitted, wait for btf");
			I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	}
	
	
	
	else if (sr1 & I2C_SR1_RXNE)
	{
		if (readBytesLeft > 3)
		{	
			uint16_t dr = I2C2->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft--;
			
			DEBUG_STREAM("RXNE: readBytesLeft=" << readBytesLeft);
		}
		
		if (readBytesLeft <= 3)
		{
			// disable RXnE, and wait for BTF
			DEBUG_STREAM("fourth last byte received, wait for btf");
			I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	}
	
	
	
	if (sr1 & I2C_SR1_BTF)
	{
		// EV8_2
		DEBUG_STREAM("BTF");
		
		if (readBytesLeft == 2)
		{
			DEBUG_STREAM("STOP");
			I2C2->CR1 |= I2C_CR1_STOP;
			
			DEBUG_STREAM("reading data1");
			uint16_t dr = I2C2->DR;
			*readPointer++ = dr & 0xff;
			
//			DEBUG_STREAM("waiting for stop");
//			while (I2C2->CR1 & I2C_CR1_STOP)
//				;
			
			DEBUG_STREAM("reading data2");
			dr = I2C2->DR;
			*readPointer++ = dr & 0xff;
			
			readBytesLeft = 0;
			checkNextOperation = CHECK_NEXT_OPERATION_YES_NO_STOP_BIT;
		}
		
		if (readBytesLeft == 3)
		{
			I2C2->CR1 &= ~I2C_CR1_ACK;
			DEBUG_STREAM("NACK");
			
			uint16_t dr = I2C2->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft--;
			
			DEBUG_STREAM("BTF: readBytesLeft=2");
		}
		
		if (checkNextOperation == CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF
			&& writeBytesLeft == 0)
		{
			DEBUG_STREAM("BTF, write=0");
			checkNextOperation = CHECK_NEXT_OPERATION_YES;
		}
	}
	
	
	
	if (checkNextOperation > CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF)
	{
		switch (nextOperation)
		{
			case xpcc::i2c::Delegate::WRITE_OP:
				if (checkNextOperation != CHECK_NEXT_OPERATION_YES_NO_STOP_BIT)
				{
					initializeWrite(delegate->writing());
					// reenable TXE
					I2C2->CR2 |= I2C_CR2_ITBUFEN;
					DEBUG_STREAM("write op");
				}
				break;
				
			case xpcc::i2c::Delegate::RESTART_OP:
				callStarting();
				DEBUG_STREAM("restart op");
				break;
				
			default:
				if (checkNextOperation != CHECK_NEXT_OPERATION_YES_NO_STOP_BIT)
				{
					I2C2->CR1 |= I2C_CR1_STOP;
					DEBUG_STREAM("STOP");
				}
				
				DEBUG_STREAM("disable interrupts");
				I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if (delegate) delegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
				delegate = 0;
				DEBUG_STREAM("write finished");
				break;
		}
		checkNextOperation = CHECK_NEXT_OPERATION_NO;
	}
}
	
// ----------------------------------------------------------------------------
extern "C" void
I2C2_ER_IRQHandler(void)
{
	DEBUG_STREAM("ERROR!");
	uint16_t sr1 = I2C2->SR1;
	uint16_t sr2 = I2C2->SR2;
	(void) sr2;
	
	if (sr1 & I2C_SR1_BERR)
	{
		DEBUG_STREAM("BUS ERROR");
		I2C2->CR1 |= I2C_CR1_STOP;
		errorState = xpcc::stm32::I2cMaster2::BUS_ERROR;
	}
	else if (sr1 & I2C_SR1_AF)
	{	// acknowledge fail
		I2C2->CR1 |= I2C_CR1_STOP;
		DEBUG_STREAM("ACK FAIL");
		 // may also be ADDRESS_NACK
		errorState = xpcc::stm32::I2cMaster2::DATA_NACK;
	}
	else if (sr1 & I2C_SR1_ARLO)
	{	// arbitration lost
		DEBUG_STREAM("ARBITRATION LOST");
		errorState = xpcc::stm32::I2cMaster2::ARBITRATION_LOST;
	}
	else
	{
		DEBUG_STREAM("UNKNOWN");
		errorState = xpcc::stm32::I2cMaster2::UNKNOWN_ERROR;
	}
	
	if (delegate)
	{
		delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
		delegate = 0;
	}
	
	// Overrun error is not handled here separately
	
	// Clear flags and interrupts
	I2C2->CR1 &= ~I2C_CR1_POS;
	I2C2->SR1 = 0;
	I2C2->SR2 = 0;
	writeBytesLeft = 0;
	readBytesLeft = 0;
	checkNextOperation = CHECK_NEXT_OPERATION_NO;
	
	DEBUG_STREAM("disable interrupts");
	I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
}
	
// ----------------------------------------------------------------------------
void
xpcc::stm32::I2cMaster2::configurePins(Mapping mapping)
{
	enable();
	
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
xpcc::stm32::I2cMaster2::enable()
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
}
	
void
xpcc::stm32::I2cMaster2::initialize(uint16_t ccrPrescaler)
{
	reset();
	enable();
	
	I2C2->CR1 = I2C_CR1_SWRST; 		// reset module
	I2C2->CR1 = 0;
	
	NVIC_EnableIRQ(I2C2_ER_IRQn);
	NVIC_EnableIRQ(I2C2_EV_IRQn);
	
	I2C2->CR2 = STM32_APB1_FREQUENCY/1000000;
	I2C2->CCR = ccrPrescaler;
	DEBUG_STREAM("ccrPrescaler=" << ccrPrescaler);
	I2C2->TRISE = 0x09;
	
	I2C2->CR1 |= I2C_CR1_PE; // Enable peripheral
}
	
void
xpcc::stm32::I2cMaster2::reset(bool error)
{
	readBytesLeft = 0;
	writeBytesLeft = 0;
	if (delegate) delegate->stopped(error ? xpcc::i2c::Delegate::ERROR_CONDITION : xpcc::i2c::Delegate::SOFTWARE_RESET);
	delegate = 0;
}
	
// MARK: - ownership
bool
xpcc::stm32::I2cMaster2::start(xpcc::i2c::Delegate *delegate)
{
	if (!::delegate && delegate && delegate->attaching())
	{
		DEBUG_STREAM("\n###\n");
		::delegate = delegate;
		callStarting();
		
		return true;
	}
	return false;
}
	
bool
xpcc::stm32::I2cMaster2::startSync(xpcc::i2c::Delegate *delegate)
{
	if (!::delegate && delegate && delegate->attaching())
	{
		DEBUG_STREAM("\n###\n");
		::delegate = delegate;
		callStarting();
		
		// memory barrier, yes you really need it
		__asm__ volatile ("dmb" ::: "memory");
		
		while(::delegate != 0)
			;
		
		return true;
	}
	return false;
}
	
uint8_t
xpcc::stm32::I2cMaster2::getErrorState()
{
	return errorState;
}
	

#endif
