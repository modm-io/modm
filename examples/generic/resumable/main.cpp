#include <modm/board/board.hpp>
#include <modm/processing/processing.hpp>

using Led = Board::LedGreen;

class BlinkingLight : public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:
    bool
    run()
    {
        PT_BEGIN();

        // set everything up
        Led::setOutput();
        Led::set();

        while (true)
        {
            Led::set();
            PT_CALL(waitForTimer());

            Led::reset();
            PT_CALL(setTimer(200));

            PT_WAIT_UNTIL(timeout.isExpired());
        }

        PT_END();
    }

    modm::ResumableResult<bool>
    waitForTimer()
    {
        RF_BEGIN();

        // nested calling is allowed
        if (RF_CALL(setTimer(100)))
        {
            RF_WAIT_UNTIL(timeout.isExpired());
            RF_RETURN(true);
        }

        RF_END_RETURN(false);
    }

    modm::ResumableResult<bool>
    setTimer(uint16_t new_timeout)
    {
        RF_BEGIN();

        timeout.restart(new_timeout);

        if(timeout.isArmed()) {
            RF_RETURN(true);
        }

        // clean up code goes here

        RF_END_RETURN(false);
    }

private:
    modm::ShortTimeout timeout;
};

BlinkingLight light;

int main(void)
{
	while (true) {
    	light.run();
	}
}
