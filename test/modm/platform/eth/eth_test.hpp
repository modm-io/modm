/*
 * Copyright (c) 2026, Luca Feggi
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class EthernetTest : public unittest::TestSuite
{
public:
	void
	testDescriptorEncoding();
	void
	testTransmitOwnershipAndWraparound();
	void
	testReceiveLeaseAndRecycle();
	void
	testReceiveErrorsAndErratum();
	void
	testFrameLengthAndExhaustion();
	void
	testStoppedTransmitAndFrameLimit();
	void
	testCacheOrdering();
	void
	testRingRecovery();
	void
	testPhyIdentificationAndAddress();
	void
	testPhyResetAndNegotiation();
	void
	testPhyTimeoutAndMdioError();
};
