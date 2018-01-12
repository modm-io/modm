#include <modm/board/board.hpp>
#include <modm/processing/timer.hpp>

#include <modm/processing/protothread.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

/*
 * Test of I2C transaction with STM32L4
 *
 * A simple I2C device with write, read, and write_read transaction is implemented.
 * A device responding to address 0x3c must be connected, e.g. SSD1306 display.
 * The display must be powercycled after writing garbage to it. It may not be initialized
 * after this test programm.
 *
 * Observe the I2C transactions either with pulseview or with sigrok-cli, e.g.
 * sigrok-cli --driver fx2lafw --config samplerate=1m --samples 64k --channels D4=sda,D5=scl --triggers sda=f --wait-trigger -P i2c:sda=sda:scl=scl
 */

using MyI2cMaster = modm::platform::I2cMaster1;

using namespace Board;

template < class I2cMaster >
class I2cTestDevice : public modm::I2cDevice<I2cMaster, 2>
{
public:
	I2cTestDevice(uint8_t address = 0x3C);

	modm::ResumableResult<bool>
	write(uint16_t len);

	modm::ResumableResult<bool>
	read(uint16_t len);

	modm::ResumableResult<bool>
	writeRead(uint16_t write_len, uint16_t read_len);

private:
	uint8_t buffer[1024];
};

template < typename I2cMaster >
I2cTestDevice<I2cMaster>::I2cTestDevice(uint8_t address) :
	modm::I2cDevice<I2cMaster, 2>(address)
{
}

template < typename I2cMaster >
modm::ResumableResult<bool>
I2cTestDevice<I2cMaster>::write(uint16_t len)
{
	RF_BEGIN();

	buffer[0] = 0xaa;
	buffer[1] = 0x55;
	buffer[2] = 0x82;
	buffer[3] = 0x11;
	buffer[4] = 0x22;

	for (uint16_t ii = 0; ii < 1024; ++ii) {
		buffer[ii] = ii + 1;
	}
	buffer[255] = 0x82;
	this->transaction.configureWrite(buffer, len);

	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}

	RF_END_RETURN(true);
}

template < typename I2cMaster >
modm::ResumableResult<bool>
I2cTestDevice<I2cMaster>::read(uint16_t len)
{
	RF_BEGIN();

	this->transaction.configureRead(buffer, len);

	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}
	for (uint16_t ii = 0; ii < len; ++ii) {
		MODM_LOG_DEBUG.printf("%02x ", buffer[ii]);
	}
	MODM_LOG_DEBUG << modm::endl;

	RF_END_RETURN(true);
}

template < typename I2cMaster >
modm::ResumableResult<bool>
I2cTestDevice<I2cMaster>::writeRead(uint16_t write_len, uint16_t read_len)
{
	RF_BEGIN();

	for (uint16_t ii = 0; ii < 1024; ++ii) {
		buffer[ii] = ii + 1;
	}
	buffer[255] = 0x82;

	this->transaction.configureWriteRead(buffer, write_len, buffer, read_len);

	if (not RF_CALL( this->runTransaction() )) {
		RF_RETURN(false);
	}

	for (uint16_t ii = 0; ii < read_len; ++ii) {
		MODM_LOG_DEBUG.printf("%02x ", buffer[ii]);
	}
	MODM_LOG_DEBUG << modm::endl;

	RF_END_RETURN(true);
}

I2cTestDevice< MyI2cMaster > i2c;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<Board::D14::Sda, Board::D15::Scl>();
	MyI2cMaster::initialize<Board::systemClock, MyI2cMaster::Baudrate::Standard>();

	LedGreen::set();

	RF_CALL_BLOCKING(i2c.ping());
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.write(0));
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.write(1));
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.write(2));
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.writeRead(0, 5));
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.writeRead(1, 5));
	modm::delayMicroseconds(25);

	RF_CALL_BLOCKING(i2c.writeRead(2, 5));
	modm::delayMicroseconds(25);

	// Blink if run without hanging.
	while(true) {
		LedGreen::toggle();
		modm::delayMilliseconds(500);
	};

	return 0;
}
