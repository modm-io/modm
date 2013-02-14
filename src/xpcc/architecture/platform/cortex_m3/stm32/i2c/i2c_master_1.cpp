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

/*
 * This driver was not so straight forward to implement, because the official
 * documentation by ST is not so clear about the reading operation.
 * So here is the easier to understand version (# = wait for next interrupt).
 *
 * Writing:
 * --------
 *	- set start bit
 *	#
 *	- check SB bit
 *	- write address
 *	#
 *	- check ADDR bit
 *	- clear ADDR bit
 *	- if no bytes are to be written, check for next operation immidiately
 *	#
 *	- check TXE bit
 *	- write data
 *	- on write of last bytes, disable Buffer interrupt, and wait for BTF interrupt
 *	#
 *	- check BTF bit
 *	- if no bytes left, check for next operation
 *
 * It if important to note, that we wait for the last byte transfer to complete
 * before checking the next operation.
 *
 *
 * Reading is a lot more complicated. In master read mode, the controller can
 * stretch the SCL line low, while there is new received data in the registers.
 * 
 * The data and the shift register together hold two bytes, so we have to send
 * NACK and the STOP condition two bytes in advance and then read both bytes!!!
 *
 * 1-byte read:
 * ------------
 *	- set start bit (RESTART!)
 *	#
 *	- check SB bit
 *	- set ACK low
 *	- write address
 *	#
 *	- check ADDR bit
 *	- clear ADDR bit
 *	- set STOP high
 *	- (wait until STOP low)
 *	- read data 1
 *	- check for next operation
 *
 * 2-byte read:
 * ------------
 *	- set start bit (RESTART!)
 *	#
 *	- check SB bit
 *	- set ACK low
 *	- set POS high (must be used ONLY in two byte transfers, clear it afterwards!)
 *	- write address
 *	#
 *	- check ADDR bit
 *	- clear ADDR bit
 *	#
 *	- check BTF bit
 *	- set STOP high
 *	- read data 1
 *	- (wait until STOP low)
 *	- read data 2
 *	- check for next operation
 *
 * 3-byte read:
 * ------------
 *	- set start bit (RESTART!)
 *	#
 *	- check SB bit
 *	- write address
 *	#
 *	- check ADDR bit
 *	- clear ADDR bit
 *	#
 *	- check BTF bit
 *	- set ACK LOW
 *	- read data 1
 *	#
 *	- check BTF bit
 *	- set STOP high
 *	- read data 1
 *	- (wait until STOP low)
 *	- read data 2
 *	- check for next operation
 *
 * N-byte read:
 * -------------
 *	- set start bit (RESTART!)
 *	#
 *	- check SB bit
 *	- write address
 *	#
 *	- check ADDR bit
 *	- enable Buffer Interrupt
 *	- clear ADDR bit
 *	#
 *	- check RXNE bit
 *	- read data < N-3
 *	#
 *	- check RXNE bit
 *	- read data N-3
 *	- disable Buffer Interrupt
 *	#
 *	- check BTF bit
 *	- set ACK low
 *	- read data N-2
 *	#
 *	- check BTF bit
 *	- set STOP high
 *	- read data N-1
 *	- (wait until STOP low)
 *	- read data N
 *	- check for next operation
 *
 * Please read the documentation of the driver before you attempt to fix this
 * driver. I strongly recommend the use of an logic analizer or oscilloscope,
 * to confirm the drivers behavior.
 * The event states are labeled as 'EVn_m' in reference to the manual.
 */

/* To debug the internal state of the driver, you can instantiate a
 * xpcc::IOStream, which will then be used to dump state data of the operations
 * via the serial port.
 * Be advised, that a typical I2C read/write operation can take 10 to 100 longer
 * because the strings have to be copied during the interrupt!
 *
 * You can enable serial debugging with this define.
 */
#define SERIAL_DEBUGGING 0

#if SERIAL_DEBUGGING
#	include "../uart/uart_5.hpp"
	typedef xpcc::stm32::BufferedUart5 DebugUart;
#	include <xpcc/io/iostream.hpp>
	extern xpcc::IOStream stream;
#	define DEBUG_STREAM(x) //stream << x << "\n"
#	define DEBUG(x) xpcc::stm32::BufferedUart5::write(x)
#else
#	define DEBUG_STREAM(x)
#	define DEBUG(x)
#endif

/*
 * It is mildly recommended to wait for the STOP operation to finish, before
 * reading the shift register, however since no interrupt is generated after a
 * STOP condition, the only solution is to busy wait until the STOP bit is 
 * cleared by hardware.
 * Busy waiting in this interrupt is potentially dangerous and can make program
 * behavior unpredictable and debugging very cumbersome.
 * Therefore, this is disabled by default, you _can_ reenable it with this define.
 */
#define WAIT_FOR_STOP_LOW 0

#include "i2c_master_1.hpp"
#include "../gpio.hpp"


namespace
{
	GPIO__IO(SclB6, B, 6);
	GPIO__IO(SdaB7, B, 7);
	
	GPIO__IO(SclB8, B, 8);
	GPIO__IO(SdaB9, B, 9);
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
	static xpcc::stm32::I2cMaster1::ErrorState errorState(xpcc::stm32::I2cMaster1::NO_ERROR);
	
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
		
		I2C1->CR1 &= ~I2C_CR1_POS;
		I2C1->SR1 = 0;
		I2C1->SR2 = 0;
		
		// generate startcondition
		I2C1->CR1 |= I2C_CR1_START;
		checkNextOperation = CHECK_NEXT_OPERATION_NO;
		
		// and enable interrupts
		DEBUG_STREAM("enable interrupts");
		I2C1->CR2 &= ~I2C_CR2_ITBUFEN;
		I2C1->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
	}
}
	
// ----------------------------------------------------------------------------
extern "C" void
I2C1_EV_IRQHandler(void)
{
	DEBUG_STREAM("\n--- interrupt ---");
	
	uint16_t sr1 = I2C1->SR1;
	
	if (sr1 & I2C_SR1_SB)
	{
		// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
		DEBUG_STREAM("startbit set");
		
		xpcc::i2c::Delegate::Starting s = delegate->started();
		uint8_t address;
		
		switch (s.next)
		{
			case xpcc::i2c::Delegate::READ_OP:
				address = (s.address & 0xfe) | xpcc::i2c::READ;
				initializeRead(delegate->reading());
				if (readBytesLeft <= 2)
				{
					DEBUG_STREAM("NACK");
					I2C1->CR1 &= ~I2C_CR1_ACK;
				}
				else
				{
					DEBUG_STREAM("ACK");
					I2C1->CR1 |= I2C_CR1_ACK;
				}
				if (readBytesLeft == 2)
				{
					DEBUG_STREAM("POS");
					I2C1->CR1 |= I2C_CR1_POS;
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
		
		I2C1->DR = address;
	}
	
	
	
	else if (sr1 & I2C_SR1_ADDR)
	{
		// EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
		DEBUG_STREAM("address sent");
		DEBUG_STREAM("writeBytesLeft=" << writeBytesLeft);
		DEBUG_STREAM("readBytesLeft=" << readBytesLeft);
		
		if (writeBytesLeft > 0 || readBytesLeft > 3)
		{
			DEBUG_STREAM("enable buffers");
			I2C1->CR2 |= I2C_CR2_ITBUFEN;
		}
		if (!readBytesLeft && !writeBytesLeft)
		{
			checkNextOperation = CHECK_NEXT_OPERATION_YES;
		}
		
		
		DEBUG_STREAM("clearing ADDR");
		uint16_t sr2 = I2C1->SR2;
		(void) sr2;
		
		
		if (readBytesLeft == 1)
		{
			DEBUG_STREAM("STOP");
			I2C1->CR1 |= I2C_CR1_STOP;
			
#if WAIT_FOR_STOP_LOW
			DEBUG_STREAM("waiting for stop");
			while (I2C1->CR1 & I2C_CR1_STOP)
				;
#endif
			
			uint16_t dr = I2C1->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft = 0;
			checkNextOperation = CHECK_NEXT_OPERATION_YES_NO_STOP_BIT;
		}
	}
	
	
	
	else if (sr1 & I2C_SR1_TXE)
	{
		// EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR
		// EV8: TxE=1, shift register not empty, data register empty, cleared by writing DR
		if (writeBytesLeft > 0)
		{
			DEBUG_STREAM("tx more bytes");
			I2C1->DR = *writePointer++; // write data
			writeBytesLeft--;
			
			DEBUG_STREAM("TXE: writeBytesLeft=" << writeBytesLeft);
			
			checkNextOperation = CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF;
		}
		// no else!
		if (writeBytesLeft == 0)
		{
			// disable TxE, and wait for EV8_2
			DEBUG_STREAM("last byte transmitted, wait for btf");
			I2C1->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	}
	
	
	
	else if (sr1 & I2C_SR1_RXNE)
	{
		if (readBytesLeft > 3)
		{
			// EV7: RxNE=1, cleared by reading DR register
			uint16_t dr = I2C1->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft--;
			
			DEBUG_STREAM("RXNE: readBytesLeft=" << readBytesLeft);
		}
		
		if (readBytesLeft <= 3)
		{
			// disable RxNE, and wait for BTF
			DEBUG_STREAM("fourth last byte received, wait for btf");
			I2C1->CR2 &= ~I2C_CR2_ITBUFEN;
		}
	}
	
	
	
	if (sr1 & I2C_SR1_BTF)
	{
		// EV8_2
		DEBUG_STREAM("BTF");
		
		if (readBytesLeft == 2)
		{
			// EV7_1: RxNE=1, cleared by reading DR register, programming STOP=1
			DEBUG_STREAM("STOP");
			I2C1->CR1 |= I2C_CR1_STOP;
			
			DEBUG_STREAM("reading data1");
			uint16_t dr = I2C1->DR;
			*readPointer++ = dr & 0xff;
			
#if WAIT_FOR_STOP_LOW
			DEBUG_STREAM("waiting for stop");
			while (I2C1->CR1 & I2C_CR1_STOP)
				;
#endif
			
			DEBUG_STREAM("reading data2");
			dr = I2C1->DR;
			*readPointer++ = dr & 0xff;
			
			readBytesLeft = 0;
			checkNextOperation = CHECK_NEXT_OPERATION_YES_NO_STOP_BIT;
		}
		
		if (readBytesLeft == 3)
		{
			// EV7_1: RxNE=1, cleared by reading DR register, programming ACK=0
			I2C1->CR1 &= ~I2C_CR1_ACK;
			DEBUG_STREAM("NACK");
			
			uint16_t dr = I2C1->DR;
			*readPointer++ = dr & 0xff;
			readBytesLeft--;
			
			DEBUG_STREAM("BTF: readBytesLeft=2");
		}
		
		if (checkNextOperation == CHECK_NEXT_OPERATION_NO_WAIT_FOR_BTF
			&& writeBytesLeft == 0)
		{
			// EV8_2: TxE=1, BTF = 1, Program Stop request.
			// TxE and BTF are cleared by hardware by the Stop condition
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
					I2C1->CR2 |= I2C_CR2_ITBUFEN;
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
					I2C1->CR1 |= I2C_CR1_STOP;
					DEBUG_STREAM("STOP");
				}
				
				DEBUG_STREAM("disable interrupts");
				I2C1->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
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
I2C1_ER_IRQHandler(void)
{
	DEBUG_STREAM("ERROR!");
	uint16_t sr1 = I2C1->SR1;
	
	if (sr1 & I2C_SR1_BERR)
	{
		DEBUG_STREAM("BUS ERROR");
		I2C1->CR1 |= I2C_CR1_STOP;
		errorState = xpcc::stm32::I2cMaster1::BUS_ERROR;
	}
	else if (sr1 & I2C_SR1_AF)
	{	// acknowledge fail
		I2C1->CR1 |= I2C_CR1_STOP;
		DEBUG_STREAM("ACK FAIL");
		 // may also be ADDRESS_NACK
		errorState = xpcc::stm32::I2cMaster1::DATA_NACK;
	}
	else if (sr1 & I2C_SR1_ARLO)
	{	// arbitration lost
		DEBUG_STREAM("ARBITRATION LOST");
		errorState = xpcc::stm32::I2cMaster1::ARBITRATION_LOST;
	}
	else
	{
		DEBUG_STREAM("UNKNOWN");
		errorState = xpcc::stm32::I2cMaster1::UNKNOWN_ERROR;
	}
	
	if (delegate)
	{
		delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
		delegate = 0;
	}
	
	// Overrun error is not handled here separately
	
	// Clear flags and interrupts
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->SR1 = 0;
	I2C1->SR2 = 0;
	writeBytesLeft = 0;
	readBytesLeft = 0;
	checkNextOperation = CHECK_NEXT_OPERATION_NO;
	
	DEBUG_STREAM("disable interrupts");
	I2C1->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
}
	
// ----------------------------------------------------------------------------
void
xpcc::stm32::I2cMaster1::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PB8_PB9) {
		SclB8::setAlternateFunction(AF_I2C1, xpcc::stm32::OPEN_DRAIN);
		SdaB9::setAlternateFunction(AF_I2C1, xpcc::stm32::OPEN_DRAIN);
	}
	else {
		SclB6::setAlternateFunction(AF_I2C1, xpcc::stm32::OPEN_DRAIN);
		SdaB7::setAlternateFunction(AF_I2C1, xpcc::stm32::OPEN_DRAIN);
	}
#else
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_I2C1_REMAP) | mapping;
	if (mapping == REMAP_PB8_PB9) {
		SclB8::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
		SdaB9::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
	}
	else {
		SclB6::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
		SdaB7::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
	}
#endif
}
	
void
xpcc::stm32::I2cMaster1::initialize(uint16_t ccrPrescaler)
{
	reset();
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	I2C1->CR1 = I2C_CR1_SWRST; 		// reset module
	I2C1->CR1 = 0;
	
	NVIC_EnableIRQ(I2C1_ER_IRQn);
	NVIC_EnableIRQ(I2C1_EV_IRQn);
	
	I2C1->CR2 = STM32_APB1_FREQUENCY/1000000;
	I2C1->CCR = ccrPrescaler;
	DEBUG_STREAM("ccrPrescaler=" << ccrPrescaler);
	I2C1->TRISE = 0x09;
	
	I2C1->CR1 |= I2C_CR1_PE; // Enable peripheral
}
	
void
xpcc::stm32::I2cMaster1::reset(bool error)
{
	readBytesLeft = 0;
	writeBytesLeft = 0;
	if (delegate) delegate->stopped(error ? xpcc::i2c::Delegate::ERROR_CONDITION : xpcc::i2c::Delegate::SOFTWARE_RESET);
	delegate = 0;
}
	
// MARK: - ownership
bool
xpcc::stm32::I2cMaster1::start(xpcc::i2c::Delegate *delegate)
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
xpcc::stm32::I2cMaster1::startSync(xpcc::i2c::Delegate *delegate)
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
xpcc::stm32::I2cMaster1::getErrorState()
{
	return errorState;
}
	
