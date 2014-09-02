// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SSD1306_HPP
#define XPCC_SSD1306_HPP

#include <xpcc/ui/display/buffered_graphic_display.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/periodic_timer.hpp>

namespace xpcc
{

struct ssd1306
{
	struct Command
	{
		enum
		{
			// fundamental commands
			SetContrastControl = 0x81,
			SetChargePump = 0x8D,
			SetEntireDisplayResumeToRam = 0xA4,
			SetEntireDisplayIgnoreRam = 0xA5,
			SetNormalDisplay = 0xA6,
			SetInvertedDisplay = 0xA7,
			SetDisplayOff = 0xAE,
			SetDisplayOn = 0xAF,

			// addressing commands
			SetLowerColumnStartAddress = 0x00,
			SetHigherColumnStartAddress = 0x10,
			SetMemoryMode = 0x20,
			SetColumnAddress = 0x21,
			SetPageAddress = 0x22,
			SetPageStartAddress = 0xB0,

			// Hardware configuration
			SetDisplayStartLine = 0x40,
			SetSegmentRemap0 = 0xA0,
			SetSegmentRemap127 = 0xA1,
			SetMultiplexRatio = 0xA8,
			SetComOutputScanDirectionIncrement = 0xC0,
			SetComOutputScanDirectionDecrement = 0xC8,
			SetDisplayOffset = 0xD3,
			SetComPins = 0xDA,

			// timing configuration
			SetDisplayClockDivideRatio = 0xD5,
			SetPreChargePeriod = 0xD9,
			SetV_DeselectLevel = 0xDB,
			Nop = 0xE3,
		};
	};
};


/**
 * Driver for SSD1306 based OLED-displays using I2C.
 *
 * The driver updates the display with 10Hz by default, which is the limit
 * with an I2C baudrate of 100kHz.
 * You may get higher update rates with higher bus speeds.
 *
 * @author	Niklas Hauser
 * @ingroup	lcd
 */
template < class I2cMaster >
class Ssd1306 : public ssd1306, public xpcc::I2cDevice<I2cMaster>,
				public xpcc::pt::NestedProtothread<1>, public BufferedGraphicDisplay<128, 64>
{
public:
	Ssd1306(uint8_t address = 0x3C);

	/// Automatically update the display with the content of the RAM buffer.
	virtual void
	update();

	/// Set a new update rate.
	/// The maximum update rate depends on I2C baudrate!
	void
	setUpdateRate(uint8_t framesPerSecond)
	{
		timer.restart(1000/framesPerSecond);
	}

	bool
	sync()
	{
		return (i2cTask == I2cTask::WriteDisplay);
	}

	// MARK: - TASKS
	/// pings the diplay
	xpcc::pt::Result
	ping(void *ctx);

	/// initializes for 3V3 with charge-pump
	xpcc::pt::Result
	initialize(void *ctx);

	/// Starts a frame transfer to the display
	/// You can use this to avoid tearing or to
	/// get higher frames rates than 10Hz, if the I2C speed allows for it.
	xpcc::pt::Result
	writeDisplay(void *ctx);

	/// invert the display
	xpcc::pt::Result ALWAYS_INLINE
	invertDisplay(void *ctx, bool inverted = true);


	/// Write a command without data
	xpcc::pt::Result
	writeCommand(void *ctx, uint8_t command);

	/// Write a command with one byte data
	xpcc::pt::Result
	writeCommand(void *ctx, uint8_t command, uint8_t data);

	/// Write a command with two bytes data
	xpcc::pt::Result
	writeCommand(void *ctx, uint8_t command, uint8_t data1, uint8_t data2);

	/// Write a command with 5 bytes data (for scrolling)
	xpcc::pt::Result
	writeCommand(void *ctx, uint8_t command,
			uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5);

	/// Write a command with 6 bytes data (for scrolling)
	xpcc::pt::Result
	writeCommand(void *ctx, uint8_t command,
			uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6);

private:
	bool
	startTransactionWithLength(uint8_t length);

	struct I2cTask
	{
		enum
		{
			Idle = 0,
			// insert all ssd1306::Command
			WriteDisplay = 0xFE,
			Ping = 0xFF
		};
	};

	class DataTransmissionAdapter : public xpcc::I2cWriteAdapter
	{
	public:
		DataTransmissionAdapter(uint8_t address)
		:	I2cWriteAdapter(address), control(0x40)
		{
		}

		bool inline
		configureWrite(uint8_t (*buffer)[8], std::size_t size)
		{
			if (I2cWriteAdapter::configureWrite(&buffer[0][0], size))
			{
				displayBuffer = buffer;
				page = 0xff;
				row = 0;
				return true;
			}
			return false;
		}

	protected:
		virtual Writing
		writing()
		{
			// the first byte is 0x40, which tells the display
			// that (a lot of) data is coming next.
			if (page == 0xff)
			{
				page = 0;
				return Writing(&control, 1, OperationAfterWrite::Write);
			}

			// the display expects the frame slightly differently than in memory
			// this is the same format as using in ks0108
			if (row >= 128)
			{
				row = 0;
				page++;
			}

			// return one byte and continue writing.
			return Writing(&displayBuffer[row++][page], 1,
					--size ? OperationAfterWrite::Write : OperationAfterWrite::Stop);
		}

		uint8_t control;
		uint8_t page;
		uint8_t row;
		uint8_t (*displayBuffer)[8];
	};

private:
	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	uint8_t commandBuffer[14];
	xpcc::PeriodicTimer<> timer;
	bool initSuccessful;
	bool updateWrite;

	xpcc::I2cTagAdapter<xpcc::I2cWriteAdapter> adapter;
	xpcc::I2cTagAdapter<DataTransmissionAdapter> adapterData;
};

} // namespace xpcc

#include "ssd1306_impl.hpp"

#endif // XPCC_SSD1306_HPP
