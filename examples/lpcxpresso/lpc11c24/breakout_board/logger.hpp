#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <xpcc/architecture.hpp>

// ugly debugging
#ifdef XPCC__HOSTED_HPP
#include <stdio.h>

namespace xpcc
{
namespace lpc
{
class Uart1
{
public:
	static void
	write(const uint8_t * ptr, size_t size)
	{
		// ...
		for (size_t ii = 0; ii < size; ++ii) {
			printf("%3d ", ptr[ii]);
		}
		printf("\n");
	}
};
}
}
#endif

namespace xpcc
{

/**
 * This is a logger for capturing fast data on the target and transfering
 * it to a computer for displaying.
 * It is intend for development, design and debugging of control loops.
 *
 * Each hardware interface (UART, CAN, ...) can have one Logger.
 * Each Logger can have several Streams which have several Channels.
 *
 * Example:
 *
 * Logger                     Logger, connected to the (USB) UART
 *   +-- engine               Stream
 *   |   +-- pwmLeft            Channel
 *   |   +-- pwmRight           Channel
 *   +-- beacon               Stream
 *       +-- ir0                Channel
 *       +-- ir1                Channel
 *
 * Each stream is independent from each other. There is no synchronization.
 * Synchronization can be achieved if write() is called at the same rate.
 *
 * How to use the Logger:
 *
 * First, configure the channels by defining a packed struct of your
 * data you like to send to the computer. It is used as a template parameter
 * for the Stream.
 *
 * Example:
 *
 * 	struct EngineSample
	{
		int16_t pwm;
		int16_t speedTicks;
		int16_t speedTime;
		int16_t current;
		uint8_t currentLimited;
	} __attribute__((packed));
 *
 * Second, outside of your main define a single logger. Each logger
 * can have several logger streams. The struct is used
 * as template parameter to configure the logger stream.
 *
 * static xpcc::Logger::OutputStream<logger::Sample> loggerStream;
 *
 * t.b.d: Optional: Define and arm a value-based trigger.
 *
 * Third, in the loop, always fill all channels with data.
 * If you do not update the data the last value will be send again.
 * (Or undefined behaviour if stream is implemented with pointers instead of memcpy)
 *
 * engineStream.channel.pwm = 55;
 *
 * Fourth, call write() once every cycle of your loop at the end.
 * All your data is written to a ringbuffer.
 * The latest 32 samples are kept. (TODO Make sample count configurable.)
 *
 * Fifth, call trigger(preTrigger) to set a trigger at the current sample.
 * preTrigger is the number of samples you want to to see before the trigger
 * (like 'pre-trigger' on an oscilloscope).
 * If the remaining buffer is filled the data is send to the computer.
 *
 * No samples are taken until the last sample is send to the computer.
 *
 * The GUI does not know anything about the data types. There is some
 * work to do.
 *
 *
 */
class Logger
{
private:
	enum class State
	{
		READY,
		TRIGGERED,
		FULL,
		FINISHED,
	};

public:
	template <typename T>
	class OutputStream
	{
		friend class Logger;

	public:
		T channel;

		OutputStream();

		inline bool
		isValid()
		{
			return true;
		}

		inline void
		write()
		{
			// when IDLE or TRIGGERED write sample to ringbuffer.
			// When the
			if ((state == State::READY) || (state == State::TRIGGERED))
			{
				// Keep copying current values to the ring buffer
				memcpy(&samples[currentSample], &channel, sizeof(T));

				if ((state == State::TRIGGERED) && (currentSample == endSample)) {
					// The buffer is now full. Writing more samples would overwrite the pre-triggered data
					state = State::FULL;
				}
				else {
					// increase index, wrap around
					currentSample = (currentSample + 1) % 32;
				}
			}
		} // write

		/**
		 * Trigger the logger stream.
		 */

		// TODO: If less than preTrigger samples were collected,
		// take a note.

		inline void
		trigger(uint16_t preTrigger)
		{
			if (state == State::READY) {
				// remember the sample of the trigger
				triggerSample = currentSample;

				// calculate last sample and wrap around
				endSample = (currentSample + 32 - preTrigger) % 32;

				// calculate first sample and wrap around
				firstSample = (currentSample + (32 + 1) - preTrigger) % 32;

				state = State::TRIGGERED;
			}
		}

		inline void
		close()
		{
		}

		inline void
		update()
		{
			if (state == State::FULL)
			{
				static uint16_t sampleIndex = firstSample;
				xpcc::lpc::Uart1::write( (const uint8_t *) (&samples[sampleIndex]), sizeof(T));

				if (sampleIndex != endSample) {
					sampleIndex = (sampleIndex + 1) % 32;
				}
				else {
					// All sent, ready again.
					currentSample = 0;
					state = State::READY;
				}
			}
		}

	private:
		T samples[32];

		State state;

		uint16_t currentSample;
		uint16_t triggerSample;
		uint16_t firstSample;
		uint16_t endSample;
	};

public:
	static void
	initialize();

//	static void
//	update();
};

} // xpcc namespace

template <typename T>
xpcc::Logger::OutputStream<T>::OutputStream() :
	state(State::READY),
	currentSample(0),
	triggerSample(0),
	firstSample(0)
{}


#endif // LOGGER_HPP
