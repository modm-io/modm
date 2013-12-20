#include <xpcc/architecture/platform.hpp>
#include <xpcc/workflow.hpp>

// IO #########################################################################
GPIO__OUTPUT(LED, A, 8);
GPIO__INPUT(Button, C, 13);
GPIO__IO(SCL, B, 8);
GPIO__IO(SDA, B, 9);


// STREAM #####################################################################
typedef xpcc::stm32::BufferedUart5 DebugUart;
DebugUart debugUart(115200, 12);
#include <xpcc/io/iodevice_wrapper.hpp>
#include <xpcc/io/iostream.hpp>
xpcc::IODeviceWrapper<DebugUart> device(debugUart);
xpcc::IOStream stream(device);


// SENSORS ####################################################################
#define USE_HARDWARE_I2C 1

#if USE_HARDWARE_I2C
typedef xpcc::stm32::I2cMaster1 Twi1;
#else
#include <xpcc/communication/i2c/software_i2c.hpp>
typedef xpcc::SoftwareI2C< SCL, SDA > Twi1;
#endif

#include <xpcc/driver/pressure/bmp085.hpp>
uint8_t barometerData[5];
xpcc::Bmp085< Twi1 > barometer(barometerData);
xpcc::PeriodicTimer<> barometerTimer(100);

#include <xpcc/driver/inertial/itg3200.hpp>
uint8_t gyroscopeData[8];
xpcc::Itg3200< Twi1 > gyroscope(gyroscopeData);
GPIO__INPUT(INTG, E, 1);
bool intgStatus = false;

#include <xpcc/driver/inertial/hmc5883l.hpp>
uint8_t magnetometerData[6];
xpcc::Hmc5883l< Twi1 > magnetometer(magnetometerData);
GPIO__INPUT(INTM, E, 2);
bool intmStatus = false;

#include <xpcc/driver/inertial/bma180.hpp>
uint8_t accelerometerData[8];
xpcc::Bma180< Twi1 > accelerometer(accelerometerData);
GPIO__INPUT(INTA, E, 0);
bool intaStatus = false;


// STM32 ######################################################################
using namespace xpcc::stm32;

static bool
initClock()
{
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, 25, 336);
	return Clock::switchToPll();
}


MAIN_FUNCTION // ##############################################################
{
	initClock();
	SysTickTimer::enable();
	
	debugUart.configurePins(DebugUart::REMAP_PC12_PD2);
	stream << "RESTART" << xpcc::endl;
	
	Button::setInput(xpcc::stm32::PULLUP);
	LED::setOutput(xpcc::Gpio::High);
	
#if USE_HARDWARE_I2C
	Twi1::configurePins(Twi1::REMAP_PB8_PB9);
	Twi1::initialize(210);
#else
	Twi1::initialize();
#endif
	
	stream << "init BMA180\n";
	accelerometer.configure(xpcc::bma180::RANGE_2G, xpcc::bma180::BW_20HZ);
	stream << "init HMC5883L\n";
	magnetometer.configure(xpcc::hmc5883l::DATA_OUTPUT_RATE_15HZ);
	stream << "init ITG3200\n";
	gyroscope.configure(255, xpcc::itg3200::FILTER_DLPF_256HZ);
	stream << "init BMP085\n";
	barometer.configure(xpcc::bmp085::MODE_ULTRA_HIGH_RESOLUTION);
	
	xpcc::PeriodicTimer<> timer(1000);
	bool buttonStatus(false);
	
	while (1)
	{
		if (buttonStatus != Button::read())
		{
			buttonStatus = Button::read();
			if (buttonStatus) timer.restart(400);
			else timer.restart(100);
		}
		
		if (timer.isExpired())
		{
			LED::toggle();
		}
		
		if(barometerTimer.isExpired())
			barometer.startReadoutSequence();
		barometer.update();
		
		if (intaStatus != INTA::read())
		{
			intaStatus = INTA::read();
			if (intaStatus)
				accelerometer.readAccelerometer();
		}
		accelerometer.update();

		if (intgStatus != INTG::read())
		{
			intgStatus = INTG::read();
			if (intgStatus)
				gyroscope.readTemperatureGyroscope();
		}
		gyroscope.update();

		if (intmStatus != INTM::read())
		{
			intmStatus = INTM::read();
			if (!intmStatus)
				magnetometer.readMagnetometer();
		}
		magnetometer.update();
		
		
		if(magnetometer.isNewDataAvailable())
		{
			uint8_t* magnet = magnetometer.getData();
			stream << "Compass: ";
			uint16_t tmp = ((magnet[0]<<8)|magnet[1]);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = ((magnet[2]<<8)|magnet[3]);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = ((magnet[4]<<8)|magnet[5]);
			stream << xpcc::hex << tmp << xpcc::ascii << "\n\n";
		}
		
		if(accelerometer.isNewDataAvailable())
		{
			int16_t* accel = reinterpret_cast<int16_t*>(accelerometer.getData());
			stream << "Accel: ";
			stream << *(reinterpret_cast<uint8_t*>(&accel[3])) << " ";
			uint16_t tmp = (accel[0] >> 2);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = (accel[1] >> 2);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = (accel[2] >> 2);
			stream << xpcc::hex << tmp << xpcc::ascii << "\n\n";
		}

		if(gyroscope.isNewTemperatureDataAvailable())
		{
			uint8_t* gyro = gyroscope.getTemperatureData();
			stream << "Gyro: ";
			uint16_t tmp = ((gyro[0]<<8)|gyro[1]);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = ((gyro[2]<<8)|gyro[3]);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = ((gyro[4]<<8)|gyro[5]);
			stream << xpcc::hex << tmp << xpcc::ascii << " ";
			tmp = ((gyro[6]<<8)|gyro[7]);
			stream << xpcc::hex << tmp << xpcc::ascii << "\n\n";
		}

		if(barometer.isNewDataAvailable())
		{
			barometer.getData();
			
			int16_t* temp = barometer.getCalibratedTemperature();
			int32_t* press = barometer.getCalibratedPressure();
			stream << "Baro: ";
			stream << *temp;
			stream << " Temp: ";
			stream << *press << "\n\n";
		}
	}
}
			
