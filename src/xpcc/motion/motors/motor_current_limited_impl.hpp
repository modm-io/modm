// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MOTOR_CURRENT_LIMITED_HPP
#	error	"Don't include this file directly, use 'motor_current_limited.hpp' instead!"
#endif

template<typename Motor, typename ToPwm>
uint16_t MotorCurrentLimited<Motor, ToPwm>::currentLimit = 0;

template<typename Motor, typename ToPwm>
int16_t MotorCurrentLimited<Motor, ToPwm>::lastPwm = 0;
