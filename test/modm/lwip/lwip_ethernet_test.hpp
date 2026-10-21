/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class LwipEthernetTest : public unittest::TestSuite
{
public:
	void testInitializationRequired();
	void testInitializationAndRouting();
	void testPhysicalMacBinding();
	void testFailedBindingCanBeReplaced();
	void testReinitializationAndRecovery();
	void testConfigurationAndLeaseContract();
	void testTransmitPath();
	void testReceivePath();
	void testReceiveStatisticsAndChecksums();
	void testRealUdpChecksumAndReassembly();
	void testOutgoingSoftwareFragmentation();
	void testLinkPollingAndTimers();
	void testInitializationFailure();
	void testSingleThreadGuard();
};
