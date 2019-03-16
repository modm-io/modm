/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_HPP
#	error	"Don't include this file directly, use 'ssd1306.hpp' instead!"
#endif

template < class I2cMaster, uint8_t Height >
modm::Ssd1306<I2cMaster, Height>::Ssd1306(uint8_t address)
:	I2cDevice<I2cMaster, 2, ssd1306::DataTransmissionAdapter<Height>>(address),
	commandBuffer{0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0}
{
	this->transaction.setCommandBuffer(commandBuffer);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::initialize()
{
	RF_BEGIN();

	commandBuffer[11] = true;
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetDisplayOff));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetDisplayClockDivideRatio, 0xF0));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetMultiplexRatio, 0x3F));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetDisplayOffset, 0x00));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetDisplayStartLine | 0x00));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetChargePump, 0x14));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetMemoryMode, 0x01));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetSegmentRemap127));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetComOutputScanDirectionDecrement));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetComPins, ((Height == 64) ? 0x12 : 0x02)));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetContrastControl, 0xCE));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetPreChargePeriod, 0xF1));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetV_DeselectLevel, 0x40));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetEntireDisplayResumeToRam));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetNormalDisplay));
//	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetColumnAddress, 0, 127));
//	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetPageAddress, 0, 7));
	commandBuffer[11] &= RF_CALL(writeCommand(Command::SetDisplayOn));

	RF_END_RETURN(commandBuffer[11]);
}

// ----------------------------------------------------------------------------
template < class I2cMaster, uint8_t Height >
modm::ResumableResult<void>
modm::Ssd1306<I2cMaster, Height>::startWriteDisplay()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(
	    this->transaction.configureDisplayWrite(this->display_buffer,
                                                (this->displayBufferWidth * this->displayBufferHeight))
	              and this->startTransaction());

	RF_END();
}

template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeDisplay()
{
	RF_BEGIN();

	RF_CALL(startWriteDisplay());

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::setRotation(Rotation rotation)
{
	RF_BEGIN();

	if ( RF_CALL(writeCommand((rotation == Rotation::Normal) ?
			Command::SetSegmentRemap127 : Command::SetSegmentRemap0)) )
	{
		RF_RETURN_CALL(writeCommand((rotation == Rotation::Normal) ?
				Command::SetComOutputScanDirectionDecrement : Command::SetComOutputScanDirectionIncrement));
	}

	RF_END_RETURN(false);
}

template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::configureScroll(uint8_t origin, uint8_t size,
		ScrollDirection direction, ScrollStep steps)
{
	RF_BEGIN();

	if (!RF_CALL(disableScroll()))
		RF_RETURN(false);

	{
		uint8_t beginY = (origin > 7) ? 7 : origin;

		uint8_t endY = ((origin + size) > 7) ? 7 : (origin + size);
		if (endY < beginY) endY = beginY;

		commandBuffer[1] = static_cast<uint8_t>(direction);
		commandBuffer[3] = 0x00;
		commandBuffer[5] = beginY;
		commandBuffer[7] = i(steps);
		commandBuffer[9] = endY;
		commandBuffer[11] = 0x00;
		commandBuffer[13] = 0xFF;
	}

	RF_WAIT_UNTIL( startTransactionWithLength(14) );

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeCommand(uint8_t command)
{
	RF_BEGIN();

	commandBuffer[1] = command;
	RF_WAIT_UNTIL( startTransactionWithLength(2) );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeCommand(uint8_t command, uint8_t data)
{
	RF_BEGIN();

	commandBuffer[1] = command;
	commandBuffer[3] = data;
	RF_WAIT_UNTIL( startTransactionWithLength(4) );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

template < class I2cMaster, uint8_t Height >
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeCommand(uint8_t command, uint8_t data1, uint8_t data2)
{
	RF_BEGIN();

	commandBuffer[1] = command;
	commandBuffer[3] = data1;
	commandBuffer[5] = data2;
	RF_WAIT_UNTIL( startTransactionWithLength(6) );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

// ----------------------------------------------------------------------------
template < class I2cMaster, uint8_t Height >
bool
modm::Ssd1306<I2cMaster, Height>::startTransactionWithLength(uint8_t length)
{
	return this->startWrite(commandBuffer, length);
}
