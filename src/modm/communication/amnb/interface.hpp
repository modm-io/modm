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

	virtual bool
	write(uint8_t data) = 0;

	virtual bool
	read(uint8_t *data) = 0;
};

template< class Uart, uint16_t TimeoutUsTx = 1000, uint16_t TimeoutUsRx = 10'000 >
class DeviceWrapper : public Device
{
public:
	bool
	hasReceived() final
	{
		return Uart::receiveBufferSize() > 0;
	}

	bool
	write(uint8_t data) final
	{
		modm::this_fiber::poll([&]{ return Uart::write(data); });

		timeout.restart(std::chrono::microseconds(TimeoutUsTx));
		modm::this_fiber::poll([&]{ return Uart::read(rx_data) or Uart::hasError() or timeout.isExpired(); });
		if (timeout.isExpired() or Uart::hasError() or rx_data != data)
		{
			Uart::discardTransmitBuffer();
			Uart::discardReceiveBuffer();
			Uart::clearError();
			return false;
		}
		return true;
	}

	bool
	read(uint8_t *data) final
	{
		timeout.restart(std::chrono::microseconds(TimeoutUsRx));
		modm::this_fiber::poll([&]{ return Uart::read(*data) or Uart::hasError() or timeout.isExpired(); });
		if (timeout.isExpired() or Uart::hasError())
		{
			Uart::discardReceiveBuffer();
			Uart::clearError();
			return false;
		}
		return true;
	}

protected:
	modm::ShortPreciseTimeout timeout;
	uint8_t rx_data;
};

template< size_t MaxHeapAllocation = 0 >
class Interface
{
public:
	Interface(Device &device)
	:	device(device) {}

	bool
	isMediumBusy() const
	{ return isReceiving or device.hasReceived(); }

	InterfaceStatus
	transmit(const Message *message)
	{
		if (isMediumBusy())
			return InterfaceStatus::MediumBusy;
		isTransmitting = true;

		tx_data = STX;
		if (not write()) return InterfaceStatus::SyncWriteFailed;
		if (not write()) return InterfaceStatus::SyncWriteFailed;

		for (tx_index = 0; tx_index < message->headerLength(); tx_index++)
			if (not write_escaped(message->self()[tx_index]))
				return InterfaceStatus::HeaderWriteFailed;

		for (tx_index = 0; tx_index < message->dataLength(); tx_index++)
			if (not write_escaped(message->get()[tx_index]))
				return InterfaceStatus::DataWriteFailed;

		isTransmitting = false;
		return InterfaceStatus::Ok;
	}

	InterfaceStatus
	receiveHeader(Message *message)
	{
		if (isTransmitting)
			return InterfaceStatus::MediumBusy;

		if (not device.hasReceived())
			return InterfaceStatus::MediumEmpty;

		if (not read()) return InterfaceStatus::SyncReadFailed;
		if (rx_data != STX) return InterfaceStatus::SyncReadFailed;
		isReceiving = true;
		if (not read()) {
			isReceiving = false;
			return InterfaceStatus::SyncReadFailed;
		}
		if (rx_data != STX) {
			isReceiving = false;
			return InterfaceStatus::SyncReadFailed;
		}

		for(rx_index = 0; rx_index < message->SMALL_HEADER_SIZE; rx_index++)
		{
			if (not read_escaped()) return InterfaceStatus::HeaderReadFailed;
			message->self()[rx_index] = rx_data;
		}
		for(; rx_index < message->headerLength(); rx_index++)
		{
			if (not read_escaped()) return InterfaceStatus::HeaderReadFailed;
			message->self()[rx_index] = rx_data;
		}

		if (not message->isHeaderValid()) {
			isReceiving = false;
			return InterfaceStatus::HeaderInvalid;
		}
		if (not message->isLarge()) isReceiving = false;
		return InterfaceStatus::Ok;
	}

	InterfaceStatus
	receiveData(Message *message, bool allocate=true)
	{
		if (not message->isLarge()) return InterfaceStatus::Ok;

		if ( (rx_allocated = allocate and (message->dataLength() <= MaxHeapAllocation)) )
			rx_allocated = message->allocate();

		for(rx_index = 0; rx_index < message->dataLength(); rx_index++)
		{
			if (not read_escaped()) return InterfaceStatus::DataReadFailed;
			if (rx_allocated) message->get()[rx_index] = rx_data;
		}
		isReceiving = false;
		if (allocate and not rx_allocated) return InterfaceStatus::AllocationFailed;

		return message->isDataValid() ? InterfaceStatus::Ok : InterfaceStatus::DataInvalid;
	}

protected:
	bool
	write_escaped(uint8_t data)
	{
		if (data == STX or data == DLE) {
			tx_data = DLE;
			if (not write()) return false;
			tx_data = data ^ 0x20;
		}
		else {
			tx_data = data;
		}
		return write();
	}

	bool
	read_escaped()
	{
		if (not read()) return false;
		if (rx_data == DLE) {
			if (not read()) return false;
			rx_data ^= 0x20;
		}
		return true;
	}

	bool
	write()
	{
		if (device.write(tx_data))
			return true;
		isTransmitting = false;
		return false;
	}

	bool
	read()
	{
		if (device.read(&rx_data))
			return true;
		isReceiving = false;
		return false;
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
