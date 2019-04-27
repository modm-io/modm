/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2017, Marten
 * Copyright (c) 2019, Markus Schmidt
 * Copyright (c) 2019, Sebastan Birke
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/matrix.hpp>
#include <modm/math/geometry/vector.hpp>

#include "matrix_vector_test.hpp"

void
MatrixVectorTest::testMatrixVectorMultiplication()
{
	const float m[] = {
		5.,
		8.,
		-4.,
		6.,
		9.,
		-5.,
		4.,
		7.,
		-2.
	};

	const float v[] = {
		2.,
		-3.,
		1.
	};

	const modm::Matrix<float, 3, 3> a(m);
	const modm::Vector3f b(v);

	modm::Vector3f c = a * b;

	TEST_ASSERT_EQUALS(c[0], -18);
	TEST_ASSERT_EQUALS(c[1], -20);
	TEST_ASSERT_EQUALS(c[2], -15);

}
