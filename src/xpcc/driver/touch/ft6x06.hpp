/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */

#ifndef XPCC_FT6X06_HPP
#define XPCC_FT6X06_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>

namespace xpcc
{

// forward declaration for friending with ft6x06::Data
template < class I2cMaster >
class Ft6x06;

struct ft6x06
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		DEV_MODE = 0x00,

		GEST_ID = 0x01,
		TD_STATUS = 0x02,

		P1_XH = 0x03,
		P1_XL = 0x04,
		P1_YH = 0x05,
		P1_YL = 0x06,
		P1_WEIGHT = 0x07,
		P1_MISC = 0x08,

		P2_XH = 0x09,
		P2_XL = 0x0A,
		P2_YH = 0x0B,
		P2_YL = 0x0C,
		P2_WEIGHT = 0x0D,
		P2_MISC = 0x0E,

		TH_GROUP = 0x80,
		TH_DIFF = 0x85,

		CTRL = 0x86,
		TIME_ENTER_MONITOR = 0x87,
		PERIOD_ACTIVE = 0x88,
		PERIOD_MONITOR = 0x89,
		RADIAN_VALUE = 0x91,
		OFFSET_LEFT_RIGHT = 0x92,
		OFFSET_UP_DOWN = 0x93,
		DISTANCE_LEFT_RIGHT = 0x94,
		DISTANCE_UP_DOWN = 0x95,
		DISTANCE_ZOOM = 0x96,

		LIB_VER_H = 0xA1,
		LIB_VER_L = 0xA2,
		CIPHER = 0xA3,
		G_MODE = 0xA4,
		PWR_MODE = 0xA5,
		FIRMID = 0xA6,
		FOCALTECH_ID = 0xA8,
		RELEASE_CODE_ID = 0xAF,

		STATE = 0xBC,
	};
	/// @endcond

public:
	enum class
	Gesture : uint8_t
	{
		NoGesture = 0x00,
		MoveUp = 0x10,
		MoveRight = 0x14,
		MoveDown = 0x18,
		MoveLeft = 0x1C,
		ZoomIn = 0x48,
		ZoomOut = 0x49,
	};

	enum class
	Event : uint8_t
	{
		NoEvent = (0b11 << 6),
		PressDown = (0b00 << 6),
		LiftUp = (0b01 << 6),
		Contact = (0b10 << 6),
	};

	enum class
	InterruptMode : uint8_t
	{
		Polling = 0,
		Trigger = 1
	};

public:
	struct
	touch_t
	{
		touch_t() : id(0), event(Event::NoEvent), x(0), y(0) {}

		uint8_t id;
		Event event;

		uint16_t x;
		uint16_t y;

		// are always zero
		// uint8_t weight;
		// uint8_t area;
	};

	struct ATTRIBUTE_PACKED
	Data
	{
		template< class I2cMaster >
		friend class Ft6x06;

		inline Gesture
		getGesture() { return Gesture(data[0]); }

		inline uint8_t
		getPoints() { return data[1]; }

		inline bool
		getTouch(touch_t *t, uint8_t index)
		{
			if (index >= 2) return false;
			// touches start at offset 2 and are 6B long
			uint8_t *d = data + 2 + index * 6;

			t->event = Event(d[0] & 0xc0);
			t->x = ((d[0] & 0x0f) << 8) | d[1];
			t->id = d[2] >> 4;
			t->y = ((d[2] & 0x0f) << 8) | d[3];
			// weight and area are always zero
			// t->weight = d[4];
			// t->area = d[5] >> 4;

			return true;
		}

	private:
		uint8_t data[14];
	};
}; // struct ft6x06

/**
 * FT6x06 capacitive touch panel driver.
 *
 * The driver is reasonably simple, due to the datasheet not being very
 * descriptive.
 *
 * @author	Niklas Hauser
 * @ingroup driver_touch
 */
template < typename I2cMaster >
class Ft6x06 : public ft6x06, public xpcc::I2cDevice< I2cMaster, 3 >
{
public:
	/// Constructor, requires an ft6x06::Data object, sets address to default of 0x2A
	Ft6x06(Data &data, uint8_t address=0x2A);

	xpcc::ResumableResult<bool>
	configure(InterruptMode mode, uint8_t activeRate = 60, uint8_t monitorRate = 25);

	/// Reads all touches and writes the result to the data object
	xpcc::ResumableResult<bool>
	readTouches();

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

protected:
	/// @cond
	/// write a 8bit value a register
	xpcc::ResumableResult<bool>
	write(Register reg, uint8_t value);

	/// read multiple 8bit values from a start register
	xpcc::ResumableResult<bool>
	read(Register reg, uint8_t *buffer, uint8_t length);
	/// @endcond

protected:
	/// @cond
	Data &data;
	// the read buffer is for a continous read from address 0x01 -> 0x0E
	uint8_t buffer[14];
	/// @endcond
};

}	// namespace xpcc

#include "ft6x06_impl.hpp"

#endif // XPCC_FT6X06_HPP
