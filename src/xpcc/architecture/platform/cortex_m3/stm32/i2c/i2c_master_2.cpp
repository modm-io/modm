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
//*
#include "../uart/uart_5.hpp"
#define DEBUG(x) xpcc::stm32::BufferedUart5::write(x)
/*/
#define DEBUG(x)
//*/

#include "i2c_master_2.hpp"

#include "../gpio.hpp"

/*
 * Note1:
 * Problem fall Nachdem das letzte byte (vor einem stop oder restart) ins dr geschrieben wird.
 * Darauf wird sofort das entsprechende start/stop flag gesetzt.
 *
 * Falls Stop wird der aktuelle adapter sofort detached und die interrupts deakiviert.
 * Das problem entsteht wenn sich ein anderer adapter bevor dieses byte fertig rausgeschifted und
 * die anschliessende stop condition generiert wurde. Weil die interrupts aktiviert werden muessen.
 *
 * Falls Restart wird I2C_CR2_ITBUFEN sofort deaktiviert.
 *
 * Das txe und btf bit bleibt gesetzt solange die stop/restart condition nicht
 * abgeschlossen ist, daher wird der interrupt sofort getriggert.
 * durch deaktivieren des I2C_CR2_ITBUFEN interrupts bleibt dieser aus solange
 * das byte noch rausgeshifted wird, setzt aber ein waehrend der stop condition.
 *
 * Aus diesem grund signalisiert die variable writeFinished dass dieser vorgang
 * evtl nicht abgeschlossen ist.
 * Solange diese gesetzt ist, muss der txe interrupt ignoriert werden.
 * Diese variable muss zurueckgesetzt werden wenn
 * sicher ist, dass das senden abgeschlossen ist. Weil man das ende der stop condition
 * nicht mitkriegt, wird das ende der start condition dafuer benutzt.
 *
 * Zusatz: (die note1 wird dadurch etwas abgewandelt)
 * bei der generierung der start condition(setzen von cr1_start) kann es sein
 * dass stop noch gesetzt ist.
 * die operation cr1|=start ist nicht atomar, sondern read modify write,
 * wenn dabei das stop bit aber geloescht wird (von der hardware) wird
 * eine erneute stop condition nach der start condition sofort generiert.
 * daher warte bis das stop bit geloescht ist.
 *
 * um zu verhindern, dass diese schleife zu lange dauert (nach dem write ein
 * byte lang) wird das delegate nicht schon vor dem letzten byte, sondern danach
 * erloest.
 */


enum DebugEnum
{
	IRQ_EV = 'i',
	STARTBIT_SET = 's',
	ADDRESS_SENT = 'a',
	TRANSMITTER = 'T',
	RECEIVER_NEXT_RESTART = 'r',
	RECEIVER_NEXT_STOP = 'S',
	RECEIVER_MANY_BYTES = 'm',
	AFTER_WRITE_FINISHED_IRQ = 'f',
	TXE_NO_BYTES_LEFT = 'l',
	TXE_NEXT_WRITE = 'w',
	TXE_NEXT_RESTART = 'R',
	TXE_NEXT_STOP = 't',
	TXE_BTF_NEXT_RESTART = 'b',
	TXE_MORE_BYTES_LEFT = 'M',
	RXNE_IRQ = 'Q',
	RXNE_MANY_BYTES_LEFT = 'X',
	RXNE_ONE_BYTE_LEFT = 'O',
	RXNE_NO_BYTES_LEFT_NEXT_RESTART = 'B',
	RXNE_NO_BYTES_LEFT_NEXT_STOP = 'K',
	CALL_STARTING = 'Y',
	
	IRQ_ER = 'E',
};


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
	
	// buffer management
	static uint8_t *readPointer;
	static const uint8_t *writePointer;
	static std::size_t readBytesLeft;
	static std::size_t writeBytesLeft;
	
	// delegating
	static xpcc::i2c::Delegate *delegate = 0;
	static xpcc::stm32::I2cMaster2::ErrorState errorState(xpcc::stm32::I2cMaster2::NO_ERROR);

	static void
	initializeWrite(xpcc::i2c::Delegate::Writing w)
	{
		writePointer = w.buffer;
		writeBytesLeft = w.size;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(w.next);
	}

	static void
	initializeRead(xpcc::i2c::Delegate::Reading r)
	{
		readPointer = r.buffer;
		readBytesLeft = r.size;
		nextOperation = static_cast<xpcc::i2c::Delegate::NextOperation>(r.next);
	}

	static void
	initializeStopAfterAddress()
	{
		//writePointer = ..;
		writeBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::STOP_OP;
	}

	static void
	initializeRestartAfterAddress()
	{
		//writePointer = ..;
		writeBytesLeft = 0;
		nextOperation = xpcc::i2c::Delegate::RESTART_OP;
	}

	void
	callStarting()
	{
		DEBUG(CALL_STARTING);
		
		// avoid setting the stop bit twice, so wait until it is cleared by hardware
		while (I2C2->CR1 & I2C_CR1_STOP)
			;
		
		// generate startcondition
		I2C2->CR1 |= I2C_CR1_START;

		// and enable interrupts
		I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;
	}
	
	enum WriteFinished
	{
		WRITE_NOT_FINISHED,
		WRITE_FINISHED,
		WRITE_FINISHED_CONDITION_GENERATED
	};
	WriteFinished writeFinished = WRITE_NOT_FINISHED;
	
	enum ReadFinished
	{
		READ_NOT_FINISHED,
		READ_FINISHED_CONDITION_GENERATED,
	};
	ReadFinished readFinished = READ_NOT_FINISHED;
}

// ----------------------------------------------------------------------------
extern "C" void
I2C2_EV_IRQHandler(void)
{
	DEBUG(IRQ_EV);

	uint16_t sr1 = I2C2->SR1;

	if (sr1 & I2C_SR1_SB)
	{	// EV5: SB=1 (startcondition generated), cleared by reading SR1 register
		// followed by writing DR register with Address.
		DEBUG(STARTBIT_SET);

		// see note1
		writeFinished = WRITE_NOT_FINISHED;
		readFinished = READ_NOT_FINISHED;
		I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;

		xpcc::i2c::Delegate::Starting s = delegate->started();
		uint8_t address;
		
		switch (s.next)
		{
			case xpcc::i2c::Delegate::READ_OP:
				address = (s.address & 0xfe) | xpcc::i2c::READ;
				initializeRead(delegate->reading());
				if (readBytesLeft <= 1)
				{
					I2C2->CR1 &= ~I2C_CR1_ACK;
				}
				else
				{
					I2C2->CR1 |= I2C_CR1_ACK;
				}
				// TODO special case if 2?!, what to do if length is 0? generate stop?

				break;
			case xpcc::i2c::Delegate::WRITE_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeWrite(delegate->writing());
				break;
				
			default:
			case xpcc::i2c::Delegate::STOP_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeStopAfterAddress();
				break;
				
			case xpcc::i2c::Delegate::RESTART_OP:
				address = (s.address & 0xfe) | xpcc::i2c::WRITE;
				initializeRestartAfterAddress();
				break;
		}
		
		I2C2->DR = address;
	}

	else if (sr1 & I2C_SR1_ADDR)
	{
		// EV6: address is written successfully
		// In case a single byte has to be received, the Acknowledge disable is
		// made here, before ADDR flag is cleared (by reading sr2) and the STOP condition
		// generation is made after clearing ADDR.
		DEBUG(ADDRESS_SENT);

		// EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
		uint16_t sr2 = I2C2->SR2;
		
		// NACK is set in operation setup, no need to clear it again

		if (sr2 & I2C_SR2_TRA)
		{
			DEBUG(TRANSMITTER);
			// EV8_1 will be generated automatically.
		}
		else
		{
			// Generate STOP condition
			if (readBytesLeft <= 1)
			{
				if (nextOperation == xpcc::i2c::Delegate::RESTART_OP)
				{
					DEBUG(RECEIVER_NEXT_RESTART);
					I2C2->CR1 |= I2C_CR1_START;
				}
				else
				{
					readFinished = READ_FINISHED_CONDITION_GENERATED;
					DEBUG(RECEIVER_NEXT_STOP);
					I2C2->CR1 |= I2C_CR1_STOP;
				}
			}
			else{
				DEBUG(RECEIVER_MANY_BYTES);
			}
		}
	}
	
	else if (sr1 & I2C_SR1_TXE)
	{
		if (writeFinished == WRITE_FINISHED_CONDITION_GENERATED)
		{
			// do nothing, the finalizing condition is being generated
		}
		else if (writeFinished == WRITE_FINISHED)
		{
			// see note1. Here you come in during a restart after write or
			// if (after stop after write) an other host is attached
			// to this module before the write of the last byte is finished
			// the first time clear this interrupt avoiding triggering during
			// sending the last byte, but during the stop
			// or restart condition this is triggered all the time

			if (sr1 & I2C_SR1_BTF)
			{// perform specially the stop condition after the last byte is ready
				if (nextOperation == xpcc::i2c::Delegate::RESTART_OP)
				{
					// should not happen (doubled code)
					DEBUG(TXE_BTF_NEXT_RESTART);
					I2C2->CR1 |= I2C_CR1_START;
					writeFinished = WRITE_FINISHED_CONDITION_GENERATED;
				}
				else
				{
					// we are ready
					// clear interrupts, generate stop condition detach delegate
					I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
					I2C2->CR1 |= I2C_CR1_STOP;
					delegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
					delegate = 0;
					writeFinished = WRITE_FINISHED_CONDITION_GENERATED;
				}
			}
			else
			{
				I2C2->CR2 &= ~I2C_CR2_ITBUFEN; // clear interrupt for txe, so wait for btf
				DEBUG(AFTER_WRITE_FINISHED_IRQ);
			}
		}
		else
		{
			// EV8..: TxE=1 and BTF may be 1, Program Stop request. TxE and BTF are
			// cleared by hardware after the Stop condition see note1
			if (writeBytesLeft == 0)
			{
				DEBUG(TXE_NO_BYTES_LEFT);
				
				if (nextOperation == xpcc::i2c::Delegate::WRITE_OP)
				{
					DEBUG(TXE_NEXT_WRITE);
					initializeWrite(delegate->writing());
				}
				else if (nextOperation == xpcc::i2c::Delegate::RESTART_OP)
				{
					DEBUG(TXE_NEXT_RESTART);

					// see note1. It is possible to set restart here, no need to wait
					// for btf
					I2C2->CR1 |= I2C_CR1_START;
					writeFinished = WRITE_FINISHED_CONDITION_GENERATED;
				}
				else
				{ // STOP and default behaviour
					DEBUG(TXE_NEXT_STOP);

					// don't set the stop flag here, wait for btf
					I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
					writeFinished = WRITE_FINISHED;
				}
			}
			else
			{ // writeBytesLeft != 0
				DEBUG(TXE_MORE_BYTES_LEFT);
				--writeBytesLeft;
				I2C2->DR = *writePointer++; // write data
			}
		}
	}
	
	else if (sr1 & I2C_SR1_RXNE)
	{
		DEBUG(RXNE_IRQ);
		// EV7: RxNE = 1, cleared by reading DR register
		uint16_t dr = I2C2->DR; // clear RXNE

		if (readBytesLeft > 0)
		{
			*readPointer++ = dr & 0xff;
			readBytesLeft--;
		}
		
		switch (readBytesLeft)
		{
			default:
				DEBUG(RXNE_MANY_BYTES_LEFT);
				// ACK is set by default, no need to set it again
				break;
				
			case 1:
				DEBUG(RXNE_ONE_BYTE_LEFT);
				// EV7_1: programming ACK = 0 and STOP/RESTART request
				I2C2->CR1 &= ~I2C_CR1_ACK;
				break;
				
			case 0:
				switch (nextOperation)
				{
					case xpcc::i2c::Delegate::RESTART_OP:
						DEBUG(RXNE_NO_BYTES_LEFT_NEXT_RESTART);
						I2C2->CR1 |= I2C_CR1_START;
						break;
						
					default: // should not happen // TODO signal wrong operation or not? eigentlich haette das schon vorher aufgefallen sein muessen
					case xpcc::i2c::Delegate::STOP_OP:
						DEBUG(RXNE_NO_BYTES_LEFT_NEXT_STOP);
						if (readFinished == READ_NOT_FINISHED)
						{
							I2C2->CR1 |= I2C_CR1_STOP;
						}
						// Disable all interrupts
						I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
						delegate->stopped(xpcc::i2c::Delegate::NORMAL_STOP);
						delegate = 0;
						break;
				}
				break;
		}
	}
}

// ----------------------------------------------------------------------------
extern "C" void
I2C2_ER_IRQHandler(void)
{
	DEBUG(IRQ_ER);
	uint16_t sr1 = I2C2->SR1;

	writeBytesLeft = 0;
	readBytesLeft = 0;

	if (sr1 & I2C_SR1_BERR)
	{
		I2C2->CR1 |= I2C_CR1_STOP;
		if (delegate)
		{
			errorState = xpcc::stm32::I2cMaster2::BUS_ERROR;
			delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
			delegate = 0;
		}
	}
	else if (sr1 & I2C_SR1_AF)
	{			// acknowledge fail
		I2C2->CR1 |= I2C_CR1_STOP;
		if (delegate)
		{
			errorState = xpcc::stm32::I2cMaster2::DATA_NACK; // may olso be ADDRESS_NACK
			delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
			delegate = 0;
		}
	}
	else if (sr1 & I2C_SR1_ARLO)
	{		// arbitration lost
		if (delegate)
		{
			errorState = xpcc::stm32::I2cMaster2::ARBITRATION_LOST;
			delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
			delegate = 0;
		}
	}
	else
	{
		if (delegate)
		{
			errorState = xpcc::stm32::I2cMaster2::UNKNOWN_ERROR;
			delegate->stopped(xpcc::i2c::Delegate::ERROR_CONDITION);
			delegate = 0;
		}
	}
	// Overrun error is not handled here separately

	// Clear flags and interrupts
	I2C2->SR1 = 0;
	I2C2->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::I2cMaster2::configurePins(Mapping mapping)
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
xpcc::stm32::I2cMaster2::enable()
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
}

void
xpcc::stm32::I2cMaster2::initialize(uint16_t ccrPrescaler)
{
	reset();
	
	// Enable clock
	enable();
	
	I2C2->CR1 = I2C_CR1_SWRST; 		// reset module
	I2C2->CR1 = 0;

	NVIC_EnableIRQ(I2C2_ER_IRQn);
	NVIC_EnableIRQ(I2C2_EV_IRQn);

	I2C2->CCR = 0x28; 	// baudrate (prescaler), only while i2c disabled, PE = 0
	// not enable interrupts | Value of peripheral clock (Unknown effect of this value yet, since Module is always running on APB frequency)
	I2C2->CR2 = 0x08;
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
		DEBUG('\n');
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
		DEBUG('\n');
		::delegate = delegate;
		callStarting();
		
		while(::delegate)
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
