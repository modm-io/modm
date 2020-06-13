/*
 * Copyright (c) 2020 Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/radio/sx1276.hpp>

using namespace Board;
using namespace modm;

int
main()
{
	Board::initialize();

	//RxTx selection on the mbed shield
	using RxTx = Board::A4;

	using SpiMosi = Board::D11;
	using SpiMiso = Board::D12;
	using SpiSck = Board::D13;
	using SpiCs = Board::D10;

	//Set the RF switch to Transmit
	RxTx::setOutput();
	RxTx::set();

	//Initialize the SPI
	SpiCs::setOutput();
	SpiCs::set();

	SpiMaster1::connect<SpiSck::Sck, SpiMosi::Mosi, SpiMiso::Miso>();
	SpiMaster1::initialize<Board::SystemClock, 1500_kHz>();

	// Create an instance of the driver
	Sx1276<SpiMaster1,SpiCs> loraModem;

	// Initialize the modem
	RF_CALL_BLOCKING(loraModem.initialize());

	// Set Modulation Parameters
	RF_CALL_BLOCKING(loraModem.setModemParams(sx1276::Bandwidth::BW_7,
											  sx1276::SpreadingFactor::SF_8,
											  sx1276::CodingRate::CR_4_8,
											  false,
											  false));
	// Set Carrier Frequency
	RF_CALL_BLOCKING(loraModem.setCarrierFrequency(433.920_MHz));

	//The message to be sent
	uint32_t counter(0);

	while (true)
	{
		MODM_LOG_INFO << "Trasmitting Message " << counter << modm::endl;
		RF_CALL_BLOCKING(loraModem.transmit(reinterpret_cast<uint8_t*>(&counter),4));
		counter++;
		modm::delay(1000ms);
	}

	return 0;
}
