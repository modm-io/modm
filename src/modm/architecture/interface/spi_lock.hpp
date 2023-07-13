/*
* Copyright (c) 2014-2017, Niklas Hauser
* Copyright (c) 2023, Christopher Durand
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_SPI_LOCK_HPP
#define MODM_INTERFACE_SPI_LOCK_HPP

#include <cstdint>
#include <modm/architecture/interface/spi.hpp>

namespace modm
{

/**
 * Helper class to implement the synchronization mechanism of @ref modm::SpiMaster
 * in SPI device drivers.
 *
 * Inherit from this class publicly and pass the derived class as the template
 * argument, as in `class SpiMaster1 : public modm::SpiLock<SpiMaster1>`.
 *
 * @tparam	Derived SPI master derived class
 * @ingroup	modm_architecture_spi
 */
template<typename Derived>
class SpiLock
{
public:
	static uint8_t
	acquire(void* ctx, Spi::ConfigurationHandler handler);

	static uint8_t
	release(void* ctx);

private:
	static inline uint8_t count{0};
	static inline void* context{nullptr};
	static inline Spi::ConfigurationHandler configuration{nullptr};
};

template<typename Derived>
uint8_t
SpiLock<Derived>::acquire(void* ctx, Spi::ConfigurationHandler handler)
{
	if (context == nullptr)
	{
		context = ctx;
		count = 1;
		// if handler is not nullptr and is different from previous configuration
		if (handler and configuration != handler) {
			configuration = handler;
			configuration();
		}
		return 1;
	}

	if (ctx == context)
		return ++count;

	return 0;
}

template<typename Derived>
uint8_t
SpiLock<Derived>::release(void* ctx)
{
	if (ctx == context)
	{
		if (--count == 0)
			context = nullptr;
	}
	return count;
}

} // namespace modm

#endif // MODM_INTERFACE_SPI_LOCK_HPP
