/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "message.hpp"
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>
#include <modm/processing/resumable.hpp>

namespace modm::amnb
{
/// @ingroup modm_communication_amnb
/// @{

enum class
InterfaceStatus : uint8_t
{
	Ok = 0,

	HeaderInvalid,
	DataInvalid,
	MediumBusy,
	MediumEmpty,

	SyncReadFailed,
	HeaderReadFailed,
	DataReadFailed,
	AllocationFailed,

	SyncWriteFailed,
	HeaderWriteFailed,
	DataWriteFailed,
};

class Device
{
public:
	virtual bool
	hasReceived() = 0;

	virtual modm::ResumableResult<bool>
	write(uint8_t data) = 0;

	virtual modm::ResumableResult<bool>
	read(uint8_t *data) = 0;
};

template< class Uart, uint16_t TimeoutUsTx = 1000, uint16_t TimeoutUsRx = 10'000 >
class DeviceWrapper : public Device, modm::Resumable<2>
{
public:
	bool
	hasReceived() final
	{
		return Uart::receiveBufferSize() > 0;
	}

	modm::ResumableResult<bool>
	write(uint8_t data) final
	{
		RF_BEGIN(0);
		RF_WAIT_UNTIL(Uart::write(data));

		timeout.restart(std::chrono::microseconds(TimeoutUsTx));
		RF_WAIT_UNTIL(Uart::read(rx_data) or Uart::hasError() or timeout.isExpired());
		if (timeout.isExpired() or Uart::hasError() or rx_data != data)
		{
			Uart::discardTransmitBuffer();
			Uart::discardReceiveBuffer();
			Uart::clearError();
			RF_RETURN(false);
		}
		RF_END_RETURN(true);
	}

	modm::ResumableResult<bool>
	read(uint8_t *data) final
	{
		RF_BEGIN(1);
		timeout.restart(std::chrono::microseconds(TimeoutUsRx));
		RF_WAIT_UNTIL(Uart::read(*data) or Uart::hasError() or timeout.isExpired());
		if (timeout.isExpired() or Uart::hasError())
		{
			Uart::discardReceiveBuffer();
			Uart::clearError();
			RF_RETURN(false);
		}
		RF_END_RETURN(true);
	}

protected:
	modm::ShortPreciseTimeout timeout;
	uint8_t rx_data;
};

template< size_t MaxHeapAllocation = 0 >
class Interface : modm::Resumable<6>
{
public:
	Interface(Device &device)
	:	device(device) {}

	bool
	isMediumBusy() const
	{ return isReceiving or device.hasReceived(); }

	modm::ResumableResult<InterfaceStatus>
	transmit(const Message *message)
	{
		RF_BEGIN(0);

		if (isMediumBusy())
			RF_RETURN(InterfaceStatus::MediumBusy);
		isTransmitting = true;

		tx_data = STX;
		if (not RF_CALL(write())) RF_RETURN(InterfaceStatus::SyncWriteFailed);
		if (not RF_CALL(write())) RF_RETURN(InterfaceStatus::SyncWriteFailed);

		for (tx_index = 0; tx_index < message->headerLength(); tx_index++)
			if (not RF_CALL(write_escaped(message->self()[tx_index])))
				RF_RETURN(InterfaceStatus::HeaderWriteFailed);

		for (tx_index = 0; tx_index < message->dataLength(); tx_index++)
			if (not RF_CALL(write_escaped(message->get()[tx_index])))
				RF_RETURN(InterfaceStatus::DataWriteFailed);

		isTransmitting = false;
		RF_END_RETURN(InterfaceStatus::Ok);
	}

	modm::ResumableResult<InterfaceStatus>
	receiveHeader(Message *message)
	{
		RF_BEGIN(1);

		if (isTransmitting)
			RF_RETURN(InterfaceStatus::MediumBusy);

		if (not device.hasReceived())
			RF_RETURN(InterfaceStatus::MediumEmpty);

		if (not RF_CALL(read())) RF_RETURN(InterfaceStatus::SyncReadFailed);
		if (rx_data != STX)      RF_RETURN(InterfaceStatus::SyncReadFailed);
		isReceiving = true;
		if (not RF_CALL(read())) {
			isReceiving = false;
			RF_RETURN(InterfaceStatus::SyncReadFailed);
		}
		if (rx_data != STX) {
			isReceiving = false;
			RF_RETURN(InterfaceStatus::SyncReadFailed);
		}

		for(rx_index = 0; rx_index < message->SMALL_HEADER_SIZE; rx_index++)
		{
			if (not RF_CALL(read_escaped())) RF_RETURN(InterfaceStatus::HeaderReadFailed);
			message->self()[rx_index] = rx_data;
		}
		for(; rx_index < message->headerLength(); rx_index++)
		{
			if (not RF_CALL(read_escaped())) RF_RETURN(InterfaceStatus::HeaderReadFailed);
			message->self()[rx_index] = rx_data;
		}

		if (not message->isHeaderValid()) {
			isReceiving = false;
			RF_RETURN(InterfaceStatus::HeaderInvalid);
		}
		if (not message->isLarge()) isReceiving = false;
		RF_END_RETURN(InterfaceStatus::Ok);
	}

	modm::ResumableResult<InterfaceStatus>
	receiveData(Message *message, bool allocate=true)
	{
		RF_BEGIN(1); // same index as receiveHeader!!!

		if (not message->isLarge()) RF_RETURN(InterfaceStatus::Ok);

		if ( (rx_allocated = allocate and (message->dataLength() <= MaxHeapAllocation)) )
			rx_allocated = message->allocate();

		for(rx_index = 0; rx_index < message->dataLength(); rx_index++)
		{
			if (not RF_CALL(read_escaped())) RF_RETURN(InterfaceStatus::DataReadFailed);
			if (rx_allocated) message->get()[rx_index] = rx_data;
		}
		isReceiving = false;
		if (allocate and not rx_allocated) RF_RETURN(InterfaceStatus::AllocationFailed);

		RF_END_RETURN(message->isDataValid() ? InterfaceStatus::Ok : InterfaceStatus::DataInvalid);
	}

protected:
	modm::ResumableResult<bool>
	write_escaped(uint8_t data)
	{
		RF_BEGIN(2);
		if (data == STX or data == DLE) {
			tx_data = DLE;
			if (not RF_CALL(write())) RF_RETURN(false);
			tx_data = data ^ 0x20;
		}
		else {
			tx_data = data;
		}
		RF_END_RETURN_CALL(write());
	}

	modm::ResumableResult<bool>
	read_escaped()
	{
		RF_BEGIN(3);
		if (not RF_CALL(read())) RF_RETURN(false);
		if (rx_data == DLE) {
			if (not RF_CALL(read())) RF_RETURN(false);
			rx_data ^= 0x20;
		}
		RF_END_RETURN(true);
	}

	modm::ResumableResult<bool>
	write()
	{
		RF_BEGIN(4);
		if (RF_CALL(device.write(tx_data)))
			RF_RETURN(true);
		isTransmitting = false;
		RF_END_RETURN(false);
	}

	modm::ResumableResult<bool>
	read()
	{
		RF_BEGIN(5);
		if (RF_CALL(device.read(&rx_data)))
			RF_RETURN(true);
		isReceiving = false;
		RF_END_RETURN(false);
	}

protected:
	Device &device;
	uint16_t tx_index;
	uint16_t rx_index;
	uint8_t tx_data;
	uint8_t rx_data;
	bool rx_allocated;
	bool isReceiving{false};
	bool isTransmitting{false};
	static constexpr uint8_t STX{0x7E};
	static constexpr uint8_t DLE{0x7D};
};
/// @}

}	// namespace modm::amnb

#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_communication_amnb
inline modm::IOStream&
operator << (modm::IOStream& s, const modm::amnb::InterfaceStatus status)
{
	using namespace modm::amnb;
	switch(status)
	{
		case InterfaceStatus::Ok:					s << "Ok";					break;
		case InterfaceStatus::HeaderInvalid:		s << "HeaderInvalid";		break;
		case InterfaceStatus::DataInvalid:			s << "DataInvalid";			break;
		case InterfaceStatus::MediumBusy:			s << "MediumBusy";			break;
		case InterfaceStatus::MediumEmpty:			s << "MediumEmpty";			break;
		case InterfaceStatus::SyncReadFailed:		s << "SyncReadFailed";		break;
		case InterfaceStatus::HeaderReadFailed:		s << "HeaderReadFailed";	break;
		case InterfaceStatus::DataReadFailed:		s << "DataReadFailed";		break;
		case InterfaceStatus::AllocationFailed:		s << "AllocationFailed";	break;
		case InterfaceStatus::SyncWriteFailed:		s << "SyncWriteFailed";		break;
		case InterfaceStatus::HeaderWriteFailed:	s << "HeaderWriteFailed";	break;
		case InterfaceStatus::DataWriteFailed:		s << "DataWriteFailed";		break;
	}
	return s;
}

} // modm namespace
#endif
