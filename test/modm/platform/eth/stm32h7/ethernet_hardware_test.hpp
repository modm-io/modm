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

class EthernetHardwareTest : public unittest::TestSuite
{
public:
	void setUp() override;
	void tearDown() override;

	void testInitializationAndPhyIdentity();
	void testInvalidPhyAddressRecovery();
	void testPhyResetRecovery();
	void testFullDuplexPhyModes();
	void testHalfDuplexPhyModeConfiguration();
	void testLinkTransitionWithOutstandingReceiveLease();
	void testCopyAndLeaseApis();
	void testTransmitLeaseStateTransitions();
	void testFrameSizeAndCacheBoundaries();
	void testDescriptorRingWraparound();
	void testTransmitDescriptorExhaustionRecovery();
	void testReceiveOwnershipPressureRecovery();
	void testProtocolAndChecksumModes();
	void testFragmentedUdpChecksumModes();
	void testMalformedChecksumRecovery();
	void testReinitializationRecovery();
	void testHardwareErrorStatusReset();
	void testDmaStoragePlacement();

private:
	bool ready{false};
};
