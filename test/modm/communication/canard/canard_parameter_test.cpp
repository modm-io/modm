/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "canard_parameter_test.hpp"

#include <canard/parameter.hpp>
#include <limits>
#include <map>

using namespace modm::canard;

namespace
{

class MapStorage : public ParameterStorage
{
public:
	bool
	read(uint32_t key, uint64_t& data) override
	{
		const auto it = values.find(key);
		if (it == values.end()) return false;
		data = it->second;
		return true;
	}

	bool
	write(uint32_t key, uint64_t data) override
	{
		values[key] = data;
		return true;
	}

	bool
	remove(uint32_t key) override
	{
		values.erase(key);
		return true;
	}

	std::map<uint32_t, uint64_t> values;
};

}	// namespace

void
CanardParameterTest::testInteger()
{
	Parameter parameter{"integer", 5, -10, 10};
	TEST_ASSERT_TRUE(parameter.isDefault());
	TEST_ASSERT_TRUE(parameter.set(-10));
	TEST_ASSERT_EQUALS(parameter.integer(), -10);
	TEST_ASSERT_FALSE(parameter.isDefault());
	TEST_ASSERT_FALSE(parameter.set(11));
	TEST_ASSERT_FALSE(parameter.set(std::numeric_limits<uint64_t>::max()));
	TEST_ASSERT_FALSE(parameter.set(true));
	TEST_ASSERT_FALSE(parameter.set(1.f));
	TEST_ASSERT_EQUALS(parameter.integer(), -10);
	parameter.reset();
	TEST_ASSERT_EQUALS(parameter.integer(), 5);
}

void
CanardParameterTest::testReal()
{
	Parameter parameter{"real", 0.5f, -1.f, 1.f};
	TEST_ASSERT_TRUE(parameter.set(0.25f));
	TEST_ASSERT_EQUALS_FLOAT(parameter.real(), 0.25f);
	TEST_ASSERT_TRUE(parameter.set(1));
	TEST_ASSERT_EQUALS_FLOAT(parameter.real(), 1.f);
	TEST_ASSERT_FALSE(parameter.set(2));
	TEST_ASSERT_FALSE(parameter.set(std::numeric_limits<float>::quiet_NaN()));
	TEST_ASSERT_FALSE(parameter.set(1e300));
	TEST_ASSERT_FALSE(parameter.set(true));
	TEST_ASSERT_EQUALS_FLOAT(parameter.real(), 1.f);
}

void
CanardParameterTest::testBoolean()
{
	Parameter parameter{"boolean", true};
	TEST_ASSERT_TRUE(parameter.set(false));
	TEST_ASSERT_FALSE(parameter.boolean());
	TEST_ASSERT_FALSE(parameter.set(1));
	TEST_ASSERT_FALSE(parameter.load(2));
	TEST_ASSERT_TRUE(parameter.load(1));
	TEST_ASSERT_TRUE(parameter.boolean());
}

void
CanardParameterTest::testKey()
{
	const Parameter integer{"name", 0, 0, 1};
	const Parameter real{"name", 0.f, 0.f, 1.f};
	const Parameter other{"other", 0, 0, 1};
	const Parameter same{"name", 1, 0, 2};
	TEST_ASSERT_EQUALS(integer.key(), same.key());
	TEST_ASSERT_TRUE(integer.key() != real.key());
	TEST_ASSERT_TRUE(integer.key() != other.key());
}

void
CanardParameterTest::testStorage()
{
	Parameter integer{"integer", 5, -10, 10};
	Parameter real{"real", 0.5f, -1.f, 1.f};
	Parameter boolean{"boolean", true};
	Parameter* const parameters[]{&integer, &real, &boolean};
	MapStorage storage;

	// defaults are not stored
	TEST_ASSERT_TRUE(saveParameters(parameters, storage));
	TEST_ASSERT_EQUALS(storage.values.size(), 0u);

	TEST_ASSERT_TRUE(integer.set(-7));
	TEST_ASSERT_TRUE(real.set(-0.75f));
	TEST_ASSERT_TRUE(boolean.set(false));
	TEST_ASSERT_TRUE(saveParameters(parameters, storage));
	TEST_ASSERT_EQUALS(storage.values.size(), 3u);

	integer.reset();
	real.reset();
	boolean.reset();
	loadParameters(parameters, storage);
	TEST_ASSERT_EQUALS(integer.integer(), -7);
	TEST_ASSERT_EQUALS_FLOAT(real.real(), -0.75f);
	TEST_ASSERT_FALSE(boolean.boolean());

	// a stored value out of range falls back to the default
	storage.values[integer.key()] = 100;
	loadParameters(parameters, storage);
	TEST_ASSERT_EQUALS(integer.integer(), 5);

	// returning to the default removes the stored value
	real.reset();
	TEST_ASSERT_TRUE(saveParameters(parameters, storage));
	TEST_ASSERT_EQUALS(storage.values.count(real.key()), 0u);

	TEST_ASSERT_TRUE(eraseParameters(parameters, &storage));
	TEST_ASSERT_EQUALS(storage.values.size(), 0u);
	TEST_ASSERT_TRUE(boolean.boolean());
}
