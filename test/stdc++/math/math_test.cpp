/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "math_test.hpp"
#include <cmath>

// non-const value to enforce use of avr-libm runtime implementations
float one = 1.0f;

void
MathTest::testMathFunctions()
{
    // cos
    TEST_ASSERT_EQUALS_FLOAT(std::cos(one * M_PI), -1);
    // sin
    TEST_ASSERT_EQUALS_FLOAT(std::sin(one * M_PI/2), 1);
    // tan
    TEST_ASSERT_EQUALS_FLOAT(std::tan(one * M_PI/4), 1);

    // fabs
    TEST_ASSERT_EQUALS_FLOAT(std::fabs(one / 2), 0.5);

    // fmod
    TEST_ASSERT_EQUALS_FLOAT(std::fmod(one * 5, 2), 1);

    // modf
    float integralPart = 0;
    float fractionalPart = std::modf(one * 1.5, &integralPart);
    TEST_ASSERT_EQUALS_FLOAT(integralPart, 1);
    TEST_ASSERT_EQUALS_FLOAT(fractionalPart, 0.5);

    // cbrt
    TEST_ASSERT_EQUALS_FLOAT(std::cbrt(one * 27), 3);

    // hypot, sqrt
    TEST_ASSERT_EQUALS_FLOAT(std::hypot(one * 2, one * 3), std::sqrt(one * 13));

    // floor
    TEST_ASSERT_EQUALS_FLOAT(std::floor(one * 3.2), 3.0);
    // ceil
    TEST_ASSERT_EQUALS_FLOAT(std::ceil(one * 3.2), 4.0);

    // frexp
    // 96 = 0.75 * 2^7
    int power = 0;
    TEST_ASSERT_EQUALS_FLOAT(std::frexp(one * 96, &power), 0.75);
    TEST_ASSERT_EQUALS(power, 7);
    // ldexp
    TEST_ASSERT_EQUALS_FLOAT(std::ldexp(one * 3, 5), 96);
    // exp
    TEST_ASSERT_EQUALS_FLOAT(std::exp(one), M_E);

    constexpr float sinh1 = (std::exp(1.0)-std::exp(-1.0))/2;
    constexpr float cosh1 = (std::exp(1.0)+std::exp(-1.0))/2;
    // cosh
    TEST_ASSERT_EQUALS_FLOAT(std::cosh(one), cosh1);
    // sinh
    TEST_ASSERT_EQUALS_FLOAT(std::sinh(one), sinh1);
    // tanh
    TEST_ASSERT_EQUALS_FLOAT(std::tanh(one), sinh1 / cosh1);

    // acos
    TEST_ASSERT_EQUALS_FLOAT(std::acos(-one), M_PI);
    // asin
    TEST_ASSERT_EQUALS_FLOAT(std::asin(one), M_PI/2);
    // atan
    TEST_ASSERT_EQUALS_FLOAT(std::atan(one), M_PI/4);
    // atan2
    TEST_ASSERT_EQUALS_FLOAT(std::atan2(one, 1), M_PI/4);

    // log
    TEST_ASSERT_EQUALS_FLOAT(std::log(std::exp(one * 3)), 3.0);
    // log10
    TEST_ASSERT_EQUALS_FLOAT(std::log10(100 * one), 2);

    // powf
    TEST_ASSERT_EQUALS_FLOAT(std::pow(2 * one, 3), 8);

    // isnan
    TEST_ASSERT_TRUE(std::isnan(NAN*one));
    // isinf
    TEST_ASSERT_FALSE(std::isinf(one));
    // isfinite
    TEST_ASSERT_TRUE(std::isfinite(one));

    // copysign
    TEST_ASSERT_TRUE(std::copysign(1.0f,-2*one) < 0);
    // signbit
    TEST_ASSERT_TRUE(std::signbit(-2*one));

    // fdim
    TEST_ASSERT_EQUALS_FLOAT(std::fdim(one * 5, one * 2), 3);

    // fma
    TEST_ASSERT_EQUALS_FLOAT(std::fma(one * 2, one * 2, one), 5);

    // fmax
    TEST_ASSERT_EQUALS_FLOAT(std::fmax(one * 5, one * 2), 5);
    // fmin
    TEST_ASSERT_EQUALS_FLOAT(std::fmin(one * 5, one * 2), 2);

    // trunc
    TEST_ASSERT_EQUALS_FLOAT(std::trunc(one * 2.7), 2);
    // round
    TEST_ASSERT_EQUALS_FLOAT(std::round(one * 2.7), 3);
    // lround
    TEST_ASSERT_EQUALS(std::lround(one * 2.7), 3);
    // lrint
    TEST_ASSERT_EQUALS(std::lrint(one * 1.8), 2);
}
