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

	//Set the RF switch to Receive
	RxTx::setOutput();
	RxTx::reset();

	//Initialize the SPI
	SpiCs::setOutput();
	SpiCs::set();

	SpiMaster1::connect<SpiSck::Sck, SpiMosi::Mosi, SpiMiso::Miso>();
	SpiMaster1::initialize<Board::SystemClock, 1500_kHz>();

	MODM_LOG_DEBUG << "Starting the modem" << modm::endl;

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

	// Enable the continous listening mode
	RF_CALL_BLOCKING(loraModem.enableListening());

	//the variable to hold the counter transmitted by the tx example
	uint32_t rxCounter(0);

	while (true)
	{
		//check the modem if new data is available and read it
		uint8_t bytesReceived = RF_CALL_BLOCKING(loraModem.readPacket(reinterpret_cast<uint8_t*>(&rxCounter),4));
		if(bytesReceived > 0)
		{
			int8_t snr = RF_CALL_BLOCKING(loraModem.getPacketSnr());

			//dirty fixed point numbers
			uint8_t snr_dec = (snr & 0x03) * 25;
			snr = snr >> 2;

			int16_t rssi = RF_CALL_BLOCKING(loraModem.getPacketRssi());

			MODM_LOG_DEBUG << "Received Message" << modm::endl;
			MODM_LOG_DEBUG << "Counter Value: " << rxCounter << modm::endl;
			MODM_LOG_DEBUG << "SNR: "<< snr <<"."<< snr_dec << " dB" << modm::endl;
			MODM_LOG_DEBUG << "RSSI: "<< rssi << " dBm" << modm::endl;
			MODM_LOG_DEBUG << modm::endl;
		}
		//check for new data every 100ms
		modm::delay(100ms);
	}

	return 0;
}
