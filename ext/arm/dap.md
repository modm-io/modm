# ARM CMSIS-DAP

This module provides the CMSIS-DAP library to implement the SWD and JTAG debug
protocols and present a CMSIS-DAP interface to OpenOCD or PyOCD via TinyUSB.
Please [see the API documentation][docs] for protocol-level details.

Note that only the SWD and JTAG protocols are supported, as the UART and SWO
implementation of CMSIS-DAP depend on the CMSIS-OS2 interface, which modm does
not provide. These interface need to be reimplemented in native modm in the
future.

The aim of this project is to make the integration simple at the cost of speed.
If you need a highly performant debug probe, you should look into using DMA
accelerated SPI to generate SWD and JTAG waveforms instead of bit banging them.

Since CMSIS-DAP is configured using CPP macros, especially the GPIO functions
cannot be implemented using the native C++ API. Therefore, all macros are
collapsed into weakly linked C callback functions.
The [original configuration API is documented here][config].

## Configuration Hooks

All callback functions are defaulted except SWCLK/TCK and SWDIO/TMS:

```cpp
// Required pins
uint32_t DAP_PinSwclkTck(DAP_Command_t cmd);
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd);
// Optional pins
uint32_t DAP_PinTdi(DAP_Command_t cmd) { return 0u; }
uint32_t DAP_PinTdo(DAP_Command_t cmd) { return 0u; }
uint32_t DAP_PinnTrst(DAP_Command_t cmd) { return 1u; }
uint32_t DAP_PinnReset(DAP_Command_t cmd) { return 1u; }
// Optional special reset handling
uint8_t DAP_Reset(void) { return 0u; }

// Optional indicators
void DAP_LedConnectedOut(DAP_Command_t cmd) {}
void DAP_LedRunningOut(DAP_Command_t cmd) {}

// Original hooks from CMSIS-DAP
uint8_t DAP_GetVendorString (char *) { return 0u; }
uint8_t DAP_GetProductString (char *) { return 0u; }
uint8_t DAP_GetSerNumString (char *) { return 0u; }
uint8_t DAP_GetTargetDeviceVendorString (char *) { return 0u; }
uint8_t DAP_GetTargetDeviceNameString (char *) { return 0u; }
uint8_t DAP_GetTargetBoardVendorString (char *) { return 0u; }
uint8_t DAP_GetTargetBoardNameString (char *) { return 0u; }
uint8_t DAP_GetProductFirmwareVersionString (char *) { return 0u; }
// Provided by modm using 1µs clock
uint32_t DAP_GetTimestamp(void);
// Implemented by modm
void DAP_Port(DAP_Command_t cmd);
```

To simplify the GPIO implementation in modm, two adapters are provided.
For single port operations, where the GPIOs stay the same, a template function
implements the command dispatch very efficiently:

```cpp
#include <cmsis/dap.hpp>

using Swclk = Board::A0;
using Swdio = Board::A1;

uint32_t DAP_PinSwclkTck(DAP_Command_t cmd)
{
	return modm::dap::cmd<Swclk>(cmd);
}
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd)
{
	return modm::dap::cmd<Swdio>(cmd);
}
```

For projects where multiple SWD ports are required, a dynamic command dispatch
helper struct is available:

```cpp
const modm::dap::Pin swclk[]{
	modm::dap::Pin::from<Board::A0>(),
	modm::dap::Pin::from<Board::A2>(),
};
const modm::dap::Pin swdio[]{
	modm::dap::Pin::from<Board::A1>(),
	modm::dap::Pin::from<Board::A3>(),
};

uint32_t DAP_PinSwclkTck(DAP_Command_t cmd)
{
	return swclk[swd_port & 0b1](cmd);
}
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd)
{
	return swdio[swd_port & 0b1](cmd);
}
```

To switch ports, use a custom CMSIS-DAP vendor command:

```cpp
static uint8_t swd_port{0};
uint32_t DAP_ProcessVendorCommand(const uint8_t *request, uint8_t *response)
{
	if (request[0] == ID_DAP_Vendor(0))
	{
		DAP_PinSwclkTck(DAP_PIN_DIR_IN);
		DAP_PinSwdioTms(DAP_PIN_DIR_IN);
		swd_port = request[1];

		response[0] = ID_DAP_Vendor(0);
		response[1] = 0;
		return (2U << 16) | 2U;
	}

	// Default fallback for unknown vendor commands
	*response = ID_DAP_Invalid;
	return ((1U << 16) | 1U);
}
```

You can then switch ports from within OpenOCD scripts and the CLI without
requiring a second USB interface like a serial port to do so:

```sh
// Port 0
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -c init -c "cmsis-dap cmd 128 0" -c shutdown
// Port1
openocd -f interface/cmsis-dap.cfg -c "transport select swd" -c init -c "cmsis-dap cmd 128 1" -c shutdown
```

## TinyUSB Configuration

TinyUSB implements the vendor class for CMSIS-DAP natively and is fully
integrated into modm. The lbuild configuration is as simple as this:

```xml
<library>
  <options>
    <option name="modm:tinyusb:config">device.vendor</option>
  </options>
  <modules>
    <module>modm:tinyusb</module>
    <module>modm:cmsis:dap</module>
  </modules>
</library>
```

Note that the USB endpoint corresponding to the vendor class must include the
string `CMSIS-DAP` in its description, the easiest way is to overwrite the
TinyUSB descriptor array:

```cpp
extern "C"
{
const char*
tud_string_desc_arr[] =
{
	NULL,			// 0: Language
	"modm",			// 1: Manufacturer
	"Multi-DAP",	// 2: Product
	NULL,			// 3: Serials, should use chip ID
	"CMSIS-DAP v2",	// 4: Interface 0 must contain "CMSIS-DAP"
};
}
```

!!! note "Using multiple endpoints"
	When using multiple endpoints (for example with a CDC port), you must make
	sure the description string order is the same as in the lbuild option.

The data interface between TinyUSB and CMSIS-DAP is implemented using just two
buffers:

```cpp
static uint8_t request_buf[CFG_TUD_VENDOR_RX_BUFSIZE];
static uint8_t response_buf[CFG_TUD_VENDOR_TX_BUFSIZE];

int main()
{
	Board::initialize();
	DAP_Setup();

	Board::initializeUsb();
	tusb_init();

	while (true)
	{
		tud_task();

		if (tud_vendor_available()) {
			if (tud_vendor_read(request_buf, sizeof(request_buf)) > 0) {
				const uint32_t response_size = DAP_ExecuteCommand(request_buf, response_buf);
				tud_vendor_write(response_buf, (uint16_t)response_size);
				tud_vendor_write_flush();
			}
		}
		// modm::this_fiber::yield(); when running in a fiber!
	}

	return 0;
}
```

See `examples/generic/cmsis-dap/main.cpp` for a complete working reference.

!!! warning "DAP Packet Count"
	`DAP_PACKET_COUNT` is configured to be 1. While this is the least performant
	setting, higher counts have all had unsolvable reliability issues. So be
	prepared to debug when increasing this count on USB HS systems for example.

[docs]: https://arm-software.github.io/CMSIS-DAP/latest/
[config]: https://arm-software.github.io/CMSIS-DAP/latest/group__DAP__ConfigIO__gr.html
