#include "../stm32f3_discovery.hpp"
#include <xpcc/processing.hpp>
#include <xpcc/math.hpp>

using namespace Board;

// create the data object
Board::lsm3::Accelerometer::Data data;
// and hand it to the sensor driver
Board::lsm3::Accelerometer accelerometer(data);


class ReaderThread : public xpcc::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		// initialize with limited range of ±2g
		PT_CALL(accelerometer.configure(accelerometer.Scale::G2));

		while (true)
		{
			// read out the sensor
			PT_CALL(accelerometer.readAcceleration());

			averageX.update(accelerometer.getData().getX());
			averageY.update(accelerometer.getData().getY());

			{
				bool xs = averageX.getValue() < -0.2;
				bool xn = averageX.getValue() >  0.2;

				bool xe = averageY.getValue() < -0.2;
				bool xw = averageY.getValue() >  0.2;

				Board::LedNorth::set(xn and not (xe or xw));
				Board::LedNorthEast::set(xn and xe);
				Board::LedNorthWest::set(xn and xw);

				Board::LedEast::set(xe and not (xs or xn));
				Board::LedWest::set(xw and not (xs or xn));

				Board::LedSouthEast::set(xs and xe);
				Board::LedSouthWest::set(xs and xw);
				Board::LedSouth::set(xs and not (xe or xw));
			}

			// repeat every 5 ms
			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	xpcc::ShortTimeout timeout;
	xpcc::filter::MovingAverage<float, 25> averageX;
	xpcc::filter::MovingAverage<float, 25> averageY;
};

ReaderThread reader;


MAIN_FUNCTION
{
	Board::initialize();
	Board::initializeLsm3();

	while (1)
	{
		reader.update();
	}

	return 0;
}
