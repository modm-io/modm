// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_CORTEX_CYCLE_COUNTER_HPP
#define XPCC_CORTEX_CYCLE_COUNTER_HPP

#include "../../../device.hpp"

namespace xpcc
{
namespace cortex
{

/**
 * @brief		CPU Cycle Counter
 * @ingroup		cortex
 *
 * Is enabled in the cortex start-up code
 */
class CycleCounter
{
public:
	static inline uint32_t getCount() {
		return DWT->CYCCNT;
	}
};

} // namespace cortex
} // namespace xpcc
#endif // XPCC_CORTEX_CYCLE_COUNTER_HPP
