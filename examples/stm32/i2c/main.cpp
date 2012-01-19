
#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/i2c/software_i2c.hpp>
#include <xpcc/driver/connectivity/i2c/sync_master.hpp>
#include <xpcc/architecture/platform/cortex_m3/stm32/i2c/i2c_2.hpp>

using namespace xpcc::stm32;

GPIO__IO(MY_SCL, B, 10);
GPIO__IO(MY_SDA, B, 11);

// choose between sotware and hardware I2C
typedef xpcc::stm32::I2c2 CortexI2c;
//typedef xpcc::SoftwareI2C<MY_SCL, MY_SDA, 114084> CortexI2c;

// this sample does not use any i2c device drivers but the i2c bus directly
typedef xpcc::i2c::SyncMaster< CortexI2c > SyncCortexI2c;

// some defines for accelerometer LIS3LV02DL

// address modifier
static const uint8_t AUTO_INCREMENT = 0x80;
// registers
static const uint8_t WHO_AM_I = 0x0f;
static const uint8_t CTRL_REG1 = 0x20;
static const uint8_t OUTXYZ = 0x28;
static const uint8_t OUTX 	= 0x28;
static const uint8_t OUTX_L = 0x28;
static const uint8_t OUTX_H = 0x29;


void
i2cStateMachine()
{
	static bool init = true;

	if (init)
	{
		// initialize accelerometer
		if (SyncCortexI2c::startCheck(0x3a))
		{
			uint8_t registerAddress = CTRL_REG1;
			uint8_t buffer[2] = {0x87};
			if (SyncCortexI2c::write(&registerAddress, 1, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET){
				SyncCortexI2c::stop();
			}
			else if (SyncCortexI2c::write(buffer, 1) != xpcc::i2c::BUS_RESET){
				// initializing complete
				init = true;
			}
		}
	}
	else {
		if (SyncCortexI2c::startCheck(0x3a))
		{
			uint8_t registerAddress = OUTX_L;
			uint8_t buffer[2];

			if (SyncCortexI2c::write(&registerAddress, 1, xpcc::i2c::SYNC_NO_STOP) == xpcc::i2c::BUS_RESET){
				SyncCortexI2c::stop();
			}
			else if (SyncCortexI2c::restart(0x3a) == xpcc::i2c::BUS_RESET){
				SyncCortexI2c::stop();
			}
			else if (SyncCortexI2c::read(buffer, 1) != xpcc::i2c::BUS_RESET){
				// reading succeed, buffer contains the read value
			}
		}
	}

	xpcc::delay_ms(30);
}

MAIN_FUNCTION
{
	// TODO Prescaler setting
	CortexI2c::initialize(10000);

	while (1)
	{
		i2cStateMachine();
	}
}

