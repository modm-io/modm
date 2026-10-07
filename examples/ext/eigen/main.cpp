/*
 * Copyright (c) 2025, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <Eigen/Eigen>

using namespace Board;

Eigen::IOFormat squarebrackets(5, 0, ", ", "\n", "[", "]");

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	while (1)
	{
		Eigen::Matrix2f mat{{1.111f, 2.2222f}, {3.333f, 4.4f}};
		Eigen::Vector2f u{-1.1f, 1.5f}, v{2.2f, 0.5f};
		MODM_LOG_INFO.width(2);
		MODM_LOG_INFO.precision(0);
		MODM_LOG_INFO << "Here is mat*mat:\n" << mat * mat << modm::endl;
		MODM_LOG_INFO.width(5);
		MODM_LOG_INFO.precision(1);
		MODM_LOG_INFO << "Here is mat*u:\n" << mat * u << modm::endl;
		MODM_LOG_INFO.width(8);
		MODM_LOG_INFO.precision(3);
		MODM_LOG_INFO << "Here is u^T*mat:\n" << u.transpose() * mat << modm::endl;
		MODM_LOG_INFO.width(10);
		MODM_LOG_INFO.precision(5);
		MODM_LOG_INFO << "Here is u^T*v:\n" << u.transpose() * v << modm::endl;
		MODM_LOG_INFO.width(15);
		MODM_LOG_INFO.precision(7);
		MODM_LOG_INFO << "Here is u*v^T:\n" << u * v.transpose() << modm::endl;
		MODM_LOG_INFO << "Let's multiply mat by itself" << modm::endl;
		mat = mat * mat;
		MODM_LOG_INFO << "Now mat is mat:\n" << mat.format(squarebrackets) << modm::endl;

		modm::delay(1s);
	}

	return 0;
}
