// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>
#include <xpcc/io/iostream.hpp>

class IoStreamTest : public unittest::TestSuite
{
public:
	virtual void
	setUp();

	virtual void
	tearDown();


	void
	testString();

	void
	testFlashString();

	// 8-bit
	void
	testStreamUint8();

	void
	testStreamInt8();

	// 16-bit
	void
	testStreamUint16();

	void
	testStreamUint16_2();

	void
	testStreamInt16();

	void
	testStreamInt16_2();

	// 32-bit
	void
	testStreamUint32();

	void
	testStreamUint32_2();

	void
	testStreamInt32();

	void
	testStreamInt32_2();

	void
	testStreamInt32_3();

	// 64-bit
	void
	testStreamUint64();

	void
	testStreamInt64();

	// float
	void
	testFloat();

	void
	testFloat2();

	void
	testFloat3();

	void
	testFloat4();

	// bool
	void
	testBool1();

	void
	testBool2();

	// hex
	void
	testHex1();

	void
	testHex2();

	void
	testHex3();

	void
	testHex4();

	void
	testHex5();

	// bin
	void
	testBin1();

	void
	testBin2();

	void
	testBin3();

	void
	testBin4();

	void
	testBin5();

	void
	testPrintf1();

	void
	testPrintf2();

	void
	testFp();

private:
	xpcc::IOStream *stream;
};
