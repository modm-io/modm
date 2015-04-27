// coding: utf-8
// ----------------------------------------------------------------------------
 /*
  * Copyright (c) 2015, Christian Menard
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
  *     * Neither the name of the copyright holder nor the names of its
  *       contributors may be used to endorse or promote products derived from
  *       this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
  * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */
// ----------------------------------------------------------------------------

#ifndef XPCC__PCA9685_HPP
#	error "Don't include this file directly, use 'pca9685.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename I2cMaster>
xpcc::Pca9685<I2cMaster>::Pca9685(uint8_t address) :
	I2cDevice<I2cMaster, 1, I2cWriteAdapter>(address)
{}

template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Pca9685<I2cMaster>::initialize(uint8_t mode1, uint8_t mode2)
{
	CO_BEGIN();

	// set the first mode register
	buffer[0] = pca9685::REG_MODE1;
	buffer[1] = mode1 | pca9685::MODE1_AI;  // ensure that auto increment is enabled

	CO_WAIT_UNTIL( this->startWrite(buffer, 2) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	if (not this->wasTransactionSuccessful())
	{
		CO_RETURN(false);
	}

	// set the second mode register

	buffer[0] = pca9685::REG_MODE2;
	buffer[1] = mode2;

	CO_WAIT_UNTIL( this->startWrite(buffer, 2) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	if (not this->wasTransactionSuccessful())
	{
		CO_RETURN(false);
	}

	// Always turn on all LEDs at tick 0 and switch them of later according
	// to the current value

	buffer[0] = pca9685::REG_ALL_LED_ON_L;
	buffer[1] = 0x00;
	buffer[2] = 0x00;

	CO_WAIT_UNTIL( this->startWrite(buffer, 3) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	if (not this->wasTransactionSuccessful())
	{
		CO_RETURN(false);
	}

	CO_END_RETURN(true);
}

template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Pca9685<I2cMaster>::setChannel(uint8_t channel, uint16_t value)
{
	CO_BEGIN();

	// there are only 16 channels
	if (channel >= 16)
		CO_RETURN(false);

	buffer[0] = pca9685::REG_LED0_OFF_L + 4 * channel;
	// The Controller turns all LEDs on at tick 0
	// and turns this LED of at value
	buffer[1] = uint8_t(value);
	buffer[2] = uint8_t(value >> 8) & 0x0f;

	CO_WAIT_UNTIL( this->startWrite(buffer, 3) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}

template<typename I2cMaster>
xpcc::co::Result<bool>
xpcc::Pca9685<I2cMaster>::setAllChannels(uint16_t value)
{
	CO_BEGIN();

	buffer[0] = pca9685::REG_ALL_LED_OFF_L;
	// The Controller turns all LEDs on at tick 0
	// and turns this LED of at tick $value
	buffer[1] = uint8_t(value);
	buffer[2] = uint8_t(value >> 8) & 0x0f;

	CO_WAIT_UNTIL( this->startWrite(buffer, 3) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}
