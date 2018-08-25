/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing/protothread.hpp>

using namespace Board;
using namespace modm::glcd;
using namespace Board::ft6;

class LineDrawer : public modm::pt::Protothread
{
public:
	LineDrawer() :
		touch(touchData),
		display{Board::getDisplay()},
		px{-1, -1}, py{-1, -1},
		c{Color::white(), Color::white()}
		{}

	bool
	update()
	{
		PT_BEGIN();

		// Configure the touchscreen to sample with 60Hz in active and monitor mode.
		PT_CALL(touch.configure(Touch::InterruptMode::Trigger, 60, 60));

		while (1)
		{
			do {
				// Wait for either touchscreen interrupt or clear screen button
				PT_WAIT_UNTIL(Int::getExternalInterruptFlag() or Button::read());
				if (Button::read()) display.clear();
			} while (not Int::getExternalInterruptFlag());

			Int::acknowledgeExternalInterruptFlag();
			LedRed::set();

			PT_CALL(touch.readTouches());

			for (int ii=0; ii < 2; ii++)
			{
				Touch::touch_t tp;
				touch.getData().getTouch(&tp, ii);
				// mirror and rotate correctly
				uint16_t x{tp.y}, y{uint16_t(480 - tp.x)};

				if (tp.event == Touch::Event::PressDown or (px[tp.id] < 0))
				{
					// remember the start point
					px[tp.id] = x; py[tp.id] = y;
				}
				else if (tp.event == Touch::Event::Contact)
				{
					// draw the line from the previous point to this one
					display.setColor(c[tp.id]);
					display.drawLine(px[tp.id], py[tp.id], x, y);
					// remember this point for next time.
					px[tp.id] = x; py[tp.id] = y;
				}
				else if (tp.event == Touch::Event::LiftUp)
				{
					// invalidate points
					px[tp.id] = -1; py[tp.id] = -1;
					// generate new random color
					c[tp.id] = Color(uint16_t(rand()));
				}
			}
			LedRed::reset();
		}

		PT_END();
	}

private:
	Touch::Data touchData;
	Touch touch;
	modm::GraphicDisplay& display;
	int16_t px[2], py[2];
	Color c[2];
};

LineDrawer drawer;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

	while(1)
	{
		drawer.update();
	}

	return 0;
}
